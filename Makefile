# Makefile

CC = gcc
ARGS = -Wall

all: hangman_client hangman_server

hangman_client: hangman_client.c
		$(CC) $(ARGS) $^ -o $@

hangman_server: hangman_server.c
		$(CC) $(ARGS) $^ -o $@ -lpthread

clean:
	rm -f *.o *~ hangman_client hangman_server
