#include "common.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @struct HeadOptions
 * @brief Agrupa as opções de configuração do utilitário head.
 */
typedef struct
{
    int num_lines;     // Número de linhas a exibir (padrão 10)
    bool show_line_num; // -n: Mostrar o número da linha
    bool show_end_line; // -E: Mostrar $ no fim de cada linha
} HeadOptions;

/**
 * @brief Exibe a ajuda integrada do utilitário head (requisito -h).
 */
static void print_usage(void)
{
    printf("Uso: %s [OPÇÕES] [FICHEIRO]...\n", program_name);
    printf("Objetivo: Lista as primeiras linhas de um ficheiro de texto para o "
           "stdout.\n");
    printf("Opções:\n");
    printf("  -n         : lista número da linha\n");
    printf("  -E         : indica fim de linha ($ no fim de cada linha)\n");
    printf("  -\"numero\"  : lista as primeiras \"número\" de linhas (ex: -20)\n");
    printf("  -h         : apresenta esta ajuda e sai imediatamente\n");
}

/**
 * @brief Processa o comando head num fluxo de entrada.
 * @param fp Ponteiro para o ficheiro ou stdin.
 * @param filename Nome do ficheiro (para o cabeçalho).
 * @param opts Opções de configuração.
 * @param print_header Se deve imprimir o nome do ficheiro (múltiplos ficheiros).
 */
static void process_head(
    FILE* fp,
    const char* filename,
    const HeadOptions* opts,
    bool print_header
)
{
    if (print_header)
    {
        printf("==> %s <==\n", filename);
    }

    char* line = NULL;
    int count = 0;
    while (count < opts->num_lines && (line = line_read(fp)) != NULL)
    {
        count++;
        if (opts->show_line_num)
        {
            printf("%d ", count);
        }

        printf("%s", line);

        if (opts->show_end_line)
        {
            printf("$");
        }
        printf("\n");
        free(line);
    }
}

/**
 * @brief Ponto de entrada do utilitário head.
 */
int main(int argc, char* argv[])
{
    program_name = argv[0];
    HeadOptions opts = {10, false, false};

    // Parsing customizado para suportar -NUM e flags padrão
    while (opt_index < argc && argv[opt_index][0] == '-' &&
           strcmp(argv[opt_index], "-") != 0)
    {
        if (strcmp(argv[opt_index], "--") == 0)
        {
            opt_index++;
            break;
        }

        // Caso especial: -10, -20, etc.
        if (isdigit(argv[opt_index][1]))
        {
            opts.num_lines = atoi(&argv[opt_index][1]);
            opt_index++;
            continue;
        }

        char opt = next_option(argc, argv, "nEh");
        if (opt == '\0')
            break;

        switch (opt)
        {
            case 'n':
                opts.show_line_num = true;
                break;
            case 'E':
                opts.show_end_line = true;
                break;
            case 'h':
                print_usage();
                return EXIT_SUCCESS;
            default:
                usage("[OPÇÕES] [FICHEIRO]...");
        }
    }

    int num_files = argc - opt_index;

    if (num_files == 0)
    {
        process_head(stdin, "stdin", &opts, false);
    }
    else
    {
        for (int i = opt_index; i < argc; i++)
        {
            FILE* fp = fopen(argv[i], "r");
            if (!fp)
            {
                warn(argv[i]);
                continue;
            }
            process_head(fp, argv[i], &opts, num_files > 1);
            if (i < argc - 1)
                printf("\n");
            fclose(fp);
        }
    }

    return EXIT_SUCCESS;
}
