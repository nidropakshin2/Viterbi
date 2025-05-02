#include <vector>
#include <cmath>
#include "Encoder.cpp"

using namespace std;

class Decoder {
    int n; // Кол-во выходов кодера 
    int m; // Память кодера

    public:
        Decoder (Encoder encoder) {
            n = encoder.n;
            m = encoder.m;
            NUM_STATES = 1 << m;
            DECODING_DEPTH = 10 * m;
            trellis = encoder.gen_trellis();
            for (int i = 0; i < NUM_STATES; i++) {
                survivor.push_back(
                    surv{
                        0,                              // metric
                        vector<int> (DECODING_DEPTH),   // data
                        vector<int> (DECODING_DEPTH)    // states
                    }
                );
            }
        }

        int decode(int recieved) {
            int aux_metric;
            vector<int> surv_metric(NUM_STATES, INT_MAX);
    
            for (int i = 0; i < NUM_STATES; i++) {
                for (int j = 0; j < 2; j++) {
                    aux_metric += comp_metric(recieved, trellis[i][j].output);
                    aux_metric += survivor[i].metric;
                    if (aux_metric < surv_metric[trellis[i][j].final]) {
                        surv_metric[trellis[i][j].final] = aux_metric;

                        surv_temp[trellis[i][j].final].data = survivor[i].data;
                    }
                }
            }
        }

    private:
        int NUM_STATES; // Кол-во возможных состояний памяти
        int DECODING_DEPTH; 
        struct trel {
            int init;
            int data;
            int final;
            int output;
        };
        struct surv {
            int metric;
            vector<int> data;
            vector<int> states;
        };
        vector<vector<trel>> trellis;
        vector<surv> survivor, surv_temp;
        int comp_metric(int recieved, int reference) {
            /*Расстояние Хэмминга*/
            int x = recieved ^ reference;
            int c = 0;
            while (x > 0) {
                c += x & 1;
                x >>= 1;
            }
            return c;
        }
};

int TestDecoder() {
    // TODO
    return 0;
}