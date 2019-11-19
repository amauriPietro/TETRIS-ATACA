#include <bits/stdc++.h>
using namespace std;

int main(){
    srand(time(NULL));
    int matr[12][6], i, j;
    for(i=0; 6>i; i++){
        for(j = 0; 6>j; j++){
            matr[i][j] = 0;
        }
    }
    int a = rand();
    for(i=6; 12>i; i++){
        for(j = 0; 6>j; j++){
            do{
                srand(rand());
                a = rand()%6;
                matr[i][j] = a;
            }while((a == matr[i][j-1] && a == matr[i][j-2])||(a == matr[i-1][j] && a == matr[i-2][j]));
        }
    }
    for(i=0; 12>i; i++){
        for(j = 0; 6>j; j++){
            cout << matr[i][j] << " ";
        }
        cout << endl;
    }
}
