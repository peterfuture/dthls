/*
 * =====================================================================================
 *
 *    Filename   :  dthls_api.c
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

#include "dthls_priv.h"
#include "dthls_session.h"

#define TAG "HLS-API"

int dthls_open(void **priv, const char *uri)
{
    int ret = DTHLS_ERROR_NONE;
    if (!uri || strlen(uri) < 2) {
        dt_info(TAG, "invalid url, quit \n");
        return -1;
    }
    dthls_session_t *session = (dthls_session_t*)malloc(sizeof(dthls_session_t));
    ret = dthls_session_open(session, uri);
    if (ret < 0) {
        return ret;
    }
    *priv = session;
    return 0;
}

int dthls_read_packet(void *priv, dt_av_pkt_t *pkt)
{
    dthls_session_t *session = (dthls_session_t *)priv;
    dthls_session_read_packet(session, pkt);
    return 0;
}

int dthls_seek(void *priv, int64_t pos, int whence)
{
    return 0;
}

int dthls_set_parameter(void *priv, int key, void *value)
{
    return 0;
}

int dthls_get_parameter(void *priv, int key, void *value)
{
    return 0;
}

int dthls_close(void *priv)
{
    dthls_session_t *session = (dthls_session_t *)priv;
    return dthls_session_close(session);
}
