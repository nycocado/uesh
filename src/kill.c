#define _POSIX_C_SOURCE 200809L
#include "common.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

static void print_usage(void)
{
    printf("Uso: %s PID\n", program_name);
    printf("Objetivo: Termina um processo ativo no sistema operacional.\n");
    printf("Opções:\n");
    printf("  -h    : apresenta esta ajuda e sai imediatamente\n");
}

int main(int argc, char** argv)
{
    program_name = argv[0];

    if (argc < 2)
    {
        usage("PID");
    }

    if (argv[1][0] == '-' && argv[1][1] == 'h')
    {
        print_usage();
        return EXIT_SUCCESS;
    }

    pid_t pid = (pid_t)atoi(argv[1]);
    if (pid <= 0)
    {
        error_msg("PID inválido");
    }

    if (kill(pid, SIGTERM) == -1)
    {
        die("kill");
    }

    return EXIT_SUCCESS;
}
