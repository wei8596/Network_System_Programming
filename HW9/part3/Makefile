PROGS = udp_client udp_server

all : $(PROGS)

udp_client : main.o lookup9.o
	$(CC) -o $@ main.o lookup9.o -lnsl

udp_server : udp_server.o lookup5.o
	$(CC) -o $@ udp_server.o lookup5.o -lnsl

clean :
	rm -f *.o $(PROGS)
