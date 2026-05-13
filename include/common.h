#pragma once

#include <stdbool.h>

#include <stdio.h>

/**
 * @brief Procura uma substring ignorando maiúsculas/minúsculas.
 * @param haystack String onde procurar.
 * @param needle String a ser procurada.
 * @return Ponteiro para a ocorrência ou NULL se não encontrar.
 */
char* str_case_find(const char* haystack, const char* needle);

/**
 * @brief Lê uma linha completa de um ficheiro com alocação dinâmica manual.
 * @param stream O ficheiro ou fluxo de entrada (stdin).
 * @return Ponteiro para a linha lida (deve ser libertado com free) ou NULL em caso de erro ou EOF.
 */
char* line_read(FILE* stream);

/**
 * @brief Nome do programa atual (deve ser definido no main de cada utilitário).
 */
extern char* program_name;

/**
 * @brief Exibe uma mensagem de erro do sistema (perror) mas não encerra o
 * programa.
 * @param msg Mensagem de contexto do erro.
 */
void warn(const char* msg);

/**
 * @brief Finaliza o programa com uma mensagem de erro do sistema (perror).
 * @param msg Mensagem de contexto do erro.
 */
void die(const char* msg);

/**
 * @brief Exibe uma mensagem de erro customizada e encerra o programa.
 * @param msg Mensagem de erro.
 */
void error_msg(const char* msg);

/**
 * @brief Índice do próximo argumento a ser processado (semelhante ao optind).
 */
extern int opt_index;

/**
 * @brief Processa o próximo caractere de opção no argv.
 * @param argc Número de argumentos.
 * @param argv Array de argumentos.
 * @param optstring String contendo os caracteres de opção válidos.
 * @return O caractere da opção encontrada, '?' para opção inválida, ou '\0' quando terminarem as opções.
 */
char next_option(int argc, char* argv[], const char* optstring);

/**
 * @brief Exibe a sintaxe de uso padrão e encerra o programa.
 * @param usage_str String contendo os argumentos (ex: "[OPÇÕES] PADRÃO
 * [FICHEIRO]").
 */
void usage(const char* usage_str);
