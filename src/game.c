#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "game.h"
#include "ui.h"
#include "input.h"

/* ══════════════════════════════════════════════════════════
 *  HU10 — Banco de TEMPLATES (expressão + variáveis + regra)
 *
 *  O enunciado real é gerado por sessão: enumera as 2^n
 *  linhas da tabela-verdade, sorteia uma, calcula a resposta.
 * ══════════════════════════════════════════════════════════ */

typedef struct {
    const char *expressao;   /* ex.: "P AND Q"                    */
    const char *vars;        /* ex.: "PQ" ou "PQR"                */
    const char *regra;       /* explicação genérica da regra      */
    int         nivel;
} DesafioTemplate;

static const DesafioTemplate templates[NUM_DESAFIOS] = {
    /* ── Nível 1 ── */
    { "P AND Q",              "PQ",  "AND so e V quando ambos os lados sao V.",           1 },
    { "P OR Q",               "PQ",  "OR e V quando pelo menos um lado e V.",             1 },
    /* ── Nível 2 ── */
    { "NOT P",                "P",   "NOT inverte o valor logico.",                       2 },
    /* ── Nível 3 ── */
    { "P IMPLICA Q",          "PQ",  "P IMPLICA Q e F apenas quando P=V e Q=F.",          3 },
    { "P BICONDICIONAL Q",    "PQ",  "BICONDICIONAL e V quando os lados sao iguais.",     3 },
    /* ── Nível Avançado — com parênteses ── */
    { "(P OR Q) AND NOT R",             "PQR",
      "Resolva o parenteses antes: (P OR Q), depois NOT R, depois AND.",                  NIVEL_AVANCADO },
    { "NOT (P AND Q) OR R",             "PQR",
      "O NOT fora do parenteses so afeta o resultado interno (P AND Q).",                 NIVEL_AVANCADO },
    { "P AND (Q OR NOT R)",             "PQR",
      "Interior do parenteses primeiro: NOT R, depois Q OR (NOT R), depois AND com P.",   NIVEL_AVANCADO },
    { "(P IMPLICA Q) AND (Q OR R)",     "PQR",
      "Avalie cada parenteses separado e depois combine com AND.",                        NIVEL_AVANCADO },
    { "NOT (P BICONDICIONAL Q) OR R",   "PQR",
      "Parenteses primeiro, depois NOT externo, depois OR.",                              NIVEL_AVANCADO }
};

/* Vetor de desafios gerados (populado em inicializar_jogo). */
Desafio desafios[NUM_DESAFIOS];
Jogador jogador;

/* ══════════════════════════════════════════════════════════
 *  RNG — xorshift32 com semente de /dev/urandom
 * ══════════════════════════════════════════════════════════ */

static unsigned int g_rng = 0;

static void rng_init(void) {
    FILE *f = fopen("/dev/urandom", "rb");
    if (f) {
        if (fread(&g_rng, sizeof(g_rng), 1, f) != 1) g_rng = 0;
        fclose(f);
    } else {
        g_rng = (unsigned int)time(NULL) ^ ((unsigned int)getpid() << 16);
    }
    if (g_rng == 0) g_rng = 0xDEADBEEFu;
}

static unsigned int rng_next(void) {
    unsigned int x = g_rng;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    g_rng = x;
    return x;
}

static int rng_range(int n) {
    if (n <= 0) return 0;
    return (int)(rng_next() % (unsigned int)n);
}

/* ══════════════════════════════════════════════════════════
 *  Avaliador recursivo de expressões lógicas
 *  Gramática (da menor para a maior precedência):
 *    expr   := impl (BICONDICIONAL impl)*
 *    impl   := or  (IMPLICA or)*
 *    or     := and (OR and)*
 *    and    := not (AND not)*
 *    not    := NOT not | prim
 *    prim   := '(' expr ')' | 'V' | 'F' | var
 * ══════════════════════════════════════════════════════════ */

typedef struct {
    const char *s;
    int         pos;
    const char *var_names;   /* "PQ" / "PQR"      */
    const int  *var_vals;    /* valores paralelos */
    int         n_vars;
} Parser;

static void skip_ws(Parser *p) {
    while (p->s[p->pos] == ' ' || p->s[p->pos] == '\t') p->pos++;
}

/* Casa palavra-chave isolada (seguida de espaço, '(', ')' ou fim). */
static int match_kw(Parser *p, const char *kw) {
    int len;
    char next;
    skip_ws(p);
    len = (int)strlen(kw);
    if (strncmp(p->s + p->pos, kw, (size_t)len) != 0) return 0;
    next = p->s[p->pos + len];
    if (next != '\0' && next != ' ' && next != '\t' &&
        next != '(' && next != ')') return 0;
    p->pos += len;
    return 1;
}

static int eval_expr(Parser *p);

static int eval_prim(Parser *p) {
    char c;
    int  i;
    skip_ws(p);
    c = p->s[p->pos];
    if (c == '(') {
        int v;
        p->pos++;
        v = eval_expr(p);
        skip_ws(p);
        if (p->s[p->pos] == ')') p->pos++;
        return v;
    }
    if (c == 'V' || c == 'v') { p->pos++; return 1; }
    if (c == 'F' || c == 'f') { p->pos++; return 0; }
    for (i = 0; i < p->n_vars; i++) {
        if (c == p->var_names[i]) {
            p->pos++;
            return p->var_vals[i];
        }
    }
    return 0;   /* expressão mal formada — fallback F */
}

static int eval_not(Parser *p) {
    if (match_kw(p, "NOT")) return !eval_not(p);
    return eval_prim(p);
}

static int eval_and(Parser *p) {
    int v = eval_not(p);
    while (match_kw(p, "AND")) {
        int r = eval_not(p);
        v = v && r;
    }
    return v;
}

static int eval_or(Parser *p) {
    int v = eval_and(p);
    while (match_kw(p, "OR")) {
        int r = eval_and(p);
        v = v || r;
    }
    return v;
}

static int eval_impl(Parser *p) {
    int v = eval_or(p);
    while (match_kw(p, "IMPLICA")) {
        int r = eval_or(p);
        v = (!v || r);
    }
    return v;
}

static int eval_expr(Parser *p) {
    int v = eval_impl(p);
    while (match_kw(p, "BICONDICIONAL")) {
        int r = eval_impl(p);
        v = (v == r);
    }
    return v;
}

/* Avalia a expressão com um vetor de valores (0/1) para as variáveis. */
static int avaliar_expressao(const char *expr, const char *vars,
                             const int *vals, int n) {
    Parser p;
    p.s         = expr;
    p.pos       = 0;
    p.var_names = vars;
    p.var_vals  = vals;
    p.n_vars    = n;
    return eval_expr(&p);
}

/* ══════════════════════════════════════════════════════════
 *  Geração de um desafio (uma linha da tabela-verdade)
 * ══════════════════════════════════════════════════════════ */

static void gerar_desafio_da_sessao(int idx) {
    const DesafioTemplate *t = &templates[idx];
    int n, total, linha, i, off, v;
    int vals[3];
    char vars_buf[DESAFIO_VARS_MAX];

    n     = (int)strlen(t->vars);
    total = 1 << n;
    linha = rng_range(total);                 /* linha sorteada        */

    /* Decodifica bits → valores das variáveis.
     * Ex.: n=2, linha=2 (binário 10) → P=1, Q=0. */
    for (i = 0; i < n; i++)
        vals[i] = (linha >> (n - 1 - i)) & 1;

    /* Avalia a expressão exatamente nesta linha. */
    v = avaliar_expressao(t->expressao, t->vars, vals, n);

    /* "P = V, Q = F" */
    off = 0;
    for (i = 0; i < n; i++) {
        off += snprintf(vars_buf + off, sizeof(vars_buf) - (size_t)off,
                        "%c = %c%s",
                        t->vars[i], vals[i] ? 'V' : 'F',
                        (i < n - 1) ? ", " : "");
    }

    /* Enunciado dinâmico. */
    snprintf(desafios[idx].enunciado, sizeof(desafios[idx].enunciado),
             "Dado %s,\n    qual o valor de: %s?",
             vars_buf, t->expressao);

    desafios[idx].resposta_correta = v ? 'V' : 'F';

    /* Explicação contextualizada com a linha aplicada. */
    snprintf(desafios[idx].explicacao, sizeof(desafios[idx].explicacao),
             "%s\n    Linha aplicada: %s\n    Resultado: %c",
             t->regra, vars_buf, v ? 'V' : 'F');

    desafios[idx].nivel        = t->nivel;
    desafios[idx].linha_tabela = linha;
    desafios[idx].total_linhas = total;
    snprintf(desafios[idx].variaveis, sizeof(desafios[idx].variaveis),
             "%s", vars_buf);
    snprintf(desafios[idx].expressao, sizeof(desafios[idx].expressao),
             "%s", t->expressao);
}

/* ══════════════════════════════════════════════════════════
 *  Ciclo de vida
 * ══════════════════════════════════════════════════════════ */

void inicializar_jogo(void) {
    int i;
    rng_init();                               /* HU10 — semente nova por sessão */
    for (i = 0; i < NUM_DESAFIOS; i++)
        gerar_desafio_da_sessao(i);           /* todas as linhas pré-computadas */
    jogador.pontuacao = 0;
    jogador.acertos   = 0;
}

static void rodar_desafio(int indice) {
    char resposta;
    int  acertou;

    tela_desafio(indice);
    resposta = ler_resposta();

    if (resposta == 'H' || resposta == 'h') {
        tela_painel_logi(indice);
        tela_desafio(indice);
        resposta = ler_resposta();
    }

    acertou = (toupper((unsigned char)resposta) ==
               desafios[indice].resposta_correta);
    if (acertou) {
        jogador.acertos++;
        jogador.pontuacao += desafios[indice].nivel * 10;
    }

    tela_feedback(acertou, indice);
}

void executar_desafio(int indice) {
    rodar_desafio(indice);
}

void executar_nivel_avancado(void) {
    int i;
    tela_intro_avancado();
    inicializar_jogo();
    for (i = NUM_DESAFIOS_BASE; i < NUM_DESAFIOS; i++)
        rodar_desafio(i);
    finalizar_jogo(NUM_DESAFIOS_AVANC);
}

void finalizar_jogo(int total_rodada) {
    tela_resultado_final(total_rodada);
}