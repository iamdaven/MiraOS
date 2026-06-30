#include "cxxrt.h"

void *memset(void *dest, int val, size_t len) {
    uint8_t *d = (uint8_t *)dest;
    for (size_t i = 0; i < len; i++)
        d[i] = (uint8_t)val;
    return dest;
}

void *memcpy(void *dest, const void *src, size_t len) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    for (size_t i = 0; i < len; i++)
        d[i] = s[i];
    return dest;
}

void *memmove(void *dest, const void *src, size_t len) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    if (d < s) {
        for (size_t i = 0; i < len; i++)
            d[i] = s[i];
    } else if (d > s) {
        for (size_t i = len; i > 0; i--)
            d[i - 1] = s[i - 1];
    }
    return dest;
}

int memcmp(const void *a, const void *b, size_t len) {
    const uint8_t *pa = (const uint8_t *)a;
    const uint8_t *pb = (const uint8_t *)b;
    for (size_t i = 0; i < len; i++) {
        if (pa[i] != pb[i])
            return pa[i] - pb[i];
    }
    return 0;
}

char *strchr(const char *s, int c) {
    while (*s) {
        if (*s == (char)c)
            return (char *)s;
        s++;
    }
    return 0;
}

char *strrchr(const char *s, int c) {
    const char *last = 0;
    while (*s) {
        if (*s == (char)c)
            last = s;
        s++;
    }
    return (char *)last;
}

size_t strcspn(const char *s, const char *reject) {
    size_t len = 0;
    while (*s) {
        const char *r = reject;
        while (*r) {
            if (*s == *r)
                return len;
            r++;
        }
        s++;
        len++;
    }
    return len;
}

size_t strspn(const char *s, const char *accept) {
    size_t len = 0;
    while (*s) {
        const char *a = accept;
        int found = 0;
        while (*a) {
            if (*s == *a) {
                found = 1;
                break;
            }
            a++;
        }
        if (!found)
            return len;
        s++;
        len++;
    }
    return len;
}

char *strpbrk(const char *s, const char *accept) {
    while (*s) {
        const char *a = accept;
        while (*a) {
            if (*s == *a)
                return (char *)s;
            a++;
        }
        s++;
    }
    return 0;
}

char *strstr(const char *haystack, const char *needle) {
    if (!*needle)
        return (char *)haystack;
    while (*haystack) {
        const char *h = haystack;
        const char *n = needle;
        while (*h && *n && *h == *n) {
            h++;
            n++;
        }
        if (!*n)
            return (char *)haystack;
        haystack++;
    }
    return 0;
}

long strtol(const char *s, char **endptr, int base) {
    (void)endptr;
    (void)base;
    long result = 0;
    int sign = 1;
    while (*s == ' ' || *s == '\t')
        s++;
    if (*s == '-') {
        sign = -1;
        s++;
    } else if (*s == '+') {
        s++;
    }
    while (*s >= '0' && *s <= '9') {
        result = result * 10 + (*s - '0');
        s++;
    }
    return result * sign;
}

unsigned long strtoul(const char *s, char **endptr, int base) {
    (void)endptr;
    (void)base;
    unsigned long result = 0;
    while (*s == ' ' || *s == '\t')
        s++;
    if (*s == '+')
        s++;
    while (*s >= '0' && *s <= '9') {
        result = result * 10 + (*s - '0');
        s++;
    }
    return result;
}

void __cxa_pure_virtual(void) {
}

int __cxa_atexit(void (*func)(void *), void *arg, void *dso_handle) {
    (void)func;
    (void)arg;
    (void)dso_handle;
    return 0;
}

void __cxa_finalize(void *dso_handle) {
    (void)dso_handle;
}