/*
 * =====================================================================================
 *
 *    Filename   :  dthls_api.h
 *    Description:
 *    Version    :  1.0
 *    Created    :  2015年10月27日 20时43分45秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s (), peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#include <stdint.h>
#include "dthls_priv.h"

int dthls_open(void **priv, const char *uri);
int dthls_read_packet(void *priv, hls_pkt_t *ppkt);
int dthls_seek(void *priv, int64_t pos, int whence);
int dthls_set_parameter(void *priv, int key, void *value);
int dthls_get_parameter(void *priv, int key, void *value);
int dthls_close(void *priv);
