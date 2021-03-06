#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../common.h"



int main(int argc, char *argv[]) {
	/* will get commented */
    //char* a[6] = {"a", "a", "2", "b.txt", "a.txt", "output.txt"};
    //argv = a;
    /* will get commented */
	char *searchString = argv[1];
	int numberOfFilesToSearch = atoi(argv[2]);
	char *fileToRead;
	char *childOutput;
	FILE *childOutputFile;
	char childOutputName[] = "op00";
	char *outputFilename = argv[numberOfFilesToSearch + 3];

	pid_t pid;
	int i;
	/* Start children. */
	for (i = 0; i < numberOfFilesToSearch; ++i) {
	  fileToRead = argv[3+i];

	  if ((pid = fork()) < 0) {
	    perror("fork");
	    abort();
	  } else if (pid == 0) {
		  childOutput = DoWorkInChild(searchString, fileToRead);

		  if(i < 10){
			  childOutputName[3] = '0' + i;
		  } else {
			  childOutputName[3] = '0' + i/10;
			  childOutputName[2] = '0' + i%10;
		  }

		  childOutputFile = fopen(childOutputName, "w");
		  fprintf(childOutputFile, "%s", childOutput);
		  fclose(childOutputFile);
		  exit(0);
	  }
	}

	/* Wait for children to exit. */
	int status;

	i = numberOfFilesToSearch;
	while (i > 0) {
	  pid = wait(&status);
	  --i;  // TODO(pts): Remove pid from the pids array.
	}

	char *totalOutput = (char *) malloc(1);

	for(i = 0; i < numberOfFilesToSearch; i++) {
		  if(i < 10){
			  childOutputName[3] = '0' + i;
		  } else {
			  childOutputName[3] = '0' + i/10;
			  childOutputName[2] = '0' + i%10;
		  }
		  totalOutput = concat(totalOutput, loadFile(childOutputName));
	}
	FILE *outputFile = fopen(outputFilename, "w");
	fprintf(outputFile, "%s", totalOutput);
    fclose(outputFile);
	return EXIT_SUCCESS;
}
