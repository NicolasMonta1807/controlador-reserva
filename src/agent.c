/**
 * @file agent.c
 * @brief Solicita al controlador la autorización para realizar una reserva para
 * grupos familiares en un día determinado.
 * @version 0.1
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include "AgentArguments.h"

int main(int argc, char *argv[])
{
  // Comprobamos el número de argumentos de entrada
  struct arguments arguments;
  init_arguments(argc, argv, &arguments);

  // Obtenemos el descriptor del pipe de comunicación
  char *pipe_id = argv[1];

  // Abrimos el pipe
  int fd_escritura = open(pipe_id, O_RDONLY);

  // Comprobamos si se ha abierto correctamente
  if (fd_escritura < 0)
  {
    perror("Error al abrir el pipe!!!");
    return -1;
  }

  // Escribimos datos en el pipe
  char mensaje[25];
  // write(fd_escritura, mensaje, strlen(mensaje));

  bool flag = true;
  while (flag)
  {
    read(fd_escritura, mensaje, 25);
    printf("leí desde el pipe %s\n", mensaje);
  }

  // Cerramos el pipe
  // close(fd_escritura);

  // return 0;
}