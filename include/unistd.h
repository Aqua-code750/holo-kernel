#ifndef UNISTD_H
#define UNISTD_H

#include <stddef.h>

#define F_OK 0
#define R_OK 4
#define W_OK 2
#define X_OK 1

int access(const char *pathname, int mode);
int close(int fd);
int read(int fd, void *buf, size_t count);
int write(int fd, const void *buf, size_t count);
int lseek(int fd, int offset, int whence);

#endif
