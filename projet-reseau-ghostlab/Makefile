all: Client.class server

Client.class:
	javac Client.java

jeu.o: jeu.c
	gcc -c jeu.c

server.o: server.c
	gcc -c server.c

server: server.o jeu.o
	gcc -pthread -Wall -o server server.o jeu.o

distclean :
	rm -rf *.o
	rm server
	rm Client.class
