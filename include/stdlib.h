#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

void* malloc(size_t size);
void free(void* ptr);
void* realloc(void* ptr, size_t size);
void* calloc(size_t nmemb, size_t size);

int atoi(const char* str);
double atof(const char* str);
int abs(int j);
void exit(int status);
int system(const char *command);

#endif
