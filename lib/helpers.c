#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <sys/types.h>

#include "helpers.h"
#include <string.h>
#include <signal.h>

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


execargs_t new_execargs_t(int argc, char** argv) {
    execargs_t ret;
    int i;
    ret.argv = (char**) malloc((argc + 1) * sizeof(char*));
    for (i = 0; i < argc; i++)
        ret.argv[i] = strdup(argv[i]);
    ret.argv[argc] = NULL;
    return ret;
}


int exec(execargs_t* args) {
    return spawn(args->argv[0], args->argv);
}

int childn;
int* childa;

void sig_handler(int sig) {	
	for (int i = 0; i < childn; i++) {
	    kill(childa[i], SIGKILL);
	    waitpid(childa[i], NULL, 0);
	}
	childn = 0;	
}

int runpiped(execargs_t** programs, size_t n) {	
    if (n == 0) return 0;
    
    int pipefd[n - 1][2];
    
    int child[n];
    for (int i = 0; i + 1 < n; i++) {
        if (pipe2(pipefd[i], O_CLOEXEC) < 0) {
            return -1;
        }
    }

    for (int i = 0; i < n; i++) {
        child[i] = fork();
        if (child[i] == -1) {
        	for (int j = 0; j < i; j++) {
        		kill(child[j], SIGKILL);
        		waitpid(child[j], NULL, 0);
        	}
        	return -1;        
        }
        if (child[i] == 0) {
            if (i + 1 < n) dup2(pipefd[i][1], STDOUT_FILENO);            
            if (i > 0) dup2(pipefd[i - 1][0], STDIN_FILENO);            
            execvp(programs[i]->argv[0], programs[i]->argv);
        }        
    }
    for (int i = 0; i + 1 < n; i++) {
        close(pipefd[i][0]);
        close(pipefd[i][1]);
    }

    childn = n;
    childa = (int*)child;
    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_handler = &sig_handler;

    if (sigaction(SIGINT, &act, NULL) < 0) {
    	for (int j = 0; j < n; j++) {
        		kill(child[j], SIGKILL);
        		waitpid(child[j], NULL, 0);
        }
    	return -1;
    }

    int status;
    for (int i = 0; i < n; i++) {
        waitpid(child[i], &status, 0);
    }
    childn = 0;    
    return 0;
}

