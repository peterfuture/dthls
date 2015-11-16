/*
 * =====================================================================================
 *
 *    Filename   :  dthls_m3u.h
 *    Description:
 *    Version    :  1.0
 *    Created    :  2015年11月13日 16时59分17秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s (), peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#ifndef DTHLS_M3U_H
#define DTHLS_M3U_H

#include "dt_queue.h"

#define INITIAL_BUFFER_SIZE 32768
#define MAX_FIELD_LEN 64
#define MAX_CHARACTERISTICS_LEN 512
#define MPEG_TIME_BASE 90000
#define MAX_URL_SIZE 4096

/*
 * Each playlist has its own demuxer. If it currently is active,
 * it has an open AVIOContext too, and potentially an AVPacket
 * containing the next packet from this stream.
 */
struct playlist {
    char uri[MAX_URL_SIZE];

    int64_t target_duration;
};

struct variant {
    int bandwidth;

    /* every variant contains at least the main Media Playlist in index 0 */
    int n_playlists;
    dt_queue_t *queue_playlists;
    char audio_group[MAX_FIELD_LEN];
    char video_group[MAX_FIELD_LEN];
    char subtitles_group[MAX_FIELD_LEN];
};

typedef struct hls_m3u {
    char *uri;
    char *location;
    int64_t filesize;
    char *content;

    int n_variants;
    dt_queue_t *queue_variants;
    int n_playlists;
    dt_queue_t *queue_playlists;
    void *curl;
} hls_m3u_t;


int dtm3u_open(hls_m3u_t *m3u);

#endif
