#ifndef GAME_H
#define GAME_H

#define NOME_MAX            40
#define NUM_DESAFIOS_BASE    5
#define NUM_DESAFIOS_AVANC   5
#define NUM_DESAFIOS        (NUM_DESAFIOS_BASE + NUM_DESAFIOS_AVANC)
#define NIVEL_AVANCADO       4

#define TIMER_SEGUNDOS       20
#define BONUS_RAPIDO_SEG     10
#define BONUS_MEDIO_SEG      15

#define DESAFIO_ENUNCIADO_MAX  256
#define DESAFIO_EXPLICACAO_MAX 384
#define DESAFIO_VARS_MAX        64
#define DESAFIO_EXPR_MAX        64

typedef struct {
    char enunciado[DESAFIO_ENUNCIADO_MAX];
    char resposta_correta;
    char explicacao[DESAFIO_EXPLICACAO_MAX];
    int  nivel;
    char variaveis[DESAFIO_VARS_MAX];
    char expressao[DESAFIO_EXPR_MAX];
    int  linha_tabela;
    int  total_linhas;
} Desafio;

typedef struct {
    char nome[NOME_MAX];
    int  pontuacao;
    int  acertos;
    char resposta_jogador[NUM_DESAFIOS];
    int  acertou[NUM_DESAFIOS];
    int  tempo_por_desafio[NUM_DESAFIOS];
    int  usou_ajuda[NUM_DESAFIOS];
} Jogador;

extern Desafio desafios[NUM_DESAFIOS];
extern Jogador jogador;

void inicializar_jogo(void);
void executar_desafio(int indice);
void executar_nivel_avancado(void);
void executar_nivel_contra_tempo(void);
void finalizar_jogo(int total_rodada);

#endif
