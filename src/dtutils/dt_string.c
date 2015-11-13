/*
 * =====================================================================================
 *
 *    Filename   :  dt_string.c
 *    Description:
 *    Version    :  1.0
 *    Created    :  2015年11月10日 15时52分25秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s
 *    Email      :  peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dt_string.h"

int dt_strstart(const char *str, const char *pfx, const char **ptr)
{
    while (*pfx && *pfx == *str) {
        pfx++;
        str++;
    }
    if (!*pfx && ptr) {
        *ptr = str;
    }
    return !*pfx;
}

int dt_stristart(const char *str, const char *pfx, const char **ptr)
{
    while (*pfx && toupper((unsigned)*pfx) == toupper((unsigned)*str)) {
        pfx++;
        str++;
    }
    if (!*pfx && ptr) {
        *ptr = str;
    }
    return !*pfx;
}

char *dt_stristr(const char *s1, const char *s2)
{
    if (!*s2) {
        return (char*)(intptr_t)s1;
    }

    do
        if (dt_stristart(s1, s2, NULL)) {
            return (char*)(intptr_t)s1;
        }
    while (*s1++);

    return NULL;
}

char *dt_strnstr(const char *haystack, const char *needle, size_t hay_length)
{
    size_t needle_len = strlen(needle);
    if (!needle_len) {
        return (char*)haystack;
    }
    while (hay_length >= needle_len) {
        hay_length--;
        if (!memcmp(haystack, needle, needle_len)) {
            return (char*)haystack;
        }
        haystack++;
    }
    return NULL;
}

size_t dt_strlcpy(char *dst, const char *src, size_t size)
{
    size_t len = 0;
    while (++len < size && *src) {
        *dst++ = *src++;
    }
    if (len <= size) {
        *dst = 0;
    }
    return len + strlen(src) - 1;
}

size_t dt_strlcat(char *dst, const char *src, size_t size)
{
    size_t len = strlen(dst);
    if (size <= len + 1) {
        return len + strlen(src);
    }
    return len + dt_strlcpy(dst + len, src, size - len);
}

size_t dt_strlcatf(char *dst, size_t size, const char *fmt, ...)
{
    size_t len = strlen(dst);
    va_list vl;

    va_start(vl, fmt);
    len += vsnprintf(dst + len, size > len ? size - len : 0, fmt, vl);
    va_end(vl);

    return len;
}

int dt_trimspace(char *str)
{
    if (!strcmp(str, "")) {
        return 0;
    }
    int i = 0;
    int size = strlen(str);
    while (i < size && isspace(str[i])) {
        ++i;
    }

    int j = size;
    while (j > i && isspace(str[j - 1])) {
        --j;
    }

    memmove(str, &str[i], j - i);
    size = j - i;
    str[size] = '\0';
    return 0;

}
