#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#include "SuMo.h"

using namespace std;

int main(int argc, char *argv[]) {
    SuMo sumo;
    int num_events = 10;
    int boards[] = {2, 4};
    vector<packet_t**> data;
    srand((unsigned)time(0));

    /* Create fake dataset */
    for (int e = 0; e < num_events; e++) {
        for (int b = 0; b < 2; b++) {
            int board = boards[b];
            sumo.DC_ACTIVE[board] = true;
            for (int j = 0; j < psecSampleCells; j++){
                sumo.adcDat[board]->Meta[j] = rand() % 600;
            }
            for(int i = 0; i < AC_CHANNELS; i++){
                for(int j = 0; j < psecSampleCells; j++){
                    int sample = rand() % 100 + 3000;
                    sumo.adcDat[board]->Data[i][j] = sample;
                }
            }
        }
        data.push_back(sumo.adcDat);
    }
    sumo.log_data("tests/data/test", data, 0);
    cout << "Made it to end of file" << endl;
    return 0;
}
