#include "DataGenerator.cpp"
#include "BSC.cpp"
#include "Encoder.cpp"
#include <cmath>

using namespace std;

int NUM_BITS = 1 << 20;

void TestPipeline() {
    float gen_p = 0.1;
    Generator generator(gen_p);
    int k=1, n=2, m=2;
    vector<vector<int>> g{{7}, {5}};
    Encoder encoder(k, n, m, g);
    float bsc_p = 0.1;
    BSC bsc(bsc_p);
    int data, encoded_data, transmitted_data;
    for (int i = 0; i < NUM_BITS; i++) {
        data = generator.generate_bit();
        encoded_data = encoder.encode(data);
        transmitted_data = bsc.transmit(encoded_data); 
        printf("%d\t%d\t%d\n", data, encoded_data, transmitted_data);
    }
}


int main() {
    TestTrellis();
}