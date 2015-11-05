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

#include "dthls_priv.h"

#define TAG "HLS-SESSION"

static int update_playlist(dthls_session_t *session)
{
    hls_m3u_t *pm3u = &session->m3u;
    pm3u->uri = session->uri;
    return dtm3u_get(pm3u);
}

int dthls_session_open(dthls_session_t *session, const char *uri)
{
    int ret = DTHLS_ERROR_NONE;
    memset(session, 0, sizeof(*session));
    session->uri = strdup(uri);
    dt_info(TAG, "hls url:%s \n", session->uri);
    ret = dtbuf_init(&session->cache, HLS_MAX_CACHE_SIZE);
    if (ret < 0) {
        dt_info(TAG, "hls cache init failed \n");
        return ret;
    }

    // parse m3u8
    ret = update_playlist(session);

    dt_info(TAG, "hls session open success \n");
    return ret;
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
