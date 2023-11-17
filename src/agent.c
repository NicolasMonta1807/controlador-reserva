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

#include "Datatypes.h"
#include "AgentArguments.h"

int main(int argc, char *argv[])
{
  // Comprobamos el número de argumentos de entrada
  struct arguments arguments;
  init_agent_arguments(argc, argv, &arguments);

  // Obtenemos el descriptor del pipe de comunicación
  char *pipe_id = arguments.pipeName;

  // Abrimos el pipe
  int fd_escritura = open(pipe_id, O_WRONLY);
  // Comprobamos si se ha abierto correctamente
  if (fd_escritura < 0)
  {
    perror("Error al abrir el pipe!!!");
    return -1;
  }

  FILE *file = fopen(arguments.requestFile, "r");
  if (file == NULL)
  {
    printf("El archivo de solicitudes no existe.\n");
  }

  struct AgentData agent;
  strcpy(agent.agentName, arguments.agentName);
  strcpy(agent.agentPipe, arguments.agentName);
  agent.id = 0;

  int fd_privado = open(arguments.agentName, O_RDWR);

  // Comprobamos si se ha abierto correctamente
  if (fd_privado < 0)
  {
    fd_privado = mkfifo(arguments.agentName, 0666);
    if (fd_privado < 0)
    {
      perror("Error al abrir el pipe!!!");
      return -1;
    }
  }

  // Escribe el agente en el pipe
  write(fd_escritura, &agent, sizeof(agent));
  int horaActual = 0;
  read(fd_privado, &horaActual, sizeof(horaActual));
  struct Family *familia = malloc(sizeof(struct Family));
  char line[256];
  int fd_privado2 = open("prueba", O_RDONLY);
  if (fd_privado2 < 0)
  {
    fd_privado2 = mkfifo("prueba", 0666);
    if (fd_privado2 < 0)
    {
      perror("Error al abrir el pipe!!!");
      return -1;
    }
  }
  while (fgets(line, sizeof(line), file))
  {
    // Divide la línea en tokens separados por comas
    char *temp = strtok(line, ",");
    strcpy(familia->name, temp);
    familia->hourIn = atoi(strtok(NULL, ","));
    familia->quantity = atoi(strtok(NULL, ","));
    // Valida que la hora de reserva no sea inferior a la hora actual de simulación
    if (familia->hourIn < horaActual)
      continue;

    // Formatea la solicitud
    printf("Nueva Familia: %s - %d\n", familia->name, familia->quantity);
    write(fd_privado, familia, sizeof(familia));
    // Escribe la solicitud en el pipe

    // Espera a que el proceso del controlador termine de procesar la solicitud

    // Lee la respuesta del pipe
    char respuesta[256];
    if (read(fd_privado2, respuesta, sizeof(respuesta)) == -1)
    {
      perror("read");
      exit(1);
    }

    // Imprime la respuesta
    printf("Respuesta: %s\n", respuesta);

    // Espera 2 segundos antes de procesar la siguiente solicitud
    sleep(2);
  }

  printf("Agente: %s Termina\n", agent.agentName);
  // Cerramos el pipe
  // close(fd_escritura);

  // return 0;
}