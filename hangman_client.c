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

char* getServerMssg(const char *buffer) {
	printf("server packet: %s\n", buffer);

	// server packet -> message to print
	char *mssg = calloc(256, sizeof(char));
	if (buffer[0]=='0') {  // if mssg flag is zero
		char *currentWordGuess = calloc(256, sizeof(char));
		char *incorrectGuesses = calloc(256, sizeof(char));
		int wordLength = buffer[1] - '0';
		int numIncorrect = buffer[2] - '0';
		for (int i=3; i<3+wordLength; i++) {
			char letter[50];
			if (buffer[i]==' ') strcat(currentWordGuess, "_");
			else {
				sprintf(letter, "%c", buffer[i]);
				strcat(currentWordGuess, letter);
			}
		}
		for (int i=3+wordLength; i<3+wordLength+numIncorrect; i++) {
			char letter[50];
			sprintf(letter, "%c", buffer[i]);
			strcat(incorrectGuesses, letter);
		}
		strcat(mssg, currentWordGuess);
		strcat(mssg, "\nIncorrect Guesses: ");
		strcat(mssg, incorrectGuesses);
		strcat(mssg, "\n\n");
	}
	return mssg;
}

char* getClientPacket(const char *buffer) {
	// letter input -> packet to send
	char *packet = calloc(256, sizeof(char));
	strcat(packet, buffer);
	return packet;
}

void gameRound(int sockfd) {
	char buffer[256];

	// get serverpacket
	bzero(buffer,256);
	read(sockfd,buffer,255);

	// print server message
	char *serverMssg = getServerMssg(buffer);
	printf("%s",serverMssg);

	// print prompt for input
	printf("Letter to guess: ");

	// get input
	bzero(buffer,256);
	fgets(buffer,255,stdin);

	// send input
	char *newBuffer = calloc(256, sizeof(char));
	newBuffer = getClientPacket(buffer);
	bzero(buffer,256);
	memcpy(buffer, newBuffer, strlen(newBuffer));
	write(sockfd, buffer, strlen(buffer));
}

int main(int argc, char *argv[]) {
	int sockfd, portno;  // socket file descriptor & port number
	struct sockaddr_in serv_addr;
	struct hostent *server;  // pointer to server's host
	char buffer[256];
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);  // create socket w/ socket() system call
    server = gethostbyname(argv[1]);
    bzero((char *) &serv_addr, sizeof(serv_addr));  // clears serv_addr
    serv_addr.sin_family = AF_INET;  // server addr family = IPv4
    bcopy((char *)server->h_addr,  // writes server's address into serv_addr
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);  // get port number specified by client
    connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));  // connect() system call

	// start game
	printf("Ready to start game?(y/n): ");
	bzero(buffer,256);  // clears buffer
	fgets(buffer,255,stdin);  // stores user input into buffer
	if (buffer[0]=='y') {
		write(sockfd,"0",1);
		while (true) gameRound(sockfd);
	}
    close(sockfd);
	return 0;
}
