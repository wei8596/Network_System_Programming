all: myshell

myshell: shell.o parse.o builtin.o run_command.o is_background.o
	gcc -Wall -o myshell shell.o parse.o builtin.o run_command.o is_background.o

shell.o:
	gcc -Wall -c shell.c

parse.o:
	gcc -Wall -c parse.c

builtin.o:builtin.c
	gcc -Wall -c builtin.c

run_command.o:run_command.c
	gcc -Wall -c run_command.c

is_background.o:is_background.c
	gcc -Wall -c is_background.c

dep:
	gcc -M *.c > .depend

clean:
	rm -f myshell *.o *~*.*~
