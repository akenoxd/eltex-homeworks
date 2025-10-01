CC=gcc
CFLAGS= -Wall -Wextra #-Werror 
LDFLAGS = 
COVFLAGS = 
DEBUGFLAGS= -g

all: server.out client.out run

rebuild: clean  

server.out: server.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

client.out: client.c	
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run: server.out client.out 
	@echo "	run with:"
	@echo "docker compose run --rm server"
	@echo "docker compose run --rm client-1"
	@echo "docker compose run --rm client-2"

clean:
	rm -f *.out