CC=gcc


all: client server clean

client : client.c utils.o
	$(CC) $^ -o $@ -lpthread

server : server.c utils.o
	$(CC) $^ -o $@ -lpthread

libra.o : utils.h utils.c
	$(CC) -c $^

clean : utils.o
	rm utils.o utils.h.gch