#ifndef SYS_STAT_H
#define SYS_STAT_H

struct stat {
    int st_mode;
    int st_size;
};

int stat(const char *pathname, struct stat *statbuf);
int mkdir(const char *pathname, int mode);

#endif
