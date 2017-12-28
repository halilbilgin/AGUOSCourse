#include"stdio.h"
#include"stdlib.h"
#include"sys/types.h"
#include"sys/socket.h"
#include"string.h"
#include"netinet/in.h"
#include"pthread.h"
#include <string.h>

#define BUFF_LENGTH 2000

int portNo;

typedef struct node {
    struct client * val;
    struct node * next;
} node_t;

struct client {
	int id;
	int connSocket;
	struct sockaddr_in address;
	char clAddress[100];
	pthread_t * receiverThread;

};

node_t * head;

void kickUser(struct client* user){
	node_t * prevHead = NULL;
	node_t * currHead = head;
	while(currHead->val != user){
		prevHead = currHead;
		if(currHead->next == NULL){
			return;
		}
		currHead = currHead->next;
	}
	if(prevHead == NULL){
		head = currHead->next;
	} else {
		prevHead->next = currHead->next;
	}

	close(currHead->val->connSocket);

	free(currHead);

}
void sendToAllExcept(char incomingBuffer[], struct client* ignoredClient) {
	int len, resultID;
	node_t * currHead = head;
	char outGoingBuffer[BUFF_LENGTH];
	if(ignoredClient != NULL)
		sprintf(outGoingBuffer, "cli%02d:%s", ignoredClient->id, incomingBuffer);
	else
		sprintf(outGoingBuffer, "srver:%s", incomingBuffer);

	if(currHead == NULL) {
		return;
	}

	for (;;) {
		if(currHead->val != ignoredClient) {

			len = sizeof(currHead->val->address);

			resultID = sendto(currHead->val->connSocket, outGoingBuffer, BUFF_LENGTH, MSG_NOSIGNAL, (struct sockaddr *) &currHead->val->address, len);
			if (resultID < 0) {
				printf("Error sending message! Client is kicked. \n");
				kickUser(currHead->val);
				break;
			}
		}

		if(currHead->next != NULL){
			currHead = currHead->next;
		} else {
			break;
		}

	}
}

void * messageReciever(void * newClient) {
	int resultID;
	struct client* client = (struct client*) newClient;
	char buffer[BUFF_LENGTH-6];
	int sendResult;
	memset(buffer, 0, BUFF_LENGTH-6);
	for (;;) {

		resultID = recvfrom(client->connSocket, buffer, BUFF_LENGTH-6, 0, NULL, NULL);
		sendResult = send(client->connSocket, "\0", 1, MSG_NOSIGNAL);

		if (sendResult < 0 || resultID < 0) {
			printf("One client is lost.\n");
			kickUser(client);
			break;
		} else {
			if(buffer[0] == '\0')
				continue;

			fputs(buffer, stdout);
			sendToAllExcept(buffer, client);
			memset(buffer, 0, BUFF_LENGTH-6);
		}
	}
}
void connectionEstablisher() {
	int connSocket;
	int len, resultID, userCounter;
	struct sockaddr_in addr;

	connSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connSocket < 0) {
		printf("Error");
		return;
	}
	printf("Socket created...\n");

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = portNo;

	resultID = bind(connSocket, (struct sockaddr *) &addr, sizeof(addr));
	if (resultID < 0) {
		printf("Error binding!\n");
		return;
	}

	listen(connSocket, 5);

	for(;;){

		struct client * newClient = malloc(sizeof(struct client));

		len = sizeof(newClient->address);
		newClient->connSocket = accept(connSocket, (struct sockaddr *) &newClient->address, &len);

		if (newClient->connSocket < 0) {
			printf("Error accepting connection!\n");
			free(newClient);
			continue;
		}

		inet_ntop(AF_INET, &(newClient->address.sin_addr), newClient->clAddress, 100);

		printf("New connection %s \n", newClient->clAddress);
		userCounter++;
		newClient->id = userCounter;

		//creating a new thread for receiving messages from the client
		resultID = pthread_create(&newClient->receiverThread, NULL, messageReciever, (void *) newClient);
		if (resultID) {
			printf("thread creation error %d", resultID);
			close(newClient->connSocket);
			free(newClient);

			continue;
		}

		node_t * currHead = head;
		while(currHead != NULL && currHead->next != NULL) currHead = currHead->next;

		if(currHead == NULL) {
			currHead = malloc(sizeof(node_t*));
			currHead->val = newClient;
			head = currHead;
		} else {
			currHead->next = malloc(sizeof(node_t*));
			currHead->next->val = newClient;
		}


	}

	//printf("Enter your messages one by one and press return key!\n");

}
void inputManager() {
	char buffer[100];

	memset(buffer, 0, BUFF_LENGTH-6);
	while (fgets(buffer, BUFF_LENGTH-5, stdin) != NULL) {
		node_t * currHead = head;
		if(currHead == NULL){
			memset(buffer, 0, BUFF_LENGTH-6);
			continue;
		}

		sendToAllExcept(&buffer, NULL);
	}

}
void main(int argc, char *argv[]) {

	if(argc < 2){
		 printf("You need to set a port!");
		 return;
	}

	portNo = atoi(argv[1]);
	pthread_t messageRecieverThread, connectionEstablisherThread;
	int resultID;

	resultID = pthread_create(&connectionEstablisherThread, NULL, connectionEstablisher, NULL);
	if(resultID< 0) {
		printf("Error creating new thread for connectionestablisher");
	}

	pthread_t inputManagerThread;
	resultID = pthread_create(&inputManagerThread, NULL, inputManager, NULL);
	if(resultID < 0) {
		printf("InputManager Thread error");
	}
	pthread_exit(NULL);
	return;
}
