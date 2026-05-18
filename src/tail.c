#include "common.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @struct TailOptions
 * @brief Agrupa as opções de configuração do utilitário tail.
 */
typedef struct
{
        int num_lines;      // Número de linhas a exibir (padrão 10)
        bool show_line_num; // -n: Mostrar o número da linha original
        bool show_end_line; // -E: Mostrar $ no fim de cada linha
} TailOptions;

/**
 * @brief Exibe a ajuda integrada do utilitário tail (requisito -h).
 */
static void print_usage(void)
{
    printf("Uso: %s [OPÇÕES] [FICHEIRO]...\n", program_name);
    printf("Objetivo: Lista as últimas linhas de um ficheiro de texto para o "
           "stdout.\n");
    printf("Opções:\n");
    printf("  -n         : lista número da linha\n");
    printf("  -E         : indica fim de linha ($ no fim de cada linha)\n");
    printf("  -\"numero\"  : lista as últimas \"número\" de linhas (ex: -20)\n"
    );
    printf("  -h         : apresenta esta ajuda e sai imediatamente\n");
}

/**
 * @brief Processa o comando tail num fluxo de entrada usando um buffer
 * circular.
 * @param fp Ponteiro para o ficheiro ou stdin.
 * @param filename Nome do ficheiro (para o cabeçalho).
 * @param opts Opções de configuração.
 * @param print_header Se deve imprimir o nome do ficheiro (múltiplos
 * ficheiros).
 */
static void process_tail(
    FILE* fp,
    const char* filename,
    const TailOptions* opts,
    bool print_header
)
{
    if (print_header)
    {
        printf("==> %s <==\n", filename);
    }

    // Aloca um array para guardar os ponteiros das linhas (buffer circular)
    char** buffer = calloc(opts->num_lines, sizeof(char*));
    int* line_numbers = calloc(opts->num_lines, sizeof(int));

    if (!buffer || !line_numbers)
    {
        die("falha ao alocar memória para o buffer");
    }

    char* current_line = NULL;
    int total_lines_read = 0;
    int index = 0;

    // Lê o ficheiro inteiro, guardando apenas as últimas N linhas
    while ((current_line = line_read(fp)) != NULL)
    {
        total_lines_read++;

        // Se a posição já tinha uma linha (buffer cheio), liberta a memória
        // antiga
        if (buffer[index])
        {
            free(buffer[index]);
        }

        buffer[index] = current_line;
        line_numbers[index] = total_lines_read;

        // Avança o índice de forma circular
        index = (index + 1) % opts->num_lines;
    }

    // Determina onde começar a imprimir (a linha mais antiga no buffer)
    int start_index = total_lines_read > opts->num_lines ? index : 0;
    int lines_to_print =
        total_lines_read > opts->num_lines ? opts->num_lines : total_lines_read;

    for (int i = 0; i < lines_to_print; i++)
    {
        int current_idx = (start_index + i) % opts->num_lines;

        if (opts->show_line_num)
        {
            printf("%d ", line_numbers[current_idx]);
        }

        printf("%s", buffer[current_idx]);

        if (opts->show_end_line)
        {
            printf("$");
        }
        printf("\n");

        free(buffer[current_idx]);
    }

    free(buffer);
    free(line_numbers);
}

/**
 * @brief Ponto de entrada do utilitário tail.
 */
int main(int argc, char* argv[])
{
    program_name = argv[0];
    TailOptions opts = {10, false, false};

    // Parsing para suportar -NUM e flags padrão
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
            if (opts.num_lines <= 0)
                opts.num_lines = 1;
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
        process_tail(stdin, "stdin", &opts, false);
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
            process_tail(fp, argv[i], &opts, num_files > 1);
            if (i < argc - 1)
                printf("\n");
            fclose(fp);
        }
    }

    return EXIT_SUCCESS;
}
