#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

struct addrstr {
	char as_buf[INET6_ADDRSTRLEN];
};
int readb(int, char*, int);
int copydata(int, int, int);
struct addrstr sender_ip(struct sockaddr_storage *);
void write_C_STR(int, char *);
int init_socket(char *, char *, int);

#endif
