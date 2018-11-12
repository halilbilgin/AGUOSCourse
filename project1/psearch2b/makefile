CC=gcc

psearch2b: main.o
	$(CC) -pthread main.o -o psearch2b -lrt

main.o: main.c
	$(CC) -c main.c

clean:
	rm *.o output.txt psearch2b
