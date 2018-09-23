all: hostinfo mydate printdir mycat pipe_ls

hostinfo:
	gcc -Wall -o hostinfo hostinfo.c

mydate:
	gcc -Wall -o mydate mydate.c

printdir:
	gcc -Wall -o printdir printdir.c

mycat:
	gcc -Wall -o mycat mycat.c

pipe_ls:
	gcc -Wall -o pipe_ls pipe_ls.c


dep:
	#gcc -M *.c > .depend

clean:
	rm -f hostinfo mydate printdir mycat pipe_ls *.o *~*.*~
