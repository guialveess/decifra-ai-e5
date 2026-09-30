#ifndef GAME_H
#define GAME_H

#define NOME_MAX            40
#define NUM_DESAFIOS_BASE    5
#define NUM_DESAFIOS_AVANC   5
#define NUM_DESAFIOS        (NUM_DESAFIOS_BASE + NUM_DESAFIOS_AVANC)
#define NIVEL_AVANCADO       4

/* Buffers do desafio gerado em runtime (HU10). */
#define DESAFIO_ENUNCIADO_MAX  256
#define DESAFIO_EXPLICACAO_MAX 384
#define DESAFIO_VARS_MAX        64
#define DESAFIO_EXPR_MAX        64

typedef struct {
    char enunciado[DESAFIO_ENUNCIADO_MAX];
    char resposta_correta;                     /* 'V' ou 'F'          */
    char explicacao[DESAFIO_EXPLICACAO_MAX];
    int  nivel;

    /* HU10 — rastreio da linha da tabela-verdade sorteada */
    char variaveis[DESAFIO_VARS_MAX];          /* "P = V, Q = F"      */
    char expressao[DESAFIO_EXPR_MAX];          /* "P AND Q"           */
    int  linha_tabela;                         /* 0 .. 2^n - 1        */
    int  total_linhas;                         /* 2^n                 */
} Desafio;

typedef struct {
    char nome[NOME_MAX];
    int  pontuacao;
    int  acertos;
} Jogador;

extern Desafio desafios[NUM_DESAFIOS];
extern Jogador jogador;

void inicializar_jogo(void);
void executar_desafio(int indice);
void executar_nivel_avancado(void);
void finalizar_jogo(int total_rodada);

#endif