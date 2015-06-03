#include "bufio.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

const int BUF_SZ = 4096;

int create_server(struct addrinfo *info) {
	int sock1 = socket(info->ai_family, SOCK_STREAM, 0);
	if (sock1 < 0) {
		return -1;
	}
	if (bind(sock1, info->ai_addr, info->ai_addrlen)) {
		return -1;
	}
	if (listen(sock1, 1)) {
		return -1;
	}
	return sock1;
}

int main(int argc, char** argv) {
	if (argc < 3) {
		write(STDOUT_FILENO, "Usage: <port1> <port2>\n", 23);
		return 1;
	}
		
	struct addrinfo *info1, *info2;
	if (getaddrinfo("localhost", argv[1], 0, &info1)) {
		return 1;
	}
	if (getaddrinfo("localhost", argv[2], 0, &info2)) {
		return 1;
	}
	
	int sock1 = create_server(info1);
	int sock2 = create_server(info2);
	if (sock1 < 0 || sock2 < 0) {
		return 1;
	}
	
	freeaddrinfo(info1);
	freeaddrinfo(info2);
	
	while (1) {
		int client1 = accept(sock1, 0, 0);
		if(client1 < 0) return 1;
		
		int client2 = accept(sock2, 0, 0);
		if(client2 < 0) return 1;

		pid_t pid1 = fork();
		if(pid1 < 0) return 1;				
		if(pid1 == 0) {
			close(sock1);
			close(sock2);
			struct buf_t *buf = buf_new(BUF_SZ);
			if (!buf) return 1;
			while (1) {
				int bfr = buf_fill(client1, buf, 1);
				if (bfr < 0) {
					if(buf_flush(client2, buf, buf_size(buf)) < 0) return 1;
					return 1;
				}
				if (bfr == 0) return 0;
				if (buf_flush(client2, buf, buf_size(buf)) < 0) return 1;
			}
			return 0;
		}
		
		pid_t pid2 = fork();
		if(pid2 < 0) {
			kill(pid1, SIGKILL);
			return 1;
		}
		if(pid2 == 0) {
			close(sock1);
			close(sock2);
			struct buf_t *buf = buf_new(BUF_SZ);
			if (!buf) return 1;
			while (1) {
				int bfr = buf_fill(client2, buf, 1);
				if (bfr < 0) {
					if(buf_flush(client1, buf, buf_size(buf)) < 0) return 1;
					return 1;
				}
				if (bfr == 0) return 0;
				if (buf_flush(client1, buf, buf_size(buf)) < 0) return 1;
			}
			return 0;
		}
		close(client1);
		close(client2);
	}
	return 0;
}
