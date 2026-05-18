#include "common.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* program_name = "program";
int opt_index = 1;    // Começa no primeiro argumento após o nome do programa
char* opt_arg = NULL; // Guarda o valor do argumento da flag, se houver
static int char_idx =
    1; // Índice do caractere dentro de um grupo de flags (ex: -abc)

char* str_case_find(const char* haystack, const char* needle)
{
    if (!*needle)
        return (char*)haystack;

    for (; *haystack; haystack++)
    {
        if (tolower((unsigned char)*haystack) ==
            tolower((unsigned char)*needle))
        {
            const char *h, *n;
            for (h = haystack, n = needle; *h && *n; h++, n++)
            {
                if (tolower((unsigned char)*h) != tolower((unsigned char)*n))
                    break;
            }
            if (!*n)
                return (char*)haystack;
        }
    }
    return NULL;
}

char* line_read(FILE* stream)
{
    size_t size = 128;
    size_t pos = 0;
    char* buffer = malloc(size);

    if (!buffer)
        return NULL;

    while (fgets(buffer + pos, size - pos, stream))
    {
        pos += strlen(buffer + pos);

        if (buffer[pos - 1] == '\n')
        {
            buffer[pos - 1] = '\0';
            return buffer;
        }

        size *= 2;
        char* new_buffer = realloc(buffer, size);
        if (!new_buffer)
        {
            free(buffer);
            return NULL;
        }
        buffer = new_buffer;
    }

    if (pos == 0)
    {
        free(buffer);
        return NULL;
    }

    return buffer;
}

char next_option(int argc, char* argv[], const char* optstring)
{
    opt_arg = NULL;

    if (opt_index >= argc || argv[opt_index][0] != '-' ||
        argv[opt_index][1] == '\0')
    {
        return '\0';
    }

    if (strcmp(argv[opt_index], "--") == 0)
    {
        opt_index++;
        return '\0';
    }

    // Suporte para flags numéricas (ex: -10) se a optstring contiver '#'
    if (isdigit(argv[opt_index][1]) && strchr(optstring, '#') != NULL)
    {
        opt_arg =
            &argv[opt_index][1]; // opt_arg aponta para o número (ex: "10")
        opt_index++;
        return '#'; // Retorna o caractere especial
    }

    char c = argv[opt_index][char_idx];
    const char* ptr = strchr(optstring, c);

    if (ptr == NULL)
    {
        char_idx++;
        if (argv[opt_index][char_idx] == '\0')
        {
            opt_index++;
            char_idx = 1;
        }
        return '?';
    }

    // Verifica se a opção exige um argumento (indicado por ':')
    if (ptr[1] == ':')
    {
        if (argv[opt_index][char_idx + 1] != '\0')
        {
            // Argumento colado à flag (ex: -on ou -oN)
            opt_arg = &argv[opt_index][char_idx + 1];
            opt_index++;
            char_idx = 1;
        }
        else if (opt_index + 1 < argc)
        {
            // Argumento separado por espaço (ex: -o n)
            opt_arg = argv[opt_index + 1];
            opt_index += 2;
            char_idx = 1;
        }
        else
        {
            // Erro: Faltou o argumento
            opt_index++;
            char_idx = 1;
            return ':';
        }
    }
    else
    {
        // Opção normal sem argumento
        char_idx++;
        if (argv[opt_index][char_idx] == '\0')
        {
            opt_index++;
            char_idx = 1;
        }
    }

    return c;
}

void warn(const char* msg)
{
    fprintf(stderr, "%s: ", program_name);
    perror(msg);
}

void die(const char* msg)
{
    fprintf(stderr, "%s: erro: ", program_name);
    perror(msg);
    exit(EXIT_FAILURE);
}

void error_msg(const char* msg)
{
    fprintf(stderr, "%s: erro: %s\n", program_name, msg);
    exit(EXIT_FAILURE);
}

void usage(const char* usage_str)
{
    fprintf(stderr, "Uso: %s %s\n", program_name, usage_str);
    fprintf(stderr, "Tente '%s -h' para mais informações.\n", program_name);
    exit(EXIT_FAILURE);
}
