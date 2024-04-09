CFLAGS=-Wall -Werror -Wfatal-errors

main.o: main.c 
	gcc $(CFLAGS) -c main.c

main.x: main.o
	gcc $(CFLAGS) -o main.x main.o -lm

clean:
	rm *.o

realclean: clean
	rm *.x