#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "game.h"
#include "ui.h"
#include "input.h"

#ifndef _WIN32
#include <unistd.h>
#include <termios.h>

/* Salva configuração original do terminal para restaurar ao sair */
static struct termios g_orig_term;

static void configurar_terminal(void) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &g_orig_term);
    raw = g_orig_term;
    raw.c_lflag &= ~(unsigned)(ECHO | ICANON); /* desliga eco e modo canônico */
    raw.c_cc[VMIN]  = 1; /* aguarda ao menos 1 caractere */
    raw.c_cc[VTIME] = 0; /* sem timeout */
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/* Restaura o terminal para o estado anterior ao raw mode */
static void restaurar_termios(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_orig_term);
}
#else
static void configurar_terminal(void) {}
static void restaurar_termios(void)   {}
#endif

/* Handler de sinal: restaura terminal e encerra ao receber SIGINT/SIGTERM */
static void restaurar_terminal(int sig) {
    (void)sig;
    restaurar_termios();
    printf("\033[?25h\033[?1049l"); /* exibe cursor e sai do alternate screen */
    fflush(stdout);
    exit(0);
}

int main(void) {
    int opcao; /* opção escolhida no menu principal (1=Jogar, 2=Como Jogar, 3=Sair) */
    int i;     /* índice do desafio atual no loop de jogo */

    /* Captura sinais para garantir que o terminal seja restaurado mesmo com Ctrl+C */
    signal(SIGINT,  restaurar_terminal);
    signal(SIGTERM, restaurar_terminal);

    /* Ativa alternate screen buffer e oculta cursor antes de iniciar a UI */
    printf("\033[?1049h\033[H\033[3J\033[2J\033[H\033[?25l");
    fflush(stdout);
    configurar_terminal();

    /* Loop principal: exibe menu e processa a opção escolhida pelo jogador */
    while (1) {
        tela_menu();
        opcao = ler_opcao();

        if (opcao == 6) { tela_saida(); break; }

        if (opcao == 5) {                          
            tela_nome();
            ler_nome(jogador.nome, NOME_MAX);
            executar_nivel_contra_tempo();
            continue;
        }

        if (opcao == 4) {                          /* Nível Avançado */
            tela_nome();
            ler_nome(jogador.nome, NOME_MAX);
            executar_nivel_avancado();
            continue;
        }

        if (opcao == 3) { tela_exemplos_escopo(); continue; }
        if (opcao == 2) { tela_como_jogar();      continue; }
        if (opcao != 1) continue;

        tela_loading();
        tela_nome();
        ler_nome(jogador.nome, NOME_MAX);
        inicializar_jogo();
        for (i = 0; i < NUM_DESAFIOS_BASE; i++)
            executar_desafio(i);
        finalizar_jogo(NUM_DESAFIOS_BASE);
    }

    /* Restaura terminal e alternate screen ao encerrar normalmente */
    restaurar_termios();
    printf("\033[?25h\033[?1049l");
    fflush(stdout);
    return 0;
}