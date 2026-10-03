# Plano de Testes Unitários - Núcleo Lógico (C)

## Objetivo
Garantir a corretude do parser lógico, da geração de tabelas-verdade e da
integração com a IA tutora.

## Módulos Alvo

### 1. `game.c` (Parser Lógico e Tabela-Verdade)
- **Caso 1:** `avaliar_expressao("P AND Q", P=1, Q=1)` deve retornar `1`.
- **Caso 2:** `avaliar_expressao("P AND Q", P=1, Q=0)` deve retornar `0`.
- **Caso 3:** `avaliar_expressao("NOT (P OR Q)", P=1, Q=0)` deve retornar `0`.
- **Caso 4:** Verificar se a enumeração de linhas (HU10) gera exatamente 2^n combinações.

### 2. `ai_client.c` (LOGI Tutora)
- **Caso 1:** Consulta com "AND" deve retornar dica contendo "ambos os lados".
- **Caso 2:** Consulta com "NOT (" deve retornar aviso sobre precedência.
- **Caso 3:** Consulta sem operadores lógicos deve retornar mensagem padrão.

### 3. `input.c` (Leitura de Terminal)
- **Caso 1:** Simular entrada "V\n" deve retornar caractere 'V'.
- **Caso 2:** Simular entrada "H\n" deve retornar caractere 'H'.
- **Caso 3:** Simular seta para cima (escape sequence) não deve travar o loop.