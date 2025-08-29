CC=gcc
CFLAGS= -Wall -Wextra -Werror 
LDFLAGS = 
COVFLAGS = 
DEBUGFLAGS= -g

all: rebuild

rebuild: clean server.out client_tcp.out client_udp.out

server.out: server.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

client_tcp.out: client_tcp.c	
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

client_udp.out: client_udp.c	
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run: server.out client_tcp.out 
	./server.out & ./client_tcp.out

clean:
	rm -f *.out