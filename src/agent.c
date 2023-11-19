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
#include <semaphore.h>
#include "Datatypes.h"
#include "AgentArguments.h"

int count_lines(char *filename)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    perror("Error al abrir el archivo");
    return -1;
  }

  int count = 0;
  int ch;
  while ((ch = fgetc(file)) != EOF)
  {
    if (ch == '\n')
    {
      count++;
    }
  }
  if (ch != EOF || count > 0)
  {
    count++;
  }
  printf("%d", count);
  fclose(file);
  return count;
}

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

  sem_t *agentSem;
  char agentSem_name[270];
  sprintf(agentSem_name, "_createSem5%s", arguments.agentName);
  agentSem = sem_open(agentSem_name, O_CREAT, 0666, 1);

  if (agentSem == SEM_FAILED)
  {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  sem_t *mutex; // Declarar el semáforo que tiene con el hilo

  // Abrir el semáforo existente
  char sem_name[270];
  sprintf(sem_name, "_sem004%s", arguments.agentName);
  mutex = sem_open(sem_name, O_CREAT, 0666, 1);

  if (mutex == SEM_FAILED)
  {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  struct AgentData agent;
  strcpy(agent.agentName, arguments.agentName);
  char nombrePipe[255];
  strcpy(nombrePipe, agent.agentName);
  char sufix[270];
  sprintf(sufix, "req%s", arguments.agentName);
  strcpy(agent.agentPipe, sufix);
  agent.id = count_lines(arguments.requestFile);

  // Escribe el agente en el pipe
  write(fd_escritura, &agent, sizeof(agent));
  close(fd_escritura);
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
    fd_privado = open(arguments.agentName, O_RDWR);
    if (fd_privado < 0)
    {
      perror("Error al abrir el pipe después de la creación!!!");
      return -1;
    }
  }
  sem_post(agentSem);
  int horaActual = 0;
  if (read(fd_privado, &horaActual, sizeof(horaActual)) > 0)
    printf(" %d\n", horaActual);
  struct Family *familia = malloc(sizeof(struct Family));
  char line[256];
  // sem_post(mutex);
  int fd_privado2 = open(agent.agentPipe, O_RDONLY);
  if (fd_privado2 < 0)
  {
    fd_privado2 = mkfifo(agent.agentPipe, 0666);
    if (fd_privado2 < 0)
    {
      perror("Error al abrir el pipe!!!");
      return -1;
    }
    fd_privado2 = open(agent.agentPipe, O_RDONLY);
    if (fd_privado2 < 0)
    {
      perror("Error al abrir el pipe después de la creación!!!");
      return -1;
    }
  }
  // sem_wait(mutex);
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
    printf("Nueva Familia: %s - %d - %d\n", familia->name, familia->quantity, familia->hourIn);
    write(fd_privado, familia, sizeof(struct Family));
    sem_post(mutex);
    // Escribe la solicitud en el pipe

    // Espera a que el proceso del controlador termine de procesar la solicitud

    // Lee la respuesta del pipe
    sem_wait(mutex);
    char respuesta[255];
    if (read(fd_privado2, respuesta, sizeof(respuesta)) == -1)
    {
      perror("read");
      exit(1);
    }

    // Imprime la respuesta
    printf("Respuesta: %s\n", respuesta);
    // write(fd_privado, &bandera, sizeof(bandera));
    //  Espera 2 segundos antes de procesar la siguiente solicitud
    sleep(2);
  }
  sem_post(mutex);
  // write(fd_privado, &bandera, sizeof(bandera));
  sem_close(agentSem);
  sem_close(mutex);
  close(fd_privado);
  close(fd_privado2);
  printf("Agente: %s Termina\n", agent.agentName);
  // Cerramos el pipe
  // close(fd_escritura);

  // return 0;
}