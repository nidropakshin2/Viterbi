#include "DataGenerator.cpp"
#include "BSC.cpp"
#include "Viterbi.cpp"
#include <cmath>
#include <string>
#include <algorithm>
#include <cstdio>

using namespace std;

int TestGeneratorFlow() {
    float p = 0.2;
    Generator G(p);
    int n = 100;
    float c = 0;
    auto flow = G.generate_flow(n);
    for (int i = 0; i < n; i++) {
        c += flow[i];
        printf("%d", flow[i]);
    }
    printf("\n%.2f", c / n);
    return 0;
}


int TestBSC() {
    float p = 0.1;
    BSC bsc(p);
    vector<int> data(100000);
    vector<int> dirty_data = bsc.transmit(data);
    float ErrRate = 0;
    for (int i = 0; i < data.size(); i++) {
        ErrRate += (data[i] != dirty_data[i]);
    }
    printf("\nError Rate: %.2f", ErrRate / data.size());
    return 0;
}

int TestEncoder() {
    int k=1, n=2, m=2;
    vector<vector<int>> g{{7}, {5}};
    Encoder encoder(k, n, m, g);
    
    vector<int> input{1, 0, 0, 0, 0, 0};
    for (int i = 0 ; i < input.size(); i++) {
        printf("%2d ", encoder.encode(input[i]));
    }
    return 0;
}

int TestTrellis() {
    int k=1, n=2, m=2;
    vector<vector<int>> g{{5}, {7}};
    Encoder encoder(k, n, m, g);
    
    auto trellis = encoder.gen_trellis();
    for (int i = 0; i < trellis.size(); i++) {
        for(int j = 0; j < 2; j++) {
            printf("%d\t%d\t%d\t", trellis[i][j].init, trellis[i][j].data, trellis[i][j].final);
            for (int k = 0; k < encoder.n; k++) {
                printf("%d ", trellis[i][j].output[k]);
            }
            printf("\n");
        }
    }
    return 0;
}


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

int main() {
    float gen_p = 0.5;
    float p = 0.01;
    int N = 100000;
    int k=1, n=2, m=2;
    vector<vector<int>> g{{7}, {5}};
    
    float error_count = Pipeline(gen_p, p, k, n, m, g, N);
    printf("\nError rate: %.6f", error_count);
}