#include <helpers.h>
#include <stdio.h>
#include <unistd.h>

const int BUF_SZ = 4096 + 1;
const char delimiter = ' ';

int main() {
	char buffer[BUF_SZ];
	char out[BUF_SZ];	
	int offset = 0;
	while (1) {
		ssize_t cnt = read_until(STDIN_FILENO, buffer + offset, BUF_SZ - offset, delimiter);	
		if (cnt < 0) return 1;
		if (cnt == 0) break;
		int l = 0;
		for (int i = offset; i < offset + cnt; i++) {
			if (buffer[i] == delimiter) {
				int cur = 0;
				for (; cur != (i - l); cur++) {
					out[cur] = buffer[i - cur - 1];					
				}
				out[cur++] = delimiter;
				l = i + 1;
				if (write_(STDOUT_FILENO, out, cur) < 0) {
					return 1;
				}
			}
		}
		for (int i = l; i < cnt + offset; i++) {
			buffer[i - l] = buffer[i];
		}
		offset += cnt - l;
	}
	for (int i = 0; i < offset; i++) {
		out[i] = buffer[offset - i - 1];
	}
	if (write_(STDOUT_FILENO, out, offset) < 0) return 1; else return 0;	
}	