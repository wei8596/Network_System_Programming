all: vm

vm:
	gcc -Wall -o vm vm.c -lpthread

dep:
	gcc -M *.c > .depend

clean:
	rm -f vm *.o *~*.*~

