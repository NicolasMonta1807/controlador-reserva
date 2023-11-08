/**
 * @file Arguments.h
 * @brief Procesamiento de argumentos de entrada
 *
 * Define los posibles argumentos de entrada para cada proceso e inicializa
 * las variables correspondientes en la estructura
 *
 * @version 0.1
 */

#ifndef _ARGUMENTS_H
#define _ARGUMENTS_H

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
    int startHour;
    int finalHour;
    int secondsHour; // Matrix file location
    int totalPeople; // Number of processes or threads of distribution
    char *pipeName;  // Percentage for determining if a matrix is sparse
};

/**
 * @brief Array of possible command line arguments in this single program
 * @see arguments
 */
static struct argp_option options[] = {
    {"startHour", 'i', "START_HOUR", 0, "Starting hour"},
    {"finalHour", 'f', "FINAL_HOUR", 0, "Final hour"},
    {"secondsHour", 's', "SECONDS_HOUR", 0, "Seconds per hour"},
    {"totalPeople", 't', "TOTAL_PEOPLE", 0, "Maximum number of people"},
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
error_t parse_opt(int key, char *arg, struct argp_state *state);

/**
 * @brief Parses and validates all the command line arguments and set them to the given structure
 * @param argc number of arguments
 * @param argv array of arguments
 * @param arguments structure to set the arguments
 * @return void
 */
void init_arguments(int argc, char *argv[], struct arguments *arguments);

/**
 * @brief usable argp structure for parsing and validating command line arguments as stated in the arpg library
 * @see argp
 */
static struct argp argp = {
    .options = options,
    .parser = parse_opt,
    .args_doc = "START_HOUR FINAL_HOUR SECONDS_HOUR TOTAL_PEOPLE PIPE_NAME",
    .doc = "Reserve Controller"};

#endif // _ARGUMENTS_H