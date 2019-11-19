#include <stdio.h>

void cai(int matr[][6]){
    int i, j, aux;
    for(i = 1; 12 > i; i++){
        for(int j=0; 6 > j; j++){
            if(matr[i][j] == 0 && matr[i-1][j] != 0){
                matr[i][j] = matr[i-1][j];
                matr[i-1][j] = 0;
                cai(matr);
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
}
