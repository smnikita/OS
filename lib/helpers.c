#include "helpers.h"

ssize_t read_(int fd, void* buf, size_t count) {
	size_t read_cnt = 0;
	while (1) {
		ssize_t res = read(fd, buf + read_cnt, count - read_cnt);
		read_cnt += res;
		if (res == 0) return read_cnt;
		if (res == -1) return -1;		
	}
}

ssize_t write_(int fd, const void* buf, size_t count) {
	size_t written_cnt = 0;
	while (1) {
		ssize_t res = write(fd, buf + written_cnt, count - written_cnt);		
		written_cnt += res;
		if (written_cnt >= count) return written_cnt;
		if (res == -1) return -1;
	}
}


ssize_t read_until(int fd, void * buf, size_t count, char delimiter) {
	ssize_t offset = 0;		
	while (1) {
		ssize_t read_cnt = read(fd, buf + offset, count - offset);
		if (read_cnt == -1) return -1;
		if (read_cnt == 0) return offset;			
		for (int i = 0; i < read_cnt; i++) {
			if (*((char*)buf + offset + i) == delimiter) {
				return offset + read_cnt;
			}
		}
		offset += read_cnt;
	}
}


int spawn(const char * file, char * const argv []) {
	int res = fork();
	if (res == 0) {
		exit(execvp(file, argv));
	} else if (res > 0) {
		int status;
		wait(&status);
		return status;
	} else {
		return -1;
	}
}