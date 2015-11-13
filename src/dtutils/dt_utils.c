/*
 * =====================================================================================
 *
 *    Filename   :  dt_utils.c
 *    Description:
 *    Version    :  1.0
 *    Created    :  2015年11月12日 17时14分03秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s
 *    Email      :  peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#include "dt_utils.h"

void dt_parse_key_value(const char *str, dt_parse_key_val_cb callback_get_buf,
                        void *context)
{
    const char *ptr = str;

    /* Parse key=value pairs. */
    for (;;) {
        const char *key;
        char *dest = NULL, *dest_end;
        int key_len, dest_len = 0;

        /* Skip whitespace and potential commas. */
        while (*ptr && (isspace(*ptr) || *ptr == ',')) {
            ptr++;
        }
        if (!*ptr) {
            break;
        }

        key = ptr;

        if (!(ptr = strchr(key, '='))) {
            break;
        }
        ptr++;
        key_len = ptr - key;

        callback_get_buf(context, key, key_len, &dest, &dest_len);
        dest_end = dest + dest_len - 1;

        if (*ptr == '\"') {
            ptr++;
            while (*ptr && *ptr != '\"') {
                if (*ptr == '\\') {
                    if (!ptr[1]) {
                        break;
                    }
                    if (dest && dest < dest_end) {
                        *dest++ = ptr[1];
                    }
                    ptr += 2;
                } else {
                    if (dest && dest < dest_end) {
                        *dest++ = *ptr;
                    }
                    ptr++;
                }
            }
            if (*ptr == '\"') {
                ptr++;
            }
        } else {
            for (; *ptr && !(isspace(*ptr) || *ptr == ','); ptr++)
                if (dest && dest < dest_end) {
                    *dest++ = *ptr;
                }
        }
        if (dest) {
            *dest = 0;
        }
    }
}
