all: convert file_lookup

convert: convert.o
	gcc -Wall -o convert convert.o

file_lookup: lookup1.o main.o
	gcc -Wall -o file_lookup main.o lookup1.o

convert.o:
	gcc -Wall -c convert.c

lookup1.o:
	gcc -Wall -c lookup1.c

main.o:
	gcc -Wall -c main.c

dep:
	gcc -M *.c > .depend

clean:
	rm -f convert file_lookup *.o *~*.*~
