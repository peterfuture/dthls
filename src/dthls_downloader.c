/*
 * =====================================================================================
 *
 *    Filename   :  dthls_downloader.c
 *    Description:
 *    Version    :  1.0
 *    Created    :  2015年12月21日 16时28分06秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s
 *    Email      :  peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "dtcurl/dtcurl_api.h"
#include "dt_log.h"
#include "dt_utils.h"
#include "dt_macro.h"
#include "dthls_error.h"
#include "dthls_priv.h"
#include "dthls_macro.h"

#include "dthls_downloader.h"

#define TAG "HLS-DOWNLOADER"

int m3u_downloader_get_filesize(void *handle, int64_t *filesize)
{
    if (!handle) {
        return DTHLS_ERROR_UNKOWN;
    }
    dtcurl_get_parameter(handle, KEY_CURL_GET_FILESIZE, filesize);
    return DTHLS_ERROR_NONE;
}

int m3u_downloader_get_location(void *handle, char *location)
{
    if (!handle) {
        return DTHLS_ERROR_UNKOWN;
    }
    dtcurl_get_parameter(handle, KEY_CURL_GET_LOCATION, location);
    return DTHLS_ERROR_NONE;
}

void *m3u_downloader_open(const char *url)
{
    void *curl_handle;
    int ret;
    int64_t filesize;
    if (strlen(url) <= 0) {
        return NULL;
    }
    ret = dtcurl_init(&curl_handle, url);
    if (ret < 0) {
        return NULL;
    }
    return curl_handle;
}

int m3u_downloader_read(void *handle, char *buf, int size, enum ReadFromURLMode mode)
{
    int wpos = 0;
    int rlen, len;
    int64_t filesize;
    int ret;

    m3u_downloader_get_filesize(handle, &filesize);
    if (mode == READ_COMPLETE) {
        len = (int)filesize;
    } else {
        len = size;
    }
    rlen = len;

    while (1) {
        ret = dtcurl_read(handle, buf + wpos, len);
        if (ret > 0) {
            dt_debug(TAG, "read %d bytes \n", ret);
            len -= ret;
            wpos += ret;
            if (len <= 0) {
                break;
            }
        }
        usleep(10000);
    }
    return (rlen - len);
}

int m3u_downloader_close(void *curl)
{
    return dtcurl_close(curl);
}


