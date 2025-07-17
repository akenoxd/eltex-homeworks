CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
AR = ar
ARFLAGS = rcs

all: main.out

run: main.out
	./main.out

debug: CFLAGS += -g
debug: main.out

main.out: main.c mc.o
	$(CC) $(CFLAGS) $^ -o $@ -lncurses

mc.o: mc.c
	$(CC) $(CFLAGS) $^ -c -o $@ -lncurses

clean:
	rm -f *.o *.out *.a output.txt

.PHONY: all clean debug
