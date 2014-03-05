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

#define CMDMAXLEN (510+1)
#define CLRF(fd) write_C_STR(fd, "\r\n")

int mode_to_num(char * mode) {
	int len = strlen(mode);
	int m = 0, i;
	for(i=0; i<len; i++) {
		if( mode[i] == 'i' ) m &= 2;
		if( mode[i] == 'w' ) m &= 3;
	}
	return m;
}

int cmd_PASS(int fd, char * password) {
	if( password == NULL ) return 0;
	write_C_STR(fd, "PASS :");
	write_C_STR(fd, password);
	CLRF(fd);

	return 0;
}

// execute USER command, return zero on local success. (irc is async)
int cmd_USER(int fd, char * user, char * mode, char * realname) {
	char buf[CMDMAXLEN];
	int r;
	r=snprintf(buf, CMDMAXLEN, "USER %s %d * :%s",
			user,
			mode_to_num( mode ),
			realname);

	write_C_STR(fd, buf);
	CLRF(fd);

	return r>CMDMAXLEN;
}

int cmd_NICK(int fd, char * nickname) {
	write_C_STR(fd, "NICK :");
	write_C_STR(fd, nickname);
	CLRF(fd);

	return 0;
}


int main(int argc, char *argv[]) {
	if( argc < 4 ) {
		printf("USAGE: %s <HOST> <PORT> <NICK> [REALNAME]\n", argv[0]);
		return -1;
	}
	char * host = argv[1];
	char * port = argv[2];
	char * nickname = argv[3];
	char * user = "guest";
	char * mode = "";
	char * password = NULL;
	char * realname = ( argc<5? "" : argv[5] );

	int fd = init_socket(host, port, AF_INET);
	if( fd < 0 ) return 1;

	cmd_PASS( fd, password );
	cmd_NICK( fd, nickname );
	cmd_USER( fd, user, mode, realname );

	return 0;
}
