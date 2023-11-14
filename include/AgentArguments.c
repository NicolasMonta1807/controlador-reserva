/**
 * @file Arguments.c
 * @brief Implementación del procesamiento de argumentos entrada
 * @version 0.1
 */

#include "AgentArguments.h" // Header file

error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;

    switch (key)
    {
    case 's':
        arguments->agentName = atoi(arg);
        break;
    case 'a':
        arguments->requestFile = atoi(arg);
        break;
    case 'p':
        arguments->pipeName = arg;
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

void init_arguments(int argc, char *argv[], struct arguments *arguments)
{
    arguments->agentName = NULL;
    arguments->requestFile = NULL;
    arguments->pipeName = NULL;

    argp_parse(&argp, argc, argv, 0, 0, arguments);
}