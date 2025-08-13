CC=gcc
CFLAGS= -Wall -Wextra -Werror 
LDFLAGS = -lrt -lpthread -lncurses 
COVFLAGS = 
DEBUGFLAGS= -g

all: rebuild

rebuild: clean server.out client.out

server.out: server.c
	$(CC) $(CFLAGS) $^ -o $@ -lrt -lpthread

client.out: client.c	
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run: server.out client.out 
	./server.out & ./client.out

clean:
	rm -f *.out