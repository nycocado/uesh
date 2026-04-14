#include "common.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* program_name = "program";

void warn(const char* msg)
{
    fprintf(stderr, "%s: ", program_name);
    perror(msg);
}

void die(const char* msg)
{
    fprintf(stderr, "%s: erro: ", program_name);
    perror(msg);
    exit(EXIT_FAILURE);
}

void error_msg(const char* msg)
{
    fprintf(stderr, "%s: erro: %s\n", program_name, msg);
    exit(EXIT_FAILURE);
}

void usage(const char* usage_str)
{
    fprintf(stderr, "Uso: %s %s\n", program_name, usage_str);
    fprintf(stderr, "Tente '%s -h' para mais informações.\n", program_name);
    exit(EXIT_FAILURE);
}
