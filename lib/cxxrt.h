#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memset(void *dest, int val, size_t len);
void *memcpy(void *dest, const void *src, size_t len);
void *memmove(void *dest, const void *src, size_t len);
int memcmp(const void *a, const void *b, size_t len);

char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
size_t strcspn(const char *s, const char *reject);
size_t strspn(const char *s, const char *accept);
char *strpbrk(const char *s, const char *accept);
char *strstr(const char *haystack, const char *needle);

long strtol(const char *s, char **endptr, int base);
unsigned long strtoul(const char *s, char **endptr, int base);

void __cxa_pure_virtual(void);
int __cxa_atexit(void (*func)(void *), void *arg, void *dso_handle);
void __cxa_finalize(void *dso_handle);

#ifdef __cplusplus
}
#endif