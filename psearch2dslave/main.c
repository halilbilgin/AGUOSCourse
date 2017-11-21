// C program to implement one side of FIFO
// This side writes first, then reads
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "../common.h"

int main(int argc, char *argv[])
{
    //char* a[6] = {"a", "a","b.txt"};
    //argv = a;

    char *searchString = argv[1];
	char *fileToRead = argv[2];

    char *childOutput = DoWorkInChild(searchString, fileToRead);

    int fd;

    // FIFO file path
    char * myfifo = "/tmp/myfifo1";
    mkfifo(myfifo, 0666);
    //mkfifo(myfifo, 0666);
    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)

    // Open FIFO for write only
    fd = open(myfifo, O_WRONLY);
    write(fd, childOutput, strlen(childOutput)+1);

    close(fd);

    return 0;
}
