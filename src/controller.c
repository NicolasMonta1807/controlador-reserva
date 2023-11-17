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
#include <semaphore.h>
#include "Datatypes.h"
#include "ControllerArguments.h"

#define MAX_AGENTS 10

struct Arguments
{
  struct AgentData agent;
};

int currentAgents = 0;

void *requests(void *arg)
{
  struct Arguments *args = (struct Arguments *)arg;
  sem_t *mutex; // Declarar el semáforo

  // Abrir el semáforo existente
  char sem_name[270];
  sprintf(sem_name, "_semaphore%s", args->agent.agentName);
  mutex = sem_open(sem_name, O_CREAT, 0666, 1);

  if (mutex == SEM_FAILED)
  {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }
  char *pipe_id = args->agent.agentName;
  printf("%s\n", args->agent.agentPipe);
  struct Family familia;
  char *answer = "yes sir";
  int fd_privado = open(pipe_id, O_RDWR);
  int fd_privado2 = open(args->agent.agentPipe, O_WRONLY);
  int i = 0;
  while (i < 3)
  {
    sem_wait(mutex);
    if (read(fd_privado, &familia, sizeof(struct Family)) > 0)
    {
      printf("Nueva Familia: %s\n", familia.name);
      write(fd_privado2, args->agent.agentName, sizeof(args->agent.agentName));
      sem_post(mutex);
      i++;
    }
  }
  sem_close(mutex);
}

int main(int argc, char *argv[])
{
  // Comprobamos el número de argumentos de entrada
  struct arguments arguments;
  init_arguments(argc, argv, &arguments);
  // hilos por cada agente
  pthread_t *threads = malloc(sizeof(pthread_t));

  // Obtenemos el descriptor del pipe de comunicación
  char *pipe_id = arguments.pipeName;

  // Inicio hilos

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

  struct Arguments *args = malloc(sizeof(struct Arguments));
  struct AgentData agent;
  int horaActual = 8;

  while (true) // AQUÍ DEBERÍAN SER LA SEÑALES
  {
    if (read(fd_escritura, &agent, sizeof(agent)) > 0)
    {
      agent.id = currentAgents;
      printf("Nuevo agente: %s - %s\n", agent.agentName, agent.agentPipe);
      args->agent = agent;
      int fd_privado = open(agent.agentName, O_RDWR);
      write(fd_privado, &horaActual, sizeof(horaActual));
      pthread_create(&threads[0], NULL, (void *)requests, args);
      pthread_detach(threads[0]);
      // getchar();
      currentAgents++;
    }
  }
  // pthread_join(threads[0], NULL);
  //   Cerramos el pipe
  close(fd_escritura);

  return 0;
}