CC=gcc
CFLAGS= -Wall -Wextra -Werror 
LDFLAGS = 
COVFLAGS = 
DEBUGFLAGS= -g

all: rebuild

rebuild: clean server.out client.out

server.out: server.o queue.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

server.o: server.c
	$(CC) -c $(CFLAGS) $^ -o $@ $(LDFLAGS)

queue.o: queue.c
	$(CC) -c $(CFLAGS) $^ -o $@ $(LDFLAGS)

client.out: client.c	
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run: server.out client.out 
	./server.out & ./client.out

clean:
	rm -f *.out