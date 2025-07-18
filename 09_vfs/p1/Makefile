CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
AR = ar
ARFLAGS = rcs

all: main.out

run: main.out
	./main.out

main.out: main.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.o *.out *.a output.txt