all: libpqueue.a hufenc hufdec

libpqueue.a:
	gcc -c pqueue.c -Wall -g
	ar rcs libpqueue.a pqueue.o

hufenc: hufenc.c
	gcc hufenc.c -o hufenc -I. -L. -lpqueue -Wall -g

hufdec: hufdec.c
	gcc hufdec.c -o hufdec -I. -L. -lpqueue -Wall -g

clean:
	rm *.o libpqueue.a hufenc hufdec
