all: MyShell

MyShell: builtin.o parse.o shell.o redirect_in.o redirect_out.o run_command.o pipe_present.o pipe_command.o is_background.o timer.o
	gcc -Wall -o MyShell builtin.o parse.o shell.o redirect_in.o redirect_out.o run_command.o pipe_present.o pipe_command.o is_background.o timer.o

builtin.o:
	gcc -Wall -c builtin.c

parse.o:
	gcc -Wall -c parse.c

shell.o:
	gcc -Wall -c shell.c

redirect_in.o:
	gcc -Wall -c redirect_in.c

redirect_out.o:
	gcc -Wall -c redirect_out.c

run_command.o:
	gcc -Wall -c run_command.c

pipe_present.o:
	gcc -Wall -c pipe_present.c

pipe_command.o:
	gcc -Wall -c pipe_command.c

is_background.o:
	gcc -Wall -c is_background.c

timer.o:
	gcc -Wall -c timer.c

dep:
	gcc -M *.c > .depend

clean:
	rm -f MyShell *.o *~*.*~

