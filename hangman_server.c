// hangman_server.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <pthread.h>

int i;

char* getMessage(const char *buffer) {
// PRECONDITION: buffer is non-empty and is all integers
// POSTCONDITION: returns a char array representing each iterative sum of buffer's digits
	char *mssg = calloc(256, sizeof(char));
	// char *tmpBuffer = calloc(256, sizeof(char));
	// memcpy(tmpBuffer, buffer, strlen(buffer));
	strcat(mssg, "From server, this is what you said: ");
	strcat(mssg, buffer);
	strcat(mssg, "\n");
	return mssg;
}

void *socketThread(void *arg) {
	i++;
	int newsockfd = *((int *)arg);
	char buffer[256];  // 256 bytes
	int j=0;
	while (j++<3) {
		bzero(buffer,256);  // clear buffer
		read(newsockfd,buffer,255);  // get string from client
		char *newBuffer = calloc(256, sizeof(char));  // makes new character array
		newBuffer = getMessage(buffer);  // populate char array with whole message
		bzero(buffer,256);  // clear buffer
		memcpy(buffer, newBuffer, strlen(newBuffer));  // overwrites buffer with the message
		write(newsockfd,buffer,strlen(buffer));  // write overwritten buffer to socket
	}
	close(newsockfd);  // close the client socket after fully handled
	i--;
}

int main(int argc, char *argv[]) {

	// MAKE VARIABLES
	int serverSocketFD, newsockfd, portno;
	socklen_t clilen;  // socket length variable
	// char buffer[256];  // 256 bytes
	struct sockaddr_in serv_addr, cli_addr;  // address structures
	pthread_t thread;

	// CREATE NEW SOCKET
	serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);

	// POPULATE serv_addr STRUCTURE
	bzero((char *) &serv_addr, sizeof(serv_addr));  // clears serv_addr struct
	portno = atoi(argv[1]);  // get port number
	serv_addr.sin_family = AF_INET;  // for IPv4
	serv_addr.sin_addr.s_addr = INADDR_ANY;  // where to accept connections from (all interfaces)
	serv_addr.sin_port = htons(portno);  // what port to bind to

	// BIND SOCKET TO THE ADDRESS
	bind(serverSocketFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

	// LISTEN ON SOCKET FOR NEW CONNECTIONS
	listen(serverSocketFD,5);

	i=1;
	while (true) {
		if (i>3) {
			printf("too many");
			break;
		}
		clilen = sizeof(cli_addr);
		newsockfd = accept(serverSocketFD, (struct sockaddr *) &cli_addr, &clilen);
		pthread_create(&thread, 0, socketThread, &newsockfd);
	}

	close(serverSocketFD);

	return 0;
}
