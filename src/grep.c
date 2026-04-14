#define _GNU_SOURCE
#include "common.h"
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @struct GrepOptions
 * @brief Agrupa as opções de configuração do utilitário grep.
 */
typedef struct
{
        bool count_only;       // -c: Reportar apenas o número de ocorrências
        bool line_numbers;     // -n: Mostrar o número da linha
        bool case_insensitive; // -i: Ignorar maiúsculas/minúsculas
        bool inverse;          // -v: Mostrar linhas que NÃO contêm a string
        bool multiple_files;   // Controle interno para exibir nome do ficheiro
} GrepOptions;

/**
 * @brief Exibe a ajuda integrada do utilitário grep (requisito -h).
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
 * @brief Processa a busca de uma string dentro de um ficheiro.
 * @param filename Nome do ficheiro a processar.
 * @param pattern String a procurar.
 * @param opts Opções de configuração.
 * @return O número de ocorrências encontradas.
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
    size_t len = 0;
    int line_num = 0;
    int matches = 0;

    while (getline(&line, &len, fp) != -1)
    {
        line_num++;
        bool found = false;

        if (opts->case_insensitive)
        {
            found = (strcasestr(line, pattern) != NULL);
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
                printf("%s", line);
            }
        }
    }

    if (opts->count_only)
    {
        if (opts->multiple_files)
        {
            printf("%s:", filename);
        }
        printf("%d\n", matches);
    }

    free(line);
    fclose(fp);
    return matches;
}

/**
 * @brief Ponto de entrada do utilitário grep.
 * @param argc O número de argumentos vindos do terminal.
 * @param argv Os apontadores de char contendo as flags e argumentos.
 * @return EXIT_SUCCESS se encontrar ocorrências, EXIT_FAILURE caso contrário.
 */
int main(int argc, char* argv[])
{
    program_name = argv[0];
    int opt;
    GrepOptions opts = {0};

    while ((opt = getopt(argc, argv, "cnivh")) != -1)
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

    if (optind >= argc)
    {
        usage("STRING [FICHEIROS]");
    }

    const char* pattern = argv[optind++];
    int num_files = argc - optind;
    opts.multiple_files = (num_files > 1);

    if (num_files == 0)
    {
        usage("STRING [FICHEIROS]");
    }

    int total_matches = 0;
    for (int i = optind; i < argc; i++)
    {
        total_matches += grep_file(argv[i], pattern, &opts);
    }

    return (total_matches > 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
