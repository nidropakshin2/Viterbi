#include "DataGenerator.cpp"
#include "BSC.cpp"
#include "Viterbi.cpp"
#include <cmath>
#include <string>
#include <algorithm>
#include <cstdio>
#include <fstream>

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

void BER(string csv_name) {
    fstream fout;
    fout.open(csv_name, ios::out);

    float gen_p = 0.5;
    int N = 1000000;
    int k=1, n=2, m=2;
    vector<vector<int>> g{{7}, {5}};

    fout << "p, BER_min, BER_avg, BER_max\n";

    for (float p = 0.005; p <= 0.1; p+=0.005) {
        vector<float> bit_error_rates(3);

        for (int i = 0; i < bit_error_rates.size(); i++) {
            bit_error_rates[i] = Pipeline(gen_p, p, k, n, m, g, N);
        }

        sort(bit_error_rates.begin(), bit_error_rates.end());

        printf("p = %.3f, BER_min, BER_avg, BER_max = %.6f, %.6f, %.6f\n", 
            p,
            bit_error_rates[0],
            bit_error_rates[1], 
            bit_error_rates[2]
        );
        
        fout << p;
        for (int i = 0; i < bit_error_rates.size(); i++) {
            fout << ", " << bit_error_rates[i];
        }
        fout << "\n";

    }
    fout.close();   
}

int main() {
    float gen_p = 0.5;
    float p = 0.1;
    int N = 100000;
    int k=1, n=2, m=6;
    vector<vector<int>> g{{171}, {37}};
    
    float error_rate = Pipeline(gen_p, p, k, n, m, g, N);
    printf("Error Rate = %.6f", error_rate);
}