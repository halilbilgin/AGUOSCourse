#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/mman.h>
#include <semaphore.h>
#include <sys/shm.h>

sem_t* semaphore;

int main(int argc, char *argv[]) {

    const char* SHARED_MEMORY_OBJECT_NAME = "SMO";
    const char* SEMAPHORE_NAME = "/semaphorename15";
    sem_unlink("/semaphorename15");

    char* word_to_be_searched = argv[1];
    char* number_of_inputs_str = argv[2];
    char* output_file_name = argv[3 + atoi(number_of_inputs_str)];
    int number_of_inputs = atoi(number_of_inputs_str);

    semaphore = sem_open(SEMAPHORE_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 1);

    if(semaphore == SEM_FAILED) {
        perror("sem_open(3) error");
        exit(EXIT_FAILURE);
    }

    sem_init(semaphore, 1, 1);

    if (sem_close(semaphore) < 0) {
        perror("sem_close(3) failed");
        sem_unlink(SEMAPHORE_NAME);
        exit(EXIT_FAILURE);
    }

    int shm_fd = shm_open(SHARED_MEMORY_OBJECT_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd== -1) {
        perror("Error opening file bru");
        exit(EXIT_FAILURE);
    }
    ftruncate(shm_fd, 0);
    close(shm_fd);
    struct stat st;

    pid_t pids[number_of_inputs];
    char *slave_location;
    char *input_file;
    char* output_str;

    for (int i = 0; i < number_of_inputs; i++) {
        input_file = argv[3 + i];
        slave_location = "../psearch2bslave/psearch2bslave";

        if ((pids[i] = fork()) < 0) {
            perror("fork failed");
            abort();
        } else if (pids[i] == 0) {
            if (execlp(slave_location, slave_location, word_to_be_searched, input_file, (char*)NULL) < 0) {
                perror("execl(2) failed");
                exit(EXIT_FAILURE);
            }
            break;
        }
    }

    for (int i = 0; i < sizeof(pids)/sizeof(pids[0]); i++) {
        if (waitpid(pids[i], NULL, 0) < 0)
            perror("waitpid(2) failed");
    }

    shm_fd = shm_open(SHARED_MEMORY_OBJECT_NAME, O_RDONLY, 0666);
    if (shm_fd== -1) {
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }

    fstat(shm_fd, &st);
    size_t size = (size_t)st.st_size;

    output_str = (char*)mmap(0, size , PROT_READ, MAP_SHARED, shm_fd, 0);

    if (output_str == MAP_FAILED) {
        perror("Map failed!");
        exit(0);
    }

    FILE *output_file = fopen(output_file_name, "w");

    fprintf(output_file, "%s", output_str);

    fclose(output_file);

    close(shm_fd);

    if (sem_unlink(SEMAPHORE_NAME) < 0) {
        perror("sem_unlink(3) failed");
    }

    shm_unlink(SHARED_MEMORY_OBJECT_NAME);

    return EXIT_SUCCESS;
}