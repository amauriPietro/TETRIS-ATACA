#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <windows.h>
#include <gl/gl.h>
#include "SOIL.h"
#include "pacman.h"

//=========================================================
// Tamanho de cada bloco da matriz do jogo
#define bloco 70
// Tamanho da matriz do jogo
#define N 12
#define P 6
// Tamanho de cada bloco da matriz do jogo na tela
#define TAM 0.1f
#define TAMBG 1.6f
//Funções que convertem a linha e coluna da matriz em uma coordenada de [-1,1]
#define MAT2X(j) ((j)*0.1f-1)
#define MAT2Y(i) (0.9-(i)*0.1f)

//=========================================================
// Estruturas usadas para controlar o jogo
struct TPoint{
    int x,y;
};

const struct TPoint direcoes[4] = {{1,0},{0,1},{-1,0},{0,-1}};

struct TPacman{
    int status;
    int xi,yi,x,y;
    int direcao,parcial;
    int pontos;
    int invencivel;
    int vivo;
    int animacao;
};



struct TCenario{
    int mapa[N][P];
        int vivo;
};

//==============================================================
// Texturas
//==============================================================

GLuint pecas[5];//pecas
GLuint grade[2];//grade
GLuint bg;
GLuint placar[10];
static void desenhaSprite(float coluna,float linha, GLuint tex);
static GLuint carregaArqTextura(char *str);

// Função que carrega todas as texturas do jogo
void carregaTexturas(){
    int i;
    char str[50];

    for(i=1; i<=2; i++){
        sprintf(str,".//Sprites//grade%d.png", i);
        grade[i] = carregaArqTextura(str);
    }
    for(i=1; i<=5; i++){
        sprintf(str,".//Sprites//peca%d.png",i);
        pecas[i] = carregaArqTextura(str);
    }
    for(i=1; i<=10; i++){
        sprintf(str,".//Sprites//sprite%d.png",i);
        placar[i] = carregaArqTextura(str);
    }
    bg = carregaArqTextura(".//Sprites//bg2.png");
}

// Função que carrega um arquivo de textura do jogo
static GLuint carregaArqTextura(char *str){
    // http://www.lonesock.net/soil.html
    GLuint tex = SOIL_load_OGL_texture
        (
            str,
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y |
            SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
        );

    /* check for an error during the load process */
    if(0 == tex){
        printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }

    return tex;
}

// Função que recebe uma linha e coluna da matriz e um código
// de textura e desenha um quadrado na tela com essa textura
void desenhaSprite(float coluna,float linha, GLuint tex){
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f); glVertex2f(coluna, linha);
        glTexCoord2f(1.0f,0.0f); glVertex2f(coluna+TAM, linha);
        glTexCoord2f(1.0f,1.0f); glVertex2f(coluna+TAM, linha+TAM);
        glTexCoord2f(0.0f,1.0f); glVertex2f(coluna, linha+TAM);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

}
void desenhaBG(float coluna,float linha, GLuint tex){
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f); glVertex2f(coluna, linha);
        glTexCoord2f(1.0f,0.0f); glVertex2f(coluna+TAMBG, linha);
        glTexCoord2f(1.0f,1.0f); glVertex2f(coluna+TAMBG, linha+TAMBG);
        glTexCoord2f(0.0f,1.0f); glVertex2f(coluna, linha+TAMBG);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

}
//==============================================================
// Cenario
//==============================================================

// Função que carrega os dados do cenário de um arquivo texto
Cenario* cenario_carrega(){
    Cenario* cen = malloc(sizeof(Cenario));
    int i,j;
    for(i=0; 5>i; i++){
        for(j = 0; 6>j; j++){
            cen->mapa[i][j] = 0;
        }
    }
    int a = rand();
    for(j = 0; 6>j; j++){
            do{
                srand(rand());
                a = rand()%6;
                cen->mapa[5][j] = a;
            }while((a == cen->mapa[5][j-1] && a == cen->mapa[5][j-2]));
        }
    for(i=6; 12>i; i++){
        for(j = 0; 6>j; j++){
            do{
                srand(rand());
                a = rand()%5 + 1;
                cen->mapa[i][j] = a;
            }while((a == cen->mapa[i][j-1] && a == cen->mapa[i][j-2])||(a == cen->mapa[i-1][j] && a == cen->mapa[i-2][j]));
        }
    }
    cen->vivo = 1;
    return cen;
}

// Libera os dados associados ao cenário
void cenario_destroy(Cenario* cen){
    free(cen);
}

// Percorre a matriz do jogo desenhando os sprites
void cenario_desenha(Cenario *cen){
    int i,j;
    desenhaBG(MAT2X(1.5),MAT2Y(15.5), bg);
    for(i=0; i<N; i++)
        for(j=0; j<P; j++)
            if(cen->mapa[i][j]>0)
                desenhaSprite(MAT2X(j+3),MAT2Y(i+4),pecas[cen->mapa[i][j]]);

}
void troca(Cenario *cen, int m, int n){
    int aux = cen->mapa[m][n+1];
    cen->mapa[m][n+1] = cen->mapa[m][n];
    cen->mapa[m][n] = aux;
}
void cai(Cenario *cen){
    int i, j;
    for(i = 1; 12 > i; i++){
        for(j=0; 6 > j; j++){
            if(cen->mapa[i][j] == 0 && cen->mapa[i-1][j] != 0){
                //ia = i;
                // recursiva
                cen->mapa[i][j] = cen->mapa[i-1][j];
                cen->mapa[i-1][j] = 0;
                cai(cen);
                //nao recursiva (bugada)
                /*
                while((matr[ia][j] == 0 && matr[ia-1][j] != 0) || ia >= 1){
                    matr[ia][j] = matr[ia-1][j];
                    matr[ia-1][j] = 0;
                    ia--;
                }
                */
            }

        }
    }
}
void checa(Cenario *cen){
    int i, j, k, cont=0;
     for(i = 0; 12 > i; i++){
        for(j = 0; 6 > j; j++){
            int a = cen->mapa[i][j];
            if(a == 0)
                continue;
            cont = 0;
            while(cen->mapa[i+cont][j] == a){
                cont++;
            }
            if(cont >= 3){
                cont=0;
                while(cen->mapa[i+cont][j] == a){
                    cen->mapa[i+cont][j] = 0;
                    cont++;
                }
            }
            /*cont = 0;
            while(cen->mapa[i][j+cont] == a){
                cont++;
            }
            if(cont >= 3){
                cont=0;
                while(cen->mapa[i][j+cont] == a){
                    cen->mapa[i][j+cont] = 0;
                    cont++;
                }
            }*/
        }
     }
}
int sobe(Cenario *cen){
    int i, j, ver = 1;
    for(j = 0; 6 > j; j++){
        printf("a");
        if(cen->mapa[0][j] != 0){
            cen->vivo = 0;
            printf("k");
            ver = 0;
            break;
            //scanf("%c");
        }
    }
    if(ver == 0)
        return 0;
            for(i = 1; 12 > i; i++){
                for(int j = 0; 6>j; j++){
                    cen->mapa[i-1][j] = cen->mapa[i][j];
                }
            }
            int a = rand();
            for(j = 0; 6>j; j++){
            do{
                srand(rand());
                a = rand()%5 + 1;
                cen->mapa[11][j] = a;
            }while((a == cen->mapa[11][j-1] && a == cen->mapa[11][j-2]));
        }
        return ver;
}

//==============================================================
// Pacman
//==============================================================


// Função que inicializa os dados associados ao pacman
Pacman* pacman_create(int x, int y){
    Pacman* pac = malloc(sizeof(Pacman));
    if(pac != NULL){
        pac->invencivel = 0;
        pac->pontos = 0;
        pac->vivo = 1;
        pac->status = 0;
        pac->direcao = 0;
        pac->parcial = 0;
        pac->xi = x;
        pac->yi = y;
        pac->x = x;
        pac->y = y;
    }
    return pac;
}

// Função que libera os dados associados ao pacman
void pacman_destroy(Pacman *pac){
    free(pac);
}

// Função que verifica se o pacman está vivo ou não
int pacman_vivo(Pacman *pac){
    if(pac->vivo)
        return 1;
    else{
        if(pac->animacao > 60)
            return 0;
        else
            return 1;
    }
}

// Função que comanda o cursor para outra direção
void comanda_Cursor(Pacman *pac, int direcao, Cenario *cen){
        if(direcao == 0 && pac->x < 7){
            pac->x ++;
        }
        if(direcao == 1 && pac->y < 15){
            pac->y ++;
        }
        if(direcao == 2 && pac-> x > 3){
            pac->x --;
        }
        if(direcao == 3 && pac->y > 4){
            pac->y --;
        }
        //troca peças de lugar
        if(direcao == 4){
            troca(cen, pac->y - 4, pac->x - 3);
        }
}

// Função que desenha o pacman
void pacman_desenha(Pacman *pac){
    float linha = pac->y, coluna = pac->x;

    if(pac->vivo){

        // Escolhe se desenha com boca aberta ou fechada
        if(pac->status < 10){
            desenhaSprite(MAT2X(coluna),MAT2Y(linha), grade[1]);
            desenhaSprite(MAT2X(coluna+1),MAT2Y(linha), grade[1]);
        }
        else{
            desenhaSprite(MAT2X(coluna),MAT2Y(linha), grade[2]);
            desenhaSprite(MAT2X(coluna+1),MAT2Y(linha), grade[2]);
        }

         //Alterna entre cursor maior/menor
        pac->status = (pac->status+1) % 20;
    }

}

void desenha_ponto(Pacman *pac){
    int i, a, s = 0, y=0, ns, no = pac->pontos;
    for(i = 3; i>=0; i--){
        a = pow(10, i);
        ns = no % a;
        s = no - ns;
        if(s/a == 0)
            desenhaSprite(MAT2X(11+y),MAT2Y(6), placar[10]);
        else
            desenhaSprite(MAT2X(11+y),MAT2Y(6), placar[s/a]);
        no = ns;
        y++;
    }
}

