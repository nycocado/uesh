#define _DEFAULT_SOURCE
#include "common.h"
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/**
 * @struct FileInfo
 * @brief Metadados básicos de um ficheiro para ordenação e listagem.
 */
typedef struct
{
        char name[256];
        off_t size;
        time_t mtime;
} FileInfo;

/**
 * @struct LsOptions
 * @brief Opções de configuração do utilitário ls.
 */
typedef struct
{
        bool long_format; // -l: Listagem detalhada
        char sort_by;     // 'n': nome, 's': tamanho, 'd': data
        bool columns;     // -c: Listagem em colunas
} LsOptions;

// Variável global auxiliar para o comparador do qsort (evita qsort_r
// não-standard)
static char current_sort_mode = 'n';

/**
 * @brief Exibe a ajuda integrada do utilitário ls (requisito -h).
 */
static void print_usage(void)
{
    printf("Uso: %s [OPÇÕES] [CAMINHO]\n", program_name);
    printf("Objetivo: Lista os nomes dos ficheiros contidos no diretório "
           "indicado no CAMINHO.\n");
    printf("-l: Listagem longa com indicação de nome, dimensão e data.\n");
    printf("-on: Ordena o output por nome.\n");
    printf("-os: Ordena o output por dimensão.\n");
    printf("-od: Ordena o output por data.\n");
    printf("-c: Listagem por colunas.\n");
    printf("-h: Apresenta esta ajuda e sai imediatamente.\n");
}

/**
 * @brief Função de comparação para o qsort baseada no critério selecionado.
 */
static int compare_files(const void* a, const void* b)
{
    const FileInfo* fa = (const FileInfo*)a;
    const FileInfo* fb = (const FileInfo*)b;

    if (current_sort_mode == 's')
    {
        if (fb->size > fa->size)
            return 1;
        if (fb->size < fa->size)
            return -1;
        return 0;
    }
    else if (current_sort_mode == 'd')
    {
        if (fb->mtime > fa->mtime)
            return 1;
        if (fb->mtime < fa->mtime)
            return -1;
        return 0;
    }
    return strcmp(fa->name, fb->name);
}

/**
 * @brief Imprime metadados detalhados de um ficheiro (formato longo).
 * @param path Caminho do diretório pai.
 * @param name Nome do ficheiro.
 */
static void print_long(const char* path, const char* name)
{
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", path, name);

    struct stat st;
    if (stat(full_path, &st) == -1)
    {
        warn(name);
        return;
    }

    struct passwd* pw = getpwuid(st.st_uid);
    struct group* gr = getgrgid(st.st_gid);
    char time_buf[64];
    strftime(
        time_buf, sizeof(time_buf), "%b %d %H:%M", localtime(&st.st_mtime)
    );

    printf(
        "%s %s %8ld %s %s\n",
        pw ? pw->pw_name : "unknown",
        gr ? gr->gr_name : "unknown",
        (long)st.st_size,
        time_buf,
        name
    );
}

/**
 * @brief Lista o conteúdo de um diretório aplicando ordenação e formatação.
 * @param path Caminho do diretório.
 * @param opts Opções configuradas.
 */
static void list_dir(const char* path, const LsOptions* opts)
{
    DIR* dir = opendir(path);
    if (!dir)
    {
        warn(path);
        return;
    }

    FileInfo* files = NULL;
    int count = 0;
    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL)
    {
        // Ignorar ficheiros ocultos por padrão
        if (entry->d_name[0] == '.')
            continue;

        FileInfo* tmp = realloc(files, sizeof(FileInfo) * (count + 1));
        if (!tmp)
        {
            closedir(dir);
            die("erro de memória no ls");
        }
        files = tmp;

        strncpy(files[count].name, entry->d_name, 255);
        files[count].name[255] = '\0';

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        struct stat st;
        if (stat(full_path, &st) == 0)
        {
            files[count].size = st.st_size;
            files[count].mtime = st.st_mtime;
        }
        else
        {
            files[count].size = 0;
            files[count].mtime = 0;
        }
        count++;
    }
    closedir(dir);

    current_sort_mode = opts->sort_by;
    if (count > 0)
    {
        qsort(files, count, sizeof(FileInfo), compare_files);
    }

    for (int i = 0; i < count; i++)
    {
        if (opts->long_format)
        {
            print_long(path, files[i].name);
        }
        else if (opts->columns)
        {
            printf("%-20s", files[i].name);
            if ((i + 1) % 4 == 0)
                printf("\n");
        }
        else
        {
            printf("%s\n", files[i].name);
        }
    }

    if (opts->columns && count % 4 != 0)
        printf("\n");

    free(files);
}

/**
 * @brief Ponto de entrada do utilitário ls.
 */
int main(int argc, char** argv)
{
    program_name = argv[0];
    LsOptions opts = {false, 'n', false};
    char opt;

    while ((opt = next_option(argc, argv, "lo:ch")) != '\0')
    {
        switch (opt)
        {
            case 'l':
                opts.long_format = true;
                break;
            case 'o':
                if (opt_arg)
                {
                    char mode = opt_arg[0];
                    if (mode == 'n' || mode == 's' || mode == 'd')
                    {
                        opts.sort_by = mode;
                    }
                }
                break;
            case 'c':
                opts.columns = true;
                break;
            case 'h':
                print_usage();
                return EXIT_SUCCESS;
            case ':':
                usage("Falta argumento para a opção -o");
                break;
            default:
                usage("[-l] [-ox] [-c] [CAMINHO]");
        }
    }

    const char* path = (opt_index < argc) ? argv[opt_index] : ".";
    list_dir(path, &opts);

    return EXIT_SUCCESS;
}
