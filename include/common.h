#pragma once

#include <stdbool.h>

#include <stdio.h>

/**
 * @brief Searches for a substring, case-insensitively.
 * @param haystack String to search in.
 * @param needle String to search for.
 * @return Pointer to the match, or NULL if not found.
 */
char* str_case_find(const char* haystack, const char* needle);

/**
 * @brief Reads a full line from a file using manual dynamic allocation.
 * @param stream File or input stream (stdin).
 * @return Pointer to the line read (must be freed with free), or NULL on error
 * or EOF.
 */
char* line_read(FILE* stream);

/**
 * @brief Current program name (must be set in each utility's main).
 */
extern char* program_name;

/**
 * @brief Prints a system error message (perror) without terminating the
 * program.
 * @param msg Error context message.
 */
void warn(const char* msg);

/**
 * @brief Terminates the program with a system error message (perror).
 * @param msg Error context message.
 */
void die(const char* msg);

/**
 * @brief Prints a custom error message and exits the program.
 * @param msg Error message.
 */
void error_msg(const char* msg);

/**
 * @brief Index of the next argument to process (similar to optind).
 */
extern int opt_index;

/**
 * @brief Holds the value of an option's argument, if any (similar to optarg).
 */
extern char* opt_arg;

/**
 * @brief Processes the next option character in argv.
 * @param argc Argument count.
 * @param argv Argument array.
 * @param optstring String of valid option characters.
 * @return The found option character, '?' for invalid option, or '\0' when
 * options end.
 */
char next_option(int argc, char* argv[], const char* optstring);

/**
 * @brief Prints the standard usage message and exits the program.
 * @param usage_str String with expected arguments (e.g. "[OPTIONS] PATTERN
 * [FILE]").
 */
void usage(const char* usage_str);
