#include "bufio.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

const int BUF_SZ = 4096;

int main(int argc, char** argv) {
	if (argc < 3) {
		write(STDOUT_FILENO, "Usage: <port> <file>\n", 21);
		return 1;
	}	
	struct addrinfo *info;
	if (getaddrinfo("localhost", argv[1], 0, &info)) {
		return 1;
	}
	int sock = socket(info->ai_family, SOCK_STREAM, 0);
	if (sock < 0) {
		return 1;
	}
	if (bind(sock, info->ai_addr, info->ai_addrlen)) {
		return 1;
	}
	if (listen(sock, 1)) {
		return 1;
	}
	freeaddrinfo(info);
	while (1) {
		int client = accept(sock, 0, 0);
		if (client < 0) {
			return 1;
		}
		pid_t pid = fork();
		if (pid < 0) return 1;		
		if (pid == 0) {
			close(sock);
			int file = open(argv[2], O_RDONLY);
			if(file < 0) {
				return 1;
			}
			struct buf_t *buf = buf_new(BUF_SZ);
			if(!buf) return 1;			
			while (1) {
				ssize_t bfr = buf_fill(file, buf, 1);
				if(bfr < 0) {
					if(buf_flush(client, buf, buf_size(buf)) < 0) return 1;
					return 1;
				}
				if(bfr == 0) break;
				if(buf_flush(client, buf, buf_size(buf)) < 0) return 1;
			}
			buf_free(buf);
			return 0;
		}
		close(client);
	}
	return 0;
}