#include "common.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

/**
 * @brief Exibe a ajuda integrada do utilitário cp (requisito -h).
 */
static void print_usage(void)
{
    printf("Uso: %s [OPÇÕES] ORIGEM DESTINO\n", program_name);
    printf("Objetivo: Efetua a cópia de ficheiros.\n");
    printf("Opções:\n");
    printf("  -i : modo interativo - pergunta antes de apagar o destino caso "
           "ele exista.\n");
    printf("  -h : apresenta esta ajuda e sai imediatamente.\n");
}

/**
 * @brief Verifica se um ficheiro já existe no sistema.
 * @param filepath Caminho do ficheiro a verificar.
 * @return true se o ficheiro existir, false caso contrário.
 */
static bool file_exists(const char* filepath)
{
    struct stat buffer;
    return (stat(filepath, &buffer) == 0);
}

/**
 * @brief Pergunta ao utilizador se deseja reescrever um ficheiro existente
 * (modo interativo).
 * @param filepath Nome do ficheiro destino.
 * @return true se o utilizador responder 'y' ou 'Y', false caso contrário.
 */
static bool ask_overwrite(const char* filepath)
{
    printf("%s: reescrever '%s'? ", program_name, filepath);
    fflush(stdout);

    char response = getchar();

    // Limpa o resto da linha no buffer de entrada para evitar bugs nas próximas
    // leituras
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;

    return (response == 'y' || response == 'Y');
}

/**
 * @brief Realiza a cópia física dos dados entre a origem e o destino.
 * @param src_path Caminho do ficheiro original.
 * @param dst_path Caminho do ficheiro destino.
 * @return true se a cópia foi bem sucedida, false em caso de erro.
 */
static bool copy_file(const char* src_path, const char* dst_path)
{
    int fd_src = open(src_path, O_RDONLY);
    if (fd_src < 0)
    {
        warn(src_path);
        return false;
    }

    // Abre o destino em modo de escrita.
    // Se não existir (O_CREAT), cria com permissões rw-r--r-- (0644).
    // Se existir (O_TRUNC), apaga o conteúdo antigo.
    int fd_dst = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dst < 0)
    {
        warn(dst_path);
        close(fd_src);
        return false;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;

    // Loop de leitura e escrita em blocos
    while ((bytes_read = read(fd_src, buffer, BUFFER_SIZE)) > 0)
    {
        ssize_t total_written = 0;

        // Garante que todo o bloco lido é escrito (importante para ficheiros
        // grandes)
        while (total_written < bytes_read)
        {
            bytes_written = write(
                fd_dst, buffer + total_written, bytes_read - total_written
            );
            if (bytes_written < 0)
            {
                warn(dst_path);
                close(fd_src);
                close(fd_dst);
                return false;
            }
            total_written += bytes_written;
        }
    }

    if (bytes_read < 0)
    {
        warn(src_path);
    }

    close(fd_src);
    close(fd_dst);

    return (bytes_read >= 0);
}

/**
 * @brief Ponto de entrada do utilitário cp.
 */
int main(int argc, char* argv[])
{
    program_name = argv[0];
    bool interactive = false;

    // Parsing para suportar as flags padrão
    while (opt_index < argc && argv[opt_index][0] == '-' &&
           strcmp(argv[opt_index], "-") != 0)
    {
        if (strcmp(argv[opt_index], "--") == 0)
        {
            opt_index++;
            break;
        }

        char opt = next_option(argc, argv, "ih");
        if (opt == '\0')
            break;

        switch (opt)
        {
            case 'i':
                interactive = true;
                break;
            case 'h':
                print_usage();
                return EXIT_SUCCESS;
            default:
                usage("[-i] ORIGEM DESTINO");
        }
    }

    // Verifica se temos exatamente ORIGEM e DESTINO (2 argumentos obrigatórios)
    if (argc - opt_index != 2)
    {
        usage("[-i] ORIGEM DESTINO");
    }

    const char* src_path = argv[opt_index];
    const char* dst_path = argv[opt_index + 1];

    // Se o modo interativo estiver ativo e o destino já existir, pergunta ao
    // utilizador
    if (interactive && file_exists(dst_path))
    {
        if (!ask_overwrite(dst_path))
        {
            // O utilizador disse não. Termina com sucesso sem fazer nada.
            return EXIT_SUCCESS;
        }
    }

    // Efetua a cópia e verifica se ocorreu erro
    if (!copy_file(src_path, dst_path))
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
