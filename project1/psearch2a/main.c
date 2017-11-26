#include <stdio.h>
#include <stdlib.h>
#include "../common.h"
#include <semaphore.h>
#include <sys/mman.h>
#include <linux/mman.h>
#define FILEPATH "/tmp/hey.bin"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define _GNU_SOURCE             /* See feature_test_macros(7) */

#include <fcntl.h>


int main(int argc, char *argv[])
{

    //char* a[6] = {"a", "a", "2", "a.txt", "b.txt", "output.txt"};
    //argv = a;

    int i;
    int fd;
    char* totalOutput;
    struct stat st;
    int rc;

    fd = open(FILEPATH, O_TRUNC | O_WRONLY);
    close(fd);

    int numberOfFilesToSearch = atoi(argv[2]);
    char *outputFilename = argv[numberOfFilesToSearch + 3];
    pid_t pid;
	int status;

    for(i = 0; i < numberOfFilesToSearch; i++) {
        char* path = "/home/halilbilgin/eclipse-workspace/psearch2aslave/bin/Debug/psearch2aslave";

      if ((pid = fork()) < 0) {
	    perror("fork");
	    abort();
	  } else if (pid == 0) {
          int status = execlp(path, path, argv[1], argv[3+i], NULL);
		  exit(0);
	  }
	  pid = wait(&status);

    }

    fd = open(FILEPATH, O_RDWR);
    if (fd == -1) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    rc = fstat(fd, &st);
    int size=st.st_size;

    totalOutput = (char *) mmap(0, size , PROT_READ, MAP_SHARED, fd, 0);

    if (totalOutput == MAP_FAILED)
        perror("asd"), exit(0);

    FILE *outputFile = fopen(outputFilename, "w");

	fprintf(outputFile, "%s", totalOutput);

    fclose(outputFile);
    if (munmap(totalOutput, strlen(totalOutput)) == -1) {
        perror("Error un-mmapping the file");
    }
    close(fd);

}

