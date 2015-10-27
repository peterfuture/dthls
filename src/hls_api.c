/*
 * =====================================================================================
 *
 *    Filename   :  hls_api.c
 *    Description:  hls process flow
 *    Version    :  1.0
 *    Created    :  2015年10月21日 11时13分18秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s
 *    Email      :  peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#include "hls_priv.h"

int hls_init(void **priv, const char *uri)
{
    int ret = HLS_ERROR_NONE;
    if (!uri || strlen(uri) < 2) {
        HLS_LOG("invalid url, quit \n");
        return -1;
    }
    hls_session_t *session = (hls_session_t*)malloc(sizeof(hls_session_t));
    ret = hls_session_open(session, uri);
    if (ret < 0) {
        return ret;
    }
    *priv = session;
    return 0;
}

int hls_stop(void *priv)
{
    hls_session_t *session = (hls_session_t *)priv;
    return hls_session_close(session);
}
