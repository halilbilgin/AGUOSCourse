#include"stdio.h"
#include"stdlib.h"
#include"sys/types.h"
#include"sys/socket.h"
#include"string.h"
#include"netinet/in.h"
#include"netdb.h"
#include"pthread.h"
#include <time.h>

#define BUFF_LENGTH 1994
int portNo;
int pong;
int connectionStatus;

void * messageReciever(void * socket) {
	int connSocket, resultID, sendResult;
	char buffer[BUFF_LENGTH+6];
	connSocket = (int) socket;
	memset(buffer, 0, BUFF_LENGTH+6);

	for (;;) {
		resultID = recvfrom(connSocket, buffer, BUFF_LENGTH+6, 0, NULL, NULL);
		if(buffer[0] == '\0')
			continue;

		sendResult = send(connSocket, "\0", 1, MSG_NOSIGNAL);
		if (sendResult < 0 || resultID < 0) {
			printf("Connection with server is lost. Bye.");
			exit(1);
		} else {
			fputs(buffer, stdout);
		}
		memset(buffer, 0, BUFF_LENGTH+6);
	}
}

int main(int argc, char**argv) {
	struct sockaddr_in addr, clientAddress;
	int connSocket, resultID;
	char buffer[BUFF_LENGTH];
	char * serverAddr;
	pthread_t rThread;

	if (argc < 3) {
		printf("You need to specify server address and port separated by space");
		return -1;
	}

	serverAddr = argv[1];
	portNo = atoi(argv[2]);

	connSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connSocket < 0) {
		printf("Error creating socket!\n");
		return -1;
	}

	printf("Server address %s, port: %d", serverAddr, portNo);
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(serverAddr);
	addr.sin_port = portNo;

	resultID = connect(connSocket, (struct sockaddr *) &addr, sizeof(addr));
	if (resultID < 0) {
		printf("connection error");
		return -1;
	}

	memset(buffer, 0, BUFF_LENGTH);

	printf("Type your message and then press enterrr \n");

	resultID = pthread_create(&rThread, NULL, messageReciever, (void *) connSocket);
	if (resultID) {
		return -1;
	}

	while (fgets(buffer, BUFF_LENGTH, stdin) != NULL) {
		resultID = sendto(connSocket, buffer, BUFF_LENGTH, 0, (struct sockaddr *) &addr, sizeof(addr));
		if (resultID < 0) {
			printf("There is an errorr %s ", buffer);
		}
	}

	close(connSocket);
	pthread_exit(NULL);

	return 0;
}

