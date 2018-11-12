CC=gcc

psearch2c: main.o
	$(CC) -pthread main.o -o psearch2c -lrt

main.o: main.c
	$(CC) -c main.c

clean:
	rm *.o output.txt psearch2c
