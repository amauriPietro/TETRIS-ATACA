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
    int direcao,passo,parcial;
    int pontos;
    int invencivel;
    int vivo;
    int animacao;
};



struct TCenario{
    int mapa[N][P];
};

//==============================================================
// Texturas
//==============================================================

GLuint pecas[5];//pecas
GLuint grade[2];//grade

static void desenhaSprite(float coluna,float linha, GLuint tex);
static GLuint carregaArqTextura(char *str);

// Função que carrega todas as texturas do jogo
void carregaTexturas(){
    int i;
    char str[50];

//    for(i=0; i<12; i++){
//        sprintf(str,".//Sprites//pacman%d.png",i);
//        pacmanTex2d[i] = carregaArqTextura(str);
//    }

    for(i=1; i<=5; i++){
        sprintf(str,".//Sprites//peca%d.png",i);
        pecas[i] = carregaArqTextura(str);
    }
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

//==============================================================
// Cenario
//==============================================================

static int cenario_EhCruzamento(int x, int y, Cenario* cen);
//static int cenario_VerificaDirecao(int mat[N][N], int y, int x, int direcao);
static void cenario_constroiGrafo(Cenario* cen);

// Função que carrega os dados do cenário de um arquivo texto
Cenario* cenario_carrega(){
    Cenario* cen = malloc(sizeof(Cenario));
    int i,j;
    for(i=0; 6>i; i++){
        for(j = 0; 6>j; j++){
            cen->mapa[i][j] = 0;
        }
    }
    int a = rand();
    for(i=6; 12>i; i++){
        for(j = 0; 6>j; j++){
            do{
                srand(rand());
                a = rand()%6;
                cen->mapa[i][j] = a;
            }while((a == cen->mapa[i][j-1] && a == cen->mapa[i][j-2])||(a == cen->mapa[i-1][j] && a == cen->mapa[i-2][j]));
        }
    }
    return cen;
}

// Libera os dados associados ao cenário
void cenario_destroy(Cenario* cen){
    free(cen);
}

// Percorre a matriz do jogo desenhando os sprites
void cenario_desenha(Cenario *cen){
    int i,j;
    for(i=0; i<N; i++)
        for(j=0; j<P; j++)
            if(cen->mapa[i][j]>0)
                desenhaSprite(MAT2X(j+7),MAT2Y(i+8),pecas[cen->mapa[i][j]]);
}



//==============================================================
// Pacman
//==============================================================

//static int pacman_eh_invencivel(Pacman *pac);
//static void pacman_morre(Pacman *pac);
//static void pacman_pontos_fantasma(Pacman *pac);
//static void pacman_AnimacaoMorte(float coluna,float linha,Pacman* pac);
//
//// Função que inicializa os dados associados ao pacman
//Pacman* pacman_create(int x, int y){
//    Pacman* pac = malloc(sizeof(Pacman));
//    if(pac != NULL){
//        pac->invencivel = 0;
//        pac->pontos = 0;
//        pac->passo = 4;
//        pac->vivo = 1;
//        pac->status = 0;
//        pac->direcao = 0;
//        pac->parcial = 0;
//        pac->xi = x;
//        pac->yi = y;
//        pac->x = x;
//        pac->y = y;
//    }
//    return pac;
//}
//
//// Função que libera os dados associados ao pacman
//void pacman_destroy(Pacman *pac){
//    free(pac);
//}
//
//// Função que verifica se o pacman está vivo ou não
//int pacman_vivo(Pacman *pac){
//    if(pac->vivo)
//        return 1;
//    else{
//        if(pac->animacao > 60)
//            return 0;
//        else
//            return 1;
//    }
//}
//
//// Função que verifica se o pacman pode ir para uma nova direção escolhida
//void comanda_Cursor(Pacman *pac, int direcao, Cenario *cen){
//    if(cen->mapa[pac->y + direcoes[direcao].y][pac->x + direcoes[direcao].x] <=2){//não é parede...
//        if(direcao == 0){
//            pac->x ++;
//        }
//        if(direcao == 1){
//            pac->y ++;
//        }
//        if(direcao == 2){
//            pac->x --;
//        }
//        if(direcao == 3){
//            pac->y --;
//        }
//        //COLOCAR A FUNÇAO SWAP AQ
//        /*if(direcao == 4){
//            //swap;
//        }*/
//    }
//}
//
//// Atualiza a posição do pacman
//void pacman_movimenta(Pacman *pac, Cenario *cen){
//    if(pac->vivo == 0)
//        return;
//
//    // Incrementa a sua posição dentro de uma célula da matriz ou muda de célula
//    if(cen->mapa[pac->y + direcoes[pac->direcao].y][pac->x + direcoes[pac->direcao].x] <=2){//não é parede...
//        if(pac->direcao < 2){
//            pac->parcial += pac->passo;
//            if(pac->parcial >= bloco){
//                pac->x += direcoes[pac->direcao].x;
//                pac->y += direcoes[pac->direcao].y;
//                pac->parcial = 0;
//            }
//        }else{
//            pac->parcial -= pac->passo;
//            if(pac->parcial <= -bloco){
//                pac->x += direcoes[pac->direcao].x;
//                pac->y += direcoes[pac->direcao].y;
//                pac->parcial = 0;
//            }
//        }
//    }
//
//    // Come uma das pastilhas no mapa
//    if(cen->mapa[pac->y][pac->x] == 1){
//        pac->pontos += 10;
//    }
//    if(cen->mapa[pac->y][pac->x] == 2){
//        pac->pontos += 50;
//        pac->invencivel = 1000;
//
//    }
//    //Remove a pastilha comida do mapa
//    cen->mapa[pac->y][pac->x] = 0;
//}
//
//// Função que desenha o pacman
//void pacman_desenha(Pacman *pac){
//    float linha, coluna;
//    float passo = (pac->parcial/(float)bloco);
//    //Verifica a posição
//    if(pac->direcao == 0 || pac->direcao == 2){
//        linha = pac->y;
//        coluna = pac->x + passo;
//    }else{
//        linha = pac->y + passo;
//        coluna = pac->x;
//    }
//
//    if(pac->vivo){
//        // Escolhe o sprite com base na direção
//        int idx = 2*pac->direcao;
//
//        // Escolhe se desenha com boca aberta ou fechada
//        if(pac->status < 15)
//            desenhaSprite(MAT2X(coluna),MAT2Y(linha), pacmanTex2d[idx]);
//        else
//            desenhaSprite(MAT2X(coluna),MAT2Y(linha), pacmanTex2d[idx+1]);
//
//        // Alterna entre boca aberta ou fechada
//        pac->status = (pac->status+1) % 30;
//
//        if(pac->invencivel > 0)
//            pac->invencivel--;
//    }else{
//        // Mostra animação da morte
//        pacman_AnimacaoMorte(coluna,linha,pac);
//    }
//}
//
//static int pacman_eh_invencivel(Pacman *pac){
//    return pac->invencivel > 0;
//}
//
//static void pacman_morre(Pacman *pac){
//    if(pac->vivo){
//        pac->vivo = 0;
//        pac->animacao = 0;
//    }
//}
//
//static void pacman_pontos_fantasma(Pacman *pac){
//    pac->pontos += 100;
//}
//
//static void pacman_AnimacaoMorte(float coluna,float linha,Pacman* pac){
//    pac->animacao++;
//    // Verifica qual dos sprites deve ser desenhado para dar o
//    // efeito de que o pacman está sumindo quando morre
//    if(pac->animacao < 15)
//        desenhaSprite(MAT2X(coluna),MAT2Y(linha), pacmanTex2d[8]);
//    else
//        if(pac->animacao < 30)
//            desenhaSprite(MAT2X(coluna),MAT2Y(linha), pacmanTex2d[9]);
//        else
//            if(pac->animacao < 45)
//                desenhaSprite(MAT2X(coluna),MAT2Y(linha), pacmanTex2d[10]);
//            else
//                desenhaSprite(MAT2X(coluna),MAT2Y(linha), pacmanTex2d[11]);
//}
