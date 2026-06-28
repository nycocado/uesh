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
 * @brief Prints the built-in help for the UEsh shell (-h flag).
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
 * @brief Dynamically prepends the executable directory to PATH.
 * @param executable_path The argv[0] path passed at startup.
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
 * @brief Builds and prints the shell prompt.
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
 * @brief Safely reads a full line from the user via line_read.
 * @param line Pointer to the string pointer.
 * @return true on success; false on EOF.
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
 * @brief Splits the raw input line into tokens.
 * @param line The raw input string from the terminal.
 * @param args Array of pointers to receive the parsed tokens.
 * @return Number of tokens found.
 */
static int parse_line(char* line, char** args)
{
    int i = 0;
    args[i] = strtok(line, " \t\n");
    while (args[i] != NULL && i < MAX_ARGS - 1)
    {
        args[++i] = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
    return i;
}

/**
 * @brief Changes the current working directory.
 * @param args Argument array (args[1] is the target directory).
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
 * @brief Prints the current working directory.
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
 * @brief Prints text to stdout.
 * @param args Array of strings to print.
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
 * @brief Suspends execution for a specified number of seconds.
 * @param args Argument array with seconds in args[1].
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
 * @brief Checks for and executes built-in commands (cd, pwd, echo, sleep,
 * exit).
 * @param args Argument list from user input.
 * @param running Pointer to the main loop running flag.
 * @return true if a built-in was matched, false if it is an external command.
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
 * @brief Invokes external processes via fork and execvp.
 * @param args Argument list with the utility name in args[0].
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
 * @brief Main entry point; sets up the environment and runs the CLI loop.
 * @param argc Argument count from the calling shell.
 * @param argv Argument array with the received flags.
 * @return Exit status returned after the loop ends.
 */
int main(int argc, char* argv[])
{
    program_name = argv[0];

    char opt;
    while ((opt = next_option(argc, argv, "h")) != '\0')
    {
        switch (opt)
        {
            case 'h':
                print_usage();
                return EXIT_SUCCESS;
            default:
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
