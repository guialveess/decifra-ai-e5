# Documento de Requisitos — LOGI Decifra.IA (U1)

**Projeto:** PI2-E5 · Integrador 2  
**Unidade:** 01 — FDS U1  
**Tickets relacionados:** PI2-98 (requisitos), PI2-99 (plano de testes e arquitetura)  
**Status:** Consolidado, revisado e aprovado pela equipe.

---

## 1. Visão Geral

O **LOGI Decifra.IA** é um jogo de terminal, escrito em **C11**, que ensina
lógica proposicional por meio de desafios interativos. O jogador responde
questões de valoração lógica (V/F), recebe feedback explicativo e pode
solicitar ajuda a uma IA tutora (**LOGI**) via tecla `[H]`.

O sistema cobre:

- Operadores lógicos: `AND`, `OR`, `NOT`, `IMPLICA`, `BICONDICIONAL`.
- Níveis progressivos de dificuldade (1 a 4).
- Precedência lógica: `NOT > AND > OR > IMPLICA > BICONDICIONAL`.
- Uso de parênteses para alterar escopo.
- Cobertura de todas as combinações da tabela-verdade (2ⁿ).
- Integração com IA tutora via bridge Python + modelo local.

---

## 2. Requisitos Funcionais (RF)

| ID | Descrição | Critério de Aceitação | Status |
|----|-----------|----------------------|--------|
| RF01 | Resolver proposições compostas com `AND` | Exibe duas variáveis (P e Q) com `AND`; exige resposta V/F; aciona `[H]` para ajuda | ✅ Implementado |
| RF02 | Resolver problemas com um único conectivo binário por questão no Nível 1 | Valida contra a tabela-verdade do operador; avança sequencialmente (1 de 5, 2 de 5...) | ✅ Implementado |
| RF03 | Resolver fórmulas com `IMPLICA` e `BICONDICIONAL` no Nível 3 | Exibe cabeçalho `Desafio X de 5 · Nivel 3`; valida com tabela-verdade específica | ✅ Implementado |
| RF04 | Receber feedback e explicação lógica | Exibe `CORRETO` (verde) ou `INCORRETO` (vermelho), seguido de `Explicação:` e pontuação `nível × 10` | ✅ Implementado |
| RF05 | Visualizar regras de progressão de nível | Indicador `Desafio X de N` no cabeçalho; tela de `Resultado Final` com acertos e pontos | ✅ Implementado |
| RF06 | Ler operadores em notação padronizada | Todos os operadores em texto plano (`AND`, `OR`, `NOT`, `IMPLICA`, `BICONDICIONAL`); tela `Como Jogar` lista-os | ✅ Implementado |
| RF07 | Respeitar ordem de precedência lógica nos enunciados | Banco hardcoded respeita `NOT > AND > OR > IMPLICA > BICONDICIONAL`; validação por consulta direta ao gabarito | ✅ Implementado |
| RF08 | Acessar ajuda contextual via tutora LOGI | Tecla `[H]` abre painel com cabeçalho `LOGI — Tutora de Lógica e Segurança em IA`, número do desafio e dica contextual; fallback estático se IA indisponível | ✅ Implementado |
| RF09 | Visualizar exemplos didáticos de escopo | Tela comparativa com dois cenários (mesmos operandos, parênteses diferentes) e diferença nos resultados | ✅ Implementado |
| RF10 | Cobertura de combinações da tabela-verdade (2ⁿ) | Sorteio de linha por sessão; cálculo do gabarito para a linha sorteada; nenhuma combinação fica sem resposta válida | ✅ Implementado |

---

## 3. Requisitos Não Funcionais (RNF)

| ID | Descrição | Status |
|----|-----------|--------|
| RNF01 | Código em **C11** padrão, compilável com `gcc -Wall -Wextra` sem warnings | ✅ |
| RNF02 | Executa em **terminal Linux** (modo raw via termios), sem dependências externas de UI | ✅ |
| RNF03 | Interface com **cores ANSI 24-bit** e layout responsivo à altura do terminal | ✅ |
| RNF04 | Núcleo lógico **puro e determinístico** (parser sem I/O), testável por asserts | ✅ |
| RNF05 | Integração com IA via **processo separado** (`popen`), com timeout de 75 ticks e fallback estático | ✅ |
| RNF06 | **Sem alocação dinâmica** no núcleo do jogo (buffers estáticos e vetores fixos) | ✅ |
| RNF07 | **Portabilidade de builds**: `Makefile` com alvo `all`, `run`, `clean` e rastreio de dependências (`-MMD -MP`) | ⚠️ Recomendado |
| RNF08 | **Documentação versionada** em `/docs` (requisitos, arquitetura, plano de testes, diagramas de atividades) | ✅ |

---

## 4. Histórias de Usuário Consolidadas

### HU1 — Proposições Compostas com o operador AND

**Cartão:** Como jogador, quero resolver proposições compostas simples utilizando o conectivo lógico AND (E), para consolidar os conceitos de valoração lógica com operadores binários.

**Conversa:** A equipe deve garantir que este desafio apresente atribuições diretas de valores (Verdadeiro/Falso) para duas variáveis (como P e Q) e exija a aplicação da tabela-verdade do operador AND.

**Confirmação:**
- Exibir questões contendo duas variáveis (ex.: `P = V e Q = F`) e o operador `AND`.
- Exigir a digitação direta da valoração correta (`V` ou `F`) para contabilizar o acerto.
- Oferecer a opção de ajuda `[H] Pedir ajuda ao LOGI`.
- Não apresentar outros conectivos (`OR`, `NOT`, etc.) no primeiro desafio.

**Cenário BDD:**
> **Dado** que o jogador está no Desafio 1 do Nível 1 do jogo,  
> **Quando** uma questão contendo duas variáveis e o operador `AND` for exibida,  
> **Então** o sistema deve exigir a digitação direta da valoração correta (`V` ou `F`) para contabilizar o acerto.

**Implementação:** `game.c` (template `P AND Q` do nível 1), `ui.c::render_desafio`.

---

### HU2 — Conectivos Binários no Nível 1

**Cartão:** Como jogador, quero enfrentar problemas com um único conectivo binário, para praticar as operações de conjunção e disjunção.

**Conversa:** Definir a inclusão de exercícios baseados em operadores como `AND` e `OR` simples, avaliando o resultado final a partir dos valores dados no enunciado.

**Confirmação:**
- Apresentar fórmulas contendo exatamente um operador binário por questão.
- Validar a resposta comparando com a tabela-verdade do operador correspondente.
- Avançar sequencialmente pelos desafios (`1 de 5`, `2 de 5`, ...) dentro do Nível 1.

**Cenário BDD:**
> **Dado** que o jogador está no Nível 1 do jogo,  
> **Quando** uma questão contendo um único operador binário (`AND` ou `OR`) for exibida,  
> **Então** o sistema deve validar a resposta do jogador comparando-a com a tabela-verdade do operador correspondente e avançar sequencialmente pelos desafios.

**Implementação:** `game.c` (templates de nível 1), `ui.c::render_desafio` (header com `Desafio X de N`).

---

### HU3 — Fórmulas com Implicação e Bicondicional no Nível 3

**Cartão:** Como jogador, quero praticar proposições compostas com os operadores de implicação e bicondicional, para exercitar a avaliação de escopos lógicos mais avançados.

**Conversa:** Combinar os conectivos de implicação (`IMPLICA`) e bicondicional (`BICONDICIONAL`) em fórmulas simples.

**Confirmação:**
- Exibir proposições com duas variáveis e os operadores `IMPLICA` ou `BICONDICIONAL` (ex.: `P IMPLICA Q`).
- Garantir que a verificação de acerto respeite a tabela-verdade específica de cada operador.
- Apresentar questões no formato `Desafio X de 5 | Nivel 3`.

**Cenário BDD:**
> **Dado** que o jogador está no Nível 3 do jogo,  
> **Quando** uma questão contendo os operadores `IMPLICA` ou `BICONDICIONAL` for exibida,  
> **Então** o sistema deve verificar a resposta do jogador com base na tabela-verdade específica do operador e exibir o cabeçalho `Desafio X de 5 | Nivel 3`.

**Implementação:** `game.c` (templates nível 3), `ui.c::render_desafio` (formatação do header).

---

### HU4 — Feedback e Explicações Lógicas

**Cartão:** Como jogador, quero receber feedback claro e explicações sobre o resultado dos meus desafios, para entender o raciocínio lógico por trás da resposta correta.

**Conversa:** Estruturar um sistema de feedback que não apenas informe se o jogador acertou ou errou, mas que explique textualmente a regra lógica aplicada.

**Confirmação:**
- Exibir mensagens de `CORRETO` ou `INCORRETO` destacadas em cores (verde/vermelho).
- Apresentar uma seção `Explicação:` detalhando o porquê do resultado.
- Exibir a pontuação ganha (`nível × 10` pontos) em caso de acerto.

**Cenário BDD:**
> **Dado** que o jogador submeteu uma resposta a um desafio,  
> **Quando** o sistema processa a validação da resposta,  
> **Então** ele deve exibir a mensagem `CORRETO` em verde ou `INCORRETO` em vermelho, seguida de uma seção `Explicação:` com o raciocínio lógico e a pontuação calculada como `nível × 10` em caso de acerto.

**Implementação:** `ui.c::render_feedback`, `game.c::rodar_desafio`.

---

### HU5 — Regras de Progressão de Nível

**Cartão:** Como jogador, quero visualizar a quantidade de questões necessárias e o critério de avanço, para saber exatamente quando passarei para o próximo nível.

**Conversa:** Estabelecer a quantidade exata de acertos exigidos em cada fase da Unidade 1 antes de liberar o nível subsequente.

**Confirmação:**
- Exibir indicador de progresso na interface do terminal (ex.: `Desafio 2 de 5`).
- Exibir tela de `Resultado Final` com resumo de acertos e pontos ao término dos desafios.

**Cenário BDD:**
> **Dado** que o jogador concluiu os 5 desafios de um nível,  
> **Quando** a tela de `Resultado Final` for exibida com o resumo de acertos e pontos,  
> **Então** o sistema deve liberar automaticamente o próximo nível se a meta de acertos for atingida, ou reiniciar o nível atual caso contrário.

**Implementação:** `ui.c::render_desafio` (indicador), `ui.c::render_resultado` (resumo), `game.c::finalizar_jogo`.

---

### HU6 — Notação Padronizada dos Operadores

**Cartão:** Como jogador, quero visualizar os operadores lógicos com uma simbologia clara no terminal, para ler as expressões sem ambiguidades.

**Conversa:** Alinhar o uso exclusivo de símbolos de texto plano compatíveis com terminais sem suporte a caracteres especiais.

**Confirmação:**
- Manter a mesma representação de símbolos em todos os níveis da Unidade 1.
- Rejeitar caracteres fora do padrão estabelecido durante a exibição.
- Apresentar tela de `Como Jogar` listando todos os operadores suportados.

**Cenário BDD:**
> **Dado** que o jogador iniciou um desafio,  
> **Quando** o sistema exibe a questão no terminal,  
> **Então** os operadores lógicos devem ser representados exclusivamente em texto plano (`AND`, `OR`, `NOT`, `IMPLICA`, `BICONDICIONAL`), rejeitando caracteres especiais fora do padrão.

**Implementação:** `ui.c::render_como_jogar`, `game.c` (templates em texto plano).

---

### HU7 — Precedência Lógica nos Enunciados (Hardcoded)

**Cartão:** Como jogador, quero receber enunciados que respeitem a ordem de precedência lógica, para ter um julgamento matematicamente correto das expressões.

**Conversa:** Garantir que as questões hardcoded sigam rigorosamente a hierarquia `NOT > AND > OR > IMPLICA > BICONDICIONAL`. Como não há parser, a validação é feita por consulta direta à resposta pré-definida.

**Confirmação:**
- As expressões apresentadas ao jogador respeitam a precedência `NOT > AND > OR > IMPLICA > BICONDICIONAL`.

**Cenário BDD:**
> **Dado** que o banco de questões pré-definidas (hardcoded) é carregado,  
> **Quando** uma questão é sorteada e exibida ao jogador,  
> **Então** o sistema deve validar a resposta consultando diretamente a resposta pré-definida no banco, sem utilizar um parser, garantindo que a precedência lógica foi respeitada na formulação manual da questão.

**Implementação:** `game.c` (banco de templates com expressões já escritas respeitando precedência).

---

### HU8 — Sistema de Ajuda Contextual (Tutora LOGI)

**Cartão:** Como jogador, quero acessar uma explicação rápida sobre o operador lógico do desafio atual, para relembrar a regra antes de responder.

**Conversa:** Implementar tela de ajuda acionada pela tecla `[H]` durante qualquer desafio, exibindo o nome da tutora (LOGI), o número do desafio atual e uma explicação contextual. A dica é gerada pelo modelo local LOGI (Qwen2.5-1.5B-Instruct + LoRA). Em caso de indisponibilidade, o sistema utiliza uma dica estática de fallback.

**Confirmação:**
- Exibir cabeçalho `LOGI — Tutora de Lógica e Segurança em IA`.
- Identificar o desafio atual (ex.: `Dica para o desafio 1:`).
- Gerar a explicação via modelo local LOGI.
- Apresentar a resposta em linguagem clara.
- Em caso de indisponibilidade, apresentar dica estática de fallback.
- A dica **não** revela a resposta correta.
- Retornar ao desafio após o jogador pressionar `ENTER`.

**Cenário BDD:**
> **Dado** que o jogador está em um desafio e pressiona a tecla `[H]`,  
> **Quando** o sistema identifica o desafio atual e o operador correspondente,  
> **Então** ele deve exibir a dica contextual gerada pelo modelo LOGI (Qwen2.5-1.5B + LoRA) ou, em caso de indisponibilidade do servidor, uma dica estática de fallback, sem revelar a resposta correta, e retornar ao desafio após o pressionamento de `ENTER`.

**Implementação:** `ai_client.c` (cliente), `scripts/ai_client.py` (bridge), `ui.c::render_logi`.

---

### HU9 — Exemplos Didáticos de Escopo

**Cartão:** Como jogador, quero visualizar exemplos comparativos do uso de parênteses, para entender como a alteração do agrupamento afeta o resultado final.

**Conversa:** Criar uma seção ou tela explicativa mostrando a mesma expressão com e sem parênteses para demonstrar a mudança de sentido lógico.

**Confirmação:**
- Exibir dois cenários com os mesmos operandos e conectivos, alterando apenas a posição dos parênteses.
- Demonstrar a diferença entre os resultados obtidos em cada caso.

**Cenário BDD:**
> **Dado** que o jogador acessa a tela de exemplos didáticos de escopo,  
> **Quando** o sistema carrega a explicação comparativa de agrupamento,  
> **Então** ele deve apresentar dois cenários contendo os mesmos operandos e conectivos alterando a posição dos parênteses e demonstrar a diferença nos resultados lógicos obtidos.

**Implementação:** `ui.c::tela_exemplos_escopo`, `ui.c::render_exemplos_escopo` (3 exemplos comparativos).

---

### HU10 — Cobertura de Combinações da Tabela-Verdade

**Cartão:** Como jogador, quero ser testado em diferentes combinações de valores de entrada (2ⁿ), para garantir um aprendizado abrangente do banco de questões.

**Conversa:** Assegurar que os cenários apresentados contemplem todas as linhas possíveis da tabela-verdade associada a cada desafio.

**Confirmação:**
- Validar a resposta do usuário contra a linha exata da tabela-verdade sorteada para a partida.
- Garantir que nenhuma combinação de variáveis fique sem resposta válida cadastrada.

**Cenário BDD:**
> **Dado** uma partida em execução com um desafio sorteado,  
> **Quando** a resposta do jogador for submetida,  
> **Então** o sistema deve validar a entrada contra a linha exata da tabela-verdade sorteada, garantindo que nenhuma combinação de variáveis fique sem resposta válida cadastrada.

**Implementação:** `game.c::inicializar_jogo` (enumeração 2ⁿ), `game.c::gerar_desafio_da_sessao` (sorteio de linha), `game.c::avaliar_expressao` (parser recursivo). Header exibe `Linha X/Y`.

---

## 5. Rastreabilidade — HU ⇄ Código

| HU | Módulo principal | Função(ões) chave |
|----|------------------|-------------------|
| HU1 | `game.c`, `ui.c` | `rodar_desafio`, `render_desafio` |
| HU2 | `game.c` | Templates nível 1 |
| HU3 | `game.c` | Templates nível 3 |
| HU4 | `ui.c`, `game.c` | `render_feedback`, `rodar_desafio` |
| HU5 | `ui.c`, `game.c` | `render_resultado`, `finalizar_jogo` |
| HU6 | `ui.c` | `render_como_jogar`, `render_desafio` |
| HU7 | `game.c` | Templates (expressões pré-formuladas) |
| HU8 | `ai_client.c`, `ui.c` | `ai_consultar`, `render_logi`, `tela_painel_logi` |
| HU9 | `ui.c` | `tela_exemplos_escopo`, `render_exemplos_escopo` |
| HU10 | `game.c` | `inicializar_jogo`, `gerar_desafio_da_sessao`, `avaliar_expressao` |

---

## 6. Artefatos complementares em `/docs`

| Documento | Descrição |
|-----------|-----------|
| `docs/arquitetura.md` / `arquitetura.svg` | Diagrama de arquitetura (C + Python bridge + LOGI) |
| `docs/plano_de_testes.md` | Plano de testes unitários por módulo |
| `docs/plano_de_testes_diagrama.md` / `.svg` | Diagrama do fluxo de execução dos testes |
| `docs/atividades.svg` | Diagrama geral de atividades (menu + subprocesso) |
| `docs/hu10_atividades.svg` | Diagrama de atividades específico da HU10 |
| `docs/requisitos.md` | Este documento |

---

## 7. Aprovação

Documento consolidado, revisado e aprovado pela equipe da **Unidade 01**.
Próxima revisão prevista para a **Unidade 02**, com foco em adaptatividade do banco de questões e evolução do bridge Python–LOGI.