#include <helpers.h>

const size_t BUF_SZ = 4097;

int main(int argn, char** argv) {    
    char buf[BUF_SZ];    
    char* args[argn];
    ssize_t buffer_size = 0;

    for (int i = 0; i < argn - 1; i++) {
        args[i] = argv[i + 1];
    }

    while (1) {
        ssize_t read_size = read_until(STDIN_FILENO, buf + buffer_size, BUF_SZ - buffer_size, '\n');
        if (read_size == -1) return 1;        
        if (read_size == 0) {
            if (buffer_size == 0) return 0;            
            args[argn - 1] = buf;
            buf[buffer_size] = 0;
            int res = spawn(args[0], args);
            if (res == 0) {
                buf[buffer_size] = '\n';
                ssize_t write_size = write_(STDOUT_FILENO, buf, buffer_size + 1);
                if (write_size == -1) return 1;            
            } else if (res == -1) return 1;            
            break;
        }
        int i = buffer_size;
        buffer_size += read_size;
        int l = 0;
        for (; i < buffer_size; i++) {
            if (buf[i] == '\n') {
                args[argn - 1] = buf + l;
                buf[i] = 0;
                int res = spawn(args[0], args);
                if (res == 0) {
                    buf[i] = '\n';
                    ssize_t write_size = write_(STDOUT_FILENO, buf + l, i - l + 1);
                    if (write_size == -1) return 1;                    
                } else if (res == -1) return 1;                
                l = i + 1;
            }
        }
        buffer_size -= l;
        for (int i = 0; i < buffer_size; i++) {
            buf[i] = buf[i + l];
        }
    }
    
    return 0;
}