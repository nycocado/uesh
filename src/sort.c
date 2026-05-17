#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char* text;
} Line;

static void print_usage(void)
{
    printf("Uso: %s [OPÇÕES] [FICHEIRO]...\n", program_name);
    printf("Objetivo: Ordena ficheiros de texto. Por padrão, ordena crescentemente e grava em FICHEIRO.sort.\n");
    printf("Opções:\n");
    printf("  -d    : ordenação decrescente\n");
    printf("  -h    : apresenta esta ajuda e sai imediatamente\n");
}

static int compare_asc(const void* a, const void* b)
{
    return strcmp(((Line*)a)->text, ((Line*)b)->text);
}

static int compare_desc(const void* a, const void* b)
{
    return strcmp(((Line*)b)->text, ((Line*)a)->text);
}

static void sort_file(const char* filename, bool reverse)
{
    FILE* fp = fopen(filename, "r");
    if (!fp)
    {
        warn(filename);
        return;
    }

    Line* lines = NULL;
    int count = 0;
    char* l;

    while ((l = line_read(fp)) != NULL)
    {
        lines = realloc(lines, sizeof(Line) * (count + 1));
        lines[count].text = l;
        count++;
    }
    fclose(fp);

    qsort(lines, count, sizeof(Line), reverse ? compare_desc : compare_asc);

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

    for (int i = 0; i < count; i++)
    {
        free(lines[i].text);
    }
    free(lines);
}

int main(int argc, char** argv)
{
    program_name = argv[0];
    bool reverse = false;
    char opt;

    while ((opt = next_option(argc, argv, "dh")) != '\0')
    {
        switch (opt)
        {
            case 'd':
                reverse = true;
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
        sort_file(argv[i], reverse);
    }

    return EXIT_SUCCESS;
}
