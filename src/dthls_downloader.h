/*
 * =====================================================================================
 *
 *    Filename   :  dthls_downloader.h
 *    Description:
 *    Version    :  1.0
 *    Created    :  2015年12月21日 16时28分47秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s (), peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#include <stdint.h>

/* downloader */
enum ReadFromURLMode {
    READ_NORMAL,
    READ_COMPLETE,
};

int m3u_downloader_get_filesize(void *handle, int64_t *filesize);
int m3u_downloader_get_location(void *handle, char *location);
void *m3u_downloader_open(const char *url);
int m3u_downloader_read(void *handle, char *buf, int size, enum ReadFromURLMode mode);
int m3u_downloader_close(void *handle);
