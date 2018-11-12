CC=gcc

psearch2bslave: main.o
	$(CC) -pthread main.o -o psearch2bslave -lrt

main.o: main.c
	$(CC) -c main.c

clean:
	rm *.o psearch2bslave
