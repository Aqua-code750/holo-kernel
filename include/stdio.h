#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>
#include <stdarg.h>

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define EOF (-1)

typedef struct {
    void* base;
    size_t size;
    size_t pos;
} FILE;

FILE* fopen(const char* filename, const char* mode);
int fclose(FILE* stream);
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);
int fseek(FILE* stream, long int offset, int whence);
long int ftell(FILE* stream);
int feof(FILE* stream);

int remove(const char *filename);
int rename(const char *old_filename, const char *new_filename);

int printf(const char* format, ...);
int fprintf(FILE* stream, const char* format, ...);
int sprintf(char* str, const char* format, ...);
int snprintf(char* str, size_t size, const char* format, ...);
int sscanf(const char *str, const char *format, ...);
int vprintf(const char* format, va_list ap);
int vfprintf(FILE *stream, const char *format, va_list ap);
int vsnprintf(char* str, size_t size, const char* format, va_list ap);
int puts(const char* s);
int putchar(int c);

extern FILE* stdout;
extern FILE* stderr;
#define fflush(x)

#endif
