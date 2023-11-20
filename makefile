GCC=gcc
CFLAGS= -I ./include -lpthread ./include/*.c

PROGS = controller agent

all: $(PROGS)

controller:
	$(GCC) $(CFLAGS) -g -o ./bin/controller ./src/controller.c

agent:
	$(GCC) $(CFLAGS) -o ./bin/agent ./src/agent.c