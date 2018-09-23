all: fifo_server fifo_client

fifo_server: lookup2.o fifo_server.o
	gcc -Wall -o fifo_server lookup2.o fifo_server.o

fifo_client: main.o lookup3.o
	gcc -Wall -o fifo_client main.o lookup3.o

lookup2.o:
	gcc -Wall -c lookup2.c

fifo_server.o:
	gcc -Wall -c fifo_server.c

main.o:
	gcc -Wall -c main.c

lookup3.o:
	gcc -Wall -c lookup3.c

dep:
	gcc -M *.c > .depend

clean:
	rm -f fifo_server fifo_client *.o *~*.*~
