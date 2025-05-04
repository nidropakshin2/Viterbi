#include "Pipeline.cpp"

using namespace std;

int main() {
    float gen_p = 0.5;
    float p = 0.1;
    int N = 100000;
    int k=1, n=2, m=6;
    vector<vector<int>> g{{171}, {37}};
    
    float error_rate = Pipeline(gen_p, p, k, n, m, g, N);
    printf("Error Rate = %.6f", error_rate);
}