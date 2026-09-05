#include <stdio.h>
#include "game.h"
#include "ui.h"
#include "input.h"

// Funções:
// inicializar_jogo() - Inicializa o jogo, configurando o estado inicial e preparando os recursos necessários.
// mostrar_menu() - Exibe o menu inicial do jogo, permitindo que o jogador escolha opções como iniciar o jogo, carregar um jogo salvo ou sair.
// ler_entrada() - Lê a entrada do jogador, capturando a opção selecionada no menu ou outras interações do usuário.
// atualizar_jogo() - Atualiza o estado do jogo com base na entrada do jogador e nas regras do jogo, processando eventos e mudanças no ambiente do jogo.

int main (void){

    printf("--------------------------------------------------------\n");
    printf("|       BEM-VINDO AO ESQUELETO DO JOGO -> W06          |\n");
    printf("--------------------------------------------------------\n\n");

    printf("[1] Iniciando Jogo...\n");
    inicializar_jogo();

    printf("\n[2] Mostrando Menu Inicial...\n");
    mostrar_menu();

    printf("\n[3] Lendo Entrada...\n");
    int opcao = ler_entrada();
    printf("Opcao selecionada: %d\n", opcao);

    printf("\n[4] Atualizando o Jogo...\n");
    atualizar_jogo();

    printf("Programa finalizado com sucesso!\n");

    return 0;
}
