#include "common.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* program_name = "program";
int opt_index = 1;     // Começa no primeiro argumento após o nome do programa
static int char_idx = 1; // Índice do caractere dentro de um grupo de flags (ex: -abc)

char* str_case_find(const char* haystack, const char* needle)
{
    if (!*needle)
        return (char*)haystack;

    for (; *haystack; haystack++)
    {
        if (tolower((unsigned char)*haystack) == tolower((unsigned char)*needle))
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
    if (opt_index >= argc || argv[opt_index][0] != '-' ||
        strcmp(argv[opt_index], "-") == 0)
    {
        return '\0';
    }

    if (strcmp(argv[opt_index], "--") == 0)
    {
        opt_index++;
        return '\0';
    }

    char c = argv[opt_index][char_idx];

    if (strchr(optstring, c) == NULL)
    {
        return '?';
    }

    char_idx++;
    if (argv[opt_index][char_idx] == '\0')
    {
        opt_index++;
        char_idx = 1;
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
