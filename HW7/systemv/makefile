all: producer consumer

producer:
	gcc -Wall -o producer producer.c

consumer:
	gcc -Wall -o consumer consumer.c

dep:
	gcc -M *.c > .depend

clean:
	rm -f producer consumer *.o *~*.*~
