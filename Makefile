CC = gcc
CFLAGS = -Wall -pthread

all: servidor cliente

servidor: servidor.c
	$(CC) $(CFLAGS) servidor.c -o servidor

client: cliente.c
	$(CC) $(CFLAGS) cliente.c -o cliente

clean:
	rm -f servidor cliente

