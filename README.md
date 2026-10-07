# LOGI - Decifra.IA

<img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/edbe883b-94cc-4641-b752-19e4452feeec" />

Jogo educacional de terminal que une lógica proposicional e conscientização sobre Inteligência Artificial.

Desenvolvido por Squad 05 como Projeto Integrador 2P - E5 no CESAR School.

Implementado em **C** na Unidade 1 e **Haskell** na Unidade 2.

---

## Sobre o Jogo

Você está em 2031. A IA já permeia tudo: contrata, diagnostica, julga, governa. A maioria das pessoas apenas aceita as respostas que ela dá.

Mas você conheceu a **LOGI**.

LOGI não é um chatbot qualquer. Ela é uma IA tutora criada para ensinar o que poucos ainda dominam: **pensar com rigor lógico**. Ela acredita que a melhor defesa contra uma IA opaca é um humano que sabe raciocinar.

A cada sessão, LOGI te apresenta desafios de lógica proposicional, os mesmos fundamentos matemáticos que estão por trás de qualquer sistema de IA. Ela explica operadores como AND, OR, NOT, IMPLICA e BICONDICIONAL não como curiosidades acadêmicas, mas como ferramentas de pensamento crítico para o mundo que vivemos.

Entre cada desafio, ela conta um pouco sobre como a IA funciona, onde ela erra, o que ela não consegue fazer sozinha, e por que você, humano, ainda importa.

**Decifre as proposições. Questione as máquinas. Pense por conta própria.**

---

## O que o jogo ensina

### Lógica Proposicional

| Operador | Regra |
|---|---|
| `AND` | verdadeiro apenas quando ambos os lados são V |
| `OR` | verdadeiro quando pelo menos um lado é V |
| `NOT` | inverte o valor lógico |
| `IMPLICA` | falso apenas quando P = V e Q = F |
| `BICONDICIONAL` | verdadeiro quando os dois lados têm o mesmo valor |

### Conscientização em IA

O jogo aborda de forma narrativa temas como:

- Viés algorítmico — quando a IA reflete os preconceitos dos dados com que foi treinada
- Opacidade de modelos — o problema da "caixa preta" e a dificuldade de auditar decisões automáticas
- Falsas certezas — por que alta acurácia em treino não garante segurança no mundo real
- Papel humano — o lugar do pensamento crítico e da responsabilidade em sistemas híbridos

---

## Fluxo do jogo

```
Menu principal
  ├─ Jogar
  │     └─ Loading (LOGI inicializa)
  │           └─ Informe seu nome
  │                 └─ Desafio 1 → [V/F] ou [H] pedir dica ao LOGI
  │                 └─ Feedback (acerto/erro + explicação)
  │                 └─ Desafio 2 ... Desafio 5
  │                 └─ Resultado final (pontuação + classificação)
  │                 └─ Volta ao menu
  ├─ Como Jogar (regras e operadores)
  └─ Sair (animação "Até logo!")
```

---

## Screenshots

**Menu principal**

![Menu principal](docs/screenshots/01-menu-principal.png)

**Tela de inicialização**

![Inicialização do LOGI](docs/screenshots/03-inicializacao.png)

**Como Jogar**

![Como Jogar](docs/screenshots/02-como-jogar.png)

**Feedback de resposta correta**

![Resposta correta com explicação](docs/screenshots/04-resposta-correta.png)

**Tutora LOGI — dica contextual**

![Tutora LOGI](docs/screenshots/05-tutora-logi.png)

---

## Funcionalidades da UI

- **Alternate screen buffer** — o jogo roda como uma CLI isolada, sem rolar o terminal
- **Posicionamento absoluto de cursor** — nenhuma tela causa scroll; todo conteúdo é impresso em coordenadas fixas via `\033[row;colH`
- **Fade-in simultâneo em tela cheia** — todas as linhas aparecem juntas com brilho crescente via ANSI true-color RGB
- **Spinner braille animado** (⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏ → ✔) nas telas de loading e saída
- **Paleta azul** derivada da identidade visual LOGI (#12122A em variantes mais claras)
- **Respostas V/F** sem diferenciação de maiúsculas/minúsculas
- **Sinal SIGINT capturado** — Ctrl+C restaura o terminal corretamente

---

## Equipe

| Integrante | Frente |
|---|---|
| Guilherme Alves de Souza | Gestão Ágil + Engenharia de Software |
| Hilton Resende | Gestão Ágil |
| João Guilherme Azevedo | Gestão Ágil |
| João Bezerra | Engenharia de Software |
| João Antonio Calazans | Design de Interação (IHC) |
| Pedro Feitosa | Lógica Matemática (LMC) |
| Ademir Pedro da Silva Filho | Motor em C (PIF) |
| Arthur Vieira Neiva Souza | Haskell (PIF) |
| Augusto Freitas Wanderley | Engenharia de Software |

---

## Estrutura do projeto

```
decifra-ai-e5/
├── src/
│   ├── main.c      # Ponto de entrada, loop principal, sinais
│   ├── game.c      # Lógica do jogo, desafios mockados, pontuação
│   ├── ui.c        # Todas as telas, animações, fade-in, paleta de cores
│   └── input.c     # Leitura de opção, nome e resposta V/F
├── include/
│   ├── game.h      # Structs Desafio e Jogador, constantes, protótipos
│   ├── ui.h        # Protótipos das telas
│   ├── input.h     # Protótipos de leitura
│   └── colors.h    # Constantes de cor ANSI (referência)
├── docs/
│   └── screenshots/  # Prints das telas do jogo
├── tests/          # Testes de unidade
└── Makefile        # Compilação do projeto em C
```

---

## Como compilar e rodar

### Pré-requisitos

- gcc
- make

### Instalação das dependências

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential
```

**macOS:**
```bash
xcode-select --install
```

**Windows (recomendado usar WSL):**
```powershell
wsl --install
# Após reiniciar:
sudo apt update
sudo apt install build-essential
```

### Build e execução

```bash
make          # Compila
make run      # Compila e executa
make clean    # Limpa arquivos compilados
```

---

## Roadmap

- [x] Telas mockadas com navegação completa (UI Unidade 1)
- [x] Animações fade-in simultâneo com ANSI true-color
- [x] Spinner braille e alternate screen buffer
- [x] Posicionamento absoluto de cursor (sem scroll em nenhuma tela)
- [x] Integração com modelo LOGI (bridge C → Python + GGUF via llama-cpp-python) — modelo v5 treinado com 13.000 exemplos (QLoRA, Qwen2.5-1.5B-Instruct, rank 32) · [ver modelo no HuggingFace](https://huggingface.co/guiiwfz/logi-1.5b-gguf)
- [ ] Motor de lógica proposicional em Haskell (Unidade 2)
- [ ] Banco de questões expandido com geração via IA
- [ ] Modo narrativo com falas contextuais da LOGI
- [ ] Histórico de partidas e evolução do jogador

---

## Convenção de branches

| Branch | Uso |
|---|---|
| `main` | Código estável, entregas |
| `develop` | Integração contínua |
| `feat/nome` | Desenvolvimento de funcionalidades |

---

## Histórias de Usuário

https://docs.google.com/document/d/1SzixVp4eZ87xHs2-ZFUsQz371UWR0J6m4fWdXbj5Fig/edit?usp=sharing

### Diagramas de Fluxo (HU1 a HU10)

### HU1 - Proposições Compostas com o operador AND
<details>
  <summary>Clique para visualizar o Diagrama HU1</summary>
  
  ![Diagrama HU1](./docs/diagrams/diagramahu1_.png)
</details>

### HU2 - Conectivos Binários no Nível 1
<details>
  <summary>Clique para visualizar o Diagrama HU2</summary>
  
  ![Diagrama HU2](./docs/diagrams/DIAGRAMAHU2.png)
</details>

### HU3 - Fórmulas com Implicação e Bicondicional
<details>
  <summary>Clique para visualizar o Diagrama HU3</summary>
  
  ![Diagrama HU3](./docs/diagrams/DIAGRAMAHU3.png)
</details>

### HU4 - Feedback e Explicações Lógicas
<details>
  <summary>Clique para visualizar o Diagrama HU4</summary>
  
  ![Diagrama HU4](./docs/diagrams/DIAGRAMAHU4.png)
</details>

### HU5 - Regras de Progressão de Nível
<details>
  <summary>Clique para visualizar o Diagrama HU5</summary>
  
  ![Diagrama HU5](./docs/diagrams/DIAGRAMAHU5.png)
</details>

### HU6 - Notação Padronizada dos Operadores
<details>
  <summary>Clique para visualizar o Diagrama HU6</summary>
  
  ![Diagrama HU6](./docs/diagrams/DIAGRAMAHU6.png)
</details>

### HU7 - Feedback Imediato de Resposta
<details>
  <summary>Clique para visualizar o Diagrama HU7</summary>
  
  ![Diagrama HU7](./docs/diagrams/diagramahu7__.png)
</details>

### HU8 - Sistema de Ajuda Contextual (Tutora LOGI)
<details>
  <summary>Clique para visualizar o Diagrama HU8</summary>
  
  ![Diagrama HU8](./docs/diagrams/DIAGRAMAHU8.png)
</details>

### HU9 - Exemplos Didáticos de Escopo
<details>
  <summary>Clique para visualizar o Diagrama HU9</summary>
  
  ![Diagrama HU9](./docs/diagrams/DIAGRAMAHU9.png)
</details>

### HU10 - Cobertura de Combinações (2^n)
<details>
  <summary>Clique para visualizar o Diagrama HU10</summary>
  
  ![Diagrama HU10](./docs/diagrams/DIAGRAMAHU10.png)
</details>

### HU11 - Modo Contra-Tempo
<details>
  <summary>Clique para visualizar o Diagrama HU10</summary>
  
  ![Diagrama HU10](./docs/diagrams/diagramahu11.png)
</details>

---

## Screencast

https://youtu.be/IIYWuKim7L0

### Protótipos e Telas do Jogo (Screenshots)
*(Como você já tem a pasta `screenshots`, pode adicionar os prints aqui também)*

| Tela | Descrição | Link |
| :--- | :--- | :--- |
| **01** | Menu Principal | [Ver Print](./docs/screenshots/01-menu-principal.png) |
| **02** | Como Jogar | [Ver Print](./docs/screenshots/02-como-jogar.png) |
| **03** | Inicialização | [Ver Print](./docs/screenshots/03-inicializacao.png) |
| **04** | Resposta Correta | [Ver Print](./docs/screenshots/04-resposta-correta.png) |
| **05** | Tutora LOGI | [Ver Print](./docs/screenshots/05-tutora-logi.png) |

## Backlog

<img width="1920" height="968" alt="Captura de Tela (262)" src="https://github.com/user-attachments/assets/bafc4976-c9a4-4516-8d1f-60b9643f258e" />

---

## Board

<img width="1920" height="1031" alt="Captura de Tela (257)" src="https://github.com/user-attachments/assets/8975c89e-ebdf-49e8-8a69-3ed899bfeb8b" />

---

## Licença

Projeto acadêmico - CESAR School 2026.
