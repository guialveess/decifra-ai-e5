# Tabelas-Verdade dos Desafios

Documento de referencia para a implementacao do banco de questoes do Decifra.IA.

Cada desafio contem a formula logica, a tabela-verdade completa com subformulas intermediarias,
e o struct em C pronto para uso pelo Ademir na implementacao.

As variaveis usam notacao binaria: 0 = Falso, 1 = Verdadeiro.

---

## Como ler este documento

Cada desafio segue esta estrutura:

- **ID**: identificador unico no formato `U{unidade}_N{nivel}_Q{questao}`
- **Formula**: expressao logica do desafio
- **Tabela**: todas as combinacoes de entrada com os resultados intermediarios e o resultado final
- **Struct C**: codigo pronto para copiar na implementacao

---

## Unidade 1 — Nivel 3 (Intermediario)

### U1_N3_Q01

**Formula:** `(P AND Q) OR (NOT R)`

| P | Q | R | P AND Q | NOT R | Resultado |
|---|---|---|---------|-------|-----------|
| 0 | 0 | 0 |    0    |   1   |     1     |
| 0 | 0 | 1 |    0    |   0   |     0     |
| 0 | 1 | 0 |    0    |   1   |     1     |
| 0 | 1 | 1 |    0    |   0   |     0     |
| 1 | 0 | 0 |    0    |   1   |     1     |
| 1 | 0 | 1 |    0    |   0   |     0     |
| 1 | 1 | 0 |    1    |   1   |     1     |
| 1 | 1 | 1 |    1    |   0   |     1     |

**Struct C:**

```c
typedef struct {
    const char *formula;
    int respostas[8];
} Desafio;

Desafio u1_n3_q01 = {
    .formula  = "(P AND Q) OR (NOT R)",
    .respostas = {1, 0, 1, 0, 1, 0, 1, 1}
};
```

As respostas seguem a ordem das linhas da tabela, de cima para baixo.

---

> Demais desafios serao adicionados conforme o conteudo for dado em aula (LMC).
