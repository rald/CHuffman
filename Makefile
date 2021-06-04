all: libpqueue.a game

libpqueue.a:
	gcc -c pqueue.c -Wall -g
	ar rcs libpqueue.a pqueue.o

game: main.c
	gcc main.c -o game -I. -L. -lpqueue -Wall -g

clean:
	rm *.o libpqueue.a game

