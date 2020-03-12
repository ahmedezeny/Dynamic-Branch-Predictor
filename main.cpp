/*
accepts the trace in file a.txt
a corelated 2 bits branch Predictor
implemented by a map which key is the xor product of the history register and the pranch adress,
the map should contain at most 2^history-reg-size
we shift the contents of the history register adding the value of the branch
and a tournament branch Predictor
//explain
in the corelated 2bit branch Predictor we found that the size of history register should be 3 for optimum perdiction to this trace
author Ahmed Elzeny _ Ahmed Abdallah

*/
#include <bits/stdc++.h>
#include <fstream>
#include <map>
using namespace std;

//the map maintaining the adresses and its state
map<long, int> prediction;
//history register
long regHis = 0;
int wrongCount=0, rightCount=0;

//a function that maintains the history register
//we can set it's length as we like let it be 3
void updateReg(bool taken){
    regHis*=2;
    if(taken)
        regHis+=1;
    if(regHis>=8)
        regHis-=8;
}


int main()
{
    ifstream inputf("a.txt");
    long a,dum;
    int b;
    while (inputf >> a >> b)
    {
        dum=b^regHis;
        //if exists in map
        if (prediction.count(dum) > 0){
            int stat= prediction.find(dum)->second;
            //prediction is not take
            if(stat==0||stat==1){
                //we were right
                if(b==0){
                    rightCount++;
                    //update map to 0;
                    prediction.find(dum)->second=0;
                }
                else{
                    wrongCount++;
                    //update map to 1 if 0, 3 if 1
                    if(stat=0)
                        prediction.find(dum)->second=1;
                    else
                        prediction.find(dum)->second=3;
                }
            //prediction is to take
            }else{
                //we were right
                if(b==1){
                    rightCount++;
                    //update map to 3;
                    prediction.find(dum)->second=3;
                }
                else{
                    wrongCount++;
                    //update map to 0 if 2, 1 if 3
                    if(stat=2)
                        prediction.find(dum)->second=0;
                    else
                        prediction.find(dum)->second=1;
                }
            }
        }else{
            prediction.insert({dum,0});
            if(b=0)
                rightCount++;
            else wrongCount++;
        }
        updateReg(b);
    }
    cout<<(double)rightCount/200;
    inputf.close();
    return 0;
}
