#include <stdio.h>
#include "input.h"

int ler_entrada() {
    int opcao = 0;
    printf("\nDigite uma opcao: ");
    scanf("%d", &opcao);
    return opcao;
}