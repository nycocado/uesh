#pragma once

#include <stdbool.h>

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
 * @brief Exibe a sintaxe de uso padrão e encerra o programa.
 * @param usage_str String contendo os argumentos (ex: "[OPÇÕES] PADRÃO
 * [FICHEIRO]").
 */
void usage(const char* usage_str);
