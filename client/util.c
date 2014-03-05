#include "util.h"


void write_C_STR(int fd, char * str) {
	//char buf[2024];
	(void) write(fd, str, strlen(str));
	//sprintf(buf, "'%s' to %d\n", str, fd);
	//write(2, buf, strlen(buf));
}

static int _bind_reusable(int fd, struct addrinfo *p) {
	int yes = 1; // used to set property reuseaddr
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))==-1) {
		return -1;
	}
	if(bind(fd, p->ai_addr, p->ai_addrlen) < 0) {
		return -1;
	}
	return 0;
}

static int _connect(int fd, struct addrinfo *p) {
	if(connect(fd, p->ai_addr, p->ai_addrlen) == -1) {
		return -1;
	}
	return 0;
}

/*------------------------------------------------------------------------
 * readb - fills buf until len bytes are read or fails to read. Returns
 * number of bytes read.
 *------------------------------------------------------------------------
 */
int readb(int fd, char *buf, int len) {
	int i;
	for(i=0; i<len; i++) {
		if( read(fd, &buf[i], 1) <= 0 ) break;
	}
	return i;
}

int copydata(int dest_fd, int src_fd, int bytes) {
	char ch;
	int ii;
	//fprintf(stderr, "Copying %d bytes from %d to %d\n", bytes, src_fd, dest_fd);
	for(ii=0; ii<bytes; ii++) {
		if( read(src_fd, &ch, 1) < 1 ) break;
		//fprintf(stderr, "[%c", ch);
		if( write(dest_fd, &ch, 1) < 1 ) break;
		//fprintf(stderr, "->%c]\n", ch);
	}
	return ii;
}

int init_socket(char * host, char * port, int addr_type) {
	struct addrinfo hints, *p;
	struct addrinfo *resolvedAddrs;
	int err_val, fd; // Reused throughout code
	int (*init_func)(int, struct addrinfo*);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = addr_type;		// IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;	// TCP

	if( host == NULL ) {
		hints.ai_flags = AI_PASSIVE;		// Use local machine addr for host
		init_func = _bind_reusable;
	}
	else {
		init_func = _connect;
	}

	if( (err_val=getaddrinfo(host,port,&hints,&resolvedAddrs)) != 0 ) {
		fprintf(stderr,"Resolve listener: %s\n", gai_strerror(err_val));
		return -2;
	}

	for(p=resolvedAddrs; p!=NULL; p = p->ai_next) {
		fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if(fd < 0) {
			perror("socket:");
			continue;
		}

		if(init_func(fd, p) < 0 ) {
			perror("init:");
			close(fd);
			continue;
		}

		break;
	}
	freeaddrinfo(resolvedAddrs);
	if(p==NULL) {
		// Failed to bind to any address.
		fprintf(stderr,"Failed to initialize socket\n");
		return -2;
	}

	return fd;
}
