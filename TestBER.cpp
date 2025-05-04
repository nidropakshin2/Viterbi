#include "Pipeline.cpp"
#include <algorithm>
#include <cstdio>
#include <fstream>

using namespace std;

void BER(string csv_name) {
    fstream fout;
    fout.open(csv_name, ios::out);

    float gen_p = 0.5;
    int N = 100000;
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
    BER("temp/BER.csv");
}