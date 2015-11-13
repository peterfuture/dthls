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
#include "dt_log.h"
#include "dt_utils.h"

#include "dthls_priv.h"
#include "dthls_m3u.h"

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

static struct playlist *new_playlist(hls_m3u_t *m3u, const char *uri, const char *base)
{
    struct playlist *pls = malloc(sizeof(struct playlist));
    if (!pls) {
        return NULL;
    }
    dt_make_absolute_url(pls->url, sizeof(pls->url), base, uri);
    dt_queue_push_tail(m3u->queue_playlist, (void *)pls);
    m3u->n_playlists++;
    return pls;
}

struct variant_info {
    char bandwidth[20];
    /* variant group ids: */
    char audio[MAX_FIELD_LEN];
    char video[MAX_FIELD_LEN];
    char subtitles[MAX_FIELD_LEN];
};

static struct variant *new_variant(hls_m3u_t *m3u, struct variant_info *info,
                                   const char *url, const char *base)
{
    struct variant *var;
    struct playlist *pls;

    pls = new_playlist(m3u, url, base);
    if (!pls) {
        return NULL;
    }

    var = malloc(sizeof(struct variant));
    if (!var) {
        return NULL;
    }

    if (info) {
        var->bandwidth = atoi(info->bandwidth);
        strcpy(var->audio_group, info->audio);
        strcpy(var->video_group, info->video);
        strcpy(var->subtitles_group, info->subtitles);
    }

    dt_queue_push_tail(m3u->queue_variants, var);
    m3u->n_variants++;
    dt_queue_push_tail(var->queue_playlist, pls);
    var->n_playlists++;
    return var;
}

static void handle_variant_args(struct variant_info *info, const char *key,
                                int key_len, char **dest, int *dest_len)
{
    if (!strncmp(key, "BANDWIDTH=", key_len)) {
        *dest     =        info->bandwidth;
        *dest_len = sizeof(info->bandwidth);
    } else if (!strncmp(key, "AUDIO=", key_len)) {
        *dest     =        info->audio;
        *dest_len = sizeof(info->audio);
    } else if (!strncmp(key, "VIDEO=", key_len)) {
        *dest     =        info->video;
        *dest_len = sizeof(info->video);
    } else if (!strncmp(key, "SUBTITLES=", key_len)) {
        *dest     =        info->subtitles;
        *dest_len = sizeof(info->subtitles);
    }
}

static int m3u_parse_stream(char *line, struct variant_info *info)
{
    const char *match = strstr(line, ":");
    if (match == NULL) {
        return -1;
    }
    dt_trimspace(line);
    int cur_pos = match - line;
    memset(info, 0, sizeof(struct variant_info));
    dt_parse_key_value(line + cur_pos + 1, (dt_parse_key_val_cb)handle_variant_args, info);
}

/*
 * para:
 * data:   store data to parse
 * inlen:  input buf length
 * buf:    store output line
 * maxlen: max analysize size one time
 *
 * */

static int read_line(char *data, int inlen, char *buf, int maxlen)
{
    int off = 0;
    while (off < inlen && off < maxlen && data[off] != '\n' && data[off] != '\0') {
        off++;
    }
    if (off >= maxlen || off >= inlen) {
        return -1;
    }
    memcpy(buf, data, off);
    if (strlen(buf) > 0) {
        dt_info(TAG, "read line:%s\n", buf);
    }
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
    int is_variant = 0;
    struct variant_info variant_info;


    char *ptr = NULL;
    int len = read_line(in, insize, line, sizeof(line));
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
        len = read_line(in + off, insize - off, line, sizeof(line));
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
        // parse line
        if (dt_strstart(line, "#EXT-X-STREAM-INF:", &ptr)) {
            memset(&variant_info, 0, sizeof(variant_info));
            m3u_parse_stream(line, &variant_info);
            is_variant = 1;
            dt_info(TAG, "Get varant stream, bandwidth:%s \n", variant_info.bandwidth);
        } else if (dt_strstart(line, "#EXT-X-KEY:", &ptr)) {
            ;
        } else if (line[0]) { // content
            if (is_variant) {
                new_variant(m3u, &variant_info, line, m3u->uri);
            }
        }
    }

    dt_info(TAG, "variant:%d playlists:%d \n", m3u->n_variants, m3u->n_playlists);

    return 0;
}

int dtm3u_open(hls_m3u_t *m3u)
{
    m3u->queue_playlist = dt_queue_new();
    m3u->queue_variants = dt_queue_new();
    m3u_download(m3u);
    m3u_parse(m3u);
    return 0;
}

int dtm3u_close(hls_m3u_t *m3u)
{
    return 0;
}
