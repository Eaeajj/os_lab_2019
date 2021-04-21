CC=gcc
CFLAGS=-I.

all : parallel_min_max process_memory zombie parallel_sum clean

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o $@ $@.c $^  $(CFLAGS)

process_memory : 
	$(CC) -o $@ $@.c 

zombie : 
	$(CC) -o $@ $@.c 

parallel_sum : utils.o sum.o utils.h sum.h
	$(CC) -o $@ $@.c $^ $(CFLAGS) -lpthread



utils.o: utils.h
	$(CC) -o $@ -c utils.c 

find_min_max.o : utils.h find_min_max.h
	$(CC) -o $@ -c find_min_max.c 

sum.o : sum.h
	$(CC) -o $@ -c sum.c 



clean_execs : 
	rm parallel_sum parallel_min_max process_memory zombie

clean : 
	rm utils.o find_min_max.o  sum.o