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
int seed;

char* getWord(const char *filename) {
	// returns random word from file
	char *chosenWord = calloc(256, sizeof(char));
	FILE *fd = fopen(filename, "r");

	// count words in file
	char word[20];
	int count = 0;
	while (fgets(word, sizeof word, fd)!=NULL) count++;
	rewind(fd);

	// store words in array
	char words[count][20];
	int i=0;
	while(1) {
		if (fgets(word, sizeof word, fd)==NULL) break;
		strcpy(words[i++], word);
	}
	fclose(fd);
	int randNum = rand() % count;
	strcpy(chosenWord, words[randNum]);
	chosenWord[strlen(chosenWord)-1] = 0;
	return chosenWord;
} 

char* getBlankWord(char *word) {
	// returns a word of blank spaces a long as word
	char *blankword = calloc(256, sizeof(char));
	for (int i=0; i<strlen(word); i++) strcat(blankword, " ");
	return blankword;
}

bool inWord(char *word, char letter) {
	// returns true if letter is in word
	for (int i=0; i<strlen(word); i++)
		if (word[i]==letter) return true;
	return false;
}

void updateCurrentWord(char *word, char *currentWord, char letter) {
	// replaces " " w/ letter in currentWord everywhere word is
	for (int i=0; i<strlen(currentWord); i++) {
		if (word[i]==letter) currentWord[i] = letter;
	}
}

char* getServerPacket(char *word, char *currentWord, char *incorrectLetters) {
	// creates server packet: msg_flag word_length num_incorrect currentWord incorrectLetters
	char *packet = calloc(256, sizeof(char));
	char wordLength[50];
	char numIncorrect[50];
	strcat(packet, "0");  // message flag
	sprintf(wordLength, "%lu", strlen(word));  // word length
	sprintf(numIncorrect, "%lu", strlen(incorrectLetters));  // num incorrect
	strcat(packet, wordLength);
	strcat(packet, numIncorrect);
	strcat(packet, currentWord);  // currentWord
	strcat(packet, incorrectLetters);
	return packet;
}

char* getServerPacketMssg(int flag, char *word) {
	// flag=0: lose, flag=1: win, flag=2: server overloaded
	char *packet = calloc(256, sizeof(char));
	int length = 36;
	length += strlen(word);
	if (flag==0) length +=11;
	else length += 8;
	char mssgLength[50];
	sprintf(mssgLength, "%i", length);
	strcat(packet, mssgLength);
	strcat(packet, ">>>The word was ");
	strcat(packet, word);
	if (flag==0) strcat(packet, "\n>>>You Lose!");
	if (flag==1) strcat(packet, "\n>>>You Win!");
	strcat(packet, "\n>>>Game Over!");
	return packet;

}

bool gameRound(int newsockfd, char *word, char *currentWord, char *incorrectLetters) {
	// returns true if game is still going, false otherwise
	char buffer[256];

	// make serverpacket
	char *newBuffer = calloc(256, sizeof(char));
	newBuffer = getServerPacket(word, currentWord, incorrectLetters);

	// send serverpacket
	bzero(buffer,256);
	memcpy(buffer, newBuffer, strlen(newBuffer));
	write(newsockfd, buffer, strlen(buffer));

	// read next letter guess
	bzero(buffer,256);
	read(newsockfd,buffer,255);
	char letterGuess[50];
	sprintf(letterGuess, "%c", buffer[0]);

	// if correct: add to currentWord
	if (inWord(word, letterGuess[0])) {
		updateCurrentWord(word, currentWord, letterGuess[0]);
	}  // if incorrect: add to incorrectLetters
	else if (!inWord(incorrectLetters, letterGuess[0]) &&
				letterGuess[0]!='\n' && letterGuess[0]) {
		strncat(incorrectLetters,letterGuess,1);
	}

	// check if game is over
	if (!strcmp(word,currentWord)) {  // if win
		bzero(newBuffer, 256);
		newBuffer = getServerPacketMssg(1,word);
		write(newsockfd, newBuffer, strlen(newBuffer));
		return false;
	}
	if (strlen(incorrectLetters)>=6) {  // if lose
		bzero(newBuffer, 256);
		newBuffer = getServerPacketMssg(0,word);
		write(newsockfd, newBuffer, strlen(newBuffer));
		return false;
	}
	return true;
}

void *socketThread(void *arg) {
	i++;
	int newsockfd = *((int *)arg);
	char buffer[256];
	bzero(buffer,256);
	if (i<4) {
		write(newsockfd, ">>>Ready to start game? (y/n): ", 31);

		// read initial client message
		read(newsockfd, buffer, 255);

		// start game (if "0")
		if (buffer[0]=='0') {
			char *word = getWord("hangman_words.txt");  // random word
			char *currentWord = getBlankWord(word);
			char *incorrectLetters = calloc(256, sizeof(char));
			while (gameRound(newsockfd, word, currentWord, incorrectLetters));
		}
	} else write(newsockfd, ">>>server-overloaded\n", 21);
	close(newsockfd);
	i--;
	return arg;
}

int main(int argc, char *argv[]) {
	int serverSocketFD, newsockfd, portno;
	socklen_t clilen;  // socket length variable
	struct sockaddr_in serv_addr, cli_addr;  // address structures
	pthread_t thread;
	serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char *) &serv_addr, sizeof(serv_addr));  // clears serv_addr struct
	portno = atoi(argv[1]);  // get port number
	serv_addr.sin_family = AF_INET;  // for IPv4
	serv_addr.sin_addr.s_addr = INADDR_ANY;  // where to accept connections from (all interfaces)
	serv_addr.sin_port = htons(portno);  // what port to bind to
	bind(serverSocketFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	listen(serverSocketFD,5);
	seed = atoi(argv[2]);
	srand(seed);

	i=0;
	while (true) {
		clilen = sizeof(cli_addr);
		newsockfd = accept(serverSocketFD, (struct sockaddr *) &cli_addr, &clilen);
		pthread_create(&thread, 0, socketThread, &newsockfd);
	}

	close(serverSocketFD);

	return 0;
}
