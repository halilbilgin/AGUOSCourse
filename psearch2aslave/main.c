#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/mman.h>
#include "../common.h"

#define FILEPATH "/tmp/hey.bin"

int main(int argc, char *argv[])
{
   //char* a[6] = {"a", "a","b.txt"};
    //argv = a;
    int result, fd;
    char* totalOutput;

    fd = open(FILEPATH, O_CREAT | O_RDWR, (mode_t)0777);
    if (fd == -1) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    char *searchString = argv[1];
    char *fileToRead = argv[2];

    char* childOutput = DoWorkInChild(searchString, fileToRead);
    struct stat st;

    int rc = fstat(fd, &st);
    int size=st.st_size;

    fallocate(fd, 0, size, strlen(childOutput));

    totalOutput = (char *) mmap(0, size+strlen(childOutput), PROT_WRITE, MAP_SHARED, fd, 0);

    if (totalOutput == MAP_FAILED)
        perror("wowowow");

    int t;

    for(t=0; t< strlen(childOutput); t++) {
        totalOutput[size+t] = childOutput[t];
    }

    if (munmap(totalOutput, strlen(totalOutput)) == -1) {
        perror("Error un-mmapping the file");
    }

    close(fd);

    return 0;
}
