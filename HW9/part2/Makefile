PROGS = isock_client isock_server

all : $(PROGS)

isock_client : main.o lookup8.o
	$(CC) -o $@ main.o lookup8.o -lnsl

isock_server : isock_server.o lookup5.o
	$(CC) -o $@ isock_server.o lookup5.o -lnsl

clean :
	rm -f *.o $(PROGS)
