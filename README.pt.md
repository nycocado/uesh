# uesh

Utilitários UNIX e interpretador de shell em C — sort, ls, tail, head, grep, replace, cp, kill e uma shell personalizada (UEsh), desenvolvidos para a unidade curricular de Sistemas Operativos.

[![License: MIT](https://img.shields.io/badge/License-MIT-3da639.svg)](LICENSE)
![Status](https://img.shields.io/badge/status-concluído-6f42c1)

[![C](https://img.shields.io/badge/C-00599C?logo=c&logoColor=white)](https://en.cppreference.com/w/c)
[![Make](https://img.shields.io/badge/Make-4495D5?logo=make&logoColor=white)](https://www.gnu.org/software/make/)

[English](README.md) | Português

## Sobre

Implementação de 9 utilitários clássicos UNIX e um interpretador de shell personalizado para a unidade curricular de Sistemas Operativos. Todos os utilitários partilham uma biblioteca comum (`lib/common.c`) para o processamento de argumentos e tratamento de erros. Desenvolvido nativamente para Linux.

## Utilitários

### Shell

| Utilitário | Descrição |
| ---------- | --------- |
| **UEsh** | Interpretador de comandos com prompt personalizado. Suporta os utilitários externos do projeto e os comandos internos: `cd`, `pwd`, `echo`, `sleep`, `exit`. |

### Obrigatórios

| Utilitário | Descrição |
| ---------- | --------- |
| **sort** | Ordena ficheiros de texto. Suporta ordenação decrescente (`-d`). O resultado é gravado em `FICHEIRO.sort`. |
| **ls** | Lista o conteúdo de um diretório. Suporta formato longo (`-l`), listagem em colunas (`-c`) e ordenação por nome, tamanho ou data (`-on`, `-os`, `-od`). |
| **grep** | Procura strings em ficheiros. Suporta pesquisa sem distinção de maiúsculas/minúsculas (`-i`), numeração de linhas (`-n`), contagem de ocorrências (`-c`) e pesquisa invertida (`-v`). |

### Opcionais

| Utilitário | Descrição |
| ---------- | --------- |
| **tail** | Apresenta as últimas N linhas de um ficheiro de texto. Suporta `-n`, `-E` e indicação numérica do número de linhas. |
| **head** | Apresenta as primeiras N linhas de um ficheiro de texto. Suporta `-n`, `-E` e indicação numérica do número de linhas. |
| **replace** | Substitui uma string em ficheiros, processando cada ficheiro num processo filho separado (`fork`). Suporta `-c`, `-n`, `-i`. |
| **cp** | Copia um ficheiro para um destino. Suporta modo interativo (`-i`). |
| **kill** | Termina um processo pelo PID via `SIGTERM`. |

## Pré-requisitos

| Ferramenta | Versão mínima |
| ---------- | ------------- |
| GCC        | 9+            |
| Make       | 4+            |

Apenas Linux — a execução nativa em macOS não é suportada.

## Como executar

```bash
make          # compila todos os utilitários
make run      # compila e inicia o UEsh
make valgrind # executa o UEsh com valgrind
make clean    # remove os artefactos de build
```

Os binários são colocados em `bin/`. Todos os utilitários suportam `-h` para obter ajuda de utilização.

## Licença

Distribuído sob a licença **MIT**, © 2026 Nycolas Souza.

É uma licença permissiva: qualquer pessoa pode usar, copiar, modificar e distribuir o código, incluindo em projetos comerciais, desde que o aviso de direitos de autor e o texto da licença sejam mantidos.

O texto completo encontra-se em [LICENSE](LICENSE).
