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
};

int TestGenerator() {
    float p = 0.2;
    Generator G(p);
    int n = 100;
    float c = 0;
    for (int i = 0; i < n; i++) {
        int data = G.generate_bit();
        c += data;
        printf("%d", data);
    }
    printf("\n%.2f", c / n);
    return 0;
}