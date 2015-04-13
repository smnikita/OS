#ifndef BUFIO_H
#define BUFIO_H

#include <stdlib.h>
#include <unistd.h>

#define fd_t int

struct buf_t {
	size_t capacity;
    size_t size;
};

struct buf_t *buf_new(size_t capacity);
void buf_free(struct buf_t *);
size_t buf_capacity(struct buf_t *);
size_t buf_size(struct buf_t *);
ssize_t buf_fill(fd_t fd, struct buf_t *buf, size_t required);
ssize_t buf_flush(fd_t fd, struct buf_t *buf, size_t required);

ssize_t buf_getline(fd_t fd, struct buf_t *buf, char *dest);
ssize_t buf_write(fd_t fd, struct buf_t *buf, char *src, size_t len);

#endif
