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
    if (c == '[' || c == 'O')
        while ((c = getchar()) != EOF && !(c >= 0x40 && c <= 0x7E));
}

int ler_opcao(void) {
    int c;
    limpar_stdin();
    while (1) {
        c = getchar();
        if (c == EOF) return 0;
        if (c == '\033') { consumir_escape(); continue; }
        if (c >= '1' && c <= '9') return c - '0';
    }
}

char ler_resposta(void) {
    int c, up;
    limpar_stdin();
    while (1) {
        c = getchar();
        if (c == EOF) return 'F';
        if (c == '\033') { consumir_escape(); continue; }
        up = toupper((unsigned char)c);
        if (up == 'V' || up == 'F' || up == 'H') return (char)c;
    }
}

void ler_nome(char *nome, int tamanho) {
    int c, i = 0;
    limpar_stdin();
    while (i < tamanho - 1) {
        c = getchar();
        if (c == EOF || c == '\n' || c == '\r') break;
        if (c == '\033') { consumir_escape(); continue; }
        if (c == 127 || c == '\b') {
            if (i > 0) { i--; printf("\b \b"); fflush(stdout); }
            continue;
        }
        nome[i++] = (char)c;
        putchar(c);
        fflush(stdout);
    }
    nome[i] = '\0';
}

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