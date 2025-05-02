#include <cstdlib>
#include <vector>
#include <stdexcept>

using namespace std;

class Encoder {
public:
    int k;  // кол-во входов (пока что реализован только один)
    int n;  // Количество выходов
    int m;  // Память
    vector<vector<int>> generators;  // Генераторные полиномы

    Encoder(int k, int n, int m, const vector<vector<int>>& gens): k(k), n(n), m(m), generators(gens) {
        if (generators.size() != n) {
            throw invalid_argument("Number of generators must match number of outputs");
        }
        NUM_STATES = 1 << m;
        memory = 0;
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
        memory = temp & (NUM_STATES - 1); // Обрезание по размеру регистра
        return output;
    }

    struct trel {
        int init;
        int data;
        int final;
        int output;
    };
    vector<vector<trel>> gen_trellis() {
        vector<vector<trel>> trellis(NUM_STATES, vector<trel> (2));
        for (int mem_state = 0; mem_state < NUM_STATES; mem_state++) {
            for (int incoming_bit = 0; incoming_bit < 2; incoming_bit++) {
                memory = mem_state;
                trellis[mem_state][incoming_bit].init = mem_state;
                trellis[mem_state][incoming_bit].data = incoming_bit;
                trellis[mem_state][incoming_bit].output = encode(incoming_bit);
                trellis[mem_state][incoming_bit].final = memory;
            }
        }
        return trellis;
    }

private:
    int NUM_STATES;
    int memory;  // Состояние регистра (m бит)
};
 
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
            printf("%d\t%d\t%d\t%d\n", trellis[i][j].init, trellis[i][j].data, trellis[i][j].final, trellis[i][j].output);
        }
    }
    return 0;
}