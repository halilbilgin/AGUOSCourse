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
#include <mqueue.h>
#include <errno.h>
#define MESSAGE_LENGTH 1000

int main(int argc, char *argv[]) {

    struct mq_attr attr;      // message queue attributes

    int rc = mq_unlink ("/test8");
    if (rc < 0) {
        printf ("Warning %d (%s) on server mq_unlink.\n", errno, strerror (errno));
    }

    attr.mq_flags = 0;                // blocking read/write
    attr.mq_maxmsg = 5;              // maximum number of messages allowed in queue
    attr.mq_msgsize = MESSAGE_LENGTH;
    attr.mq_curmsgs = 0;              // number of messages currently in queue

    mqd_t mq = mq_open("/test8", O_CREAT, 0666, &attr);

    if (mq == -1)
    {
        printf("ERROR: %s\n", strerror(errno));
        exit(0);
    }

    char* word_to_be_searched = argv[1];
    char* number_of_inputs_str = argv[2];
    char* output_file_name = argv[3 + atoi(number_of_inputs_str)];
    int number_of_inputs = atoi(number_of_inputs_str);

    pid_t pid;
    char *slave_location;
    char *input_file;
    FILE *output_file = fopen(output_file_name, "w");

    for (int i = 0; i < number_of_inputs; i++) {
        input_file = argv[3 + i];
        slave_location = "../psearch2cslave/psearch2cslave";

        if ((pid = fork()) < 0) {
            perror("fork failed");
            abort();
        } else if (pid == 0) {
            execlp(slave_location, slave_location, word_to_be_searched, input_file, (char*)NULL);
            exit(0);
        }else {
            /* Wait for each child to finish its task. */
            int st;
            pid = wait(&st);
            char b[10000];
            mq_receive(mq, b, sizeof(b), NULL);
            fprintf(output_file, "%s", b);
        }
    }

    mq_close(mq);

    fclose(output_file);

    return EXIT_SUCCESS;
}