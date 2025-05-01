#include <cstdlib>
#include <vector>
#include <windows.h>
#include <stdexcept>

using namespace std;

class Encoder {
    int k;  // кол-во входов (пока что реализован только один)
    int n;  // Количество выходов
    int m;  // Память
    vector<vector<int>> generators;  // Генераторные полиномы

public:
    Encoder(int k, int n, int m, const vector<vector<int>>& gens): k(k), n(n), m(m), generators(gens) {
        if (generators.size() != n) {
            throw invalid_argument("Number of generators must match number of outputs");
        }
    }

    int encode(int data) {
        int output = 0;
        int temp = memory;
        temp = (temp << 1) ^ (data & 1);
        for (int i = 0; i < n; i++) {
            int result = 0;
            for (int j = m; j >= 0; j--) {
                result ^= ((temp & generators[i][0]) >> j) & 1;
            }
            output = (output << 1) ^ result;
        }
        memory = temp;
        return output;
    }

private:
    int memory = 0;  // Состояние регистра (m бит)
};
 
int TestEncoder() {
    int k=1, n=2, m=2;
    vector<vector<int>> g{{7}, {5}};
    Encoder encoder(k, n, m, g);
    
    vector<int> input{1, 1, 0, 1, 0, 0};
    for (int i = 0 ; i < input.size(); i++) {
        printf("%2d ", encoder.encode(input[i]));
    }
    return 0;
}
