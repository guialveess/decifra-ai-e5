# Relatório Técnico — LOGI Decifra.IA

**Projeto Integrador 2 — Squad 05 | CESAR School 2026.2**

---

## 1. Visão Geral

O LOGI Decifra.IA é um jogo educacional de terminal implementado em linguagem C, integrado a um modelo de linguagem fine-tunado (LOGI) para geração de dicas contextuais. O sistema é composto por um núcleo imperativo em C, uma bridge de integração com Python e um servidor de inferência local baseado em LoRA sobre o modelo Qwen2.5-3B-Instruct.

---

## 2. Arquitetura do Sistema

### 2.1 Diagrama de Componentes

```
┌─────────────────────────────────────────────────────────┐
│                     Jogador (terminal)                  │
└───────────────────────────┬─────────────────────────────┘
                            │ entrada: V / F / H
                            ▼
┌─────────────────────────────────────────────────────────┐
│                    Núcleo C (src/)                      │
│                                                         │
│  main.c ──► game.c ──► ui.c                            │
│               │          │                              │
│               │          └──► input.c                   │
│               │                                         │
│               └──► ai_client.c  ◄── [tecla H]          │
└───────────────────────────┬─────────────────────────────┘
                            │ popen() com argumento do enunciado
                            ▼
┌─────────────────────────────────────────────────────────┐
│               Bridge Python (scripts/)                  │
│                                                         │
│  ai_client.py ──► HTTP POST /ask ──► local_server.py   │
└───────────────────────────┬─────────────────────────────┘
                            │ inferência local
                            ▼
┌─────────────────────────────────────────────────────────┐
│              Modelo LOGI (HuggingFace)                  │
│                                                         │
│  Base: Qwen/Qwen2.5-3B-Instruct                        │
│  Adapter: guiiwfz/logi (LoRA)                          │
│  Device: MPS (Apple Silicon) / CPU (fallback)          │
└─────────────────────────────────────────────────────────┘
```

---

## 3. Módulos C

### 3.1 `src/main.c` — Ponto de entrada e loop principal

Responsável por inicializar o ambiente de terminal, registrar handlers de sinal e executar o loop principal do jogo.

**Responsabilidades:**
- Ativa o *alternate screen buffer* (`\033[?1049h`) para isolar a UI do histórico do terminal
- Configura *raw mode* via `tcsetattr`, desligando `ECHO` e `ICANON` para leitura caractere a caractere sem aguardar Enter
- Registra `restaurar_terminal` para `SIGINT` e `SIGTERM`, garantindo que o terminal seja restaurado mesmo em interrupções
- Executa o `while(1)` do menu principal, roteando para Jogar, Como Jogar ou Sair

**Fluxo principal:**
```
configurar_terminal()
while(1):
    tela_menu() → ler_opcao()
    opcao == 1 → tela_loading → ler_nome → inicializar_jogo
                 → for(desafios) executar_desafio → finalizar_jogo
    opcao == 2 → tela_como_jogar
    opcao == 3 → tela_saida → break
restaurar_termios()
```

---

### 3.2 `src/game.c` — Lógica do jogo e banco de desafios

Contém o banco de desafios (hardcoded), o estado global do jogador e a lógica de execução de cada desafio.

**Estrutura `Desafio`** (definida em `game.h`):
```c
typedef struct {
    char enunciado[256];      // texto exibido ao jogador
    char resposta_correta;    // 'V' ou 'F'
    char explicacao[256];     // texto exibido no feedback
    int  nivel;               // 1, 2 ou 3 — define os pontos ganhos (nivel × 10)
} Desafio;
```

**Banco de desafios (Unidade 1):**

| # | Operador | Nível | Resposta |
|---|---|:---:|:---:|
| 1 | `P AND Q` (P=V, Q=F) | 1 | F |
| 2 | `P OR Q` (P=V, Q=V) | 1 | V |
| 3 | `NOT P` (P=F) | 2 | V |
| 4 | `P IMPLICA Q` (P=V, Q=F) | 3 | F |
| 5 | `P BICONDICIONAL Q` (P=V, Q=V) | 3 | V |

**Decisão técnica:** as questões são estáticas na Unidade 1 para garantir cobertura completa dos 5 operadores e controle total sobre a progressão de dificuldade (níveis 1→3). A geração dinâmica via IA está planejada para a Unidade 2.

**Pontuação:** `pontos = nivel × 10`, resultando em +10 (nível 1), +20 (nível 2) ou +30 (nível 3) por acerto.

---

### 3.3 `src/ui.c` — Interface do terminal

Implementa todas as telas do jogo usando sequências de escape ANSI diretamente, sem dependências externas de biblioteca de UI.

**Técnicas implementadas:**

| Técnica | Implementação |
|---|---|
| Alternate screen buffer | `\033[?1049h` / `\033[?1049l` |
| Posicionamento absoluto de cursor | `\033[row;colH` via `ir(r, c)` |
| True-color RGB | `\033[38;2;R;G;Bm` via `set_cor(r, g, b)` |
| Fade-in simultâneo | 7 passos de brilho (5%→100%) com `g_b` como multiplicador |
| Spinner braille | Sequência UTF-8: ⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏ → ✔ |
| Cursor oculto | `\033[?25l` / `\033[?25h` |

**Paleta de cores:** azul derivado da identidade LOGI — `rgb(100, 140, 255)` para elementos primários, `rgb(200, 210, 230)` para texto secundário, `rgb(60, 90, 180)` para divisores.

**Telas implementadas:**
- `tela_menu` — menu principal com opções 1/2/3
- `tela_como_jogar` — instruções e tabela de operadores
- `tela_loading` — animação de inicialização com spinner braille e caixa de status
- `tela_nome` — captura do nome do jogador
- `tela_desafio` — exibe enunciado, nível, pontuação e opções V/F/H
- `tela_painel_logi` — dica da tutora LOGI (IA ou fallback estático)
- `tela_feedback` — exibe CORRETO/INCORRETO em verde/vermelho, explicação e pontos
- `tela_resultado_final` — resumo de acertos, pontuação e classificação
- `tela_saida` — animação de despedida

---

### 3.4 `src/input.c` — Leitura de entrada

Abstrai a leitura de caracteres em raw mode, isolando o tratamento de sequências de escape e garantindo que apenas entradas válidas cheguem à lógica do jogo.

**Funções:**

| Função | Aceita | Descarta |
|---|---|---|
| `ler_opcao()` | `1`–`9` | escapes, letras, símbolos |
| `ler_resposta()` | `V`, `F`, `H` (case-insensitive) | todo o resto |
| `ler_nome()` | qualquer caractere + backspace | escapes de terminal |

**Decisão técnica:** `consumir_escape()` descarta sequências CSI/SS3 geradas por teclas especiais (setas, PgUp, F1–F12), impedindo que lixo de terminal quebre o loop de leitura em diferentes emuladores.

---

### 3.5 `src/ai_client.c` — Bridge C → Python

Responsável por invocar o script Python de inferência a partir do código C, sem dependências de bibliotecas de rede ou HTTP em C.

**Mecanismo:**
```c
// monta o comando shell
snprintf(cmd, sizeof(cmd), "python3 scripts/ai_client.py '%s'", escaped);

// abre pipe não-bloqueante para leitura da resposta
FILE *f = popen(cmd, "r");
fcntl(fd, F_SETFL, O_NONBLOCK);

// lê com timeout de 75 ticks × 80ms = 6 segundos
while (total < max_len - 1 && ticks < TIMEOUT_TICKS) { ... }
```

**Tratamento de falhas:**
- Se `scripts/ai_client.py` não existir → retorna -1 (sem crash)
- Se o pipe não retornar dados no timeout → retorna -1
- Em qualquer falha → `tela_painel_logi` usa dicas estáticas como fallback

**Decisão técnica:** `popen()` foi escolhido por ser portável e não exigir bibliotecas externas em C. O timeout de 6 segundos garante que a UX não trave caso o servidor Python demore a responder.

---

## 4. Bridge Python e Modelo LOGI

### 4.1 `scripts/ai_client.py` — Cliente HTTP

Script standalone invocado pelo `ai_client.c` via `popen()`. Recebe o enunciado do desafio como argumento posicional e envia uma requisição HTTP POST para o servidor de inferência local.

```
python3 scripts/ai_client.py '<enunciado>'
        │
        └─► POST http://127.0.0.1:8787/ask
            body: {"pergunta": "<enunciado>"}
            │
            └─► stdout: <dica gerada pelo modelo>
```

### 4.2 `scripts/local_server.py` — Servidor de inferência

Servidor HTTP local (porta 8787) que carrega o modelo LOGI em formato GGUF via `llama-cpp-python` e responde requisições de geração de texto.

**Carregamento do modelo:**
```python
llm = Llama.from_pretrained(
    repo_id="guiiwfz/logi-1.5b-gguf",
    filename="*Q4_K_M.gguf",   # ~940 MB; use *Q6_K.gguf para mais qualidade
    n_ctx=1024,
    n_gpu_layers=-1,            # usa Metal/CUDA se disponível; 0 para CPU
)
```

O modelo é baixado automaticamente do HuggingFace na primeira execução e cacheado localmente.

**System prompt (mesmo utilizado no treinamento):**
```
Voce e LOGI, tutor do jogo educativo Decifra.IA.
Seu escopo inclui logica proposicional, alfabetizacao em IA e seguranca digital
relacionada ao uso de IA. Responda em portugues, com linguagem simples e correta.
Em calculos de logica, mostre os passos. Nao invente fontes ou capacidades.
```

**Parâmetros de geração:** `max_tokens=300`, `temperature=0.2`

### 4.3 Modelo LOGI

| Atributo | Valor |
|---|---|
| Modelo base | `Qwen/Qwen2.5-1.5B-Instruct` |
| Repositório ajustado | [huggingface.co/guiiwfz/logi-1.5b-gguf](https://huggingface.co/guiiwfz/logi-1.5b-gguf) |
| Versão | v5 |
| Método de fine-tuning | QLoRA 4-bit com Unsloth |
| LoRA rank / alpha | 32 / 32 |
| Dataset | 13.000 exemplos próprios (12.350 treino / 650 validação) |
| Épocas | 3 |
| Hardware de treino | Tesla T4 no Google Colab |
| Formato de distribuição | GGUF — Q4_K_M (~940 MB) e Q6_K (~1.2 GB) |
| Device em produção | Metal (Apple Silicon) com fallback para CPU |
| Escopo | Lógica proposicional + alfabetização em IA + segurança digital |

---

## 5. Fluxo de Dados Completo

```
Jogador pressiona [H] durante um desafio
        │
        ▼
executar_desafio() em game.c
        │
        ▼
tela_painel_logi(indice) em ui.c
        │
        ├─► ai_consultar(enunciado, buffer, size) em ai_client.c
        │           │
        │           ├─► verifica se scripts/ai_client.py existe
        │           ├─► monta cmd: python3 ai_client.py '<enunciado>'
        │           ├─► popen() → pipe não-bloqueante
        │           ├─► lê stdout com timeout de 6s
        │           └─► retorna 0 (sucesso) ou -1 (falha/timeout)
        │
        ├─► [sucesso] exibe dica gerada pelo modelo LOGI
        └─► [falha]   exibe dica estática de dicas_l1/dicas_l2
```

---

## 6. Decisões Técnicas

| Decisão | Justificativa |
|---|---|
| Raw mode via `tcsetattr` | Permite leitura caractere a caractere sem Enter, necessário para a UX fluida do terminal |
| Alternate screen buffer | Isola a UI do histórico do terminal; ao sair, o terminal é restaurado sem rastros do jogo |
| Fade-in com multiplicador de brilho | Efeito visual sem bibliotecas externas, usando apenas ANSI true-color e `usleep` |
| `popen()` para bridge C→Python | Portável, sem dependências adicionais em C; o Python gerencia toda a complexidade de HTTP e inferência |
| LoRA sobre Qwen2.5-3B | Fine-tuning eficiente com baixo custo computacional; o adapter é pequeno e o modelo base permanece inalterado |
| Questões hardcoded na U1 | Garante cobertura exata dos 5 operadores e controle de progressão; geração dinâmica entra na U2 |
| Fallback estático na dica LOGI | O jogo não depende do servidor Python para funcionar; a UX é preservada mesmo offline |
| `SIGINT`/`SIGTERM` capturados | Garante que o terminal seja sempre restaurado ao raw mode ao encerrar, evitando que o shell fique quebrado |

---

## 7. Instruções de Build

### Pré-requisitos

- `gcc` e `make`
- Python 3.10+ com `torch`, `transformers` e `peft` (opcional — apenas para a tutora LOGI)

### Compilação e execução

```bash
# Compila o projeto
make

# Compila e executa
make run

# Remove arquivos compilados
make clean
```

### Iniciar o servidor LOGI (opcional)

```bash
# Instala llama-cpp-python (CPU)
pip install llama-cpp-python

# Com suporte a Metal (Apple Silicon):
CMAKE_ARGS="-DGGML_METAL=on" pip install llama-cpp-python

# Com suporte a CUDA (NVIDIA):
CMAKE_ARGS="-DGGML_CUDA=on" pip install llama-cpp-python

# Inicia o servidor (o modelo GGUF é baixado automaticamente na 1ª execução)
python3 scripts/local_server.py
```

> Sem o servidor rodando, o jogo funciona normalmente — as dicas da tutora LOGI usam o fallback estático.

---

## 8. Estrutura de Arquivos

```
logi-decifra-ai/
├── src/
│   ├── main.c          # Loop principal, raw mode, sinais
│   ├── game.c          # Banco de desafios, lógica de execução, pontuação
│   ├── ui.c            # Todas as telas, animações, fade-in, ANSI
│   ├── input.c         # Leitura de V/F/H, opção de menu, nome
│   └── ai_client.c     # Bridge C → Python via popen()
├── include/
│   ├── game.h          # Structs Desafio e Jogador, NUM_DESAFIOS, NOME_MAX
│   ├── ui.h            # Protótipos das telas
│   ├── input.h         # Protótipos de leitura
│   └── colors.h        # Constantes de cor ANSI
├── scripts/
│   ├── local_server.py # Servidor HTTP de inferência (porta 8787)
│   ├── ai_client.py    # Cliente HTTP chamado via popen()
│   └── requirements.txt
├── docs/
│   ├── relatorio-tecnico.md   # Este documento
│   ├── proposicoes-logicas.md # Banco de exercícios da LMC
│   ├── fundamentos_logica_proposicional.md
│   ├── tabelas_verdade_proposicoes_logicas.md
│   ├── casos_testes_logicos.md
│   └── screenshots/
└── Makefile
```

---

*Squad 05 — CESAR School 2026.2*
