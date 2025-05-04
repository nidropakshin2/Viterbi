#include <vector>
#include <random>
#include <ctime>
#include <cstdio>

using namespace std;

class Generator {
    float p;
    mt19937 gen;  // Генератор (член класса)
    bernoulli_distribution dist;  // Распределение (член класса)

public:
    // Конструктор по умолчанию
    Generator() : p(0.5), gen(random_device{}()), dist(0.5) {}

    // Конструктор с параметром p
    Generator(float prob) : p(prob), gen(random_device{}()), dist(prob) {}

    int generate_bit() {
        return dist(gen);
    }

    int generate_byte() {
        int data = 0; 
        for (int i = 0; i < 8; i++)
            data = (data << 1) ^ dist(gen);
        return data;
    }

    vector<int> generate_flow(int block_size) {
        vector<int> data(block_size, 0);
        for (int i = 0; i < block_size; i++)
            data[i] ^= dist(gen);
        return data;
    }

    vector<int> impulse(int block_size) {
        vector<int> data(block_size, 0);
        data[0] = 1;
        return data;
    }
};
