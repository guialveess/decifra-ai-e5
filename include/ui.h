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

#endif