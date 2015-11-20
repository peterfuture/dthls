/*
 * =====================================================================================
 *
 *    Filename   :  dt_mem.h
 *    Description:
 *    Version    :  1.0
 *    Created    :  2015年11月19日 15时44分56秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s (), peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#include "dt_error.h"

/**
 * Multiply two size_t values checking for overflow.
 * @return  0 if success, AVERROR(EINVAL) if overflow.
 */
static inline int dt_size_mult(size_t a, size_t b, size_t *r)
{
    size_t t = a * b;
    /* Hack inspired from glibc: only try the division if nelem and elsize
     * are both greater than sqrt(SIZE_MAX). */
    if ((a | b) >= ((size_t)1 << (sizeof(size_t) * 4)) && a && t / a != b) {
        return DTERROR(EINVAL);
    }
    *r = t;
    return 0;
}

void *dt_malloc(size_t size);
void *dt_realloc(void *ptr, size_t size);
void *dt_realloc_f(void *ptr, size_t nelem, size_t elsize);
int dt_reallocp(void *ptr, size_t size);
void *dt_realloc_array(void *ptr, size_t nmemb, size_t size);
int dt_reallocp_array(void *ptr, size_t nmemb, size_t size);
void dt_free(void *ptr);
void dt_freep(void *arg);
void *dt_mallocz(size_t size);
void *dt_calloc(size_t nmemb, size_t size);
char *dt_strdup(const char *s);
char *dt_strndup(const char *s, size_t len);
void *dt_memdup(const void *p, size_t size);
