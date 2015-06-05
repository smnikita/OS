#define _GNU_SOURCE

#include "bufio.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <poll.h>
#include <stdio.h>

#define BUF_SZ 4096
#define MAX_CONNECTS 127

struct pollfd polls[MAX_CONNECTS * 2 + 2];
struct buf_t* buffs[MAX_CONNECTS * 2];
int polls_cnt = 0;
int current = 0;

int create_server(struct addrinfo* info) {
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

void close_pair(int i) {
	if (polls_cnt == 2 * MAX_CONNECTS) polls[current].events = POLLIN;
	close(polls[i + 2].fd);
	close(polls[(i + 2) ^ 1].fd);
	buf_free(buffs[i]);
	buf_free(buffs[i ^ 1]);
	if (polls_cnt > 3) {
		i &= ~1;
		polls[i + 2] = polls[polls_cnt];
		polls[i + 3] = polls[polls_cnt + 1];
		buffs[i] = buffs[polls_cnt - 2];
		buffs[i + 1] = buffs[polls_cnt - 1];
	}
	polls_cnt -= 2;
}

int main(int argc, char** argv) {
	if (argc != 3) {
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
	
	polls[0].fd = sock1;
	polls[1].fd = sock2;

	polls[0].events = POLLIN;

	while (1) {
		int cnt = poll(polls, polls_cnt + 2, -1);
		if (cnt < 0) {			
			continue;
		}
		short event = polls[current].revents;
		if (event) {
			if (event & POLLIN) {
				polls[current].events = 0;	
				polls[current].revents = 0;	
				int new_fd = accept(polls[current].fd, 0, 0);
				if (new_fd < 0) {					
					continue;
				}
				buffs[polls_cnt] = buf_new(BUF_SZ);
				polls[polls_cnt + 2].events = POLLIN | POLLRDHUP;
				polls[polls_cnt + 2].fd = new_fd;
				polls_cnt++;
				current ^= 1;
				if (polls_cnt < 2 * MAX_CONNECTS)
					polls[current].events = POLLIN;
			}
		}
		for (int i = 0; i < polls_cnt; i++) {
			short event = polls[i + 2].revents;
			polls[i + 2].revents = 0;
			if (event) {
				if (event & POLLOUT) {
					size_t old = buf_size(buffs[i ^ 1]);
					buf_flush(polls[i + 2].fd, buffs[i ^ 1], 1);
					if (buf_size(buffs[i ^ 1]) == 0) 
						polls[i + 2].events &= ~POLLOUT;
					if (old == buf_capacity(buffs[i ^ 1]) && buf_size(buffs[i ^ 1]) < buf_capacity(buffs[i ^ 1])) 
						polls[(i + 2) ^ 1].events |= POLLIN;
				}
				if (event & POLLIN) {
					size_t old = buf_size(buffs[i]);	
					buf_fill(polls[i + 2].fd, buffs[i], buf_size(buffs[i]) + 1);
					if (buf_size(buffs[i]) == buf_capacity(buffs[i]))
						polls[i + 2].events &= ~POLLIN;
					if (old == 0 && buf_size(buffs[i]) > 0) 
						polls[(i + 2) ^ 1].events |= POLLOUT;
				}
				if (event & POLLRDHUP) {
					shutdown(polls[(i + 2) ^ 1].fd, SHUT_WR);
					polls[(i + 2) ^ 1].events &= ~POLLOUT;
					if (~polls[i + 2].events & ~polls[(i + 2) ^ 1].events) {
						close_pair(i);
						i |= 1;
						continue;
					}
				}
				if (event & POLLHUP) {
					shutdown(polls[(i + 2) ^ 1].fd, SHUT_RD);
					polls[(i + 2) ^ 1].events &= ~POLLIN;
					if (~polls[i + 2].events & ~polls[(i + 2) ^ 1].events) {
						close_pair(i);
						i |= 1;
						continue;
					}
				}
				if (event & POLLERR) {
					i |= 1;
					close_pair(i);
				}
			}
		}
	}
	return 0;
}
