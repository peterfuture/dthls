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

#include "dtcurl/dtcurl_api.h"

#include "dthls_priv.h"

static int m3u_download(hls_m3u_t *m3u)
{
    void *curl_ctx;
    dtcurl_init(&curl_ctx, m3u->uri);
    char buf[1024];
    int ret = 0;
    while (1) {
        ret = dtcurl_read(curl_ctx, buf, 1024);
        if (ret > 0) {
            printf("read %d bytes \n", ret);
        }
        if (ret < 0) {
            break;
        }
        usleep(10000);
    }

    return 0;
}

static int m3u_parse()
{
    return 0;
}

int dtm3u_get(hls_m3u_t *m3u)
{
    m3u_download(m3u);
    return 0;
}
