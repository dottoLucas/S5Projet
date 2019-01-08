CC=gcc
CFLAGS=-Wall -Werror -g

projet:Partie1.o projet.o Partie2.o
	$(CC) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c $<

projet.o:Partie1.h Partie2.h
Partie1.o: Partie1.h
Partie2.o:Partie2.h
.PHONY:clean exec
clean:
	rm *.o	
exec: clean
	rm projet	