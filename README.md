# Projeto de Sistemas Operativos: Utilitários UNIX em C

Este repositório contém o código-fonte desenvolvido para o projeto prático da unidade curricular de Sistemas Operativos. O objetivo central é a implementação de um conjunto de comandos e utilitários clássicos do ambiente UNIX, operando nativamente em sistema operativo Linux e desenvolvidos integralmente em Linguagem C.

## Estrutura do Projeto

O desenvolvimento foi pautado pela modularização e reaproveitamento de código. Para evitar a duplicação de esforço, foi criada uma biblioteca de funções genéricas utilizada por todos os comandos para tarefas como o parsing de argumentos e o tratamento de erros.

* `/src`: Contém o código-fonte principal de cada utilitário e da Shell.
* `/lib`: Contém os ficheiros de cabeçalho (`.h`) e a implementação (`.c`) das funções comuns.
* `Makefile`: Script de automação para compilar todos os comandos simultaneamente, garantindo um processo de build limpo.

## Comandos Implementados

O projeto é composto por 9 utilitários no total (3 obrigatórios e 6 opcionais selecionados pelo grupo). Todos os comandos suportam a flag `-h`, que apresenta a sintaxe de uso e o objetivo principal do programa, encerrando a execução imediatamente.

### Interpretador Central

* **UEsh**: Interpretador de comandos (Shell) com prompt personalizada. Suporta a invocação dos comandos externos desenvolvidos neste projeto e implementa nativamente os comandos internos: `cd`, `pwd`, `echo`, `sleep` e `exit`.

### Comandos Obrigatórios

* **sort**: Ordena ficheiros de texto. Suporta ordenação decrescente com a opção `-d`.
* **ls**: Lista os nomes dos ficheiros contidos num diretório. Suporta listagem longa (`-1`), listagem por colunas (`-c`) e ordenação dinâmica do output (`-ox`).

### Comandos Opcionais

* **tail**: Lista as últimas linhas de um ficheiro de texto (suporta `-n`, `-E` e indicação do número de linhas).
* **head**: Lista as primeiras linhas de um ficheiro de texto (suporta `-n`, `-E` e indicação do número de linhas).
* **grep**: Procura strings dentro de ficheiros. Suporta ignorar maiúsculas/minúsculas (`-i`), numeração de linhas (`-n`), contagem de ocorrências (`-c`) e pesquisa invertida (`-v`).
* **replace**: Procura uma string dentro de ficheiros e substitui por outra, processando múltiplos ficheiros paralelamente (via fork).
* **cp**: Efetua a cópia de ficheiros da origem para um destino, suportando o modo interativo (`-i`).
* **kill**: Termina um processo ativo no sistema através do respetivo identificador (PID).

## Pré-requisitos e Ambiente

* **Sistema Operativo**: Linux. O desenvolvimento ou execução nativa em MacOS não é suportada.
* **Compilador**: GCC.
* **Ferramentas**: Make.

## Compilação e Execução

Para compilar todo o projeto, navegue até à pasta raiz do repositório e execute o comando:

```bash
make
