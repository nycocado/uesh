#include "common.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @struct TailOptions
 * @brief Configuration options for the tail utility.
 */
typedef struct
{
        int num_lines;      // number of lines to display (default 10)
        bool show_line_num; // -n: show original line number
        bool show_end_line; // -E: show $ at end of each line
} TailOptions;

/**
 * @brief Prints the built-in help for the tail utility (-h flag).
 */
static void print_usage(void)
{
    printf("Uso: %s [OPÇÕES] [FICHEIRO]...\n", program_name);
    printf("Objetivo: Lista as últimas linhas de um ficheiro de texto para o "
           "stdout.\n");
    printf("-n: Lista número da linha.\n");
    printf("-E: Indica fim de linha ($ no fim de cada linha).\n");
    printf("-\"numero\": Lista as últimas \"número\" de linhas (ex: -20).\n");
    printf("-h: Apresenta esta ajuda e sai imediatamente.\n");
}

/**
 * @brief Processes the tail command on an input stream using a circular buffer.
 * @param fp Pointer to the file or stdin.
 * @param filename Filename (for the header line).
 * @param opts Configuration options.
 * @param print_header Whether to print the filename header (multiple files).
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

    // allocate array to hold line pointers (circular buffer)
    char** buffer = calloc(opts->num_lines, sizeof(char*));
    int* line_numbers = calloc(opts->num_lines, sizeof(int));

    if (!buffer || !line_numbers)
    {
        die("falha ao alocar memória para o buffer");
    }

    char* current_line = NULL;
    int total_lines_read = 0;
    int index = 0;

    // read the entire file, keeping only the last N lines
    while ((current_line = line_read(fp)) != NULL)
    {
        total_lines_read++;

        // if this slot already held a line (buffer full), free the old memory
        if (buffer[index])
        {
            free(buffer[index]);
        }

        buffer[index] = current_line;
        line_numbers[index] = total_lines_read;

        // advance index circularly
        index = (index + 1) % opts->num_lines;
    }

    // find the oldest entry in the buffer to start printing from
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
 * @brief Entry point for the tail utility.
 */
int main(int argc, char* argv[])
{
    program_name = argv[0];
    TailOptions opts = {10, false, false};

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
