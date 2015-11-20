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

static int m3u_download(hls_m3u_t *m3u, const char*uri)
{
    void *curl_ctx = m3u->curl;
    int ret = dtcurl_init(&curl_ctx, uri);
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
    memset(pls, 0, sizeof(struct playlist));
    dt_make_absolute_url(pls->uri, sizeof(pls->uri), base, uri);
    dt_array_add(&m3u->playlists, &m3u->n_playlists, pls);
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
    memset(var, 0, sizeof(struct variant));
    if (info) {
        var->bandwidth = atoi(info->bandwidth);
        strcpy(var->audio_group, info->audio);
        strcpy(var->video_group, info->video);
        strcpy(var->subtitles_group, info->subtitles);
    }
    dt_array_add(&m3u->variants, &m3u->n_variants, var);
    dt_array_add(&var->playlists, &var->n_playlists, pls);
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

struct rendition_info {
    char type[16];
    char uri[MAX_URL_SIZE];
    char group_id[MAX_FIELD_LEN];
    char language[MAX_FIELD_LEN];
    char assoc_language[MAX_FIELD_LEN];
    char name[MAX_FIELD_LEN];
    char defaultr[4];
    char forced[4];
    char characteristics[MAX_CHARACTERISTICS_LEN];
};

static struct rendition *new_rendition(hls_m3u_t *m3u, struct rendition_info *info,
                                       const char *url_base)
{
    struct rendition *rend;
    enum DTMediaType type = DTMEDIA_TYPE_UNKNOWN;
    char *characteristic;
    char *chr_ptr;
    char *saveptr;

    if (!strcmp(info->type, "AUDIO")) {
        type = DTMEDIA_TYPE_AUDIO;
    } else if (!strcmp(info->type, "VIDEO")) {
        type = DTMEDIA_TYPE_VIDEO;
    } else if (!strcmp(info->type, "SUBTITLES")) {
        type = DTMEDIA_TYPE_SUBTITLE;
    } else if (!strcmp(info->type, "CLOSED-CAPTIONS"))
        /* CLOSED-CAPTIONS is ignored since we do not support CEA-608 CC in
         * AVC SEI RBSP anyway */
    {
        return NULL;
    }

    if (type == DTMEDIA_TYPE_UNKNOWN) {
        return NULL;
    }

    /* URI is mandatory for subtitles as per spec */
    if (type == DTMEDIA_TYPE_SUBTITLE && !info->uri[0]) {
        return NULL;
    }
#if 0
    /* TODO: handle subtitles (each segment has to parsed separately) */
    if (c->strict_std_compliance > FF_COMPLIANCE_EXPERIMENTAL)
        if (type == DTMEDIA_TYPE_SUBTITLE) {
            return NULL;
        }
#endif
    rend = malloc(sizeof(struct rendition));
    if (!rend) {
        return NULL;
    }

    dt_array_add(&m3u->renditions, &m3u->n_renditions, rend);

    rend->type = type;
    strcpy(rend->group_id, info->group_id);
    strcpy(rend->language, info->language);
    strcpy(rend->name, info->name);

    /* add the playlist if this is an external rendition */
    if (info->uri[0]) {
        rend->playlist = new_playlist(m3u, info->uri, url_base);
        if (rend->playlist) {
            dt_array_add(&rend->playlist->renditions, &rend->playlist->n_renditions, rend);
        }
    }

    if (info->assoc_language[0]) {
        int langlen = strlen(rend->language);
        if (langlen < sizeof(rend->language) - 3) {
            rend->language[langlen] = ',';
            strncpy(rend->language + langlen + 1, info->assoc_language,
                    sizeof(rend->language) - langlen - 2);
        }
    }
#if 0
    if (!strcmp(info->defaultr, "YES")) {
        rend->disposition |= AV_DISPOSITION_DEFAULT;
    }
    if (!strcmp(info->forced, "YES")) {
        rend->disposition |= AV_DISPOSITION_FORCED;
    }

    chr_ptr = info->characteristics;
    while ((characteristic = av_strtok(chr_ptr, ",", &saveptr))) {
        if (!strcmp(characteristic, "public.accessibility.describes-music-and-sound")) {
            rend->disposition |= AV_DISPOSITION_HEARING_IMPAIRED;
        } else if (!strcmp(characteristic, "public.accessibility.describes-video")) {
            rend->disposition |= AV_DISPOSITION_VISUAL_IMPAIRED;
        }

        chr_ptr = NULL;
    }
#endif

    return rend;
}

static void handle_rendition_args(struct rendition_info *info, const char *key,
                                  int key_len, char **dest, int *dest_len)
{
    if (!strncmp(key, "TYPE=", key_len)) {
        *dest     =        info->type;
        *dest_len = sizeof(info->type);
    } else if (!strncmp(key, "URI=", key_len)) {
        *dest     =        info->uri;
        *dest_len = sizeof(info->uri);
    } else if (!strncmp(key, "GROUP-ID=", key_len)) {
        *dest     =        info->group_id;
        *dest_len = sizeof(info->group_id);
    } else if (!strncmp(key, "LANGUAGE=", key_len)) {
        *dest     =        info->language;
        *dest_len = sizeof(info->language);
    } else if (!strncmp(key, "ASSOC-LANGUAGE=", key_len)) {
        *dest     =        info->assoc_language;
        *dest_len = sizeof(info->assoc_language);
    } else if (!strncmp(key, "NAME=", key_len)) {
        *dest     =        info->name;
        *dest_len = sizeof(info->name);
    } else if (!strncmp(key, "DEFAULT=", key_len)) {
        *dest     =        info->defaultr;
        *dest_len = sizeof(info->defaultr);
    } else if (!strncmp(key, "FORCED=", key_len)) {
        *dest     =        info->forced;
        *dest_len = sizeof(info->forced);
    } else if (!strncmp(key, "CHARACTERISTICS=", key_len)) {
        *dest     =        info->characteristics;
        *dest_len = sizeof(info->characteristics);
    }
    /*
     * ignored:
     * - AUTOSELECT: client may autoselect based on e.g. system language
     * - INSTREAM-ID: EIA-608 closed caption number ("CC1".."CC4")
     */
}

static int m3u_parse_rendition(char *line, struct rendition_info *info)
{
    const char *match = strstr(line, ":");
    if (match == NULL) {
        return -1;
    }
    dt_trimspace(line);
    int cur_pos = match - line;
    memset(info, 0, sizeof(struct rendition_info));
    dt_parse_key_value(line + cur_pos + 1, (dt_parse_key_val_cb)handle_rendition_args, info);
}

/* used by parse_playlist to allocate a new variant+playlist when the
 * playlist is detected to be a Media Playlist (not Master Playlist)
 * and we have no parent Master Playlist (parsing of which would have
 * allocated the variant and playlist already)
 * *pls == NULL  => Master Playlist or parentless Media Playlist
 * *pls != NULL => parented Media Playlist, playlist+variant allocated */
static int ensure_playlist(hls_m3u_t *m3u, struct playlist **pls, const char *url)
{
    if (*pls) {
        return 0;
    }
    if (!new_variant(m3u, NULL, url, NULL)) {
        return DTHLS_ERROR_UNKOWN;
    }
    *pls = m3u->playlists[m3u->n_playlists - 1];
    return 0;
}

static void debug_dump_playlist(struct playlist *pls)
{
    int i = 0;
    dt_info(TAG, "uri:%s- segments num:%d \n", pls->uri, pls->n_segments);
    for (i = 0; i < pls->n_segments; i++) {
        struct segment *seg = pls->segments[i];
        if (seg) {
            dt_info(TAG, "index:%d uri:%s \n", i, seg->url);
        }
    }
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
static int m3u_parse(hls_m3u_t *m3u, const char*url, struct playlist *pls)
{
    dt_info(TAG, "Enter parse m3u8 \n");

    int ret = 0, is_segment = 0, is_variant = 0;
    int64_t duration = 0;
    enum KeyType key_type = KEY_NONE;
    uint8_t iv[16] = "";
    int has_iv = 0;
    char key[MAX_URL_SIZE] = "";
    char line[MAX_URL_SIZE];
    const char *ptr;
    int close_in = 0;
    int64_t seg_offset = 0;
    int64_t seg_size = -1;
    uint8_t *new_url = NULL;
    struct variant_info variant_info;
    char tmp_str[MAX_URL_SIZE];
    struct segment *cur_init_section = NULL;

    char *in = m3u->content;
    int insize = (int)m3u->filesize;
    int off = 0;


    int len = read_line(in, insize, line, sizeof(line));
    if (len < 0) {
        dt_info(TAG, "Error invalid header \n");
        return DTHLS_ERROR_UNKOWN;
    }
    if (strcmp(line, "#EXTM3U")) {
        return DTHLS_ERROR_UNKOWN;
    }
    off += len;

    if (pls) {
        ;
    }

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
            continue;// comming soon
        } else if (dt_strstart(line, "#EXT-X-MEDIA:", &ptr)) {
            struct rendition_info info = {{0}};
            m3u_parse_rendition(line, &info);
            new_rendition(m3u, &info, url);
        } else if (dt_strstart(line, "#EXT-X-TARGETDURATION:", &ptr)) {
            ret = ensure_playlist(m3u, &pls, url);
            if (ret < 0) {
                goto fail;
            }
            pls->target_duration = atoi(ptr) * DTHLS_TIME_BASE;
            dt_info(TAG, "Get target duration:%lld s \n", pls->target_duration / DTHLS_TIME_BASE);
        } else if (dt_strstart(line, "#EXT-X-MEDIA-SEQUENCE:", &ptr)) {
            ret = ensure_playlist(m3u, &pls, url);
            if (ret < 0) {
                goto fail;
            }
            pls->start_seq_no = atoi(ptr);
            dt_info(TAG, "Get mediasequence:%lld\n", pls->start_seq_no);
        } else if (dt_strstart(line, "#EXT-X-PLAYLIST-TYPE:", &ptr)) {
            ret = ensure_playlist(m3u, &pls, url);
            if (ret < 0) {
                goto fail;
            }
            if (!strcmp(ptr, "EVENT")) {
                pls->type = PLS_TYPE_EVENT;
            } else if (!strcmp(ptr, "VOD")) {
                pls->type = PLS_TYPE_VOD;
            }
            dt_info(TAG, "Get playlist type:%lld\n", pls->type);
        } else if (dt_strstart(line, "#EXT-X-MAP:", &ptr)) {
            continue;
        } else if (dt_strstart(line, "#EXT-X-ENDLIST:", &ptr)) {
            if (pls) {
                pls->finished = 1;
            }
            dt_info(TAG, "Get ENDLIST TAG\n");
        } else if (dt_strstart(line, "#EXTINF:", &ptr)) {
            is_segment = 1;
            duration = atof(ptr) * DTHLS_TIME_BASE;
        } else if (dt_strstart(line, "#EXT-X-BYTERANGE:", &ptr)) {
            seg_size = atoi(ptr);
            ptr = strchr(ptr, '@');
            if (ptr) {
                seg_offset = atoi(ptr + 1);
            }
        } else if (dt_strstart(line, "#", &ptr)) {
            continue;
        } else if (line[0]) { // content
            if (is_variant) {
                if (!new_variant(m3u, &variant_info, line, m3u->uri)) {
                    ret = DTHLS_ERROR_UNKOWN;
                    goto fail;
                }
                is_variant = 0;
            }

            if (is_segment) {
                struct segment *seg;
                if (!pls) {
                    if (!new_playlist(m3u, url, NULL)) {
                        ret = DTHLS_ERROR_UNKOWN;
                        goto fail;
                    }
                    pls = m3u->playlists[m3u->n_playlists - 1];
                }
                seg = malloc(sizeof(struct segment));
                if (!seg) {
                    ret = DTHLS_ERROR_UNKOWN;
                    goto fail;
                }
                seg->duration = duration;
                seg->key_type = key_type;
                if (has_iv) {
                    memcpy(seg->iv, iv, sizeof(iv));
                } else {
                    int seq = pls->start_seq_no + pls->n_segments;
                    memset(seg->iv, 0, sizeof(seg->iv));
                    ;//-fixme AV_WB32(seg->iv + 12, seq);
                }

                if (key_type != KEY_NONE) {
                    dt_make_absolute_url(tmp_str, sizeof(tmp_str), url, key);
                    seg->key = strdup(tmp_str);
                    if (!seg->key) {
                        free(seg);
                        ret = DTHLS_ERROR_UNKOWN;
                        goto fail;
                    }
                } else {
                    seg->key = NULL;
                }

                dt_make_absolute_url(tmp_str, sizeof(tmp_str), url, line);
                seg->url = strdup(tmp_str);
                if (!seg->url) {
                    free(seg->key);
                    free(seg);
                    ret = DTHLS_ERROR_UNKOWN;
                    goto fail;
                }
                dt_array_add(&pls->segments, &pls->n_segments, seg);
                is_segment = 0;
                seg->size = seg_size;
                if (seg_size >= 0) {
                    seg->url_offset = seg_offset;
                    seg_offset += seg_size;
                    seg_size = -1;
                } else {
                    seg->url_offset = 0;
                    seg_offset = 0;
                }
                seg->init_section = cur_init_section;

            }

        }
    }

    if (pls) {
        pls->last_load_time = dt_gettime();
    }
fail:
    return ret;
}

static int m3u_update(hls_m3u_t *m3u)
{
    int i;
    int ret;
    m3u_download(m3u, m3u->uri);
    m3u_parse(m3u, m3u->uri, NULL);

    // check variants case
    if (m3u->n_playlists > 1) {
        for (i = 0; i < m3u->n_playlists; i++) {
            struct playlist *pls = m3u->playlists[i];
            m3u_download(m3u, pls->uri);
            if (ret = m3u_parse(m3u, pls->uri, pls) < 0) {
                goto fail;
            }
        }
    }

    // debug
    dt_info(TAG, "variant:%d playlists:%d \n", m3u->n_variants, m3u->n_playlists);
    for (i = 0; i < m3u->n_playlists; i++) {
        struct playlist *pls = m3u->playlists[i];
        debug_dump_playlist(pls);
    }
fail:
    return 0;
}

int dtm3u_open(hls_m3u_t *m3u)
{
    int ret = DTHLS_ERROR_NONE;
    int i = 0;
    ret = m3u_update(m3u);
    if (ret < 0) {
        return ret;
    }

    if (m3u->variants[0]->playlists[0]->n_segments == 0) {
        dt_error(TAG, "Empty playlist. \n");
        return DTHLS_ERROR_UNKOWN;
    }

    if (m3u->variants[0]->playlists[0]->finished) {
        int64_t duration = 0;
        for (i = 0; i < m3u->variants[0]->playlists[0]->n_segments; i++) {
            duration += m3u->variants[0]->playlists[0]->segments[i]->duration;
        }
        m3u->duration = duration;
        dt_info(TAG, "Get duration %"PRId64"\n", duration);
    }

    return ret;
}

int dtm3u_close(hls_m3u_t *m3u)
{
    return 0;
}
