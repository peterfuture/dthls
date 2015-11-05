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

int dthls_init(void **priv, const char *uri);
int dthls_stop(void *priv);
