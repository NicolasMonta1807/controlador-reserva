/**
 * @file Arguments.h
 * @brief Procesamiento de argumentos de entrada
 *
 * Define los posibles argumentos de entrada para cada proceso e inicializa
 * las variables correspondientes en la estructura
 *
 * @version 0.1
 */

#ifndef _AGENTARGUMENTS_H
#define _AGENTARGUMENTS_H

#include <stdlib.h> // Standard libraries
#include <argp.h>   // Library for parsing unix-style argument

/**
 * @brief stores the command line arguments
 * @see argp
 */
struct arguments
{
    char *args[2]; // Flag and argument

    // Possible arguments
    char *agentName;   // The agent's name
    char *requestFile; // The file that has to be read
    char *pipeName;    // The pipe that is going to be used
};

/**
 * @brief Array of possible command line arguments in this single program
 * @see arguments
 */
static struct argp_option options[] = {
    {"agentName", 's', "AGENT_NAME", 0, "Agent name"},
    {"requestFile", 'a', "REQUEST_FILE", 0, "Request file"},
    {"pipeName", 'p', "PIPE_NAME", 0, "Name of the pipe"},
    {0}};

/**
 * @brief Cnd validates a single command line argument and set them to the proper structure field
 *
 * This function should be passed to the argp final structure as the parser argument.
 *
 * @param key flag or key of the argument
 * @param arg argument value
 * @param state state of the argument parser (given by argp.h)
 * @return error_t error code if the argument is invalid or 0 if it is valid
 */
error_t parse_agent_opt(int key, char *arg, struct argp_state *state);

/**
 * @brief Parses and validates all the command line arguments and set them to the given structure
 * @param argc number of arguments
 * @param argv array of arguments
 * @param arguments structure to set the arguments
 * @return void
 */
void init_agent_arguments(int argc, char *argv[], struct arguments *arguments);

/**
 * @brief usable argp structure for parsing and validating command line arguments as stated in the arpg library
 * @see argp
 */
static struct argp argp = {
    .options = options,
    .parser = parse_agent_opt,
    .args_doc = "AGENT_NAME REQUEST_FILE PIPE_NAME",
    .doc = "Reserve Controller"};

#endif // _AGENTARGUMENTS_H