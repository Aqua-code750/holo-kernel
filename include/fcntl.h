#ifndef FCNTL_H
#define FCNTL_H

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2
#define O_CREAT  0x0200
#define O_TRUNC  0x0400
#define O_BINARY 0

int open(const char *pathname, int flags, ...);

#endif
