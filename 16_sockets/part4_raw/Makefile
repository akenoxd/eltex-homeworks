CC=gcc
CFLAGS= -Wall -Wextra -Werror 
LDFLAGS = 
COVFLAGS = 
DEBUGFLAGS= -g

all: rebuild

rebuild: clean raw_sniffer_udp.out 

raw_sniffer_udp.out: raw_sniffer_udp.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# client.out: client.c	
# 	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
run: raw_sniffer_udp.out 
	./raw_sniffer_udp.out 

clean:
	rm -f *.out