/*
 * =====================================================================================
 *
 *    Filename   :  hls_m3u.c
 *    Description:  download-parser m3u
 *    Version    :  1.0
 *    Created    :  2015年10月21日 11时11分14秒
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

#include "dthls_priv.h"
#include "dt_log.h"

#define TAG "M3U"

static int m3u_download(hls_m3u_t *m3u)
{
    void *curl_ctx = m3u->curl;
    int ret = dtcurl_init(&curl_ctx, m3u->uri);
    if (ret < 0) {
        return DTHLS_ERROR_UNKOWN;
    }
    while (1) {
        dtcurl_get_parameter(curl_ctx, KEY_CURL_GET_FILESIZE, &m3u->filesize);
        if (m3u->filesize > 0) {
            break;
        }
        usleep(10 * 1000);
    }
    dt_info(TAG, "get filesize:%" PRId64 "\n", m3u->filesize);

    m3u->content = (unsigned char *)malloc((int)m3u->filesize);
    if (!m3u->content) {
        dt_info(TAG, "malloc m3u buffer failed \n");
        return DTHLS_ERROR_UNKOWN;
    }
    int wpos = 0;
    int len = (int)m3u->filesize;
    while (1) {
        ret = dtcurl_read(curl_ctx, m3u->content + wpos, len);
        if (ret > 0) {
            dt_info(TAG, "read %d bytes \n", ret);
            len -= ret;
            wpos += ret;
            if (len <= 0) {
                break;
            }
        }
        usleep(10000);
    }
    return 0;
}

static int m3u_parse(hls_m3u_t *m3u)
{
    dt_info(TAG, "Enter parse m3u8 \n");
    return 0;
}

int dtm3u_get(hls_m3u_t *m3u)
{
    m3u_download(m3u);
    return 0;
}
