#define _GNU_SOURCE
#include "common.h"
#include <ctype.h>
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @struct ReplaceOptions
 * @brief Agrupa as opções de configuração do utilitário replace.
 */
typedef struct
{
        bool report_count; // -c: Reportar número de substituições realizadas
        bool show_line_num;    // -n: Indicar número da linha afetada
        bool case_insensitive; // -i: Ignorar maiúsculas/minúsculas
} ReplaceOptions;

/**
 * @brief Exibe a ajuda integrada do utilitário replace (requisito -h).
 */
static void print_usage(void)
{
    printf(
        "Uso: %s [OPÇÕES] STRING-ORG STRING-DEST [FICHEIROS]\n", program_name
    );
    printf("Objetivo: Procura uma string dentro de ficheiros e substitui por "
           "outra, processando os ficheiros paralelamente (via fork).\n");
    printf("-c: Informa apenas o número de substituições realizadas.\n");
    printf("-n: Indica o número da linha, seguido de \":\", e a linha onde "
           "houve a substituição.\n");
    printf("-i: Ignora maiúsculas e minúsculas durante a busca.\n");
    printf("-h: Apresenta esta ajuda e sai imediatamente.\n");
}

/**
 * @brief Realiza a busca case-insensitive de uma substring.
 *
 * @param haystack A string onde será feita a busca.
 * @param needle A substring a ser encontrada.
 * @return Um ponteiro para o início da substring encontrada, ou NULL se não
 * encontrada.
 */
static char* strcasestr_custom(const char* haystack, const char* needle)
{
    if (!*needle)
        return (char*)haystack;
    for (; *haystack; haystack++)
    {
        if (toupper((unsigned char)*haystack) ==
            toupper((unsigned char)*needle))
        {
            const char* h = haystack;
            const char* n = needle;
            while (*h && *n &&
                   toupper((unsigned char)*h) == toupper((unsigned char)*n))
            {
                h++;
                n++;
            }
            if (!*n)
                return (char*)haystack;
        }
    }
    return NULL;
}

/**
 * @brief Processa a substituição de strings em um único ficheiro.
 * @param filename O nome do ficheiro a processar.
 * @param old_str A string original a ser substituída.
 * @param new_str A nova string.
 * @param opts Opções de configuração.
 */
static void process_file(
    const char* filename,
    const char* old_str,
    const char* new_str,
    const ReplaceOptions* opts
)
{
    FILE* fp = fopen(filename, "r");
    if (!fp)
    {
        warn(filename);
        exit(EXIT_FAILURE);
    }

    char tmp_name[PATH_MAX];
    snprintf(tmp_name, sizeof(tmp_name), "%s.tmp.XXXXXX", filename);
    int fd = mkstemp(tmp_name);
    if (fd == -1)
    {
        warn("erro ao criar ficheiro temporário");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    FILE* tmp_fp = fdopen(fd, "w");
    if (!tmp_fp)
    {
        warn("erro ao abrir ficheiro temporário");
        close(fd);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    char* line = NULL;
    size_t len = 0;
    int line_num = 0;
    int total_subs = 0;

    while (getline(&line, &len, fp) != -1)
    {
        line_num++;
        char* current_pos = line;
        bool line_modified = false;

        while (true)
        {
            char* pos;
            if (opts->case_insensitive)
            {
                pos = strcasestr_custom(current_pos, old_str);
            }
            else
            {
                pos = strstr(current_pos, old_str);
            }

            if (!pos)
                break;

            line_modified = true;
            total_subs++;

            fwrite(current_pos, 1, pos - current_pos, tmp_fp);
            fputs(new_str, tmp_fp);

            current_pos = pos + strlen(old_str);
        }

        fputs(current_pos, tmp_fp);

        if (line_modified && opts->show_line_num)
        {
            printf("[%s] %d:%s", filename, line_num, line);
        }
    }

    fclose(fp);
    fclose(tmp_fp);
    free(line);

    if (rename(tmp_name, filename) != 0)
    {
        warn("erro ao substituir ficheiro original");
        exit(EXIT_FAILURE);
    }

    if (opts->report_count)
    {
        printf("[%s] %d substituições realizadas\n", filename, total_subs);
    }
}

/**
 * @brief Ponto de entrada do utilitário replace.
 * @param argc O número de argumentos vindos do terminal.
 * @param argv Os apontadores de char contendo as flags e argumentos.
 * @return EXIT_SUCCESS se concluído com sucesso, EXIT_FAILURE caso contrário.
 */
int main(int argc, char* argv[])
{
    program_name = argv[0];
    int opt;
    ReplaceOptions opts = {0};

    while ((opt = getopt(argc, argv, "cnih")) != -1)
    {
        switch (opt)
        {
            case 'c':
                opts.report_count = true;
                break;
            case 'n':
                opts.show_line_num = true;
                break;
            case 'i':
                opts.case_insensitive = true;
                break;
            case 'h':
                print_usage();
                return EXIT_SUCCESS;
            default:
                usage("[OPÇÕES] STRING-ORG STRING-DEST [FICHEIROS]");
        }
    }

    if (argc - optind < 2)
    {
        usage("[OPÇÕES] STRING-ORG STRING-DEST [FICHEIROS]");
    }

    const char* old_str = argv[optind++];
    const char* new_str = argv[optind++];
    int num_files = argc - optind;

    if (num_files == 0)
    {
        usage("[OPÇÕES] STRING-ORG STRING-DEST [FICHEIROS]");
    }

    int children_created = 0;
    for (int i = optind; i < argc; i++)
    {
        pid_t pid = fork();

        if (pid == 0)
        {
            process_file(argv[i], old_str, new_str, &opts);
            exit(EXIT_SUCCESS);
        }
        else if (pid < 0)
        {
            warn("fork");
        }
        else
        {
            children_created++;
        }
    }

    int status;
    bool all_success = true;
    for (int i = 0; i < children_created; i++)
    {
        wait(&status);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        {
            all_success = false;
        }
    }

    return all_success ? EXIT_SUCCESS : EXIT_FAILURE;
}
