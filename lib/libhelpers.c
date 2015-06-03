#include "libhelpers.h"


int exec(struct execargs_t* args) {
	pid_t pid = fork();
	if (pid == 0) {
		execvp(args->name, args->args);
	}
	return pid;
}

int 

int runpiped(struct execargs_t** programs, size_t n) {

}