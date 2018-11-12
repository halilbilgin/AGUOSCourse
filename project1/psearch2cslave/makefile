CC=gcc

psearch2cslave: main.o
	$(CC) -pthread main.o -o psearch2cslave -lrt

main.o: main.c
	$(CC) -c main.c

clean:
	rm *.o psearch2cslave
