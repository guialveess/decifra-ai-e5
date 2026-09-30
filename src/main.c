#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "game.h"
#include "ui.h"
#include "input.h"

#ifndef _WIN32
#include <unistd.h>
#include <termios.h>

static struct termios g_orig_term;

static void configurar_terminal(void) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &g_orig_term);
    raw = g_orig_term;
    raw.c_lflag &= ~(unsigned)(ECHO | ICANON);
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

static void restaurar_termios(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_orig_term);
}
#else
static void configurar_terminal(void) {}
static void restaurar_termios(void)   {}
#endif

static void restaurar_terminal(int sig) {
    (void)sig;
    restaurar_termios();
    printf("\033[?25h\033[?1049l");
    fflush(stdout);
    exit(0);
}

int main(void) {
    int opcao;
    int i;

    signal(SIGINT,  restaurar_terminal);
    signal(SIGTERM, restaurar_terminal);

    printf("\033[?1049h\033[H\033[3J\033[2J\033[H\033[?25l");
    fflush(stdout);
    configurar_terminal();

    while (1) {
        tela_menu();
        opcao = ler_opcao();

        if (opcao == 5) {
            tela_saida();
            break;
        }

        if (opcao == 4) {                       /* Nível Avançado */
            tela_nome();
            ler_nome(jogador.nome, NOME_MAX);
            executar_nivel_avancado();
            continue;
        }

        if (opcao == 3) {                       /* HU9 — Exemplos */
            tela_exemplos_escopo();
            continue;
        }

        if (opcao == 2) {                       /* Como Jogar */
            tela_como_jogar();
            continue;
        }

        if (opcao != 1) continue;

        /* Fluxo normal do jogo */
        tela_loading();

        tela_nome();
        ler_nome(jogador.nome, NOME_MAX);

        inicializar_jogo();

        for (i = 0; i < NUM_DESAFIOS_BASE; i++) {
            executar_desafio(i);
        }

        finalizar_jogo(NUM_DESAFIOS_BASE);
    }

    restaurar_termios();
    printf("\033[?25h\033[?1049l");
    fflush(stdout);

    return 0;
}