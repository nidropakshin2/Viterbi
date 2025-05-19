#include <vector>
#include <cmath>
#include <cstdlib>
#include <stdio.h>
#include <stdexcept>
#include <climits>

using namespace std;

struct trel {
    int init;
    int data;
    int final;
    vector<int> output;
};


FILE* enc_log = fopen("encoder.txt", "w");
FILE* dec_log = fopen("decoder.txt", "w");


class Encoder {    
    public:
        int k;  // кол-во входов (пока что реализован только один)
        int n;  // Количество выходов
        int m;  // Память
        vector<vector<int>> generators;  // Генераторные полиномы в !!!восьмеричной!!! СИ
        int NUM_STATES;

        Encoder(int k, int n, int m, const vector<vector<int>>& gens): k(k), n(n), m(m), generators(gens) {
            if (generators.size() != n) {
                throw invalid_argument("Number of generators must match number of outputs");
            }
            NUM_STATES = 1 << m;
            memory = 0;
            // fprintf(enc_log, "Encoder initialized with k, n, m: %d %d %d\n", k, n, m);
            // fprintf(enc_log, "Memory: %d\n", memory);
        }

        vector<int> encode(int data) {
            vector<int> output;
            int temp = memory;
            temp = (temp << 1) ^ (data & 1);
            // fprintf(enc_log, "\nMemory: %d, Data: %d, NewMemory: %d\n", data, memory, temp & (NUM_STATES - 1));
            // fprintf(enc_log, "Output: ");
            for (int i = 0; i < n; i++) {
                int result = 0;
                for (int j = m; j >= 0; j--) {
                    result ^= ((temp & generators[i][0]) >> j) & 1;
                }
                // fprintf(enc_log, "%d ", result);
                output.push_back(result);
            }
            memory = temp & (NUM_STATES - 1); // Обрезание по размеру регистра
            return output;
        }

        vector<vector<trel>> gen_trellis() {
            // fprintf(dec_log, "\nGENERATING TRELLIS\n");
            vector<vector<trel>> trellis(NUM_STATES, vector<trel>(2));
            for (int mem_state = 0; mem_state < NUM_STATES; mem_state++) {
                for (int incoming_bit = 0; incoming_bit < 2; incoming_bit++) {
                    vector<int> output;
                    int temp = mem_state;
                    temp = (temp << 1) ^ (incoming_bit & 1);
                    for (int i = 0; i < n; i++) {
                        int result = 0;
                        for (int j = m; j >= 0; j--) {
                            result ^= ((temp & generators[i][0]) >> j) & 1;
                        }
                        output.push_back(result);
                    }
                    int final_state = temp & (NUM_STATES - 1);
                    trellis[mem_state][incoming_bit] = { 
                        mem_state, 
                        incoming_bit, 
                        final_state, 
                        output 
                    };
                }
            }
            // fprintf(dec_log, "\n");
            // for (int i = 0; i < trellis.size(); i++) {
            //     for(int j = 0; j < 2; j++) {
                    // fprintf(dec_log,"%d\t%d\t%d\t", trellis[i][j].init, trellis[i][j].data, trellis[i][j].final);
                    // for (int k = 0; k < n; k++) {
                        // fprintf(dec_log,"%d ", trellis[i][j].output[k]);
                    // }
                    // fprintf(dec_log,"\n");
            //     }
            // }
            return trellis;
        }

    private:
        int memory;  // Состояние регистра (m бит)
};


class Decoder {    
    public:
        int n; // Кол-во выходов кодера 
        int m; // Память кодера
        int current_step; // Шаг декодирования
        int DECODING_DEPTH; // Задержка декодера или глубина кодирования

        Decoder (Encoder encoder) {
            n = encoder.n;
            m = encoder.m;
            NUM_STATES = 1 << m;
            DECODING_DEPTH = 6 * m;
            // fprintf(dec_log, "Decoder initialized with n, m, Decoding depth: %d %d %d\n", n, m, DECODING_DEPTH);

            trellis = encoder.gen_trellis();
            survivor = vector<surv> (NUM_STATES);
            for (int i = 0; i < NUM_STATES; i++) {
                survivor[i].metric = 0;
                survivor[i].data = vector<int>(DECODING_DEPTH, 0);
                survivor[i].states = vector<int>(DECODING_DEPTH, 0);
            }
            surv_temp = survivor;
            current_step = 0;
        }

        int decode(vector<int> received) {

            surv_metric = vector<int> (NUM_STATES, INT_MAX);
            
            // fprintf(dec_log, "Received: ");
            for (int i = 0; i < n; i++)
                // fprintf(dec_log, "%d ", received[i]);
            // fprintf(dec_log, "\n");

            for (int i = 0; i < NUM_STATES; i++) {
                // fprintf(dec_log, "\tState %d\n", i);
                for (int j = 0; j < 2; j++) {
                    // fprintf(dec_log, "\t\tBranch: %d\n", j);
                    // fprintf(dec_log, "\t\t\tfinal state: %d\n", trellis[i][j].final);
                    /* Считаем метрику между принятой последовательностью и возможной отправленной */
                    int metric_temp = comp_metric(received, trellis[i][j].output);
                    // fprintf(dec_log, "\t\t\tmetric(");
                    // for (int k = 0; k < n; k++)
                        // fprintf(dec_log, "%d", received[k]);
                    // fprintf(dec_log, ", ");
                    // for (int k = 0; k < n; k++)
                        // fprintf(dec_log, "%d", trellis[i][j].output[k]);
                    // fprintf(dec_log, ") = %d\n", metric_temp);

                    metric_temp += survivor[i].metric;
                    // fprintf(dec_log, "\t\t\tsurvivor[%d].metric: %d\n", i, survivor[i].metric);
    
                    int final_state = trellis[i][j].final;

                    /* Если метрика улучшилась, то обновляем пути */
                    if (metric_temp < surv_metric[final_state]) {
                        // fprintf(dec_log, "\t\t\t\tBetter metric: %d\n", metric_temp);
                        surv_metric[final_state] = metric_temp;

                        /* Обновляем пути data и состояний */
                        surv_temp[final_state].metric = metric_temp;
                        surv_temp[final_state].data = survivor[i].data;
                        surv_temp[final_state].states = survivor[i].states;
                        
                        // surv_log(surv_temp, true);

                        surv_temp[final_state].data.erase(surv_temp[final_state].data.begin());
                        surv_temp[final_state].data.push_back(trellis[i][j].data);
                
                        surv_temp[final_state].states.erase(surv_temp[final_state].states.begin());
                        surv_temp[final_state].states.push_back(final_state);
                        // surv_log(surv_temp, true);
                    }
                                            
                    // fprintf(dec_log, "\t\t\t\tsurv_metric: ");
                    // for (int k = 0; k < NUM_STATES; k++)
                        // fprintf(dec_log, "%d ", surv_metric[k]);
                    // fprintf(dec_log, "\n");
                }
            }

            // Обновляем метрики путей

            // surv_log(survivor, false);
            survivor = surv_temp;
            // surv_log(survivor, false);
            current_step++;
    
            // Возвращаем декодированный бит с задержкой
            if (current_step >= DECODING_DEPTH) {
                int best_state = 0;
                int min_metric = survivor[0].metric;
                for (int i = 1; i < NUM_STATES; i++) {
                    if (survivor[i].metric < min_metric) {
                        min_metric = survivor[i].metric;
                        best_state = i;
                    }
                }
                return survivor[best_state].data[0]; // Биты с задержкой DECODING_DEPTH
            }
            return -1;
        }

    private:
        int NUM_STATES; // Кол-во возможных состояний памяти 
        vector<vector<trel>> trellis;
        
        struct surv {
            int metric;
            vector<int> data;
            vector<int> states;
        };
        vector<surv> survivor, surv_temp;
        vector<int> surv_metric;


        int comp_metric(vector<int> received, vector<int> reference) {
            /* Расстояние Хэмминга */
            if (received.size() != reference.size())
                throw invalid_argument("received and reference sizes must match");
            int c = 0;
            for (int i = 0; i < received.size(); i++) {
                if (abs(received[i]) > 1 || abs(reference[i]) > 1)
                    throw invalid_argument("received and reference must contain only 0s and 1s");
                c += received[i] ^ reference[i];
            }
            return c;
        }

        /* Вспомогательная функция для логгирования*/
        void surv_log(vector<surv> sv, bool temp) {
            if (temp) fprintf(dec_log, "\n\t\t\t\t\t\t\t\t\t\tSURVIVOR_TEMP\n"); 
            else fprintf(dec_log, "\n\t\t\t\t\t\t\t\t\t\tSURVIVOR\n");
            for (int i = 0; i < NUM_STATES; i++) {
                surv s = sv[i];
                fprintf(dec_log, "\t\t\t\t\t\t\t\t\t\tS_%d: \n", i);
                fprintf(dec_log, "\t\t\t\t\t\t\t\t\t\t\tmetric: %d\n", s.metric);

                fprintf(dec_log, "\t\t\t\t\t\t\t\t\t\t\tdata  : ");
                for (int j = 0; j < DECODING_DEPTH; j++)
                    fprintf(dec_log, "%d ", s.data[j]);
                fprintf(dec_log, "\n");

                fprintf(dec_log, "\t\t\t\t\t\t\t\t\t\t\tstates: ");
                for (int j = 0; j < DECODING_DEPTH; j++)
                    fprintf(dec_log, "%d ", s.states[j]);
                fprintf(dec_log, "\n");
            }
        }
};

int TestTrellis() {
    int k=1, n=2, m=2;
    vector<vector<int>> g{{5}, {7}};
    Encoder encoder(k, n, m, g);
    
    auto trellis = encoder.gen_trellis();
    for (int i = 0; i < trellis.size(); i++) {
        for(int j = 0; j < 2; j++) {
            printf("%d\t%d\t%d\t", trellis[i][j].init, trellis[i][j].data, trellis[i][j].final);
            for (int k = 0; k < encoder.n; k++) {
                printf("%d ", trellis[i][j].output[k]);
            }
            printf("\n");
        }
    }
    return 0;
}
