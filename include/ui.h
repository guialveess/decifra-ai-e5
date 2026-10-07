#ifndef UI_H
#define UI_H

void limpar_tela(void);

void tela_menu(void);
void tela_como_jogar(void);
void tela_exemplos_escopo(void);
void tela_intro_avancado(void);
void tela_loading(void);
void tela_nome(void);
void tela_desafio(int indice);
void tela_painel_logi(int indice);
void tela_feedback(int acertou, int indice);
void tela_resultado_final(int total_rodada);
void tela_saida(void);


void tela_intro_contra_tempo(void);
void tela_desafio_contra_tempo(int indice, int segs_restantes);
char ler_resposta_contra_tempo(int segundos, int *usados_out);
void tela_feedback_contra_tempo(int acertou, int indice,
                                int usados, int timeout,
                                int usou_ajuda);
void tela_resultado_contra_tempo(int total_rodada);

#endif