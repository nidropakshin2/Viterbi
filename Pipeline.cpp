#include "utils/DataGenerator.cpp"
#include "utils/BSC.cpp"
#include "Viterbi.cpp"

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