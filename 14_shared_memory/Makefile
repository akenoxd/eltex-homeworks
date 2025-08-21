CC = gcc
CFLAGS = -Wall -Wextra -Werror #-std=c11

POSIX_DIR = posix
SYSV_DIR = sysv

POSIX_TARGETS = $(POSIX_DIR)/server $(POSIX_DIR)/client
SYSV_TARGETS = $(SYSV_DIR)/server $(SYSV_DIR)/client

all: posix sysv

posix: $(POSIX_TARGETS)

$(POSIX_DIR)/server: $(POSIX_DIR)/server.c
	$(CC) $(CFLAGS) $^ -o $@ -lrt -lpthread

$(POSIX_DIR)/client: $(POSIX_DIR)/client.c
	$(CC) $(CFLAGS) $^ -o $@ -lrt -lpthread

sysv: $(SYSV_TARGETS)

$(SYSV_DIR)/server: $(SYSV_DIR)/server.c
	$(CC) $(CFLAGS) $^ -o $@

$(SYSV_DIR)/client: $(SYSV_DIR)/client.c
	$(CC) $(CFLAGS) $^ -o $@

run: posix sysv
	@echo "Running POSIX"
	./$(POSIX_DIR)/server &
	./$(POSIX_DIR)/client
	@echo "Running SYSV"
	./$(SYSV_DIR)/server &
	./$(SYSV_DIR)/client


run-posix: posix
	./$(POSIX_DIR)/server &
	./$(POSIX_DIR)/client

run-sysv: sysv
	./$(SYSV_DIR)/server &
	./$(SYSV_DIR)/client

clean:
	rm -f $(POSIX_TARGETS) $(SYSV_TARGETS)

.PHONY: all posix sysv run-posix run-sysv clean