#include <random>
#include <cmath>
#include <ctime>

using namespace std;

class BSC {
    float p;    // веорятность ошибки
    public:
        BSC (float p): p(p) {
            std::random_device rd;
            std::mt19937 gen(rd());
            dist = std::bernoulli_distribution (p);         
        }


        int transmit(int data) {
            int dirty_data;
            if (dist(gen)) {
                srand(time(NULL));
                if (data != 0)
                    return data ^ (1 << (rand() % ((int)log2(data) + 1)));
                else
                    return data ^ 1;
            }
            else {
                return data;
            }
        }
    private:
        std::random_device rd;
        std::mt19937 gen;
        std::bernoulli_distribution dist;
};

// int TestBSC() {
//     float p = 0.1;
//     BSC bsc(p);
//     vector<int> data(100000);
//     vector<int> dirty_data = bsc.transmit(data);
//     float ErrRate = 0;
//     for (int i = 0; i < data.size(); i++) {
//         ErrRate += (data[i] != dirty_data[i]);
//     }
//     printf("\nError Rate: %.2f", ErrRate / data.size());
//     return 0;
// }