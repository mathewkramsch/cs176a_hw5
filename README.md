# CS 176A: Homework 5
## Game-Server and Player-Client for Hangman Game in C

**Authors:** Irene Evans and Mathew Kramsch

---
## Usage 
### Client Input/Output:
**Input:**  <br>
```
./hangman_client IP PORT
```

Example:  <br>
```
$ ./hangman_client localhost 8080
```

**Output:**  <br>
```
>>>./hangman_client 0.0.0.0 8080
>>> Ready to start game? (y/n): y
>>>_ _ _
>>>Incorrect Guesses:
>>>
>>>Letter to guess: a
>>>_ _ _
>>>Incorrect Guesses: a
>>>
>>>Letter to guess: d
>>>d _ _ ← Server responds
>>>Incorrect Guesses: a
>>>
>>>Letter to guess: g
>>>d _ g
>>>Incorrect Guesses: a
>>>
>>>Letter to guess: o
>>>The word was dog
>>>You Win!
>>>Game Over!
```
---
### Server Input:
```
$ ./hangman_server PORT SEED
```
Example:  <br>
```
$ ./hangman_server 8080 8
```
