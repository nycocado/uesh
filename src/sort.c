#include "common.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @struct Line
 * @brief Estrutura para representar uma linha de texto.
 */
typedef struct
{
        char* text;
} Line;

/**
 * @struct SortOptions
 * @brief Agrupa as opções de configuração do utilitário sort.
 */
typedef struct
{
        bool reverse; // -d: Ordenação decrescente
} SortOptions;

/**
 * @brief Exibe a ajuda integrada do utilitário sort (requisito -h).
 */
static void print_usage(void)
{
    printf("Uso: %s [OPÇÕES] [FICHEIRO]...\n", program_name);
    printf("Objetivo: Ordena ficheiros de texto. Por padrão, ordena "
           "crescentemente e grava num ficheiro chamado FICHEIRO.sort.\n");
    printf("-d: Ordenação decrescente.\n");
    printf("-h: Apresenta esta ajuda e sai imediatamente.\n");
}

/**
 * @brief Comparador para ordenação ascendente (strcmp).
 */
static int compare_asc(const void* a, const void* b)
{
    return strcmp(((Line*)a)->text, ((Line*)b)->text);
}

/**
 * @brief Comparador para ordenação descendente (strcmp invertido).
 */
static int compare_desc(const void* a, const void* b)
{
    return strcmp(((Line*)b)->text, ((Line*)a)->text);
}

/**
 * @brief Processa a ordenação de um ficheiro individual.
 * @param filename Nome do ficheiro a ordenar.
 * @param opts Opções de configuração.
 */
static void sort_file(const char* filename, const SortOptions* opts)
{
    FILE* fp = fopen(filename, "r");
    if (!fp)
    {
        warn(filename);
        return;
    }

    Line* lines = NULL;
    int count = 0;
    char* line_content;

    // Carrega todas as linhas para a memória dinamicamente
    while ((line_content = line_read(fp)) != NULL)
    {
        Line* tmp = realloc(lines, sizeof(Line) * (count + 1));
        if (!tmp)
        {
            fclose(fp);
            die("falha de memória ao ordenar");
        }
        lines = tmp;
        lines[count].text = line_content;
        count++;
    }
    fclose(fp);

    if (count == 0)
        return;

    // Ordenação in-place usando qsort
    qsort(
        lines, count, sizeof(Line), opts->reverse ? compare_desc : compare_asc
    );

    // Constrói o nome do ficheiro de saída (original.sort)
    char out_name[1024];
    snprintf(out_name, sizeof(out_name), "%s.sort", filename);

    FILE* out_fp = fopen(out_name, "w");
    if (!out_fp)
    {
        warn(out_name);
    }
    else
    {
        for (int i = 0; i < count; i++)
        {
            fprintf(out_fp, "%s\n", lines[i].text);
        }
        fclose(out_fp);
    }

    // Libertação de memória
    for (int i = 0; i < count; i++)
    {
        free(lines[i].text);
    }
    free(lines);
}

/**
 * @brief Ponto de entrada do utilitário sort.
 */
int main(int argc, char** argv)
{
    program_name = argv[0];
    SortOptions opts = {false};
    char opt;

    while ((opt = next_option(argc, argv, "dh")) != '\0')
    {
        switch (opt)
        {
            case 'd':
                opts.reverse = true;
                break;
            case 'h':
                print_usage();
                return EXIT_SUCCESS;
            default:
                usage("[-d] [FICHEIRO]...");
        }
    }

    if (opt_index >= argc)
    {
        usage("[-d] [FICHEIRO]...");
    }

    for (int i = opt_index; i < argc; i++)
    {
        sort_file(argv[i], &opts);
    }

    return EXIT_SUCCESS;
}
