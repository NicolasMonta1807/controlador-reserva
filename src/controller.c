/**
 * @file controller.c
 * @brief Recibe las solicitudes de los agentes de reserva, las autoriza o las rechaza
 * simulando el tiempo transcurrido, admitiendo la entrada y salida de los visitantes.
 * Al final del día genera un reporte de las visitas realizadas.
 * @version 0.1
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "Datatypes.h"
#include "ControllerArguments.h"

#define MAX_AGENTS 10

struct point
{
  int x;
  int y;
};

int main(int argc, char *argv[])
{
  // Comprobamos el número de argumentos de entrada
  struct arguments arguments;
  init_arguments(argc, argv, &arguments);

  // Obtenemos el descriptor del pipe de comunicación
  char *pipe_id = arguments.pipeName;

  // Abrimos el pipe
  int fd_escritura = open(pipe_id, O_RDONLY);

  // Comprobamos si se ha abierto correctamente
  if (fd_escritura < 0)
  {
    fd_escritura = mkfifo(pipe_id, 0666);
    if (fd_escritura < 0)
    {
      perror("Error al abrir el pipe!!!");
      return -1;
    }
  }

  int currentAgents = 0;
  struct AgentData *agents = malloc(sizeof(struct AgentData) * MAX_AGENTS);
  struct AgentData agent;
  int horaActual = 10;
  while (true)
  {
    read(fd_escritura, &agent, sizeof(agent));
    printf("Nuevo agente: %s - %s\n", agent.agentName, agent.agentPipe);
    int fd_privado = open(agent.agentName, O_RDWR);
    write(fd_privado, &horaActual, sizeof(horaActual));
    sleep(10);
    char *answer = "yes sir";
    write(fd_privado, answer, sizeof(answer));
  }

  // Cerramos el pipe
  // close(fd_escritura);

  return 0;
}