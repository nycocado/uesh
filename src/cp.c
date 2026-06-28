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
 * @brief Prints the built-in help for the cp utility (-h flag).
 */
static void print_usage(void)
{
    printf("Uso: %s [OPÇÕES] ORIGEM DESTINO\n", program_name);
    printf("Objetivo: Efetua a cópia de ficheiros.\n");
    printf("-i: Modo interativo - pergunta antes de apagar o destino caso "
           "ele exista.\n");
    printf("-h: Apresenta esta ajuda e sai imediatamente.\n");
}

/**
 * @brief Checks whether a file exists on the filesystem.
 * @param filepath Path to the file to check.
 * @return true if the file exists, false otherwise.
 */
static bool file_exists(const char* filepath)
{
    struct stat buffer;
    return (stat(filepath, &buffer) == 0);
}

/**
 * @brief Asks the user whether to overwrite an existing file (interactive
 * mode).
 * @param filepath Destination filename.
 * @return true if the user answers 'y' or 'Y', false otherwise.
 */
static bool ask_overwrite(const char* filepath)
{
    printf("%s: reescrever '%s'? ", program_name, filepath);
    fflush(stdout);

    char response = getchar();

    // flush the rest of the input line to avoid stale input on next reads
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;

    return (response == 'y' || response == 'Y');
}

/**
 * @brief Copies data from source to destination.
 * @param src_path Source file path.
 * @param dst_path Destination file path.
 * @return true if the copy succeeded, false on error.
 */
static bool copy_file(const char* src_path, const char* dst_path)
{
    int fd_src = open(src_path, O_RDONLY);
    if (fd_src < 0)
    {
        warn(src_path);
        return false;
    }

    // open destination for writing; O_CREAT creates it (0644) if absent,
    // O_TRUNC clears existing content.
    int fd_dst = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dst < 0)
    {
        warn(dst_path);
        close(fd_src);
        return false;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;

    // read-write loop in chunks
    while ((bytes_read = read(fd_src, buffer, BUFFER_SIZE)) > 0)
    {
        ssize_t total_written = 0;

        // ensure every byte of the read chunk is written (important for large
        // files)
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
 * @brief Entry point for the cp utility.
 */
int main(int argc, char* argv[])
{
    program_name = argv[0];
    bool interactive = false;

    char opt;
    while ((opt = next_option(argc, argv, "ih")) != '\0')
    {
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

    // ensure exactly SOURCE and DEST are provided (2 required arguments)
    if (argc - opt_index != 2)
    {
        usage("[-i] ORIGEM DESTINO");
    }

    const char* src_path = argv[opt_index];
    const char* dst_path = argv[opt_index + 1];

    // if interactive mode is on and the destination exists, ask the user
    if (interactive && file_exists(dst_path))
    {
        if (!ask_overwrite(dst_path))
        {
            // user declined; exit successfully without doing anything
            return EXIT_SUCCESS;
        }
    }

    // perform the copy and check for errors
    if (!copy_file(src_path, dst_path))
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
