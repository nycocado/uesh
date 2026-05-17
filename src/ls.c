#define _GNU_SOURCE
#include "common.h"
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

typedef struct
{
    char name[256];
    off_t size;
    time_t mtime;
} FileInfo;

typedef struct
{
    bool long_format;
    char sort_by; // 'n' name, 's' size, 'd' date
    bool columns;
} LsOptions;

static void print_usage(void)
{
    printf("Uso: %s [OPÇÕES] [CAMINHO]\n", program_name);
    printf("Objetivo: Lista os nomes dos ficheiros contidos no diretório indicado no CAMINHO.\n");
    printf("Opções:\n");
    printf("  -l    : listagem longa com indicação de nome, dimensão e data do ficheiro\n");
    printf("  -on   : ordena por nome (padrão)\n");
    printf("  -os   : ordena por dimensão\n");
    printf("  -od   : ordena por data\n");
    printf("  -c    : listagem por colunas\n");
    printf("  -h    : apresenta esta ajuda e sai imediatamente\n");
}

static int compare_files(const void* a, const void* b, void* sort_by)
{
    const FileInfo* fa = (const FileInfo*)a;
    const FileInfo* fb = (const FileInfo*)b;
    char mode = *(char*)sort_by;

    if (mode == 's')
    {
        return (fb->size - fa->size); // Descendente por tamanho
    }
    else if (mode == 'd')
    {
        return (fb->mtime - fa->mtime); // Descendente por data
    }
    return strcmp(fa->name, fb->name); // Ascendente por nome
}

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
        st.st_size,
        time_buf,
        name
    );
}

static void list_dir(const char* path, const LsOptions* opts)
{
    DIR* dir = opendir(path);
    if (!dir)
    {
        die(path);
    }

    FileInfo* files = NULL;
    int count = 0;
    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue;

        files = realloc(files, sizeof(FileInfo) * (count + 1));
        strncpy(files[count].name, entry->d_name, 255);

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        struct stat st;
        if (stat(full_path, &st) == 0)
        {
            files[count].size = st.st_size;
            files[count].mtime = st.st_mtime;
        }
        count++;
    }
    closedir(dir);

    // Qsort_r não é padrão em todo o lado, usamos qsort simples com variável estática ou passamos o modo
    char sort_mode = opts->sort_by;
    qsort_r(files, count, sizeof(FileInfo), (int (*)(const void *, const void *, void *))compare_files, &sort_mode);

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
                // next_option não suporta argumentos de opção facilmente na sua lib.
                // Vou assumir que o usuário digita -on, -os, -od colado.
                // Como a sua next_option processa char a char:
                // Se o usuário passar -ox, o 'o' é capturado aqui, e o 'x' será o próximo.
                // Precisamos de uma pequena correção na lógica para ler o próximo char.
                if (argv[opt_index][2] != '\0') {
                    opts.sort_by = argv[opt_index][2];
                    // Avança para o próximo argumento pois consumimos o sub-char manualmente
                    opt_index++;
                }
                break;
            case 'c':
                opts.columns = true;
                break;
            case 'h':
                print_usage();
                return EXIT_SUCCESS;
            default:
                usage("[-l] [-ox] [-c] [CAMINHO]");
        }
    }

    const char* path = (opt_index < argc) ? argv[opt_index] : ".";
    list_dir(path, &opts);

    return EXIT_SUCCESS;
}
