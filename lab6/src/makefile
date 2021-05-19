CC=gcc
CFLAGS=-I.
all: MultModulo client server

client : MultModulo.o MultModulo.h
	$(CC) -o client MultModulo.c MultModulo.h client.c -pthread $(CFLAGS)

server : MultModulo.o MultModulo.h
	$(CC) -o server MultModulo.c MultModulo.h server.c -pthread $(CFLAGS)

MultModulo : MultModulo.h
	$(CC) -o MultModulo.o -c MultModulo.c

clean :
	rm -f client server MultModulo.o