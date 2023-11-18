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
#include <signal.h>
#include "Datatypes.h"
#include "ControllerArguments.h"

#define MAX_AGENTS 10

struct Arguments
{
  struct AgentData agent;
};

int currentAgents = 0;
int horaActual = 0;
int secondsHour = 0;
int horaFinal = 0;
int totalPeople = 0;
struct Park park;
struct Report report;
void handler(int signum)
{
  if (signum == SIGALRM)
  {
    if (horaActual != horaFinal)
    {
      horaActual++;

      alarm(secondsHour);
    }
  }
}

void init_park(struct Park *park, struct arguments *args)
{
  park->startHour = args->startHour;
  park->endHour = args->finalHour;
  park->hours = malloc(sizeof(struct Hour) * 24);
  for (int i = park->startHour; i <= park->endHour; i++)
  {
    park->hours[i].reserved = 0;
    park->hours[i].hour = i;
    park->hours[i].families = malloc(sizeof(struct Family) * MAX_FAMILIES);
  }
}
void init_report(struct Report *report, struct Park *park)
{
  report->num_denied = 0;
  report->num_accepted = 0;
  report->num_rescheduled = 0;

  for (int i = park->startHour; i <= park->endHour; i++)
  {
    report->num_people[i] = 0;
  }
}

char *process_reservation(struct Park *park, struct Family family, struct Report *report)
{
  char *message = malloc(255);
  if (!message)
  {
    return "Error: No se ha podido asignar memoria al mensaje.\n";
  }
  printf("hora fam: %d - hora park: %d", family.hourIn, park->endHour);
  if (family.hourIn > park->endHour)
  {
    report->num_denied++;
    return "Reserva negada por tarde.";
  }
  else if ((family.quantity + report->num_people[family.hourIn]) > totalPeople)
  {
    report->num_denied++;
    return "Reserva negada, supera la capacidad permitida.";
  }
  else
  {
    if (park->hours[family.hourIn].reserved + family.quantity <= MAX_FAMILIES)
    {
      park->hours[family.hourIn].reserved += family.quantity;
      report->num_accepted++;
      report->num_people[family.hourIn] += family.quantity;
      message = "Reserva OK.";
      return message;
    }
    else
    {
      for (int i = park->startHour; i <= park->endHour; i++)
      {
        if (park->hours[i].reserved + family.quantity <= MAX_FAMILIES)
        {
          park->hours[i].reserved += family.quantity;
          report->num_rescheduled++;
          report->num_people[i] += family.quantity;
          int ret = sprintf(message, "Reserva garantizada para otras horas %d.", i);
          if (ret < 0)
          {
            return "Error: Fallo al generar mensaje de reservación.";
          }
          else
          {
            return message;
          }
        }
      }
      report->num_denied++;
      return "Reserva negada, debe volver otro día.";
    }
  }
}

void *requests(void *arg)
{
  struct Arguments *args = (struct Arguments *)arg;
  sem_t *mutex; // Declarar el semáforo

  // Abrir el semáforo existente
  char sem_name[270];
  sprintf(sem_name, "_sem11%s", args->agent.agentName);
  mutex = sem_open(sem_name, O_CREAT, 0666, 1);

  if (mutex == SEM_FAILED)
  {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }
  char *pipe_id = args->agent.agentName;
  printf("%s\n", args->agent.agentPipe);
  struct Family familia;
  char *answer;
  int fd_privado = open(pipe_id, O_RDWR);
  int fd_privado2 = open(args->agent.agentPipe, O_WRONLY);
  int i = 0;
  while (i < args->agent.id)
  {
    sem_wait(mutex);
    if (read(fd_privado, &familia, sizeof(familia)) > 0)
    {
      printf("Nueva Familia: %s\n", familia.name);
      printf("Hora en la que quieren entrar: %d\n", familia.hourIn);
      char *message = process_reservation(&park, familia, &report);
      write(fd_privado2, message, 255);
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
  secondsHour = arguments.secondsHour;
  horaActual = arguments.startHour;
  horaFinal = arguments.finalHour;
  totalPeople = arguments.totalPeople;

  init_park(&park, &arguments);
  init_report(&report, &park);
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

  signal(SIGALRM, handler);
  alarm(arguments.secondsHour);

  while (horaActual < horaFinal) // AQUÍ DEBERÍAN SER LA SEÑALES
  {

    if (read(fd_escritura, &agent, sizeof(agent)) > 0)
    {
      printf("Nuevo agente: %s - %s\n", agent.agentName, agent.agentPipe);
      args->agent = agent;
      int fd_privado = open(agent.agentName, O_RDWR);
      write(fd_privado, &horaActual, sizeof(horaActual));
      pthread_create(&threads[0], NULL, (void *)requests, args);
      pthread_detach(threads[0]);
      // getchar();
      currentAgents++;
    }
    if (horaActual == horaFinal)
      printf("Salí lo juro\n");
  }
  // pthread_join(threads[0], NULL);
  //   Cerramos el pipe
  close(fd_escritura);

  return 0;
}