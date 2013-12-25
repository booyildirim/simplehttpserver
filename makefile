CC	= gcc

default: mysrv

mysrv:  
	$(CC) -o mysrv myserver.c

clean:
	rm mysrv
