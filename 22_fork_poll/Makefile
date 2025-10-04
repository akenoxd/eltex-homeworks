CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
AR = ar
ARFLAGS = rcs

all: main.out

main.out: main.c 
	$(CC) $(CFLAGS) $^  -o $@

liblist.a: list.o
	$(AR) $(ARFLAGS) $@ $^

clean:
	rm -f *.o *.out *.a