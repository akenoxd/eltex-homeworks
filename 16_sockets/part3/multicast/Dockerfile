FROM gcc:latest

WORKDIR /app

COPY Makefile .
COPY server.c .
COPY client.c .

RUN make
