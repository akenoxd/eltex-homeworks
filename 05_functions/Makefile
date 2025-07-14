CC=gcc
CFLAGS= #-Wall -Wextra -Werror #--std=c11
# LDFLAGS = -lcheck -lm -lsubunit
COVFLAGS = 
DEBUGFLAGS= -g

all: rebuild

rebuild: clean p2.out

p2.out: p2.c
	$(CC) $(CFLAGS) -fno-stack-protector -no-pie $(COVFLAGS) $^ $(LDFLAGS) -o $@

crack: p2.out
	cat in | ./p2.out

run: p1_p2.out p3.out p4.out
	./p1_p2.out
	./p3.out
	./p4.out

clean:
	rm -f main *.a *.o *.gcno *.gcda *.gcov coverage*
	rm -rf report
	rm -f *.out