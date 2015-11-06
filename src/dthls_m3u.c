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

#define INITIAL_BUFFER_SIZE 32768
#define MAX_FIELD_LEN 64
#define MAX_CHARACTERISTICS_LEN 512
#define MPEG_TIME_BASE 90000
struct variant_info {
    char bandwidth[20];
    /*  variant group ids: */
    char audio[MAX_FIELD_LEN];
    char video[MAX_FIELD_LEN];
    char subtitles[MAX_FIELD_LEN];
};

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
    dtcurl_get_parameter(curl_ctx, KEY_CURL_GET_LOCATION, &m3u->location);
    dt_info(TAG, "get location:%s \n", (!m3u->location) ? m3u->uri : m3u->location);
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

static int read_line(char *data, char *buf, int maxlen)
{
    int off = 0;
    while (off < maxlen && data[off] != '\n' && data[off] != '\0') {
        off++;
    }
    if (off >= maxlen) {
        return -1;
    }
    memcpy(buf, data, off);
    dt_info(TAG, "read line:%s\n", buf);
    return off;
}

#define LINE_MAX_LENGTH 4096
static int m3u_parse(hls_m3u_t *m3u)
{
    dt_info(TAG, "Enter parse m3u8 \n");
    char line[LINE_MAX_LENGTH];
    char *in = m3u->content;
    int insize = (int)m3u->filesize;
    int off = 0;
    int len = read_line(in + off, line, sizeof(line));
    if (len < 0) {
        dt_info(TAG, "Error invalid header \n");
        return DTHLS_ERROR_UNKOWN;
    }
    if (strcmp(line, "#EXTM3U")) {
        return DTHLS_ERROR_UNKOWN;
    }
    off += len;
    while (1) {
        memset(line, 0, sizeof(line));
        len = read_line(in + off, line, sizeof(line));
        if (len < 0) {
            break;
        }
        if (len == 0) {
            off += 1;
            continue;
        }
        off += len;
        if (off >= insize) {
            break;
        }
    }

    return 0;
}

int dtm3u_get(hls_m3u_t *m3u)
{
    m3u_download(m3u);
    m3u_parse(m3u);
    return 0;
}
