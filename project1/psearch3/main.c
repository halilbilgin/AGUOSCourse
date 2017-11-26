#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include "../common.h"
#define READ 0
#define WRITE 1

typedef struct node{
   char* data;
   struct node *next;
} listNode ;

typedef struct task {
   int i;
   char* filename;
   char* searchString;
} workerTask;

sem_t* semaphores;

listNode* head;

void workerThread(void *i) {

    workerTask *a = ((workerTask *) i);

    sem_wait(&semaphores[a->i]);

    char* result = DoWorkInChild(a->searchString, a->filename);

    listNode* cur = head;
    while(cur->next !=NULL) {
        cur = cur->next;
    }
    if(cur->data == NULL) {
       cur->data = result;
    } else {
        cur->next = (listNode*) malloc(sizeof(listNode));

        cur->next->data = result;

    }

    free(a->searchString);
    sem_post(&semaphores[a->i + 1]);
    free(a);

}
char * string_copy( const char *from, char *to )
{
    char *p;
    for ( p = to; ( *p = *from ) != '\0'; ++p, ++from )
    {
        ;
    }

    return to;
}
int main(int argc, char *argv[]) {

	/* will get commented */
    //char* a[6] = {"a", "a", "2", "b.txt", "a.txt", "output.txt"};
    //argv = a;
    /* will get commented */

    head = (listNode*) malloc(sizeof(listNode));

	char *fileToRead;
	char *childOutput;
	pid_t pid;
	int i;
	char *searchString = argv[1];

	int numberOfFilesToSearch = atoi(argv[2]);

    int tid[numberOfFilesToSearch];

	sem_t s[numberOfFilesToSearch];
    semaphores = s;
	if (semaphores == MAP_FAILED)
		printf("omg"), exit(0);

	char *outputFilename = argv[numberOfFilesToSearch + 3];

	for (i = 0; i <= numberOfFilesToSearch; i++) {
		sem_init(&semaphores[i], numberOfFilesToSearch, 0);
	}

	sem_post(&semaphores[0]);

	/* Start children. */


	for (i = 0; i < numberOfFilesToSearch; ++i) {

        workerTask* arg = (workerTask * ) malloc(sizeof(workerTask));

        arg->filename = argv[3+i];
        arg->searchString = malloc(strlen(searchString)+1);
        string_copy(searchString, arg->searchString);

        arg->i = i;
        pthread_create(&tid[i], NULL, &workerThread, (void *)arg);
    }

    sem_wait(&semaphores[numberOfFilesToSearch]);

	char *totalOutput = (char *) malloc(strlen(head->data));
    string_copy(head->data, totalOutput);

	while(head->next!=NULL){
        head = head->next;
        totalOutput = concat(totalOutput, head->data);
	};

	FILE *outputFile = fopen(outputFilename, "w");
	fprintf(outputFile, "%s", totalOutput);
    fclose(outputFile);
    free(totalOutput);
	return EXIT_SUCCESS;
}
