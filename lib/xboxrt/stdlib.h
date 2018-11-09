#ifndef XBOXRT_STDLIB
#define XBOXRT_STDLIB

#include <stdarg.h>
#include <stddef.h>

#define EXIT_SUCCESS 1 /*implementation defined*/		
#define EXIT_FAILURE 0 /*implementation defined*/

int vsnprintf(char *buf, unsigned int len, const char *fmt, va_list ap);
int vsprintf(char *buf, const char *fmt, va_list args);
int snprintf(char *str, unsigned int len, const char* fmt, ...);
int sprintf(char *str, const char *fmt, ...);

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t count, size_t size);
void *realloc(void *ptr, size_t size);

long strtol(const char *nptr, char **endptr, register int base);

int atoi(const char *nptr);

#include <stdbool.h>
#include <assert.h>

static void exit(int status) {
  assert(false);
}

void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));

void *bsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));

#endif
