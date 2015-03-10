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