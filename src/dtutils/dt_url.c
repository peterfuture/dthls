/*
 * =====================================================================================
 *
 *    Filename   :  dt_url.c
 *    Description:
 *    Version    :  1.0
 *    Created    :  2015年11月13日 17时58分56秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s
 *    Email      :  peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#include "dt_url.h"

void dt_make_absolute_url(char *buf, int size, const char *base, const char *rel)
{
    char *sep, *path_query;
    /* Absolute path, relative to the current server */
    if (base && strstr(base, "://") && rel[0] == '/') {
        if (base != buf) {
            dt_strlcpy(buf, base, size);
        }
        sep = strstr(buf, "://");
        if (sep) {
            /* Take scheme from base url */
            if (rel[1] == '/') {
                sep[1] = '\0';
            } else {
                /* Take scheme and host from base url */
                sep += 3;
                sep = strchr(sep, '/');
                if (sep) {
                    *sep = '\0';
                }
            }
        }
        dt_strlcat(buf, rel, size);
        return;
    }
    /* If rel actually is an absolute url, just copy it */
    if (!base || strstr(rel, "://") || rel[0] == '/') {
        dt_strlcpy(buf, rel, size);
        return;
    }
    if (base != buf) {
        dt_strlcpy(buf, base, size);
    }

    /* Strip off any query string from base */
    path_query = strchr(buf, '?');
    if (path_query) {
        *path_query = '\0';
    }

    /* Is relative path just a new query part? */
    if (rel[0] == '?') {
        dt_strlcat(buf, rel, size);
        return;
    }

    /* Remove the file name from the base url */
    sep = strrchr(buf, '/');
    if (sep) {
        sep[1] = '\0';
    } else {
        buf[0] = '\0';
    }
    while (dt_strstart(rel, "../", NULL) && sep) {
        /* Remove the path delimiter at the end */
        sep[0] = '\0';
        sep = strrchr(buf, '/');
        /* If the next directory name to pop off is "..", break here */
        if (!strcmp(sep ? &sep[1] : buf, "..")) {
            /* Readd the slash we just removed */
            dt_strlcat(buf, "/", size);
            break;
        }
        /* Cut off the directory name */
        if (sep) {
            sep[1] = '\0';
        } else {
            buf[0] = '\0';
        }
        rel += 3;
    }
    dt_strlcat(buf, rel, size);
}
