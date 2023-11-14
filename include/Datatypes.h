#define MAX_BUFFER 255

struct AgentData
{
  char agentName[MAX_BUFFER];
  char agentPipe[MAX_BUFFER];
  int id;
};

struct Hour
{
  int hour;
  int reserved;
  struct Family *families;
};

struct Family
{
  char name[MAX_BUFFER];
  int quantity;
  int hourIn;
};

struct Park
{
  struct Hour *hours;
  int startHour;
  int endHour;
};