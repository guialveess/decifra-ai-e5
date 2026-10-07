#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ui.h"
#include "game.h"
#include "input.h"
#include "ai_client.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#endif

static double g_b       = 1.0;
static int    g_idx     = 0;
static int    g_acertou = 0;
static int    g_rows    = 24;
static char   g_dica_ai[AI_RESP_MAX] = "";
static int    g_tem_ai  = 0;
static int g_timer_row = 0;
static int g_timer_col = 5;
static int g_input_row = 0;
static int g_input_col = 15;

/* ═══════════════ Utilitários ═══════════════ */

static void pausa_ms(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

static void atualizar_tamanho(void) {
#ifndef _WIN32
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_row > 4)
        g_rows = ws.ws_row;
#endif
}

void limpar_tela(void) {
    printf("\033[H\033[3J\033[2J\033[H");
    fflush(stdout);
}

static void set_cor(int r, int g, int b) {
    printf("\033[38;2;%d;%d;%dm",
           (int)(r * g_b), (int)(g * g_b), (int)(b * g_b));
}
static void rst(void) { printf("\033[0m"); }
static void p(int r, int g, int b, const char *s) {
    set_cor(r, g, b); printf("%s", s); rst();
}
static void ir(int r, int c) { printf("\033[%d;%dH", r, c); }

static void linha_div(void) {
    set_cor(60, 90, 180);
    printf("────────────────────────────────────────────────");
    rst();
}

static void mostrar_cursor(void)  { printf("\033[?25h"); fflush(stdout); }
static void esconder_cursor(void) { printf("\033[?25l"); fflush(stdout); }

static void ler_enter(void) {
    int c;
    while (1) {
        c = getchar();
        if (c == EOF || c == '\n' || c == '\r') return;
        if (c == '\033') {
            c = getchar();
            if (c == '[' || c == 'O')
                while ((c = getchar()) != EOF && !(c >= 0x40 && c <= 0x7E));
        }
    }
}

static void fade_tela(void (*render)(void)) {
    static const double steps[] = {0.05, 0.15, 0.3, 0.5, 0.7, 0.87, 1.0};
    int n = 7, i;
    atualizar_tamanho();
    esconder_cursor();
    for (i = 0; i < n; i++) {
        printf("\033[H\033[3J\033[2J\033[H");
        fflush(stdout);
        g_b = steps[i];
        render();
        fflush(stdout);
        pausa_ms(32);
    }
    g_b = 1.0;
    mostrar_cursor();
}

static const char *g_frames[] = {
    "\xe2\xa0\x8b", "\xe2\xa0\x99", "\xe2\xa0\xb9", "\xe2\xa0\xb8",
    "\xe2\xa0\xbc", "\xe2\xa0\xb4", "\xe2\xa0\xa6", "\xe2\xa0\xa7",
    "\xe2\xa0\x87", "\xe2\xa0\x8f"
};

static void loading_item_em(int linha, const char *texto) {
    int i;
    for (i = 0; i < 12; i++) {
        ir(linha, 5);
        set_cor(100, 120, 160);
        printf("%s  %s", g_frames[i % 10], texto);
        rst(); fflush(stdout);
        pausa_ms(70);
    }
    ir(linha, 5);
    set_cor(100, 140, 255);
    printf("✔   "); rst();
    printf("%s", texto); fflush(stdout);
    pausa_ms(80);
}

static void print_com_parenteses_destacados(const char *s) {
    int i;
    for (i = 0; s[i]; i++) {
        if (s[i] == '(' || s[i] == ')') { set_cor(255, 220, 80); printf("%c", s[i]); }
        else                            { set_cor(200, 210, 230); printf("%c", s[i]); }
    }
    rst();
}

static int total_do_nivel(int nivel) {
    return (nivel == NIVEL_AVANCADO) ? NUM_DESAFIOS_AVANC : NUM_DESAFIOS_BASE;
}
static int offset_do_nivel(int nivel) {
    return (nivel == NIVEL_AVANCADO) ? NUM_DESAFIOS_BASE : 0;
}

/* ═══════════════ MENU (6 opções) ═══════════════ */

static void render_menu(void) {
    int r = (g_rows - 13) / 2;
    int c = 5;
    if (r < 1) r = 1;
    ir(r,    c); p(100,140,255, "LOGI - Decifra.IA");
                 p(120,125,160, "  Logica e Seguranca em IA");
    ir(r+2,  c); linha_div();
    ir(r+4,  c); p(100,140,255, "1"); p(200,210,230, "   Jogar");
    ir(r+5,  c); p(100,140,255, "2"); p(200,210,230, "   Como Jogar");
    ir(r+6,  c); p(100,140,255, "3"); p(200,210,230, "   Exemplos de Escopo");
    ir(r+7,  c); p(255,180, 60, "4"); p(200,210,230, "   Nivel Avancado (Parenteses)");
    ir(r+8,  c); p(255,120, 60, "5"); p(200,210,230, "   Contra-Tempo (Avancado)");
    ir(r+9,  c); p(100,140,255, "6"); p(200,210,230, "   Sair");
    ir(r+11, c); linha_div();
    ir(r+13, c); p(120,125,160, "opcao: ");
}

void tela_menu(void) { fade_tela(render_menu); }

/* ═══════════════ COMO JOGAR ═══════════════ */

static void render_como_jogar(void) {
    int r = (g_rows - 20) / 2;
    int c = 5;
    if (r < 1) r = 1;
    ir(r,    c); p(100,140,255, "Como Jogar");
    ir(r+1,  c); linha_div();
    ir(r+3,  c); p(200,210,230, "Voce recebera desafios de logica proposicional e seguranca em IA.");
    ir(r+4,  c); p(200,210,230, "Para os desafios de logica, responda V (Verdadeiro) ou F (Falso).");
    ir(r+6,  c); p(200,210,230, "Pressione "); p(100,140,255, "H");
                 p(200,210,230, " para pedir ajuda ao LOGI, a IA tutora do jogo.");
    ir(r+8,  c); p(200,210,230, "No modo "); p(255,180,60, "Contra-Tempo");
                 p(200,210,230, ", acertos rapidos valem bonus (50% em ate 10s, 25% em ate 15s).");
    ir(r+9,  c); p(200,210,230, "Usar [H] pausa o timer, mas desativa o bonus da questao.");
    ir(r+11, c); p(120,125,160, "Operadores logicos:");
    ir(r+13, c); p(100,140,255, "AND            "); p(200,210,230, "verdadeiro quando ambos os lados sao V");
    ir(r+14, c); p(100,140,255, "OR             "); p(200,210,230, "verdadeiro quando pelo menos um lado e V");
    ir(r+15, c); p(100,140,255, "NOT            "); p(200,210,230, "inverte o valor logico");
    ir(r+16, c); p(100,140,255, "IMPLICA        "); p(200,210,230, "falso apenas quando P = V e Q = F");
    ir(r+17, c); p(100,140,255, "BICONDICIONAL  "); p(200,210,230, "verdadeiro quando os dois lados sao iguais");
    ir(r+19, c); linha_div();
    ir(r+20, c); p(120,125,160, "Pressione ENTER para continuar...");
}

void tela_como_jogar(void) {
    fade_tela(render_como_jogar);
    mostrar_cursor();
    ler_enter();
    esconder_cursor();
}

/* ═══════════════ EXEMPLOS DE ESCOPO (HU9) ═══════════════ */

typedef struct {
    const char *variaveis;
    const char *expr_a, *passos_a, *res_a;
    const char *expr_b, *passos_b, *res_b;
    const char *explicacao;
} ExemploEscopo;

#define NUM_EXEMPLOS_ESCOPO 3
static int g_exemplo_idx = 0;

static const ExemploEscopo exemplos_escopo[NUM_EXEMPLOS_ESCOPO] = {
    { "P = V  |  Q = F  |  R = F",
      "P OR Q AND R", "V OR F AND F  ->  V OR F  ->  V", "V",
      "(P OR Q) AND R", "(V OR F) AND F  ->  V AND F  ->  F", "F",
      "Sem parenteses, AND tem precedencia sobre OR.\n    Com parenteses, o OR e avaliado primeiro." },
    { "P = V  |  Q = F",
      "NOT P AND Q", "NOT V AND F  ->  F AND F  ->  F", "F",
      "NOT (P AND Q)", "NOT (V AND F)  ->  NOT F  ->  V", "V",
      "O parenteses muda o escopo do NOT: ele passa a\n    aplicar-se ao resultado de P AND Q." },
    { "P = V  |  Q = V  |  R = F",
      "P AND Q OR R", "V AND V OR F  ->  V OR F  ->  V", "V",
      "P AND (Q OR R)", "V AND (V OR F)  ->  V AND V  ->  V", "V",
      "Aqui os dois cenarios coincidem, mas os passos\n    intermediarios sao diferentes." }
};

static void render_exemplos_escopo(void) {
    char buf[160];
    int r = (g_rows - 22) / 2;
    int c = 5;
    const ExemploEscopo *e = &exemplos_escopo[g_exemplo_idx];
    if (r < 1) r = 1;

    ir(r,    c); p(100,140,255, "LOGI - Decifra.IA");
                 p(120,125,160, "  |  Exemplos Didaticos: Escopo");
    ir(r+2,  c); linha_div();
    ir(r+4,  c); p(120,125,160, "Mesmas variaveis, apenas os parenteses mudam:");
    snprintf(buf, sizeof(buf), "Variaveis: %s", e->variaveis);
    ir(r+5,  c); p(200,210,230, buf);

    ir(r+7,  c); p(100,140,255, "Cenario A:   "); p(200,210,230, e->expr_a);
    snprintf(buf, sizeof(buf), "  Passos:     %s", e->passos_a);
    ir(r+8,  c); p(120,125,160, buf);
    snprintf(buf, sizeof(buf), "  Resultado:  [%s]", e->res_a);
    ir(r+9,  c); p(80,200,120, buf);

    ir(r+11, c); p(100,140,255, "Cenario B:   "); p(200,210,230, e->expr_b);
    snprintf(buf, sizeof(buf), "  Passos:     %s", e->passos_b);
    ir(r+12, c); p(120,125,160, buf);
    snprintf(buf, sizeof(buf), "  Resultado:  [%s]", e->res_b);
    ir(r+13, c); p(80,200,120, buf);

    ir(r+15, c); p(200,210,230, "Por que muda?");
    ir(r+16, c); p(120,125,160, e->explicacao);

    snprintf(buf, sizeof(buf), "Exemplo %d de %d",
             g_exemplo_idx + 1, NUM_EXEMPLOS_ESCOPO);
    ir(r+19, c); p(120,125,160, buf);
    ir(r+20, c); p(120,125,160, "[N] Proximo exemplo    [ENTER] Voltar ao menu");
    ir(r+21, c); linha_div();
}

void tela_exemplos_escopo(void) {
    while (1) {
        int c;
        fade_tela(render_exemplos_escopo);
        while (1) {
            c = getchar();
            if (c == EOF) break;
            if (c == '\033') {
                int d = getchar();
                if (d == '[' || d == 'O')
                    while ((d = getchar()) != EOF && !(d >= 0x40 && d <= 0x7E));
                continue;
            }
            break;
        }
        esconder_cursor();
        if (c == 'n' || c == 'N') {
            g_exemplo_idx = (g_exemplo_idx + 1) % NUM_EXEMPLOS_ESCOPO;
            continue;
        }
        break;
    }
}

/* ═══════════════ INTRO NÍVEL AVANÇADO ═══════════════ */

static void render_intro_avancado(void) {
    int r = (g_rows - 14) / 2;
    int c = 5;
    if (r < 1) r = 1;
    ir(r,    c); p(255, 180, 60, "NIVEL AVANCADO");
                 p(120,125,160, "  Escopo e Parenteses");
    ir(r+2,  c); linha_div();
    ir(r+4,  c); p(200,210,230, "Neste nivel os parenteses mudam quem e avaliado primeiro.");
    ir(r+5,  c); p(200,210,230, "Resolva sempre de dentro para fora, seguindo a ordem:");
    ir(r+7,  c); p(100,140,255, "NOT > AND > OR > IMPLICA > BICONDICIONAL");
    ir(r+9,  c); p(200,210,230, "Os parenteses aparecerao destacados em ");
                 p(255,220, 80, "amarelo");
                 p(200,210,230, ".");
    ir(r+11, c); p(200,210,230, "Voce pode pedir ajuda a LOGI com a tecla H.");
    ir(r+13, c); linha_div();
    ir(r+14, c); p(120,125,160, "Pressione ENTER para comecar...");
}

void tela_intro_avancado(void) {
    fade_tela(render_intro_avancado);
    mostrar_cursor(); ler_enter(); esconder_cursor();
}

/* ═══════════════ INTRO CONTRA-TEMPO (HU11) ═══════════════ */

static void render_intro_contra_tempo(void) {
    char buf[80];
    int r = (g_rows - 16) / 2;
    int c = 5;
    if (r < 1) r = 1;
    ir(r,    c); p(255, 120, 60, "CONTRA-TEMPO");
                 p(120,125,160, "  Modo cronometrado");
    ir(r+2,  c); linha_div();
    ir(r+4,  c); p(200,210,230, "Voce tera ");
                 snprintf(buf, sizeof(buf), "%d segundos", TIMER_SEGUNDOS);
                 p(255,220, 80, buf);
                 p(200,210,230, " para responder cada questao.");
    ir(r+6,  c); p(200,210,230, "Bonus de velocidade (apenas em acertos):");
    ir(r+7,  c); p(80,200,120,   "  - ate 10s  =>  +50%");
    ir(r+8,  c); p(255,200, 80,  "  - ate 15s  =>  +25%");
    ir(r+9,  c); p(200,210,230,  "  - acima    =>  pontos normais");
    ir(r+11, c); p(200,210,230, "Se o tempo esgotar, a questao conta como erro.");
    ir(r+12, c); p(200,210,230, "Pressionar [H] pausa o timer, mas desativa o bonus.");
    ir(r+14, c); linha_div();
    ir(r+15, c); p(120,125,160, "Pressione ENTER para comecar...");
}

void tela_intro_contra_tempo(void) {
    fade_tela(render_intro_contra_tempo);
    mostrar_cursor(); ler_enter(); esconder_cursor();
}

/* ═══════════════ LOADING ═══════════════ */

void tela_loading(void) {
    int mid, c;
    atualizar_tamanho();
    mid = g_rows / 2;
    c = 5;
    esconder_cursor();
    limpar_tela();

    ir(mid - 8, c); p(100,140,255, "LOGI - Decifra.IA"); rst(); fflush(stdout);

    loading_item_em(mid - 6, "Inicializando sistema de logica e IA");
    loading_item_em(mid - 4, "Carregando banco de questoes");
    loading_item_em(mid - 2, "Conectando a IA tutora");
    loading_item_em(mid,     "Pronto");
    pausa_ms(180);

    int bR = c + 57;
    set_cor(60, 90, 180);
    ir(mid+2, c); printf("┌─ sistema ──────────────────────────────────────────────┐");
    ir(mid+3, c); set_cor(60,90,180); printf("│"); rst();
    ir(mid+3, c+1); printf("  "); p(120,125,160,"Jogo    "); printf("    "); p(100,140,255,"LOGI - Decifra.IA");
    ir(mid+3, bR); set_cor(60,90,180); printf("│"); rst();
    ir(mid+4, c); set_cor(60,90,180); printf("│"); rst();
    ir(mid+4, c+1); printf("  "); p(120,125,160,"Versao  "); printf("    "); p(100,140,255,"1.0  PI2");
    ir(mid+4, bR); set_cor(60,90,180); printf("│"); rst();
    ir(mid+5, c); set_cor(60,90,180); printf("│"); rst();
    ir(mid+5, c+1); printf("  "); p(120,125,160,"Modo    "); printf("    "); p(100,140,255,"Logica Proposicional e Seguranca em IA");
    ir(mid+5, bR); set_cor(60,90,180); printf("│"); rst();
    ir(mid+6, c); set_cor(60,90,180); printf("│"); rst();
    ir(mid+6, c+1); printf("  "); p(120,125,160,"Status  "); printf("    "); p(100,140,255,"PRONTO");
    ir(mid+6, bR); set_cor(60,90,180); printf("│"); rst();
    set_cor(60, 90, 180);
    ir(mid+7, c); printf("└────────────────────────────────────────────────────────┘");
    rst(); fflush(stdout);

    ir(mid+9, c); p(120,125,160, "Pressione ENTER para continuar...");
    mostrar_cursor(); fflush(stdout);
    ler_enter();
    esconder_cursor();
}

/* ═══════════════ NOME ═══════════════ */

static void render_nome(void) {
    int r = (g_rows - 6) / 2;
    int c = 5;
    if (r < 1) r = 1;
    ir(r,   c); p(100,140,255, "LOGI - Decifra.IA");
    ir(r+2, c); linha_div();
    ir(r+4, c); p(200,210,230, "Como voce se chama?");
    ir(r+6, c); p(120,125,160, "Nome: ");
}

void tela_nome(void) { fade_tela(render_nome); }

/* ═══════════════ DESAFIO (normal + avançado) ═══════════════ */

static void render_desafio(void) {
    char buf[256];
    int r = (g_rows - 12) / 2;
    int c = 5;
    if (r < 1) r = 1;
    snprintf(buf, sizeof(buf),
             "LOGI - Decifra.IA  |  Desafio %d de %d  |  Nivel %d  |  Pontos: %d  |  Linha %d/%d",
             g_idx + 1 - offset_do_nivel(desafios[g_idx].nivel),
             total_do_nivel(desafios[g_idx].nivel),
             desafios[g_idx].nivel,
             jogador.pontuacao,
             desafios[g_idx].linha_tabela + 1,
             desafios[g_idx].total_linhas);
    ir(r,    c); p(100,140,255, buf);
    ir(r+2,  c); linha_div();
    ir(r+4,  c); p(200,210,230, desafios[g_idx].enunciado);
    ir(r+7,  c); linha_div();
    ir(r+9,  c); p(100,140,255, "[V]"); p(200,210,230, " Verdadeiro");
                 p(100,140,255, "     [F]"); p(200,210,230, " Falso");
                 p(100,140,255, "     [H]"); p(200,210,230, " Pedir ajuda ao LOGI");
    ir(r+11, c); p(120,125,160, "resposta: ");
}

static void render_desafio_avancado(void) {
    char buf[256];
    int r = (g_rows - 14) / 2;
    int c = 5;
    if (r < 1) r = 1;
    snprintf(buf, sizeof(buf),
             "LOGI - Decifra.IA  |  AVANCADO %d de %d  |  Nivel %d  |  Pontos: %d  |  Linha %d/%d",
             g_idx - NUM_DESAFIOS_BASE + 1, NUM_DESAFIOS_AVANC,
             desafios[g_idx].nivel, jogador.pontuacao,
             desafios[g_idx].linha_tabela + 1, desafios[g_idx].total_linhas);
    ir(r,    c); p(255, 180, 60, buf);
    ir(r+2,  c); linha_div();
    ir(r+4,  c); print_com_parenteses_destacados(desafios[g_idx].enunciado);
    ir(r+9,  c); linha_div();
    ir(r+11, c); p(100,140,255, "[V]"); p(200,210,230, " Verdadeiro");
                 p(100,140,255, "     [F]"); p(200,210,230, " Falso");
                 p(100,140,255, "     [H]"); p(200,210,230, " Pedir ajuda ao LOGI");
    ir(r+13, c); p(120,125,160, "resposta: ");
}

void tela_desafio(int indice) {
    g_idx = indice;
    if (desafios[indice].nivel == NIVEL_AVANCADO)
        fade_tela(render_desafio_avancado);
    else
        fade_tela(render_desafio);
}





/* Só desenha o timer — NÃO mexe no cursor. O caller decide onde ele fica. */
static void render_timer_inline(int segs) {
    char buf[40];
    ir(g_timer_row, g_timer_col);
    if (segs > 10)      p(80, 200, 120, "⏱ ");
    else if (segs > 5)  p(255, 200, 80, "⏱ ");
    else                p(220, 70, 70,  "⏱ ");
    snprintf(buf, sizeof(buf), "%2ds restantes   ", segs);
    printf("%s", buf); rst();
    fflush(stdout);
}

static void render_desafio_contra_tempo(void) {
    char buf[280];
    int r = (g_rows - 15) / 2;
    int c = 5;
    if (r < 1) r = 1;

    /* Cabeçalho — inclui a linha da tabela-verdade (HU10) */
    snprintf(buf, sizeof(buf),
             "LOGI - Decifra.IA  |  CONTRA-TEMPO %d de %d  |  Nivel %d  |  "
             "Pontos: %d  |  Linha %d/%d",
             g_idx - NUM_DESAFIOS_BASE + 1, NUM_DESAFIOS_AVANC,
             desafios[g_idx].nivel, jogador.pontuacao,
             desafios[g_idx].linha_tabela + 1,
             desafios[g_idx].total_linhas);
    ir(r,    c); p(255, 120, 60, buf);

    /* Linha do timer */
    g_timer_row = r + 1;
    g_timer_col = c;
    render_timer_inline(TIMER_SEGUNDOS);

    /* Corpo */
    ir(r+3,  c); linha_div();
    ir(r+5,  c); print_com_parenteses_destacados(desafios[g_idx].enunciado);
    ir(r+9,  c); linha_div();

    /* Opções — texto curto para não quebrar em 80 colunas */
    ir(r+11, c); p(100,140,255, "[V]"); p(200,210,230, " Verdadeiro");
                 p(100,140,255, "    [F]"); p(200,210,230, " Falso");
                 p(100,140,255, "    [H]"); p(200,210,230, " Ajuda (pausa timer)");

    /* Linha de input — isolada, com cursor posicionado */
    g_input_row = r + 13;
    g_input_col = c + 10;
    ir(g_input_row, c); p(120,125,160, "resposta: ");
    fflush(stdout);
}

void tela_desafio_contra_tempo(int indice, int segs_restantes) {
    g_idx = indice;
    fade_tela(render_desafio_contra_tempo);
    render_timer_inline(segs_restantes);
    ir(g_input_row, g_input_col);      /* devolve o cursor à linha de resposta */
    fflush(stdout);
}

char ler_resposta_contra_tempo(int segundos, int *usados_out) {
    int restantes = segundos;
    int inicio    = segundos;

    limpar_stdin();
    render_timer_inline(restantes);
    ir(g_input_row, g_input_col);
    fflush(stdout);

    while (restantes > 0) {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        tv.tv_sec  = 1;
        tv.tv_usec = 0;

        r = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
        if (r < 0) break;
        if (r == 0) {
            restantes--;
            render_timer_inline(restantes);
            ir(g_input_row, g_input_col);
            fflush(stdout);
            continue;
        }

        {
            unsigned char ch = 0;
            int n = (int)read(STDIN_FILENO, &ch, 1);
            if (n != 1) continue;

            if (ch == 0x1B) {
                unsigned char s1 = 0, s2 = 0;
                if (read(STDIN_FILENO, &s1, 1) == 1) {
                    if (s1 == '[' || s1 == 'O') {
                        while (read(STDIN_FILENO, &s2, 1) == 1 &&
                               !(s2 >= 0x40 && s2 <= 0x7E)) { }
                    }
                }
                continue;
            }

            {
                int up = toupper((int)ch);
                if (up == 'V' || up == 'F' || up == 'H') {
                    /* Eco visual na linha de input */
                    ir(g_input_row, g_input_col);
                    printf("%c", ch); fflush(stdout);
                    *usados_out = inicio - restantes;
                    return (char)ch;
                }
            }
        }
    }
    *usados_out = inicio;
    return -1;
}

/* ═══════════════ PAINEL LOGI ═══════════════ */

static const char *dicas_l1[NUM_DESAFIOS] = {
    "AND so e verdadeiro quando os dois lados sao V.",
    "OR e verdadeiro quando pelo menos um lado e V.",
    "NOT inverte o valor logico.",
    "P IMPLICA Q e falso apenas quando P = V e Q = F.",
    "BICONDICIONAL e V quando os dois lados tem o mesmo valor.",
    "Parenteses sao avaliados antes de qualquer conectivo externo.",
    "NOT fora do parenteses so afeta o resultado interno.",
    "Comece resolvendo o interior de cada parenteses.",
    "Avalie cada parenteses separadamente e depois combine os resultados.",
    "Resolva o parenteses, depois o NOT externo, depois o OR."
};
static const char *dicas_l2[NUM_DESAFIOS] = {
    "Se qualquer lado for F, o resultado e F.",
    "So e falso quando os dois lados sao F.",
    "NOT V = F  e  NOT F = V.",
    "Em todos os outros casos o resultado e V.",
    "V BICON V = V  e  F BICON F = V.",
    "Ordem de precedencia: NOT > AND > OR > IMPLICA > BICONDICIONAL.",
    "Ex.: NOT (V AND F) = NOT F = V.",
    "Trabalhe de dentro para fora, um parenteses por vez.",
    "Combine com AND/OR/IMPLICA respeitando o resultado de cada bloco.",
    "Depois do parenteses, aplique NOT e siga a ordem normal."
};

static void render_logi(void) {
    char buf[48];
    int r = (g_rows - 11) / 2;
    int c = 5;
    if (r < 1) r = 1;
    ir(r,    c); p(100,140,255, "LOGI");
                 p(120,125,160, "  Tutora de Logica e Seguranca em IA");
    ir(r+2,  c); linha_div();
    snprintf(buf, sizeof(buf), "Dica para o desafio %d:",
             g_idx + 1 - offset_do_nivel(desafios[g_idx].nivel));
    ir(r+4,  c); p(120,125,160, buf);
    if (g_tem_ai) {
        ir(r+6, c); p(100,140,255, g_dica_ai);
    } else {
        ir(r+6, c); p(100,140,255, dicas_l1[g_idx]);
        ir(r+7, c); p(100,140,255, dicas_l2[g_idx]);
    }
    ir(r+9,  c); linha_div();
    ir(r+10, c); p(120,125,160, "Pressione ENTER para continuar...");
}

void tela_painel_logi(int indice) {
    g_idx  = indice;
    g_tem_ai = 0;
    g_dica_ai[0] = '\0';

    ir(g_rows / 2, 5);
    p(100,140,255, "LOGI");
    p(120,125,160, "   consultando...");
    fflush(stdout);

    if (ai_consultar(desafios[indice].enunciado,
                     g_dica_ai, sizeof(g_dica_ai)) == 0)
        g_tem_ai = 1;

    fade_tela(render_logi);
    mostrar_cursor(); ler_enter(); esconder_cursor();
}

/* ═══════════════ FEEDBACK (normal) ═══════════════ */

static void render_feedback(void) {
    char buf[128];
    int r = (g_rows - 12) / 2;
    int c = 5;
    int total = total_do_nivel(desafios[g_idx].nivel);
    int off   = offset_do_nivel(desafios[g_idx].nivel);
    if (r < 1) r = 1;
    snprintf(buf, sizeof(buf), "LOGI - Decifra.IA  |  Desafio %d de %d",
             g_idx + 1 - off, total);
    ir(r,    c); p(100,140,255, buf);
    ir(r+2,  c); linha_div();
    if (g_acertou) {
        snprintf(buf, sizeof(buf), "CORRETO  +%d pontos",
                 desafios[g_idx].nivel * 10);
        ir(r+4, c); p(80,200,120, buf);
    } else {
        snprintf(buf, sizeof(buf), "INCORRETO  resposta correta: %c",
                 desafios[g_idx].resposta_correta);
        ir(r+4, c); p(220,70,70, buf);
    }
    ir(r+6,  c); p(120,125,160, "Explicacao:");
    ir(r+7,  c); p(200,210,230, desafios[g_idx].explicacao);
    ir(r+10, c); linha_div();
    ir(r+11, c); p(120,125,160, "Pressione ENTER para continuar...");
}

void tela_feedback(int acertou, int indice) {
    g_idx = indice;
    g_acertou = acertou;
    fade_tela(render_feedback);
    mostrar_cursor(); ler_enter(); esconder_cursor();
}

/* ═══════════════ FEEDBACK CONTRA-TEMPO (HU11) ═══════════════ */

static int g_ft_acertou = 0;
static int g_ft_usados  = 0;
static int g_ft_timeout = 0;
static int g_ft_ajuda   = 0;

static void render_feedback_contra_tempo(void) {
    char buf[160];
    int r = (g_rows - 13) / 2;
    int c = 5;
    int base = desafios[g_idx].nivel * 10;
    int pontos = base;
    if (r < 1) r = 1;

    snprintf(buf, sizeof(buf),
             "LOGI - Decifra.IA  |  CONTRA-TEMPO  |  Desafio %d de %d",
             g_idx - NUM_DESAFIOS_BASE + 1, NUM_DESAFIOS_AVANC);
    ir(r,    c); p(255, 120, 60, buf);
    ir(r+2,  c); linha_div();

    if (g_ft_timeout) {
        ir(r+4, c); p(220,70,70, "TEMPO ESGOTADO  resposta correta: ");
        snprintf(buf, sizeof(buf), "%c", desafios[g_idx].resposta_correta);
        printf("%s", buf); rst();
    } else if (g_ft_acertou) {
        if (!g_ft_ajuda) pontos = (g_ft_usados <= BONUS_RAPIDO_SEG) ? base * 3 / 2
                                 : (g_ft_usados <= BONUS_MEDIO_SEG)  ? base * 5 / 4
                                 : base;
        snprintf(buf, sizeof(buf), "CORRETO  +%d pontos  (%ds)", pontos, g_ft_usados);
        ir(r+4, c); p(80,200,120, buf);
        if (g_ft_ajuda)
            { ir(r+5, c); p(200,210,230, "  bonus de velocidade desativado (usou [H])"); }
    } else {
        snprintf(buf, sizeof(buf), "INCORRETO  resposta correta: %c  (%ds)",
                 desafios[g_idx].resposta_correta, g_ft_usados);
        ir(r+4, c); p(220,70,70, buf);
    }

    ir(r+7,  c); p(120,125,160, "Explicacao:");
    ir(r+8,  c); p(200,210,230, desafios[g_idx].explicacao);
    ir(r+11, c); linha_div();
    ir(r+12, c); p(120,125,160, "Pressione ENTER para continuar...");
}

void tela_feedback_contra_tempo(int acertou, int indice,
                                int usados, int timeout, int usou_ajuda) {
    g_idx       = indice;
    g_ft_acertou = acertou;
    g_ft_usados  = usados;
    g_ft_timeout = timeout;
    g_ft_ajuda   = usou_ajuda;
    fade_tela(render_feedback_contra_tempo);
    mostrar_cursor(); ler_enter(); esconder_cursor();
}

/* ═══════════════ RESULTADO FINAL (normal) ═══════════════ */

static int g_total_rodada = 0;

static void render_resultado(void) {
    char buf[64];
    int r = (g_rows - 10) / 2;
    int c = 5;
    if (r < 1) r = 1;
    ir(r,   c); p(100,140,255, "LOGI - Decifra.IA");
                p(120,125,160, "  Resultado Final");
    ir(r+2, c); linha_div();
    snprintf(buf, sizeof(buf), "Jogador    %s", jogador.nome);
    ir(r+4, c); p(120,125,160, buf);
    snprintf(buf, sizeof(buf), "Acertos    %d de %d", jogador.acertos, g_total_rodada);
    ir(r+5, c); p(100,140,255, buf);
    snprintf(buf, sizeof(buf), "Pontos     %d", jogador.pontuacao);
    ir(r+6, c); p(100,140,255, buf);
    if (jogador.acertos == g_total_rodada)
        { ir(r+8, c); p(80,200,120,  "PERFEITO  Voce domina a logica proposicional!"); }
    else if (jogador.acertos * 2 >= g_total_rodada)
        { ir(r+8, c); p(100,140,255, "BOM  Continue praticando!"); }
    else
        { ir(r+8, c); p(120,125,160, "INICIANTE  Nao desista, a logica leva tempo."); }
    ir(r+9,  c); linha_div();
    ir(r+10, c); p(120,125,160, "Pressione ENTER para continuar...");
}

void tela_resultado_final(int total_rodada) {
    g_total_rodada = total_rodada;
    fade_tela(render_resultado);
    mostrar_cursor(); ler_enter(); esconder_cursor();
}

/* ═══════════════ RESULTADO CONTRA-TEMPO (HU11) ═══════════════ */

static int media_tempo(int total) {
    int i, soma = 0, n = 0;
    for (i = NUM_DESAFIOS_BASE; i < NUM_DESAFIOS_BASE + total; i++) {
        soma += jogador.tempo_por_desafio[i];
        n++;
    }
    return (n > 0) ? soma / n : 0;
}

static void render_resultado_contra_tempo(void) {
    char buf[80];
    int r = (g_rows - 14) / 2;
    int c = 5;
    int media = media_tempo(NUM_DESAFIOS_AVANC);
    if (r < 1) r = 1;

    ir(r,    c); p(255, 120, 60, "LOGI - Decifra.IA");
                 p(120,125,160, "  Resultado — Contra-Tempo");
    ir(r+2,  c); linha_div();

    snprintf(buf, sizeof(buf), "Jogador       %s", jogador.nome);
    ir(r+4,  c); p(120,125,160, buf);
    snprintf(buf, sizeof(buf), "Acertos       %d de %d",
             jogador.acertos, NUM_DESAFIOS_AVANC);
    ir(r+5,  c); p(100,140,255, buf);
    snprintf(buf, sizeof(buf), "Pontos        %d", jogador.pontuacao);
    ir(r+6,  c); p(100,140,255, buf);
    snprintf(buf, sizeof(buf), "Tempo medio   %ds por questao", media);
    ir(r+7,  c); p(200,210,230, buf);

    ir(r+9,  c); linha_div();
    if (jogador.acertos == NUM_DESAFIOS_AVANC && media <= 8)
        { ir(r+11, c); p(80,200,120,  "EXCELENTE  Rapido e preciso!"); }
    else if (jogador.acertos >= 4)
        { ir(r+11, c); p(100,140,255, "MUITO BOM  Escopo dominado, ganhe em velocidade."); }
    else if (jogador.acertos >= 2)
        { ir(r+11, c); p(200,210,230, "BOM  Revise a ordem de precedencia e tente de novo."); }
    else
        { ir(r+11, c); p(220,70,70,   "INICIANTE  Pratique primeiro o modo sem tempo."); }

    ir(r+13, c); linha_div();
    ir(r+14, c); p(120,125,160, "Pressione ENTER para continuar...");
}

void tela_resultado_contra_tempo(int total_rodada) {
    (void)total_rodada;
    fade_tela(render_resultado_contra_tempo);
    mostrar_cursor(); ler_enter(); esconder_cursor();
}

/* ═══════════════ SAIDA ═══════════════ */

void tela_saida(void) {
    int i, r, c;
    atualizar_tamanho();
    r = g_rows / 2;
    c = 5;
    limpar_tela();
    for (i = 0; i < 16; i++) {
        ir(r, c);
        set_cor(100 * (i + 1) / 16, 140 * (i + 1) / 16, 255 * (i + 1) / 16);
        printf("%s  Ate logo!", g_frames[i % 10]);
        rst(); fflush(stdout);
        pausa_ms(80);
    }
    ir(r, c);
    set_cor(100, 140, 255);
    printf("✔   Ate logo!");
    rst(); fflush(stdout);
    pausa_ms(400);
}