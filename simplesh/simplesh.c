#include "bufio.h"
#include "helpers.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

const int BUF_SIZE = 4096;
const int ARGS_NUM = 2048;

void sigint_handler(int sig) {

}

int is_splitter(char c) {
    return c == '|' || c == '\0' || c == ' ' || c == '\t' || c == '\n';
}

int main() {
    struct sigaction sigact;
    memset(&sigact, '\0', sizeof(sigact));
    sigact.sa_handler = &sigint_handler;

    if (sigaction(SIGINT, &sigact, NULL) < 0) return 1;

    struct buf_t* buf = buf_new(BUF_SIZE);
    char str[BUF_SIZE];
    int rc, i, offset, k, argc;
    while (1) {
        if (write_(STDOUT_FILENO, "$ ", 2) < 1) return 1;
        rc = buf_getline(STDIN_FILENO, buf, str);
        if (rc == 0) return 0;        
        if (rc < 0) {
            if (write_(STDOUT_FILENO, "\n$ ", 2) < 1) return 1;            
            continue;
        }
        str[rc] = '\0';
        execargs_t* arguments[ARGS_NUM];
        offset = 0;
        k = 0;
        while (1) {            
            argc = 0;
            char *s = str + offset;
            if (*s == '\0') return 0;            
            s++;
            while (1) {
                if (!is_splitter(*(s - 1)) && is_splitter(*s)) argc++;
                if (*s == '\0' || *s == '|') break;
                s++;
            }            

            if (argc == 0) break;            
            char* argv[argc];
            for (i = 0; i < argc; i++) {                
                while (str[offset] == ' ' || str[offset] == '\t') {
                    offset++;
                }
                int start = offset;
                while (!is_splitter(str[offset])) {
                    offset++;
                }
                if (start == offset) {
                    argv[i] = 0;
                } else {
                    argv[i] = strndup(str + start, offset - start);
                }
            }
            arguments[k] = (execargs_t*) malloc(sizeof(execargs_t));
            *arguments[k] = new_execargs_t(argc, argv);            
             while (str[offset] == ' ' || str[offset] == '\t') {
               offset++;
            }
            if (str[offset] == '|') offset++;
            k++;
        }
        if (runpiped(arguments, k) < 0) return 1;
    }

    return 0;
}
