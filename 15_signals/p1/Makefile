CC=gcc
CFLAGS= #-Wall -Wextra -Werror --std=c11
# LDFLAGS = -lcheck -lm -lsubunit
COVFLAGS = 
DEBUGFLAGS= -g

all: rebuild

rebuild: clean recv.out send.out

recv.out: recv.c
	$(CC) $(CFLAGS) $^ -o $@

send.out: send.c	
	$(CC) $(CFLAGS) $^ -o $@

run: recv.out send.out
	./recv.out
	./send.out


clean:
	rm -f main *.a *.o *.gcno *.gcda *.gcov coverage*
	rm -rf report
	rm -f *.out