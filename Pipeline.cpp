#include "utils/DataGenerator.cpp"
#include "utils/BSC.cpp"
#include "Viterbi.cpp"
#include <unistd.h>

using namespace std;

float Pipeline(float gen_p, float p, int k, int n, int m, vector<vector<int>> &g, int N) {

    Generator G(gen_p);
    BSC bsc(p);
    Encoder encoder(k, n, m, g);
    Decoder decoder(encoder);

    auto data = G.generate_flow(N);
    
    // for (int i = 0 ; i < data.size(); i++) {
    //     printf("%d ", data[i]);
    // }
    // printf("\n");

    vector<int> transmitted;
    for (int i = 0; i < data.size(); i++) {

        vector<int> encoded_data = encoder.encode(data[i]);
        vector<int> transmitted_data = bsc.transmit(encoded_data);

        transmitted.insert(transmitted.end(), transmitted_data.begin(), transmitted_data.end());

    }

    vector<int> decoded(decoder.DECODING_DEPTH, 0);
    for (int i = 0; i < transmitted.size() - decoder.n; i+=decoder.n) {

        vector<int> received_data(decoder.n);
        copy(transmitted.begin() + i, transmitted.begin() + i + decoder.n, received_data.begin());

        int decoded_data = decoder.decode(received_data);
        if (decoded_data >= 0)
            decoded.push_back(decoded_data);
            
    }


    // for (int i = decoder.DECODING_DEPTH; i < decoded.size(); i++) {
    //     printf("%d ", decoded[i]);
    // }
    // printf("\n");

    float error_count = 0;

    for (int i = decoder.DECODING_DEPTH; i < N; i++) {
        error_count += (decoded[i] != data[i - decoder.DECODING_DEPTH]); 
    }

    return error_count / N;
}


float RealTimePipeline(float gen_p, float p, int k, int n, int m, vector<vector<int>> &g, int N, int speed) {
    Generator G(gen_p);
    BSC bsc(p);
    Encoder encoder(k, n, m, g);
    Decoder decoder(encoder);

    auto data = G.generate_flow(N);
    
    float error_count = 0;
    float error_rate = INFINITY;

    vector<int> data_buffer(20), decoded_buffer(20);
    printf("\e[?25l");
    for (int i = 0; i < data.size(); i++) {

        vector<int> encoded_data = encoder.encode(data[i]);
        vector<int> transmitted_data = bsc.transmit(encoded_data);

        int decoded_data = decoder.decode(transmitted_data);

        if (i + 1 >= decoder.DECODING_DEPTH) {
            error_count += (decoded_data != data[i - decoder.DECODING_DEPTH + 1]);
            error_rate = error_count / i;
        }
        printf("\n");

        printf("\rData           : ");
        for (int i = 0; i < data_buffer.size(); i++) {
            if (i < data_buffer.size() - 1) printf("%d ", data_buffer[i]);
            else printf("[%d]", data_buffer[i]);

            data_buffer[i] = data_buffer[(i+1) % data_buffer.size()];
        }
        if (i + 1 >= decoder.DECODING_DEPTH)
            data_buffer[data_buffer.size() - 1] = data[i - decoder.DECODING_DEPTH + 1];
        else 
            data_buffer[data_buffer.size() - 1] = 0;
        
        printf("\nEncoded bit    : [%d] -> ", (i + 1 >= decoder.DECODING_DEPTH) ? data[i - decoder.DECODING_DEPTH + 1]: 0); //\x1b[;47 \x1b[0m
        for (int i = 0; i < encoded_data.size(); i++)
            printf("%d ", encoded_data[i]);
        
        printf("\nTransmitted bit: ");
        for (int i = 0; i < transmitted_data.size(); i++)
            printf("%d ", transmitted_data[i]);
        printf("-> {%d}", (i + 1 >= decoder.DECODING_DEPTH) ? decoded_data: 0);

        printf("\nDecoded data   : ");

        for (int j = 0; j < decoded_buffer.size(); j++) {
            if (j < decoded_buffer.size() - 1) {
                if (decoded_buffer[j] > 1) {
                    printf("%c ", decoded_buffer[j]);
                }
                else
                    printf("%d ", decoded_buffer[j]);
            }
            else {
                if (decoded_buffer[j] > 1) {
                    printf("{%c}", decoded_buffer[j]);
                }
                else
                    printf("{%d}", decoded_buffer[j]);
            }
            decoded_buffer[j] = decoded_buffer[(j+1) % decoded_buffer.size()];
        }

        if (i + 1 >= decoder.DECODING_DEPTH)
            decoded_buffer[decoded_buffer.size() - 1] = decoded_data;
        else 
            decoded_buffer[decoded_buffer.size() - 1] = 0;
        usleep(speed * 1000);

        printf("\nError Rate     : %.3f", error_rate);

        printf("\x1b[A");
        printf("\x1b[A");
        printf("\x1b[A");
        printf("\x1b[A");
        printf("\x1b[A");
    }
    printf("\n");
    return error_rate;
}
