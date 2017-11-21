#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{

    //char* a[6] = {"a", "a", "2", "/home/halilbilgin/eclipse-workspace/psearch2dslave/a.txt", "/home/halilbilgin/eclipse-workspace/psearch2dslave/b.txt", "output.txt"};
    //argv = a;
    int fd1;
    char * myfifo = "/tmp/myfifo1";

    int i;
    char* totalOutput = malloc(1);

    int numberOfFilesToSearch = atoi(argv[2]);
    char *outputFilename = argv[numberOfFilesToSearch + 3];
    pid_t pid;
	int status;
    mkfifo(myfifo, 0666);

    for(i = 0; i < numberOfFilesToSearch; i++) {
        char* path = "/home/halilbilgin/eclipse-workspace/psearch2dslave/bin/Debug/psearch2dslave";

        if ((pid = fork()) < 0) {
            perror("fork");
            abort();
        } else if (pid == 0) {
            int status = execlp(path, path, argv[1], argv[3+i], NULL);
            exit(0);
        }

        char a[1];

        fd1 = open(myfifo,O_RDONLY);
        do{
            read(fd1, a, 1);
            if(a[0] == '\0')
                break;
            char *b = malloc(1);
            b[0]=a[0];

            totalOutput = concat(totalOutput, b);
            free(b);

        } while(a[0] != '\0');

        // Print the read string and close
        close(fd1);
    }

    FILE *outputFile = fopen(outputFilename, "w");
	fprintf(outputFile, "%s", totalOutput);
    fclose(outputFile);
    return 0;
}
