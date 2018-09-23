PROGS = usock_client usock_server

all : $(PROGS)

usock_client : main.o lookup7.o
	$(CC) -o $@ main.o lookup7.o

usock_server : usock_server.o lookup5.o
	$(CC) -o $@ usock_server.o lookup5.o

clean :
	rm -f *.o $(PROGS)
