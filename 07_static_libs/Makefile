CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
AR = ar
ARFLAGS = rcs
OP_DIR = operations

all: main.out

run: main.out
	./main.out

main.out: main.o libcalc.a
	$(CC) $(CFLAGS) $^ -L. -lcalc -o $@

libcalc.a: $(OP_DIR)/add.o $(OP_DIR)/sub.o $(OP_DIR)/mul.o $(OP_DIR)/div.o
	$(AR) $(ARFLAGS) $@ $^

$(OP_DIR)/add.o: $(OP_DIR)/add.c 
	$(CC) $(CFLAGS) -c $< -o $@

$(OP_DIR)/sub.o: $(OP_DIR)/sub.c 
	$(CC) $(CFLAGS) -c $< -o $@

$(OP_DIR)/mul.o: $(OP_DIR)/mul.c 
	$(CC) $(CFLAGS) -c $< -o $@

$(OP_DIR)/div.o: $(OP_DIR)/div.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o *.out *.a $(OP_DIR)/*.o