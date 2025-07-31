CC=gcc
CFLAGS= -Wall -Wextra -Werror --std=c11
# LDFLAGS = -lcheck -lm -lsubunit
COVFLAGS = 
DEBUGFLAGS= -g

all: rebuild

rebuild: clean server.out client.out

server.out: server.c
	$(CC) $(CFLAGS) $^ -o $@

client.out: client.c	
	$(CC) $(CFLAGS) $^ -o $@

run: server.out client.out 
	./server.out & ./client.out


clean:
	rm -f main *.a *.o *.gcno *.gcda *.gcov coverage*
	rm -rf report
	rm -f *.out