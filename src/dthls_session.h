/*
 * =====================================================================================
 *
 *    Filename   :  dthls_session.h
 *    Description:
 *    Version    :  1.0
 *    Created    :  2015年11月27日 14时15分56秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s (), peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#ifndef DTHLS_SESSION_H
#define DTHLS_SESSION_H

#include "dt_macro.h"
#include "dt_buffer.h"
#include "dt_lock.h"
#include "dt_log.h"

#include "dthls_error.h"
#include "dthls_macro.h"

#include "dthls_m3u.h"

typedef enum hls_state {
    HLS_MOD_UNINITED,
    HLS_MOD_INITED,
    HLS_MOD_RUNNING,
    HLS_MOD_STOPED
} hls_status_t;

typedef struct hls_ctrl {
    int request_quit;
    hls_status_t state;
} hls_ctrl_t;

typedef struct hls_session {
    char *uri;
    int is_variant;
    int is_live;
    dt_buffer_t cache;
    hls_m3u_t m3u;
    hls_ctrl_t ctrl;

    pthread_t download_pid;
} dthls_session_t;

#endif
