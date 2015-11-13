/*
 * =====================================================================================
 *
 *    Filename   :  dt_string.h
 *    Description:
 *    Version    :  1.0
 *    Created    :  2015年11月10日 15时53分47秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s (), peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#ifndef DT_STRING_H
#define DT_STRING_H

#include <string.h>

int dt_strstart(const char *str, const char *pfx, const char **ptr);
int dt_trimspace(char *str);
int dt_stristart(const char *str, const char *pfx, const char **ptr);
char *dt_stristr(const char *s1, const char *s2);
char *dt_strnstr(const char *haystack, const char *needle, size_t hay_length);
size_t dt_strlcpy(char *dst, const char *src, size_t size);
size_t dt_strlcat(char *dst, const char *src, size_t size);
size_t dt_strlcatf(char *dst, size_t size, const char *fmt, ...);

#endif
