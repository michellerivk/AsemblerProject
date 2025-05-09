first_passing: first_passing.o
	gcc first_passing.o -o first_passing

first_passing.o: first_passing.c data_structs.h
	gcc -ansi -Wall -pedantic -g -c first_passing.c

clean:
	rm -f *.o mainmat