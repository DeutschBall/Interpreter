#include <iostream>
#include <cmath>
using namespace std;

int main(){
    double res=1.0;
    for(int i=1;true;i++){
        res=0.99*res;
        if(res<0.01){
            cout<<i<<" "<<res<<endl;
            break;
        }
    }


    return 0;
}