all: producer consumer

producer:
	gcc -Wall -o producer producer.c -lrt

consumer:
	gcc -Wall -o consumer consumer.c  -lrt

dep:
	gcc -M *.c > .depend

clean:
	rm -f producer consumer *.o *~*.*~
