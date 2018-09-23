all: shm_server shm_client

shm_server: lookup5.o shm_server.o
	gcc -Wall -o shm_server lookup5.o shm_server.o

shm_client: main.o lookup6.o
	gcc -Wall -o shm_client main.o lookup6.o

lookup5.o:
	gcc -Wall -c lookup5.c

shm_server.o:
	gcc -Wall -c shm_server.c

main.o:
	gcc -Wall -c main.c

lookup6.o:
	gcc -Wall -c lookup6.c

dep:
	gcc -M *.c > .depend

clean:
	rm -f shm_server shm_client *.o *~*.*~
