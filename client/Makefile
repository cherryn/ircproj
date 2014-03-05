CC= gcc
CFLAGS= -g -DLINUX 

all: ircclient

ircclient: ircclient.c util.o
	$(CC) $(CFLAGS) -o ircclient ircclient.c util.o

util.o: util.h util.c
	$(CC) $(CFLAGS) -c util.c

clean:
	rm ircclient
