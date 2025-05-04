#include <random>
#include <cmath>
#include <ctime>

using namespace std;

class BSC {
    float p;    // веорятность ошибки
    public:
        BSC (): p(0.1) {
            std::random_device rd;
            std::mt19937 gen(rd());
            dist = std::bernoulli_distribution (p);         
        }

        BSC (float p): p(p) {
            std::random_device rd;
            std::mt19937 gen(rd());
            dist = std::bernoulli_distribution (p);         
        }


        int transmit(int data_bit) {
            int dirty_data_bit;
            if (dist(gen)) {
                return data_bit ^ 1;
            }
            else {
                return data_bit;
            }
        }

        vector<int> transmit(vector<int> &data) {
            vector<int> dirty_data;
            for (int i = 0; i < data.size(); i++) {
                if (dist(gen)) {
                    dirty_data.push_back(data[i] ^ 1);
                }
                else {
                    dirty_data.push_back(data[i]);
                }
            }
            return dirty_data;
        }
    private:
        std::random_device rd;
        std::mt19937 gen;
        std::bernoulli_distribution dist;
};
