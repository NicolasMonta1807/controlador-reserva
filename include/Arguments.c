/**
 * @file Arguments.c
 * @brief Implementación del procesamiento de argumentos entrada
 * @version 0.1
 */

#include "Arguments.h" // Header file

error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;

    switch (key)
    {
    case 'i':
        arguments->startHour = atoi(arg);
        break;
    case 'f':
        arguments->finalHour = atoi(arg);
        break;
    case 's':
        arguments->secondsHour = atoi(arg);
        break;
    case 't':
        arguments->totalPeople = atoi(arg);
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
    arguments->startHour = 0;
    arguments->finalHour = 0;
    arguments->secondsHour = 0;
    arguments->totalPeople = 0;
    arguments->pipeName = NULL;

    argp_parse(&argp, argc, argv, 0, 0, arguments);
}