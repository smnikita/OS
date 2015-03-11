#include <helpers.h>
#include <stdio.h>

const int BUF_SZ = 4096;

int main() {
	char buf[BUF_SZ];
	while (1) {
		ssize_t cnt = read_(STDIN_FILENO, buf, BUF_SZ);
		if (cnt == -1) return 1;
		if (cnt == 0)  return 0;
		ssize_t write_rs = write_(STDOUT_FILENO, buf, cnt);
		if (write_rs == -1) return 1;
	}	
}