#include <stdio.h>
#include "ui.h"

void limpar_tela(void) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void mostrar_menu(void) {
    printf("\n----------- MENU DO JOGO -----------\n");
    printf("1. Jogar\n");
    printf("2. Sair\n");
    printf("------------------------------------\n");
}
