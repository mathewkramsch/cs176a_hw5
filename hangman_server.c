// hangman_server.c

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include<strings.h>
#include <netdb.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h> 
#include <math.h>

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

int main(int argc, char *argv[]) {

	// MAKE VARIABLES
	struct sockaddr_in from;
	struct sockaddr_in server;
	socklen_t fromlen;
	char buff[256];

	// CREATE NEW SOCKET
	int sock = socket(AF_INET,SOCK_DGRAM,0);
	server.sin_family = AF_INET;  // for IPv4
	server.sin_addr.s_addr=INADDR_ANY;  // where to accept connections (all interfaces)
	server.sin_port=htons(atoi(argv[1]));  // what port to bind to (get from user)

	// BIND SOCKET TO THE ADDRESS
	bind(sock,(struct sockaddr *)&server,sizeof(server));
	fromlen = sizeof(struct sockaddr_in);  //length of sockaddr struct pointed to

	// READ & WRITE TO SOCKET
	while (true) {  // keeps socket open for incoming requests
		recvfrom(sock,buff,256,0,(struct sockaddr *)&from,&fromlen);  // gets string from client
		char *newBuffer = calloc(256, sizeof(char));  // makes new character array
		newBuffer = getMessage(buff);  // populate char array with whole message
		bzero(buff,256);  // clear buffer
		memcpy(buff, newBuffer, strlen(newBuffer));  // overwrites buffer with the message
		sendto(sock,buff,256,0,(struct sockaddr *)&from,fromlen);  // write overwritten buffer to socket
		bzero(buff,256);  // clear the buffer again
	}

	// CLOSE THE SOCKET
	close(sock);
	return 0;
}
