all: stack

stack:
	gcc -Wall -o stack stack.c -lpthread

dep:
	gcc -M *.c > .depend

clean:
	rm -f stack *.o *~*.*~
