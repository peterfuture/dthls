/*
 * =====================================================================================
 *
 *    Filename   :  dthls_session.c
 *    Description:
 *    Version    :  1.0
 *    Created    :  2015年10月27日 20时49分56秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s
 *    Email      :  peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#include "dthls_session.h"

#define TAG "HLS-SESSION"

static int update_playlist(dthls_session_t *session)
{
    int ret = 0;
    hls_m3u_t *pm3u = &session->m3u;
    pm3u->uri = session->uri;
    return dtm3u_open(pm3u);
}

static void *hls_session_download_loop(void *arg)
{
    dthls_session_t *session = (dthls_session_t *)arg;
    dt_buffer_t *p_cache = &session->cache;
    hls_m3u_t *m3u = &session->m3u;
    hls_ctrl_t *p_ctrl = &session->ctrl;

    while (p_ctrl->request_quit) {
        // get one segment url
        // download segment
    }
}

int dthls_session_open(dthls_session_t *session, const char *uri)
{
    int ret = DTHLS_ERROR_NONE;

    // Init ffmpeg
#ifdef ENABLE_FFMPEG
    av_register_all();
    dt_info(TAG, "register all ffmpeg elements.\n");
#endif

    memset(session, 0, sizeof(*session));
    session->uri = strdup(uri);
    dt_info(TAG, "hls url:%s \n", session->uri);
    ret = dtbuf_init(&session->cache, HLS_MAX_CACHE_SIZE);
    if (ret < 0) {
        dt_info(TAG, "hls cache init failed \n");
        return ret;
    }

    // parse m3u8
    hls_m3u_t *pm3u = &session->m3u;
    pm3u->uri = session->uri;
    ret = dtm3u_open(pm3u);
    if (ret < 0) {
        dt_error(TAG, "playlist parse failed \n");
        return ret;
    }

    // start read thread
    //ret = pthread_create(&session->download_pid, NULL, hls_download_loop, (void *)session);
    //pthread_setname_np(session->download_pid, "hlsdownloop");
    dt_info(TAG, "hls session open success \n");
    return ret;
}

int dthls_session_read_packet(dthls_session_t *session, dt_av_pkt_t *pkt)
{
    return dtm3u_read_packet(&session->m3u, pkt);
}

int dthls_session_close(dthls_session_t *session)
{
    if (!session) {
        return DTHLS_ERROR_UNKOWN;
    }
    if (session->uri) {
        free(session->uri);
    }
    dtbuf_release(&session->cache);
    dt_info(TAG, "hls session closed \n");
    return DTHLS_ERROR_NONE;
}
