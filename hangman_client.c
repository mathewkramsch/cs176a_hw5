// hangman_client.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {

	// MAKE VARIABLES
	int sockfd, portno;  // socket file descriptor & port number
	struct sockaddr_in serv_addr;
	struct hostent *server;  // pointer to server's host
	char buffer[256];

	// CREATE CLIENT SOCKET TO CONNECT TO SERVER
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);  // create socket w/ socket() system call

	// convert IP address passed by user to valid server address
    server = gethostbyname(argv[1]);

	// POPULATE serv_addr STRUCT W/ RELEVANT PARAMS
    bzero((char *) &serv_addr, sizeof(serv_addr));  // clears serv_addr
    serv_addr.sin_family = AF_INET;  // server addr family = IPv4
    bcopy((char *)server->h_addr,  // writes server's address into serv_addr
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);  // get port number specified by client

	// USE SERV_ADDR TO CONNECT TO SERVER
    connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));  // connect() system call

	// CONNECTION SET UP: CLIENT CAN USE SOCKET TO WRITE/READ
	while (true) {
		// WRITE TO SOCKET
		printf("Send something to server: ");
		bzero(buffer,256);  // clears buffer
		fgets(buffer,255,stdin);  // stores user input into buffer
		write(sockfd,buffer,strlen(buffer));

		// READ FROM SOCKET
		bzero(buffer,256);
		read(sockfd,buffer,255);
		printf("%s",buffer);  // print what server wrote
	}

	// CLOSE SOCKET
    close(sockfd);


	return 0;
}
