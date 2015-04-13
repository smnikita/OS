#include <helpers.h>
#include <bufio.h>

const size_t BUF_SZ = 4096;

int main(int argn, char** argv) {            
	struct buf_t *out = buf_new(BUF_SZ);
    struct buf_t *in = buf_new(BUF_SZ);
    char str[BUF_SZ];
    int i;    
    char* args[argn];
    for (i = 0; i < argn - 1; i++) {
        args[i] = argv[i + 1];
    }
    while (1) {
        ssize_t read_size = buf_getline(STDIN_FILENO, in, str);
        if (read_size == -1) {
            return 1;
        }
        if (read_size == 0) {
            buf_flush(STDOUT_FILENO, out, buf_size(out));
            return 0;
        }
        args[argn - 1] = str;
        if (str[read_size - 1] == '\n') {
            read_size--;
        }
        str[read_size] = 0;
        int status = spawn(args[0], args);
        if (status == 0) {
            str[read_size] = '\n';
            if (buf_write(STDOUT_FILENO, out, str, read_size + 1) == -1) {
                return 1;
            }
        }
    }
	/*
    	struct buf_t *in = buf_new(1000);
    	struct buf_t *out = buf_new(1000);    	
    	char str[1000];
		while (1) {
			ssize_t cnt = buf_getline(STDIN_FILENO, in, str);
			printf("%i\n", (int)cnt);
			if (cnt == -1) return 1;
			if (cnt == 0)  return 0;
			ssize_t write_rs = buf_write(STDOUT_FILENO, out, str, cnt); 
			if (write_rs == -1) return 1;
		}	
	*/
    return 0;
}



