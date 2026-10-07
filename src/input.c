#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "input.h"

#ifndef _WIN32
#include <unistd.h>
#include <termios.h>
void limpar_stdin(void) { tcflush(STDIN_FILENO, TCIFLUSH); }
#else
void limpar_stdin(void) {}
#endif

static void consumir_escape(void) {
    int c = getchar();
    /* Lê até o caractere final do escape (intervalo 0x40–0x7E) */
    if (c == '[' || c == 'O')
        while ((c = getchar()) != EOF && !(c >= 0x40 && c <= 0x7E));
}

int ler_opcao(void) {
    int c;
    limpar_stdin();
    while (1) {
        c = getchar();
        if (c == EOF) return 0;                   /* fim de arquivo: retorna 0 */
        if (c == '\033') { consumir_escape(); continue; } /* ignora escape */
        if (c >= '1' && c <= '9') return c - '0'; /* converte char para inteiro */
    }
}

char ler_resposta(void) {
    int c, up;
    limpar_stdin();
    while (1) {
        c = getchar();
        if (c == EOF) return 'F';                  /* fallback seguro em EOF */
        if (c == '\033') { consumir_escape(); continue; }
        up = toupper((unsigned char)c);            /* normaliza maiúsculas/minúsculas */
        if (up == 'V' || up == 'F' || up == 'H') return (char)c;
        /* qualquer outro caractere é silenciosamente descartado */
    }
}

void ler_nome(char *nome, int tamanho) {
    int c, i = 0;
    limpar_stdin();
    while (i < tamanho - 1) {
        c = getchar();
        if (c == EOF || c == '\n' || c == '\r') break; /* Enter encerra a leitura */
        if (c == '\033') { consumir_escape(); continue; }
        if (c == 127 || c == '\b') {
            /* Backspace: apaga último caractere do buffer e da tela */
            if (i > 0) { i--; printf("\b \b"); fflush(stdout); }
            continue;
        }
        nome[i++] = (char)c;
        putchar(c);   /* eco manual do caractere digitado */
        fflush(stdout);
    }
    nome[i] = '\0'; /* garante terminador nulo */
}

/* Aguarda o jogador pressionar Enter antes de continuar */
void aguardar_enter(void) {
    int c;
    printf("\n    Pressione ENTER para continuar...");
    fflush(stdout);
    while (1) {
        c = getchar();
        if (c == EOF || c == '\n' || c == '\r') break;
        if (c == '\033') consumir_escape();
    }
}