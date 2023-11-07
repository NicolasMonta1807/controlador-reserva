GCC=gcc
CFLAGS= -I ./include -lpthread

PROGS = controller agent

all: $(PROGS)

controller:
	$(GCC) $(CFLAGS) -o ./bin/controller ./src/controller.c

agent:
	$(GCC) $(CFLAGS) -o ./bin/agent ./src/agent.c