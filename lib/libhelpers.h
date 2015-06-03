#ifndef LIBHELPERS_C
#define LIBHELPERS_C

#include <sys/types.h>


struct execargs_t {
	char *name;
	char **args;
};

int exec(struct execargs_t* args);
int runpiped(struct execargs_t** programs, size_t n);

#endif