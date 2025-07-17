CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 
OP_DIR = operations

all: main.out

run: main.out
	LD_LIBRARY_PATH=. ./main.out

main.out: main.o libcalc.so
	$(CC) $(CFLAGS) $< -L. -lcalc -o $@

libcalc.so: $(OP_DIR)/add.o $(OP_DIR)/sub.o $(OP_DIR)/mul.o $(OP_DIR)/div.o
	$(CC) -shared $^ -o $@

$(OP_DIR)/add.o: $(OP_DIR)/add.c 
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(OP_DIR)/sub.o: $(OP_DIR)/sub.c 
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(OP_DIR)/mul.o: $(OP_DIR)/mul.c 
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(OP_DIR)/div.o: $(OP_DIR)/div.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o *.out *.a *.so $(OP_DIR)/*.o