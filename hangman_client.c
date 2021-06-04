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
	// server packet -> message to print
	char *mssg = calloc(256, sizeof(char));
	if (buffer[0]=='0') {  // if mssg flag is zero
		char *currentWordGuess = calloc(256, sizeof(char));
		char *incorrectGuesses = calloc(256, sizeof(char));
		int wordLength = buffer[1] - '0';
		int numIncorrect = buffer[2] - '0';
		for (int i=3; i<3+wordLength; i++) {
			char letter[50];
			if (buffer[i]==' ') strcat(currentWordGuess, "_ ");
			else {
				sprintf(letter, "%c ", buffer[i]);
				strcat(currentWordGuess, letter);
			}
		}
		for (int i=3+wordLength; i<3+wordLength+numIncorrect; i++) {
			char letter[50];
			sprintf(letter, "%c ", buffer[i]);
			strcat(incorrectGuesses, letter);
		}
		currentWordGuess[strlen(currentWordGuess)-1] = 0;
		incorrectGuesses[strlen(incorrectGuesses)-1] = 0;
		strcat(mssg, ">>>");
		strcat(mssg, currentWordGuess);
		strcat(mssg, "\n>>>Incorrect Guesses: ");
		strcat(mssg, incorrectGuesses);
		strcat(mssg, "\n>>>\n");
	}
	else {  // if mssg flag set (to length of message data)
		char *length = calloc(256, sizeof(char));
		for (int i=0; !isalpha(buffer[i]); i++) {
			char digit[50];
			sprintf(digit, "%c", buffer[i]);
			strcat(length, digit);
		}
		int dataLength = atoi(length);
		int startIndex = 1;
		if (dataLength > 0) startIndex = 2;
		for (int i=startIndex; i<startIndex + dataLength; i++) {
			char letter[50];
			sprintf(letter, "%c", buffer[i]);
			strcat(mssg, letter);
		}
		strcat(mssg, "\n");
	}
	return mssg;
}

char* getClientPacket(char *buffer) {
	// letter input -> packet to send
	char *packet = calloc(256, sizeof(char));
	for (int i=0; buffer[i]; i++) buffer[i] = tolower(buffer[i]);
	strcat(packet, buffer);
	return packet;
}

bool gameRound(int sockfd) {
	char buffer[256];

	// get serverpacket
	bzero(buffer,256);
	read(sockfd,buffer,255);

	// print server message
	char *serverMssg = getServerMssg(buffer);
	printf("%s",serverMssg);

	if (buffer[0]=='0') {
		do {
			printf(">>>Letter to guess: ");
			bzero(buffer,256);
			if (fgets(buffer,255,stdin)==NULL) {
				printf("\n");
				return false;
			}
			if (strlen(buffer)>2 || !isalpha(buffer[0])) printf(">>>Error! Please guess one letter.\n");
		} while (strlen(buffer)>2 || !isalpha(buffer[0]));

		char *newBuffer = calloc(256, sizeof(char));
		bzero(newBuffer, 256);
		newBuffer = getClientPacket(buffer);
		write(sockfd, newBuffer, strlen(newBuffer));
	} else return false;
	return true;
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

	bzero(buffer,256);
	read(sockfd,buffer,255);
	printf("%s",buffer);

	// start game
	if (buffer[3]!='s') {  // if not server-overloaded message
		bzero(buffer,256);  // clears buffer
		if (fgets(buffer,255,stdin)==NULL) {
			printf("\n");
			close(sockfd);
			return 0;
		}
		if (buffer[0]=='y') {
			write(sockfd,"0",1);
			while (gameRound(sockfd));
		}
	}
    close(sockfd);
	return 0;
}
