#include "bufio.h"

struct buf_t *buf_new(size_t capacity) {
    struct buf_t *p = malloc(2 * sizeof(size_t) + capacity);    
    p->capacity = capacity;
    p->size = 0;
    return p;
}

void buf_free(struct buf_t * p) {
    free(p);
}

size_t buf_capacity(struct buf_t * p) {
    #ifdef DEBUG
        if (buf == NULL) abort();
    #endif
    return p->capacity;
}

size_t buf_size(struct buf_t * p) {
    #ifdef DEBUG
        if (buf == NULL) abort();        
    #endif
    return p->size;
}

ssize_t buf_fill(fd_t fd, struct buf_t *buf, size_t required) {
    #ifdef DEBUG
        if (buf == NULL || buf->capacity < required) abort();        
    #endif
    char* buffer = (char*)buf + 2 * sizeof(size_t);
    ssize_t read_size;
    while (1) {
        read_size = read(fd, buffer + buf->size, required - buf->size);
        if (read_size == -1) {
            return -1;
        }
        buf->size += read_size;
        if (read_size == 0 || buf->size >= required) {
            return buf->size;
        }
    }
}

ssize_t buf_flush(fd_t fd, struct buf_t *buf, size_t required) {
    #ifdef DEBUG
        if (buf == NULL) abort();        
    #endif
    char* buffer = (char*)buf + 2 * sizeof(size_t);    
    ssize_t written_cnt = 0;
    while (1) {
        ssize_t write_size = write(fd, buffer + written_cnt, buf->size - written_cnt);
        if (write_size == -1) return -1;        
        written_cnt += write_size;
        if (written_cnt >= required || buf->size == written_cnt) {
            int i;
            for (i = 0; i < buf->size - written_cnt; i++) {
                buf[i] = buf[i + written_cnt];
            }
            buf->size -= written_cnt;
            return written_cnt;
        }
    }
}

ssize_t buf_getline(fd_t fd, struct buf_t *buf, char* dest) {
    #ifdef DEBUG
        if (buf == NULL) abort();        
    #endif
    char* buffer = (char*)buf + 2 * sizeof(size_t);
    int i = 0, j = 0, written_cnt = 0;    


    for (i = 0; i < buf->size; i++) {
        dest[written_cnt] = buffer[i];
        written_cnt++;
        if (buffer[i] == '\n') {
            for (j = 0; j < buf->size - written_cnt; j++) {
                buffer[j] = buffer[j + written_cnt];
            }
            buf->size -= written_cnt;
            return written_cnt;
        }
    }


    buf->size = 0;
    while (1) {
        ssize_t read_size = read(fd, buffer, buf->capacity);
        if (read_size == -1) return -1;        
        if (read_size == 0) return written_cnt;
        
        buf->size = read_size;
        for (i = 0; i < read_size; i++) {
            dest[written_cnt] = buffer[i];
            written_cnt++;
            if (buffer[i] == '\n') {
                for (j = 0; j < buf->size - i - 1; j++) {
                    buffer[j] = buffer[j + i + 1];
                }
                buf->size -= i + 1;
                return written_cnt;
            }
        }
    }   
}

ssize_t buf_write(fd_t fd, struct buf_t *buf, char* src, size_t len) {
    #ifdef DEBUG
        if (buf == NULL) abort();        
    #endif
    char* buffer = (char*)buf + 2 * sizeof(size_t);
    int i = 0, j = 0;


    for (i = 0; i < len; i++) {
        if (buf->size == buf->capacity) {
            ssize_t write_size = write(fd, buffer, buf->capacity);
            if (write_size <= 0) return -1;            
            for (j = 0; j < buf->capacity - write_size; j++) {
                buffer[j] = buffer[j + write_size];
            }
            buf->size -= write_size;
        }
        buffer[buf->size] = src[i];
        buf->size++;
    }
    return len;
}