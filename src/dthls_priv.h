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

#include <stdint.h>

typedef struct hls_pkt {
    uint8_t *data;
    int size;
    int64_t pts;
    int64_t dts;
    int duration;
    int key_frame;
    int type;
} hls_pkt_t;

#endif
