#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

ssize_t read_(int fd, void *buf, size_t count);
ssize_t write_(int fd, const void *buf, size_t count);
ssize_t read_until(int fd, void * buf, size_t count, char delimiter);
int spawn(const char * file, char * const argv []);

typedef struct execargs_t {
    char** argv;
} execargs_t;

execargs_t new_execargs_t(int argc, char** argv);

int exec(execargs_t* args);

int runpiped(execargs_t** programs, size_t n);

#endif