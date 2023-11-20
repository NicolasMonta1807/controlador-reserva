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

void init_park(struct Park *park, struct arguments *args)
{
  park->startHour = args->startHour;
  park->endHour = args->finalHour;
  park->hours = malloc(sizeof(struct Hour) * 24);
  for (int i = park->startHour; i <= park->endHour; i++)
  {
    park->hours[i].reserved = 0;
    park->hours[i].numFamilies = 0;
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
  if (family.hourIn > park->endHour)
  {
    report->num_denied++;
    return "Reserva negada por tarde.";
  }
  else
  {
    if (park->hours[family.hourIn].reserved + family.quantity <= totalPeople && family.hourIn >= horaActual)
    {
      park->hours[family.hourIn].reserved += family.quantity;
      park->hours[family.hourIn + 1].reserved += family.quantity;
      park->hours[family.hourIn].families[park->hours[family.hourIn].numFamilies] = family;
      park->hours[family.hourIn + 1].families[park->hours[family.hourIn + 1].numFamilies] = family;
      park->hours[family.hourIn].numFamilies++;
      report->num_accepted++;
      report->num_people[family.hourIn] += family.quantity;
      report->num_people[family.hourIn + 1] += family.quantity;
      message = "Reserva OK.";
      return message;
    }
    else
    {
      for (int i = horaActual + 1; i < park->endHour; i++)
      {
        if (park->hours[i].reserved + family.quantity <= totalPeople)
        {
          family.hourIn = i;
          park->hours[i].reserved += family.quantity;
          park->hours[i + 1].reserved += family.quantity;
          park->hours[i].families[park->hours[i].numFamilies] = family;
          park->hours[i + 1].families[park->hours[i + 1].numFamilies] = family;
          park->hours[i].numFamilies++;
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

void print_report(struct Park *park, struct Report *reports)
{
  int max_people = reports->num_people[park->startHour];
  int min_people = reports->num_people[park->endHour];

  for (int i = park->startHour; i < park->endHour; i++)
  {
    if (reports->num_people[i] > max_people)
    {
      max_people = reports->num_people[i];
    }
    if (reports->num_people[i] < min_people)
    {
      min_people = reports->num_people[i];
    }
  }

  printf("\n---------------------------------------------");
  // Imprimir la hora con mayor numero de personas
  printf("\nHoras pico: ");
  for (int i = park->startHour; i < park->endHour; i++)
  {
    if (reports->num_people[i] == max_people)
    {
      printf("%d ", i);
    }
  }
  printf("\n");

  // Imprimir la hora con menor numero de personas
  printf("Horas con menor numero de personas: ");
  for (int i = park->startHour; i < park->endHour; i++)
  {
    if (reports->num_people[i] == min_people)
    {
      printf("%d ", i);
    }
  }
  printf("\n");

  // Imprimir el numero de solicitudes aceptadas, negadas y reprogramadas
  printf("Número de solicitudes negadas: %d\n", reports->num_denied);
  printf("Número de solicitudes aceptadas en su hora: %d\n", reports->num_accepted);
  printf("Número de solicitudes reprogramadas: %d\n", reports->num_rescheduled);
}

void calculateParkTraffic(struct Park *park, int *numPeopleEntering, int *numPeopleLeaving, int *numFamiliesIN, int *numFamiliesOUT, struct Family **familiesEntering, struct Family **familiesLeaving)
{
  // Inicializar las variables
  *numPeopleEntering = 0;
  *numPeopleLeaving = 0;
  *numFamiliesIN = 0;
  *numFamiliesOUT = 0;
  *familiesEntering = NULL;
  *familiesLeaving = NULL;

  // Calcular el tráfico de personas que entran y salen del parque en la hora actual
  for (int i = park->startHour; i <= park->endHour; i++)
  {
    int size = park->hours[i].numFamilies;
    for (int j = 0; j < size; j++)
    {
      struct Family *family = &(park->hours[i].families[j]);
      if (family->hourIn == horaActual)
      {
        (*numPeopleEntering) += family->quantity;
        (*numFamiliesIN)++;
        (*familiesEntering) = realloc(*familiesEntering, (*numFamiliesIN) * sizeof(struct Family));
        (*familiesEntering)[(*numFamiliesIN) - 1] = *family;
      }
      // Asumiendo que la hora de salida se puede calcular
      int exitHour = family->hourIn + 2; // Ajusta esto según sea necesario
      if (exitHour == horaActual)
      {
        (*numPeopleLeaving) += family->quantity;
        (*numFamiliesOUT)++;
        (*familiesLeaving) = realloc(*familiesLeaving, (*numFamiliesOUT) * sizeof(struct Family));
        (*familiesLeaving)[(*numFamiliesOUT) - 1] = *family;
      }
    }
  }
}

void printParkTraffic()
{

  int numPeopleEntering = 0, numPeopleLeaving = 0, numFamiliesIN = 0, numFamiliesOUT = 0;
  struct Family *familiesEntering = NULL, *familiesLeaving = NULL;

  calculateParkTraffic(&park, &numPeopleEntering, &numPeopleLeaving, &numFamiliesIN, &numFamiliesOUT, &familiesEntering, &familiesLeaving);
  printf("\n---------------------------------------------");
  printf("\nHora actual: %d\n", horaActual);
  printf("Personas entrando al parque: %d\n", numPeopleEntering);
  printf("Personas saliendo del parque: %d\n", numPeopleLeaving);

  printf("\nDetalles de las familias entrando:\n");

  for (int i = 0; i < numFamiliesIN; i++)
  {
    printf("Familia %d:\n", i + 1);
    printf("Nombre: %s\n", familiesEntering[i].name);
    printf("Número de miembros: %d\n", familiesEntering[i].quantity);
  }

  printf("\nDetalles de las familias saliendo:\n");
  for (int i = 0; i < numFamiliesOUT; i++)
  {
    printf("Familia %d:\n", i + 1);
    printf("Nombre: %s\n", familiesLeaving[i].name);
    printf("Número de miembros: %d\n", familiesLeaving[i].quantity);
  }

  // Liberar la memoria asignada para los arrays de familias
  free(familiesEntering);
  free(familiesLeaving);
}

void handler(int signum)
{
  if (signum == SIGALRM)
  {
    if (horaActual != horaFinal)
    {
      horaActual++;
      printParkTraffic();

      alarm(secondsHour);
    }
  }
}

void *requests(void *arg)
{
  struct Arguments *args = (struct Arguments *)arg;
  sem_t *mutex; // Declarar el semáforo

  // Abrir el semáforo existente
  char sem_name[270];
  sprintf(sem_name, "semaforoHilo%s", args->agent.agentName);
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

  int i = 1;
  while (i == 1)
  {
    sem_wait(mutex);
    if (read(fd_privado, &familia, sizeof(familia)) > 0)
    {

      char *message = process_reservation(&park, familia, &report);
      write(fd_privado2, message, 255);
      sem_post(mutex);
    }
    else
    {
      i = 0;
    }
  }
  sem_close(mutex);
  sem_unlink(sem_name);
  close(fd_privado);
  close(fd_privado2);
}

int main(int argc, char *argv[])
{
  // Comprobamos el número de argumentos de entrada
  struct arguments arguments;
  init_arguments(argc, argv, &arguments);
  // hilos por cada agente
  pthread_t *threads = malloc(sizeof(pthread_t) * MAX_AGENTS);

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

    fd_escritura = open(pipe_id, O_RDONLY);
    if (fd_escritura < 0)
    {
      perror("Error al abrir el pipe después de la creación!!!");
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
      if (fd_privado < 0)
      {
        perror("Error al abrir el pipe después de la creación!!!");
        return -1;
      }

      write(fd_privado, &horaActual, sizeof(horaActual));

      close(fd_privado);
      pthread_create(&threads[currentAgents], NULL, (void *)requests, args);
      pthread_detach(threads[currentAgents]);

      currentAgents++;
    }
  }
  //   Cerramos el pipe
  printf("\n\nReporte Final\n");
  print_report(&park, &report);
  close(fd_escritura);
  unlink(pipe_id);

  return 0;
}