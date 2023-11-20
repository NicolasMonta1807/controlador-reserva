GCC=gcc
CFLAGS= -I ./include -lpthread ./include/*.c

PROGS = controller agent
BINDIR = ./bin

all: $(PROGS)

$(BINDIR):
	mkdir -p $(BINDIR)

controller: $(BINDIR)
	$(GCC) $(CFLAGS) -g -o $(BINDIR)/controller ./src/controller.c

agent: $(BINDIR)
	$(GCC) $(CFLAGS) -o $(BINDIR)/agent ./src/agent.c
