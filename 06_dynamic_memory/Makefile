CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
AR = ar
ARFLAGS = rcs

all: main.out

main.out: main.o liblist.a
	$(CC) $(CFLAGS) $^ -L. -llist -o $@

liblist.a: list.o
	$(AR) $(ARFLAGS) $@ $^

main.o: main.c list.h
	$(CC) $(CFLAGS) -c $<

list.o: list.c list.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o *.out *.a