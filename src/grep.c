#include "common.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @struct GrepOptions
 * @brief Configuration options for the grep utility.
 */
typedef struct
{
        bool count_only;       // -c: report only the match count
        bool line_numbers;     // -n: show line numbers
        bool case_insensitive; // -i: ignore case
        bool inverse;          // -v: show lines that do NOT contain the string
        bool multiple_files;   // internal flag to prefix filename in output
} GrepOptions;

/**
 * @brief Prints the built-in help for the grep utility (-h flag).
 */
static void print_usage(void)
{
    printf("Uso: %s [OPÇÕES] STRING [FICHEIROS]\n", program_name);
    printf("Objetivo: Procura strings dentro de ficheiros. Por padrão, mostra "
           "todas as linhas que contêm a string procurada.\n");
    printf(
        "-c: Não mostra as linhas, informando apenas o número de ocorrências.\n"
    );
    printf("-n: Indica o número da linha no início, seguido de \":\", e o "
           "conteúdo da linha.\n");
    printf("-i: Ignora maiúsculas e minúsculas na busca.\n");
    printf("-v: Trabalha inversamente (mostra apenas as linhas que NÃO contêm "
           "a string).\n");
    printf("-h: Apresenta esta ajuda e sai imediatamente.\n");
}

/**
 * @brief Searches for a pattern in a file.
 * @param filename Name of the file to search.
 * @param pattern String to search for.
 * @param opts Configuration options.
 * @return Number of matches found.
 */
static int
grep_file(const char* filename, const char* pattern, const GrepOptions* opts)
{
    FILE* fp = fopen(filename, "r");
    if (!fp)
    {
        warn(filename);
        return 0;
    }

    char* line = NULL;
    int line_num = 0;
    int matches = 0;

    while ((line = line_read(fp)) != NULL)
    {
        line_num++;
        bool found = false;

        if (opts->case_insensitive)
        {
            found = (str_case_find(line, pattern) != NULL);
        }
        else
        {
            found = (strstr(line, pattern) != NULL);
        }

        if (opts->inverse)
        {
            found = !found;
        }

        if (found)
        {
            matches++;
            if (!opts->count_only)
            {
                if (opts->multiple_files)
                {
                    printf("%s:", filename);
                }
                if (opts->line_numbers)
                {
                    printf("%d:", line_num);
                }
                printf("%s\n", line);
            }
        }
        free(line);
    }

    if (opts->count_only)
    {
        if (opts->multiple_files)
        {
            printf("%s:", filename);
        }
        printf("%d\n", matches);
    }

    fclose(fp);
    return matches;
}

/**
 * @brief Entry point for the grep utility.
 * @param argc Argument count from the terminal.
 * @param argv Argument array with flags and arguments.
 * @return EXIT_SUCCESS if matches are found, EXIT_FAILURE otherwise.
 */
int main(int argc, char* argv[])
{
    program_name = argv[0];
    GrepOptions opts = {0};
    char opt;

    while ((opt = next_option(argc, argv, "cnivh")) != '\0')
    {
        switch (opt)
        {
            case 'c':
                opts.count_only = true;
                break;
            case 'n':
                opts.line_numbers = true;
                break;
            case 'i':
                opts.case_insensitive = true;
                break;
            case 'v':
                opts.inverse = true;
                break;
            case 'h':
                print_usage();
                return EXIT_SUCCESS;
            default:
                usage("STRING [FICHEIROS]");
        }
    }

    if (opt_index >= argc)
    {
        usage("STRING [FICHEIROS]");
    }

    const char* pattern = argv[opt_index++];
    int num_files = argc - opt_index;
    opts.multiple_files = (num_files > 1);

    if (num_files == 0)
    {
        usage("STRING [FICHEIROS]");
    }

    int total_matches = 0;
    for (int i = opt_index; i < argc; i++)
    {
        total_matches += grep_file(argv[i], pattern, &opts);
    }

    return (total_matches > 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
