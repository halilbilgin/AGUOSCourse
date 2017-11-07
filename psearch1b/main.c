/*
 * psearch1b.c
 *
 *  Created on: Oct 14, 2017
 *      Author: halilbilgin
 */


/*
 * psearch1b.c
 *
 *  Created on: Oct 14, 2017
 *      Author: halilbilgin
 */


/*
 ============================================================================
 Name        : OS.c
 Author      : Halil BIlgin
 Version     :
 Copyright   : Project is under MIT Licence
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <linux/mman.h>
#include "../common.h"

#define READ 0
#define WRITE 1

sem_t* semaphores;

int main(int argc, char *argv[]) {
	char *fileToRead;
	char *childOutput;
	pid_t pid;
	int i;
	char *searchString = argv[1];
	int numberOfFilesToSearch = atoi(argv[2]);
    numberOfFilesToSearch = 2;

	char *totalOutput = (char *) mmap(NULL, 1, PROT_READ|PROT_WRITE,
				MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	semaphores = mmap(NULL, (numberOfFilesToSearch+1)*sizeof(sem_t), PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (semaphores == MAP_FAILED || totalOutput == MAP_FAILED)
		printf("omg"), exit(0);

	char *outputFilename = argv[numberOfFilesToSearch + 3];

	for (i = 0; i <= numberOfFilesToSearch; i++) {
		sem_init(&semaphores[i], numberOfFilesToSearch, 0);
	}

	sem_post(&semaphores[0]);

	/* Start children. */

	for (i = 0; i < numberOfFilesToSearch; ++i) {
	  fileToRead = argv[3+i];

	  if ((pid = fork()) < 0) {
	    perror("fork");
	    abort();
	  } else if (pid == 0) {

		  sem_wait(&semaphores[i]);
		  int oldsize = strlen(totalOutput);
		  void* temp;
		  childOutput = DoWorkInChild(searchString, fileToRead);

		  mremap(totalOutput, oldsize,
		  						  oldsize+strlen(childOutput)+1, MREMAP_MAYMOVE);

		  int t;

		  for(t=0; t< strlen(childOutput); t++) {

			  totalOutput[oldsize+t] = childOutput[t];

		  }


		  sem_post(&semaphores[i + 1]);
		  exit(0);
	  }

	}

	sem_wait(&semaphores[numberOfFilesToSearch]);

	FILE *outputFile = fopen(outputFilename, "w");
	fprintf(outputFile, "%s", totalOutput);
    fclose(outputFile);
	return EXIT_SUCCESS;
}
