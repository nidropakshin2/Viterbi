#include "Pipeline.cpp"

using namespace std;

int main() {
    float gen_p = 0.5;
    float p = 0.1;
    int N = 100000;
    int k=1, n=2, m=6;
    int speed = 100; 
    vector<vector<int>> g{{0171}, {0133}};
    
    // float error_rate = Pipeline(gen_p, p, k, n, m, g, N);
    // printf("Error Rate = %.6f", error_rate);
    float error_rate = RealTimePipeline(gen_p, p, k, n, m, g, N, speed);
    printf("\n\n\nError Rate = %.6f", error_rate);
}

