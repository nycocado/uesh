#define _XOPEN_SOURCE 700
#include "common.h"
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGS 64

/**
 * @brief Exibe a ajuda integrada do utilitário UEsh.
 */
static void print_usage(void)
{
    printf("Uso: %s\n", program_name);
    printf("Objetivo: Inicia a Shell (Interpretador de comandos) com suporte a "
           "comandos externos e internos. Além de apresentar informações "
           "relevantes como utilizador e nome do diretório corrente.\n");
    printf("cd <nome-diretório>: Altera o diretório corrente.\n");
    printf("pwd: Mostra o diretório corrente.\n");
    printf("echo <string>: Escreve no ecrã a string indicada.\n");
    printf("sleep <num>: Para a execução durante 'num' segundos.\n");
    printf("exit: Sai do shell.\n");
    printf("-h: Apresenta esta ajuda e sai imediatamente.\n");
}

/**
 * @brief Adiciona dinamicamente a pasta raiz dos executáveis ao PATH da shell.
 * @param executable_path O caminho argv[0] originado na execução.
 */
static void setup_environment(const char* executable_path)
{
    char exe_dir[PATH_MAX];
    if (realpath(executable_path, exe_dir) != NULL)
    {
        char* last_slash = strrchr(exe_dir, '/');
        if (last_slash != NULL)
        {
            *last_slash = '\0';

            const char* old_path = getenv("PATH");
            if (old_path)
            {
                size_t new_len = strlen(exe_dir) + strlen(old_path) + 2;
                char* new_path = malloc(new_len);
                if (new_path)
                {
                    snprintf(new_path, new_len, "%s:%s", exe_dir, old_path);
                    setenv("PATH", new_path, 1);
                    free(new_path);
                }
            }
            else
            {
                setenv("PATH", exe_dir, 1);
            }
        }
    }
}

/**
 * @brief Constroi e imprime o prompt visual da shell.
 */
static void print_prompt(void)
{
    char cwd[PATH_MAX];
    char hostname[HOST_NAME_MAX];
    char* username = getlogin();

    if (gethostname(hostname, sizeof(hostname)) != 0)
    {
        strncpy(hostname, "unknown", sizeof(hostname));
    }

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        strncpy(cwd, "?", sizeof(cwd));
    }

    printf("%s@%s (%s) > ", username ? username : "user", hostname, cwd);
    fflush(stdout);
}

/**
 * @brief Lê de forma segura uma linha inteira fornecida pelo utilizador usando
 * line_read.
 * @param line Ponteiro para o ponteiro da string.
 * @return true caso tenha lido com sucesso; false caso encontre EOF.
 */
static bool read_line(char** line)
{
    if (*line)
    {
        free(*line);
    }
    *line = line_read(stdin);
    return (*line != NULL);
}

/**
 * @brief Quebra a string completa capturada em múltiplos tokens.
 * @param line A string contendo o texto cru do terminal.
 * @param args O array de ponteiros que receberão os tokens quebrados.
 * @return O número de tokens encontrados.
 */
static int parse_line(char* line, char** args)
{
    int i = 0;
    args[i] = strtok(line, " ");
    while (args[i] != NULL && i < MAX_ARGS - 1)
    {
        args[++i] = strtok(NULL, " ");
    }
    args[i] = NULL;
    return i;
}

/**
 * @brief Altera o diretório de trabalho atual.
 * @param args Array de argumentos (args[1] é o destino).
 */
static void builtin_cd(char** args)
{
    const char* dir = args[1] ? args[1] : getenv("HOME");
    if (dir && chdir(dir) != 0)
    {
        warn("cd");
    }
}

/**
 * @brief Exibe o diretório de trabalho atual.
 */
static void builtin_pwd(void)
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
    }
    else
    {
        warn("pwd");
    }
}

/**
 * @brief Imprime texto no ecrã.
 * @param args Array de strings a serem impressas.
 */
static void builtin_echo(char** args)
{
    for (int j = 1; args[j] != NULL; j++)
    {
        printf("%s%s", args[j], args[j + 1] ? " " : "");
    }
    printf("\n");
}

/**
 * @brief Suspende a execução por um determinado tempo.
 * @param args Array contendo os segundos em args[1].
 */
static void builtin_sleep(char** args)
{
    if (args[1] != NULL)
    {
        sleep(atoi(args[1]));
    }
    else
    {
        fprintf(
            stderr, "%s: sleep: faltou indicar os segundos\n", program_name
        );
    }
}

/**
 * @brief Verifica e processa comandos built-in (cd, pwd, echo, sleep, exit).
 * @param args A lista de argumentos do input de usuário.
 * @param running Ponteiro para flag de execução do loop principal.
 * @return true se era comando interno, false se era comando externo.
 */
static bool execute_builtin(char** args, bool* running)
{
    if (strcmp(args[0], "exit") == 0)
    {
        *running = false;
        return true;
    }
    if (strcmp(args[0], "cd") == 0)
    {
        builtin_cd(args);
        return true;
    }
    if (strcmp(args[0], "pwd") == 0)
    {
        builtin_pwd();
        return true;
    }
    if (strcmp(args[0], "echo") == 0)
    {
        builtin_echo(args);
        return true;
    }
    if (strcmp(args[0], "sleep") == 0)
    {
        builtin_sleep(args);
        return true;
    }
    return false;
}

/**
 * @brief Invoca processos externos via fork e execvp.
 * @param args A lista de argumentos contendo o utilitário em args[0].
 */
static void execute_external(char** args)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
        {
            fprintf(
                stderr,
                "%s: comando não encontrado: %s\n",
                program_name,
                args[0]
            );
            exit(EXIT_FAILURE);
        }
    }
    else if (pid < 0)
    {
        die("fork");
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
}

/**
 * @brief Ponto central de inicialização do projeto e loop eterno da interface
 * CLI.
 * @param argc O número de argumentos vindos do bash inicial.
 * @param argv Os apontadores de char contendo as flags recebidas.
 * @return Estado de execução retornado ao fim do loop.
 */
int main(int argc, char* argv[])
{
    program_name = argv[0];

    if (argc > 1)
    {
        if (strcmp(argv[1], "-h") == 0)
        {
            print_usage();
            return EXIT_SUCCESS;
        }
        else
        {
            usage("");
        }
    }

    setup_environment(argv[0]);

    char* line = NULL;
    char* args[MAX_ARGS];
    bool running = true;

    while (running)
    {
        print_prompt();

        if (!read_line(&line))
        {
            printf("\n");
            break;
        }

        if (parse_line(line, args) == 0)
        {
            continue;
        }

        if (!execute_builtin(args, &running))
        {
            execute_external(args);
        }
    }

    free(line);
    return EXIT_SUCCESS;
}
