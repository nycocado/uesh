#include "common.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* program_name = "program";
int opt_index = 1;    // starts at the first argument after the program name
char* opt_arg = NULL; // holds the option argument value, if any
static int char_idx =
    1; // index of character within a flag group (e.g. -abc)

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

    // support for numeric flags (e.g. -10) when optstring contains '#'
    if (isdigit(argv[opt_index][1]) && strchr(optstring, '#') != NULL)
    {
        opt_arg =
            &argv[opt_index][1]; // opt_arg points to the number (e.g. "10")
        opt_index++;
        return '#'; // returns the special character
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

    // check if the option requires an argument (indicated by ':')
    if (ptr[1] == ':')
    {
        if (argv[opt_index][char_idx + 1] != '\0')
        {
            // argument attached to flag (e.g. -on or -oN)
            opt_arg = &argv[opt_index][char_idx + 1];
            opt_index++;
            char_idx = 1;
        }
        else if (opt_index + 1 < argc)
        {
            // argument separated by space (e.g. -o n)
            opt_arg = argv[opt_index + 1];
            opt_index += 2;
            char_idx = 1;
        }
        else
        {
            // error: missing argument
            opt_index++;
            char_idx = 1;
            return ':';
        }
    }
    else
    {
        // normal option with no argument
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
