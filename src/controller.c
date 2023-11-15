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
#include <pthread.h>

#include "Datatypes.h"
#include "ControllerArguments.h"

#define MAX_AGENTS 10

struct Arguments
{
  char *pipe_id;
  struct AgentData *agents;
};

int currentAgents = 0;

void *awaitAgents(void *arg)
{
  struct Arguments *args = (struct Arguments *)arg;
  char *pipe_id = args->pipe_id;
  struct AgentData *agents = args->agents;

  int fd_escritura = open(pipe_id, O_RDONLY);

  // Comprobamos si se ha abierto correctamente
  if (fd_escritura < 0)
  {
    fd_escritura = mkfifo(pipe_id, 0666);
    if (fd_escritura < 0)
    {
      perror("Error al abrir el pipe!!!");
      return NULL;
    }
  }

  struct AgentData agent;
  int horaActual = 7;

  while (true) // AQUÍ DEBERÍAN SER LA SEÑALES
  {
    read(fd_escritura, &agent, sizeof(agent));
    agent.id = currentAgents;
    agents[currentAgents] = agent;
    printf("Nuevo agente: %s - %s\n", agents[currentAgents].agentName, agents[currentAgents].agentPipe);
    int fd_privado = open(agents[currentAgents].agentName, O_RDWR);
    write(fd_privado, &horaActual, sizeof(horaActual));
    sleep(0.5);
    // currentAgents++;
  }

  // Cerramos el pipe
  close(fd_escritura);
}

void *requests(void *arg)
{
  struct Arguments *args = (struct Arguments *)arg;
  char *pipe_id = args->pipe_id;
  struct AgentData *agents = args->agents;

  while (true)
  {
    char *answer = "yes sir";
    int fd_privado = open(agents[currentAgents].agentName, O_RDWR);
    write(fd_privado, answer, sizeof(answer));
  }
}

int main(int argc, char *argv[])
{
  // Comprobamos el número de argumentos de entrada
  struct arguments arguments;
  init_arguments(argc, argv, &arguments);

  // Obtenemos el descriptor del pipe de comunicación
  char *pipe_id = arguments.pipeName;

  // Inicio hilos
  struct AgentData *agents = malloc(sizeof(struct AgentData) * MAX_AGENTS);
  pthread_t *threads = malloc(sizeof(pthread_t) * 2);

  struct Arguments *args = malloc(sizeof(struct Arguments));
  args->pipe_id = pipe_id;
  args->agents = agents;

  pthread_create(&threads[0], NULL, (void *)awaitAgents, args);
  pthread_create(&threads[1], NULL, (void *)requests, args);

  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);

  printf("Presiona Enter para finalizar el programa...\n");
  getchar();

  return 0;
}