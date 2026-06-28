#include "common.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @struct Line
 * @brief Represents a single line of text.
 */
typedef struct
{
        char* text;
} Line;

/**
 * @struct SortOptions
 * @brief Configuration options for the sort utility.
 */
typedef struct
{
        bool reverse; // -d: descending sort
} SortOptions;

/**
 * @brief Prints the built-in help for the sort utility (-h flag).
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
 * @brief Comparator for ascending order (strcmp).
 */
static int compare_asc(const void* a, const void* b)
{
    return strcmp(((Line*)a)->text, ((Line*)b)->text);
}

/**
 * @brief Comparator for descending order (reversed strcmp).
 */
static int compare_desc(const void* a, const void* b)
{
    return strcmp(((Line*)b)->text, ((Line*)a)->text);
}

/**
 * @brief Sorts a single file.
 * @param filename Name of the file to sort.
 * @param opts Configuration options.
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

    // load all lines into memory dynamically
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

    // in-place sort using qsort
    qsort(
        lines, count, sizeof(Line), opts->reverse ? compare_desc : compare_asc
    );

    // build the output filename (original.sort)
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

    // free memory
    for (int i = 0; i < count; i++)
    {
        free(lines[i].text);
    }
    free(lines);
}

/**
 * @brief Entry point for the sort utility.
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
