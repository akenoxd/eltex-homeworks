CC=gcc
CFLAGS= -Wall -Wextra -Werror --std=c11
# LDFLAGS = -lcheck -lm -lsubunit
COVFLAGS = 
DEBUGFLAGS= -g

all: rebuild

rebuild: clean p1_p2.out p3.out p4.out


p1_p2.out: p1_p2.c
	$(CC) $(CFLAGS) $(COVFLAGS) $^ $(LDFLAGS) -o $@

p3.out: p3.c
	$(CC) $(CFLAGS) $(COVFLAGS) $^ $(LDFLAGS) -o $@

p4.out: p4.c
	$(CC) $(CFLAGS) $(COVFLAGS) $^ $(LDFLAGS) -o $@

run: p1_p2.out p3.out p4.out
	./p1_p2.out
	./p3.out
	./p4.out

clean:
	rm -f main *.a *.o *.gcno *.gcda *.gcov coverage*
	rm -rf report
	rm -f *.out