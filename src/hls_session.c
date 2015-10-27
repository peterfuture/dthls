/*
 * =====================================================================================
 *
 *    Filename   :  hls_session.c
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

#include "hls_priv.h"

int hls_session_open(hls_session_t *session, const char *uri)
{
    int ret = HLS_ERROR_NONE;
    memset(session, 0, sizeof(*session));
    session->uri = strdup(uri);
    HLS_LOG("hls url:%s \n", session->uri);
    ret = hls_buf_init(&session->cache, HLS_MAX_CACHE_SIZE);
    if (ret < 0) {
        HLS_LOG("hls cache init failed \n");
        return ret;
    }
    HLS_LOG("hls session open success \n");
    return HLS_ERROR_NONE;
}

int hls_session_close(hls_session_t *session)
{
    if (!session) {
        return HLS_ERROR_UNKOWN;
    }
    if (session->uri) {
        free(session->uri);
    }
    hls_buf_release(&session->cache);
    HLS_LOG("hls session closed \n");
    return HLS_ERROR_NONE;
}
