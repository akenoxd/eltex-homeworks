CC=gcc
CFLAGS= -Wall -Wextra -Werror --std=c11
# LDFLAGS = -lcheck -lm -lsubunit
COVFLAGS = 
DEBUGFLAGS= -g

all: rebuild

rebuild: clean p1.out p2.out p3.out 

p1.out: p1.c
	$(CC) $(CFLAGS) $^ -o $@

p2.out: p2.c	
	$(CC) $(CFLAGS) $^ -o $@

p3.out: p3.c
	$(CC) $(CFLAGS) $^ -o $@

p4.out: p4.c
	$(CC) $(CFLAGS) $^ -o $@

run: p1.out p2.out p3.out
	./p1.out
	./p2.out
	./p3.out

clean:
	rm -f main *.a *.o *.gcno *.gcda *.gcov coverage*
	rm -rf report
	rm -f *.out