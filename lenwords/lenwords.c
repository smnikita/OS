#include <helpers.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

const int BUF_SZ = 4096 + 3;

int main() {
	char buffer[BUF_SZ];
	char write_buffer[33];
	ssize_t cnt = 1;
	int offset = 0, lst = 0;
	while (1) {
		cnt = read_until(STDIN_FILENO, buffer + offset, BUF_SZ - offset, ' ');
		if (cnt <= 0) break;
		lst = 0;
		for (int i = offset; i < offset + cnt; i++) {
			if (buffer[i] == ' ') {
				sprintf(write_buffer, "%d\n", i - lst);
				lst = i + 1;
				if (write_(STDOUT_FILENO, write_buffer, strlen(write_buffer)) < 0) return 1;
			}
		}
		for (int i = lst; i < cnt + offset; i++) {
			buffer[i - lst] = buffer[i];
		}
		offset = cnt - lst + offset;
	}
	sprintf(write_buffer, "%d\n", offset - 1);
	if (write_(STDOUT_FILENO, write_buffer, strlen(write_buffer)) < 0) {
		return 1;
	}
	if (cnt < 0) return 1; else return 0;
}
	