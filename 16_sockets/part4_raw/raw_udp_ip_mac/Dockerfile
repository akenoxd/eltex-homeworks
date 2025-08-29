FROM ubuntu:latest

# Установка необходимых пакетов
RUN apt-get update && apt-get install -y iproute2 make gcc tcpdump

# Копирование необходимых файлов
COPY Makefile .
COPY server.c .
COPY client.c .

# Сборка приложения
RUN make

# CMD ["./server.out"]