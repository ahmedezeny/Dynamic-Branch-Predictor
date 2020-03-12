/*
accepts the trace in file a.txt
Gshare:
a correlated 2 bits branch Predictor
implemented by a map which key is the xor product of the history register and the branch address,
the map should contain at most 2^history-reg-size
we shift the contents of the history register adding the value of the branch
in the correlated 2bit branch Predictor we found that if we increased the length of the history register the hit rate would drop

Bimodal:
to make a prediction, the predictor selects a counter from the table using using the lower-order n bits of the instruction's address (we chose n=5).
The direction prediction is made based on the value of the counter. After each branch (correctly predicted or not),
the hardware increments or decrements the corresponding counter to bias the counter toward the actual branch outcome
(the outcome given in the trace file). As these are two-bit saturating counters, decrementing a minimum counter or incrementing a maxed out counter should have no impact.

Tournament:
The tournament table compares the two other tables (gshare and bimodal).
Based on the result of the lookup in the chooser table, the final prediction is either the prediction from the bimodal predictor
(if the chosen counter indicates a preference for bimodal) or the prediction from the gshare predictor (otherwise).


author Ahmed Elzeny _ Ahmad Abdallah

*/
#include <bits/stdc++.h>
#include <fstream>
#include <map>

using namespace std;

//the map maintaining the addresses and its state
map<long, int> gshare;
map<long, int> bimodal;
map<long, int> tournament;
//history register
long regHis = 0;
int wc_gshare = 0, rc_gshare = 0;
int wc_bimodal = 0, rc_bimodal = 0;
bool last_gshare = false, last_bimodal = false;
int wc_tournament = 0, rc_tournament = 0;


//a function that maintains the history register
//we can set it's length as we like let it be 3
void updateReg(bool taken) {
    regHis *= 2;
    if (taken)
        regHis += 1;
    if (regHis >= 8)
        regHis -= 8;
}


void gshareit(long long int a, int b) {
    long long int dum = (a % 100000) ^regHis;
    if (gshare.count(dum) > 0) {
        int stat = gshare.find(dum)->second;
        //gshare is not taken
        if (stat == 0 || stat == 1) {
            //we were right
            if (b == 0) {
                rc_gshare++;
                last_gshare = true;
                //update map to 0;
                gshare.find(dum)->second = 0;
            } else {
                wc_gshare++;
                last_gshare = false;
                //update map to 1 if 0, 3 if 1
                if (stat == 0)
                    gshare.find(dum)->second = 1;
                else
                    gshare.find(dum)->second = 3;
            }
            //gshare is taken
        } else {
            //we were right
            if (b == 1) {
                rc_gshare++;
                last_gshare = true;
                //update map to 3;
                gshare.find(dum)->second = 3;
            } else {
                wc_gshare++;
                last_gshare = false;
                //update map to 0 if 2, 1 if 3
                if (stat == 2)
                    gshare.find(dum)->second = 0;
                else
                    gshare.find(dum)->second = 1;
            }
        }
    } else {
        gshare.insert({dum, 0});
        if (b == 0)
            rc_gshare++;
        else wc_gshare++;
    }
    updateReg(b);
}

void bimodalit(long long int a, int b) {
    int least5bits = a % 100000;
    if (bimodal.count(least5bits) > 0) {
        int state = bimodal.find(least5bits)->second;
        if (state == 0 || state == 1) {
            //we were right
            if (b == 0) {
                rc_bimodal++;
                last_bimodal = true;
                //update map to 0;
                bimodal.find(least5bits)->second = 0;
            } else {
                wc_bimodal++;
                last_bimodal = false;
                //update map to 1 if 0, 3 if 1
                if (state == 0)
                    bimodal.find(least5bits)->second = 1;
                else
                    bimodal.find(least5bits)->second = 3;
            }
            //gshare is taken
        } else {
            //we were right
            if (b == 1) {
                rc_bimodal++;
                last_bimodal = true;
                //update map to 3;
                bimodal.find(least5bits)->second = 3;
            } else {
                wc_bimodal++;
                last_bimodal = false;
                //update map to 0 if 2, 1 if 3
                if (state == 2)
                    bimodal.find(least5bits)->second = 0;
                else
                    bimodal.find(least5bits)->second = 1;
            }
        }
    } else {
        bimodal.insert({least5bits, 0});
        if (b == 0)
            rc_bimodal++;
        else wc_bimodal++;
    }
}

void tournamentit(long long int a, int b) {
    if (last_bimodal == last_gshare) {
        if (last_bimodal) rc_tournament++;
        else wc_tournament++;
    }
    int least5bits = a % 100000;
    if (tournament.count(least5bits) > 0) {
        int state = tournament.find(least5bits)->second;
        if (state == 0 || state == 1) {
            if (last_gshare) {
                tournament.find(least5bits)->second++;
                wc_tournament++;
            } else rc_tournament++;
        } else {
            if (last_bimodal) {
                tournament.find(least5bits)->second++;
                wc_tournament++;
            } else rc_tournament++;
        }
    } else {
        tournament.insert({least5bits, 0});
        if (b == 0)
            rc_tournament++;
        else wc_tournament++;
    }
}

int main() {
    ifstream inputf("a.txt");
    long long a = 0;
    int b = 0;
    while (inputf >> a >> b) {
        gshareit(a, b);
        bimodalit(a, b);
        tournamentit(a, b);
    }

    cout << "gshare: " << rc_gshare / 2 << "%" << endl;
    cout << "bimodal: " << rc_bimodal / 2 << "%" << endl;
    cout << "tournament: " << rc_tournament / 2 << "%" << endl;
    inputf.close();
    return 0;
}
