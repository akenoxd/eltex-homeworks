CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

all: main.out

main.out: main.o list.o
	$(CC) $(CFLAGS) main.o list.o -o main.out

main.o: main.c list.h
	$(CC) $(CFLAGS) -c main.c

list.o: list.c list.h
	$(CC) $(CFLAGS) -c list.c

clean:
	rm -f *.o *.out

run: main.out
	./main.out
