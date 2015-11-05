/*
 * =====================================================================================
 *
 *    Filename   :  dthls_priv.h
 *    Description:
 *    Version    :  1.0
 *    Created    :  2015年10月27日 11时17分38秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s (), peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#ifndef DTHLS_PRIV_H
#define DTHLS_PRIV_H

#include "dt_macro.h"
#include "dt_buffer.h"
#include "dt_lock.h"
#include "dt_log.h"

#include "dthls_error.h"
#include "dthls_macro.h"

typedef enum hls_state {
    HLS_MOD_UNINITED,
    HLS_MOD_INITED,
    HLS_MOD_RUNNING,
    HLS_MOD_STOPED
} hls_status_t;

typedef struct segment_item {
    char *uri;
} segment_item_t;

typedef struct segment_list {
    segment_item_t *item;
} segment_list_t;

typedef struct bandwidth_list {
    int cur_index;
    segment_list_t seg_list;
} bandwidth_list_t;

typedef struct hls_playlist {
    int is_variant;
    int cur_index;
    bandwidth_list_t bw_list;
} dthls_playlist_t;

typedef struct hls_ctrl {
    hls_status_t state;
} dthls_ctrl_t;

typedef struct hls_session {
    char *uri;
    int is_variant;
    int is_live;

    dt_buffer_t cache;

    dthls_playlist_t play_list;
    dthls_ctrl_t ctl;
} dthls_session_t;

#endif
