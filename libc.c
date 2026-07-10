#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "limits.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

int errno = 0;

extern void* kmalloc(size_t size);
extern void kfree(void* ptr);
extern void* krealloc(void* ptr, size_t size);

extern uint32_t doom_wad_addr;
extern uint32_t doom_wad_size;

FILE _stdout = {0, 0, 0};
FILE _stderr = {0, 0, 0};
FILE* stdout = &_stdout;
FILE* stderr = &_stderr;

// Memory
void* malloc(size_t size) { return kmalloc(size); }
void free(void* ptr) { kfree(ptr); }
void* realloc(void* ptr, size_t size) { return krealloc(ptr, size); }
void* calloc(size_t nmemb, size_t size) {
    void* ptr = kmalloc(nmemb * size);
    if (ptr) memset(ptr, 0, nmemb * size);
    return ptr;
}

// Memory block
void* memcpy(void* dest, const void* src, size_t n) {
    char* d = dest;
    const char* s = src;
    while(n--) *d++ = *s++;
    return dest;
}

void* memset(void* s, int c, size_t n) {
    char* p = s;
    while(n--) *p++ = (char)c;
    return s;
}

void* memmove(void* dest, const void* src, size_t n) {
    char* d = dest;
    const char* s = src;
    if (d < s) {
        while (n--) *d++ = *s++;
    } else {
        d += n;
        s += n;
        while (n--) *--d = *--s;
    }
    return dest;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* p1 = s1;
    const unsigned char* p2 = s2;
    while(n--) {
        if (*p1 != *p2) return *p1 - *p2;
        p1++; p2++;
    }
    return 0;
}

// Strings
size_t strlen(const char* s) {
    size_t len = 0;
    while(s[len]) len++;
    return len;
}

char *strchr(const char *s, int c) {
    while (*s != (char)c) {
        if (!*s++) return 0;
    }
    return (char *)s;
}

char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while((*d++ = *src++));
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) dest[i] = src[i];
    for ( ; i < n; i++) dest[i] = '\0';
    return dest;
}

int strcmp(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    if (n == 0) return 0;
    while(n-- > 0 && *s1 && (*s1 == *s2)) {
        if (n == 0) return 0;
        s1++; s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char* strrchr(const char* s, int c) {
    const char* last = 0;
    do {
        if (*s == (char)c) last = s;
    } while(*s++);
    return (char*)last;
}

char* strstr(const char* haystack, const char* needle) {
    size_t n = strlen(needle);
    while(*haystack) {
        if (!memcmp(haystack, needle, n)) return (char*)haystack;
        haystack++;
    }
    return 0;
}

int strcasecmp(const char* s1, const char* s2) {
    while(*s1 && (tolower((unsigned char)*s1) == tolower((unsigned char)*s2))) { s1++; s2++; }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

int strncasecmp(const char* s1, const char* s2, size_t n) {
    if (n == 0) return 0;
    while(n-- > 0 && *s1 && (tolower((unsigned char)*s1) == tolower((unsigned char)*s2))) {
        if (n == 0) return 0;
        s1++; s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

char* strdup(const char* s) {
    size_t len = strlen(s);
    char* d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

// Ctype
int toupper(int c) { return (c >= 'a' && c <= 'z') ? c - 32 : c; }
int tolower(int c) { return (c >= 'A' && c <= 'Z') ? c + 32 : c; }
int isspace(int c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f'; }
int isdigit(int c) { return c >= '0' && c <= '9'; }
int isalpha(int c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
int isalnum(int c) { return isalpha(c) || isdigit(c); }

int atoi(const char* str) {
    int res = 0;
    int sign = 1;
    while(isspace(*str)) str++;
    if (*str == '-') { sign = -1; str++; }
    else if (*str == '+') str++;
    while(isdigit(*str)) {
        res = res * 10 + (*str - '0');
        str++;
    }
    return res * sign;
}

static void print_itoa(char** buf_ptr, size_t* rem, int val, int precision) {
    if (*rem == 0) return;
    int is_neg = 0;
    if (val < 0) { 
        is_neg = 1; 
        val = -val; 
    }
    
    char tmp[32];
    int i = 0;
    if (val == 0) {
        tmp[i++] = '0';
    } else {
        while (val) { tmp[i++] = (val % 10) + '0'; val /= 10; }
    }
    
    while (i < precision && i < 31) {
        tmp[i++] = '0';
    }
    
    if (is_neg && i < 31) {
        tmp[i++] = '-';
    }
    
    while (i > 0 && *rem > 1) {
        **buf_ptr = tmp[--i];
        (*buf_ptr)++;
        (*rem)--;
    }
}

static void print_xtoa(char** buf_ptr, size_t* rem, uint32_t val, int is_upper) {
    if (*rem == 0) return;
    if (val == 0) { 
        if (*rem > 1) { **buf_ptr = '0'; (*buf_ptr)++; (*rem)--; }
        return; 
    }
    char tmp[16];
    int i = 0;
    const char* digits = is_upper ? "0123456789ABCDEF" : "0123456789abcdef";
    while (val) { tmp[i++] = digits[val % 16]; val /= 16; }
    while (i > 0 && *rem > 1) {
        **buf_ptr = tmp[--i];
        (*buf_ptr)++;
        (*rem)--;
    }
}

int vsnprintf(char* str, size_t size, const char* format, va_list ap) {
    if (size == 0) return 0;
    char* ptr = str;
    size_t rem = size;

    while (*format && rem > 1) {
        if (*format == '%') {
            format++;
            int pad = 0;
            char pad_char = ' ';
            (void)pad_char;
            if (*format == '0') { pad_char = '0'; format++; }
            while (*format >= '0' && *format <= '9') {
                pad = pad * 10 + (*format - '0');
                format++;
            }
            int precision = -1;
            if (*format == '.') {
                format++;
                precision = 0;
                while (*format >= '0' && *format <= '9') {
                    precision = precision * 10 + (*format - '0');
                    format++;
                }
            }
            if (*format == 'l' || *format == 'L') format++; // skip long modifier
            
            if (*format == 'd' || *format == 'i') {
                int val = va_arg(ap, int);
                print_itoa(&ptr, &rem, val, precision);
            } else if (*format == 'u') {
                uint32_t val = va_arg(ap, uint32_t);
                print_itoa(&ptr, &rem, (int)val, precision); 
            } else if (*format == 'x' || *format == 'X') {
                uint32_t val = va_arg(ap, uint32_t);
                print_xtoa(&ptr, &rem, val, *format == 'X');
            } else if (*format == 's') {
                char* s = va_arg(ap, char*);
                if (!s) s = "(null)";
                while (*s && rem > 1) { *ptr++ = *s++; rem--; }
            } else if (*format == 'c') {
                char c = (char)va_arg(ap, int);
                if (rem > 1) { *ptr++ = c; rem--; }
            } else {
                if (rem > 1) { *ptr++ = '%'; rem--; }
                if (rem > 1) { *ptr++ = *format; rem--; }
            }
        } else {
            if (rem > 1) { *ptr++ = *format; rem--; }
        }
        format++;
    }
    *ptr = '\0';
    return (ptr - str);
}

int snprintf(char* str, size_t size, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    int res = vsnprintf(str, size, format, ap);
    va_end(ap);
    return res;
}

int sprintf(char* str, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    int res = vsnprintf(str, 4096, format, ap); // unsafe fallback
    va_end(ap);
    return res;
}

int vprintf(const char* format, va_list ap) {
    char buf[1024];
    int res = vsnprintf(buf, sizeof(buf), format, ap);
    puts(buf);
    return res;
}

int printf(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    int res = vprintf(format, ap);
    va_end(ap);
    return res;
}

int fprintf(FILE* stream, const char* format, ...) {
    (void)stream;
    va_list ap;
    va_start(ap, format);
    int res = vprintf(format, ap);
    va_end(ap);
    return res;
}

int vfprintf(FILE *stream, const char *format, va_list ap) {
    (void)stream;
    return vprintf(format, ap);
}

int puts(const char* s); // Provided by kernel.c

char* getenv(const char* name) {
    (void)name;
    return 0;
}

void exit(int status) {
    (void)status;
    puts("\n[KERNEL PANIC] exit() called!\n");
    while(1) { asm("hlt"); }
}

// File IO
FILE* fopen(const char* filename, const char* mode) {
    (void)mode;
    // Only support reading DOOM1.WAD
    if (strstr(filename, "WAD") || strstr(filename, "wad")) {
        FILE* f = malloc(sizeof(FILE));
        f->base = (void*)doom_wad_addr;
        f->size = doom_wad_size;
        f->pos = 0;
        return f;
    }
    return 0; // Not found
}

int remove(const char *filename) {
    (void)filename;
    return -1;
}

int rename(const char *old_filename, const char *new_filename) {
    (void)old_filename;
    (void)new_filename;
    return -1;
}

int access(const char *pathname, int mode) {
    (void)pathname;
    (void)mode;
    return -1;
}

int abs(int j) {
    return j < 0 ? -j : j;
}

double fabs(double x) {
    return x < 0.0 ? -x : x;
}

long long __divdi3(long long num, long long den) {
    int minus = 0;
    long long v;

    if (num < 0) {
        num = -num;
        minus = 1;
    }
    if (den < 0) {
        den = -den;
        minus ^= 1;
    }

    unsigned long long n = (unsigned long long)num;
    unsigned long long d = (unsigned long long)den;
    unsigned long long q = 0;
    unsigned long long r = 0;
    
    for (int i = 63; i >= 0; i--) {
        r = (r << 1) | ((n >> i) & 1);
        if (r >= d) {
            r -= d;
            q |= (1ULL << i);
        }
    }
    
    v = (long long)q;
    if (minus) {
        v = -v;
    }
    return v;
}

int system(const char *command) {
    (void)command;
    return -1;
}

double atof(const char *str) {
    (void)str;
    return 0.0;
}

int sscanf(const char *str, const char *format, ...) {
    (void)str;
    (void)format;
    return 0;
}

int stat(const char *pathname, void *statbuf) {
    (void)pathname; (void)statbuf;
    return -1;
}
int mkdir(const char *pathname, int mode) {
    (void)pathname; (void)mode;
    return -1;
}
int open(const char *pathname, int flags, ...) {
    (void)pathname; (void)flags;
    return -1;
}
int close(int fd) {
    (void)fd;
    return -1;
}
int read(int fd, void *buf, size_t count) {
    (void)fd; (void)buf; (void)count;
    return -1;
}
int write(int fd, const void *buf, size_t count) {
    (void)fd; (void)buf; (void)count;
    return -1;
}
int lseek(int fd, int offset, int whence) {
    (void)fd; (void)offset; (void)whence;
    return -1;
}

int fclose(FILE* stream) {
    if (stream && stream != stdout && stream != stderr) {
        free(stream);
    }
    return 0;
}

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    if (!stream) return 0;
    size_t total = size * nmemb;
    if (stream->pos + total > stream->size) {
        total = stream->size - stream->pos;
    }
    if (total == 0) return 0;
    
    memcpy(ptr, (uint8_t*)stream->base + stream->pos, total);
    stream->pos += total;
    return total / size;
}

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream) {
    (void)ptr; (void)size; (void)stream;
    return nmemb; // Fake write
}

int fseek(FILE* stream, long int offset, int whence) {
    if (!stream) return -1;
    if (whence == SEEK_SET) {
        stream->pos = offset;
    } else if (whence == SEEK_CUR) {
        stream->pos += offset;
    } else if (whence == SEEK_END) {
        stream->pos = stream->size + offset;
    }
    if (stream->pos > stream->size) stream->pos = stream->size;
    return 0;
}

long int ftell(FILE* stream) {
    if (!stream) return 0;
    return stream->pos;
}

int feof(FILE* stream) {
    if (!stream) return 1;
    return stream->pos >= stream->size;
}
