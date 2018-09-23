all: msgq_server msgq_client

msgq_server: lookup2.o msgq_server.o
	gcc -Wall -o msgq_server lookup2.o msgq_server.o

msgq_client: main.o lookup4.o
	gcc -Wall -o msgq_client main.o lookup4.o

lookup2.o:
	gcc -Wall -c lookup2.c

msgq_server.o:
	gcc -Wall -c msgq_server.c

main.o:
	gcc -Wall -c main.c

#turn off stack protection
#if not turn off,
#it breaks the ability to compile anything that's not linked against the standard userspace libraries
lookup4.o:
	gcc -Wall -c -fno-stack-protector lookup4.c

dep:
	gcc -M *.c > .depend

clean:
	rm -f msgq_server msgq_client *.o *~*.*~
