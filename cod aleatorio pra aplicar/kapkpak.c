#include <stdio.h>
#include <time.h>
#include <windows.h>

void troca(int *a, int *b){
    int c;
    c = *a;
    *a = *b;
    *b = c;
}

void cai(int matr[][6]){
    int i, j, aux, ia;
    for(i = 1; 12 > i; i++){
        for(int j=0; 6 > j; j++){
            if(matr[i][j] == 0 && matr[i-1][j] != 0){

                printf("%d", i);
                ia = i;
                /*
                // recursiva
                matr[i][j] = matr[i-1][j];
                matr[i-1][j] = 0;
                cai(matr);
                */
                //nao recursiva
                while((matr[ia][j] == 0 && matr[ia-1][j] != 0) || ia >= 1){
                    matr[ia][j] = matr[ia-1][j];
                    matr[ia-1][j] = 0;
                    ia--;
                }
            }

        }
    }
}

int main(){
    int matr[12][6], i, j;
    for(i = 0; 12 > i; i++){
        for(int j=0; 6 > j; j++){
            scanf("%d", &matr[i][j]);
        }
    }
    cai(matr);
    printf("\n");
     for(i = 0; 12 > i; i++){
        for(int j = 0; 6 > j; j++){
            printf("%d ", matr[i][j]);
        }
        printf("\n");
    }
    Sleep(10000);
}
