#ifndef DT_UTILS_H
#define DT_UTILS_H

#include <stddef.h>
#include <string.h>

//From FFMPEG
/**
 * Callback function type for ff_parse_key_value.
 *
 * @param key a pointer to the key
 * @param key_len the number of bytes that belong to the key, including the '='
 *                char
 * @param dest return the destination pointer for the value in *dest, may
 *             be null to ignore the value
 * @param dest_len the length of the *dest buffer
 */
typedef void (*dt_parse_key_val_cb)(void *context, const char *key,
                                    int key_len, char **dest, int *dest_len);


#endif
