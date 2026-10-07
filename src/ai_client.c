#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "ai_client.h"

#define RESP_CAP_INTERNO 512

static int append(char *dst, int cap, int used, const char *s) {
    int n;
    if (used >= cap - 1) return used;
    n = snprintf(dst + used, (size_t)(cap - used), "%s", s);
    if (n < 0)            return used;
    if (used + n >= cap)  return cap - 1;
    return used + n;
}

static int contem(const char *s, const char *needle) {
    return s && needle && strstr(s, needle) != NULL;
}

static int not_antes_de_paren(const char *s) {
    const char *p = s;
    while ((p = strstr(p, "NOT")) != NULL) {
        const char *q = p + 3;
        while (*q == ' ' || *q == '\t') q++;
        if (*q == '(') return 1;
        p = q;
    }
    return 0;
}

static int tem_algum_operador(const char *s) {
    return contem(s, "AND") || contem(s, "OR")  || contem(s, "NOT") ||
           contem(s, "IMPLICA") || contem(s, "BICONDICIONAL");
}

int ai_consultar(const char *pergunta, char *resposta, int max_len) {
    int used = 0;
    int tem_paren, tem_and, tem_or, tem_not, tem_impl, tem_bicon;

    if (!pergunta || !resposta || max_len < 64)
        return -1;

    resposta[0] = '\0';

    tem_paren = contem(pergunta, "(") || contem(pergunta, ")");
    tem_and   = contem(pergunta, "AND");
    tem_or    = contem(pergunta, "OR");
    tem_not   = contem(pergunta, "NOT");
    tem_impl  = contem(pergunta, "IMPLICA");
    tem_bicon = contem(pergunta, "BICONDICIONAL");

    used = append(resposta, max_len, used, "LOGI: ");

    if (tem_paren) {
        used = append(resposta, max_len, used,
                      "ha parenteses, comece por eles. ");
        if (not_antes_de_paren(pergunta))
            used = append(resposta, max_len, used,
                          "O NOT fora do parenteses so afeta o resultado interno. ");
        else
            used = append(resposta, max_len, used,
                          "Resolva de dentro para fora. ");
    } else if (tem_not) {
        used = append(resposta, max_len, used,
                      "NOT inverte o valor logico. ");
    }

    if (tem_and)
        used = append(resposta, max_len, used,
                      "AND = V so quando ambos os lados sao V. ");
    if (tem_or)
        used = append(resposta, max_len, used,
                      "OR = V quando ao menos um lado e V. ");
    if (tem_impl)
        used = append(resposta, max_len, used,
                      "IMPLICA = F apenas quando P=V e Q=F. ");
    if (tem_bicon)
        used = append(resposta, max_len, used,
                      "BICONDICIONAL = V quando os dois lados sao iguais. ");

    if (!tem_paren && !tem_not && !tem_algum_operador(pergunta))
        used = append(resposta, max_len, used,
                      "Nao identifiquei operadores; pense em cada conectivo separadamente. ");

    used = append(resposta, max_len, used,
                  "Ordem: NOT > AND > OR > IMPLICA > BICONDICIONAL.");

    return (used > 0) ? 0 : -1;
}