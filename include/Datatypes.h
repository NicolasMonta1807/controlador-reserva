#define MAX_BUFFER 255
#define HOURS 24
#define MAX_FAMILIES 20
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
  int numFamilies;
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

struct Report
{
  int num_people[HOURS];
  int num_denied;
  int num_accepted;
  int num_rescheduled;
};