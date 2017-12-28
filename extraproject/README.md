# Sockets

**Project description:**

Implement a chat room. Multiple Users can connect. when one of them sends a message, everyone in the chat room should be able to see the message and the sender. A client could be running on a different laptop that is connected to the local network. 

**Solution description:**

Compiling server 
gcc server.c -lpthread

Compiling client
gcc client.c -lpthread


Running server
./server 4123

Running clients
./client 127.0.0.1 4123

