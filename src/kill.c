#define _POSIX_C_SOURCE 200809L
#include "common.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @brief Exibe a ajuda integrada do utilitário kill (requisito -h).
 */
static void print_usage(void)
{
    printf("Uso: %s PID\n", program_name);
    printf("Objetivo: Termina um processo ativo no sistema operacional através "
           "do envio de um sinal SIGTERM.\n");
    printf("-h: Apresenta esta ajuda e sai imediatamente.\n");
}

/**
 * @brief Ponto de entrada do utilitário kill.
 */
int main(int argc, char** argv)
{
    program_name = argv[0];

    // Verifica se foi passado pelo menos um argumento (o PID)
    if (argc < 2)
    {
        usage("PID");
    }

    // Suporte ao requisito global -h
    if (argv[1][0] == '-' && argv[1][1] == 'h')
    {
        print_usage();
        return EXIT_SUCCESS;
    }

    // Converte o argumento para PID
    pid_t pid = (pid_t)atoi(argv[1]);
    if (pid <= 0)
    {
        error_msg("PID inválido. Deve ser um número inteiro positivo.");
    }

    // Envia o sinal SIGTERM (término amigável) ao processo
    if (kill(pid, SIGTERM) == -1)
    {
        // die utiliza perror internamente para detalhar a falha (ex: No such
        // process)
        die("kill");
    }

    return EXIT_SUCCESS;
}
