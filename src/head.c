#include "common.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @struct HeadOptions
 * @brief Configuration options for the head utility.
 */
typedef struct
{
        int num_lines;      // number of lines to display (default 10)
        bool show_line_num; // -n: show line number
        bool show_end_line; // -E: show $ at end of each line
} HeadOptions;

/**
 * @brief Prints the built-in help for the head utility (-h flag).
 */
static void print_usage(void)
{
    printf("Uso: %s [OPÇÕES] [FICHEIRO]...\n", program_name);
    printf("Objetivo: Lista as primeiras linhas de um ficheiro de texto para o "
           "stdout.\n");
    printf("-n: Lista número da linha.\n");
    printf("-E: Indica fim de linha ($ no fim de cada linha).\n");
    printf("-\"numero\": Lista as primeiras \"número\" de linhas (ex: -20).\n");
    printf("-h: Apresenta esta ajuda e sai imediatamente.\n");
}

/**
 * @brief Processes the head command on an input stream.
 * @param fp Pointer to the file or stdin.
 * @param filename Filename (for the header line).
 * @param opts Configuration options.
 * @param print_header Whether to print the filename header (multiple files).
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
 * @brief Entry point for the head utility.
 */
int main(int argc, char* argv[])
{
    program_name = argv[0];
    HeadOptions opts = {10, false, false};

    char opt;
    while ((opt = next_option(argc, argv, "nEh#")) != '\0')
    {
        switch (opt)
        {
            case '#':
                opts.num_lines = atoi(opt_arg);
                if (opts.num_lines <= 0)
                    opts.num_lines = 1;
                break;
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
