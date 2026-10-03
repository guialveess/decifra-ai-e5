# Diagrama do Plano de Testes Unitários

```mermaid
flowchart TD
    Start(( )):::start

    A[Sistema inicia suíte de testes unitários]
    B[Compila binário de teste com asserts]
    C[Executa testes por módulo]

    subgraph M1["game.c — Parser Lógico e Tabela-Verdade"]
        G1[Teste: avaliar P AND Q com P=V, Q=V → esperado V]
        G2[Teste: avaliar P AND Q com P=V, Q=F → esperado F]
        G3[Teste: avaliar NOT P com P=F → esperado V]
        G4[Teste: avaliar P IMPLICA Q com P=V, Q=F → esperado F]
        G5[Teste: avaliar P BICON Q com P=V, Q=V → esperado V]
        G6[Teste: avaliar P OR Q AND R sem parênteses]
        G7[Teste: avaliar P OR Q AND R com parênteses]
        G8[Teste: enumeração da tabela gera exatamente 2^n linhas]
    end

    subgraph M2["input.c — Leitura de Teclado"]
        I1[Teste: entrada V retorna caractere V]
        I2[Teste: entrada F retorna caractere F]
        I3[Teste: entrada H retorna caractere H]
        I4[Teste: seta para cima não trava o loop]
    end

    subgraph M3["ai_client.c — Cliente da IA Tutora"]
        A1[Teste: consulta com AND retorna dica sobre ambos os lados]
        A2[Teste: consulta com NOT abre parênteses avisa sobre precedência]
        A3[Teste: consulta sem operadores retorna mensagem padrão]
    end

    subgraph M4["ui.c — Renderização"]
        U1[Teste: fade_tela não corrompe buffer do terminal]
        U2[Teste: render_desafio_avancado destaca parênteses em amarelo]
        U3[Teste: render_feedback exibe resposta correta em caso de erro]
    end

    D{Todos os testes passaram?}
    P[Gera relatório: 100% de aprovação]
    Q[Registra módulo e caso falhado]
    End(( )):::end

    Start --> A --> B --> C
    C --> M1 --> D
    C --> M2 --> D
    C --> M3 --> D
    C --> M4 --> D
    D -->|sim| P --> End
    D -->|não| Q --> End

    classDef start fill:#0f172a,stroke:#0f172a,color:#fff;
    classDef end fill:#ffffff,stroke:#0f172a,stroke-width:3px;
    classDef decision fill:#fef3c7,stroke:#d97706;
    class M1 fill:#eff6ff,stroke:#3b82f6
    class M2 fill:#f0fdf4,stroke:#22c55e
    class M3 fill:#fffbeb,stroke:#f59e0b
    class M4 fill:#faf5ff,stroke:#a855f7
```

## Legenda

| Módulo | Foco | Cor |
|--------|------|-----|
| `game.c` | Parser lógico, tabela-verdade, sorteio de linha | Azul |
| `input.c` | Leitura de teclado em modo raw | Verde |
| `ai_client.c` | Cliente de consulta à IA tutora | Âmbar |
| `ui.c` | Renderização ANSI e telas | Roxo |