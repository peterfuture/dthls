/*
 * =====================================================================================
 *
 *    Filename   :  hls_test.c
 *    Description:
 *    Version    :  1.0
 *    Created    :  2015年10月27日 20时42分00秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s
 *    Email      :  peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */
#include <stdio.h>
#include "dthls_api.h"

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("test url \n");
        return 0;
    }

    printf("===========================================\n");
    printf("Enter HLS Unit Test\n");
    //const char *uri = "http://devimages.apple.com/iphone/samples/bipbop/gear1/prog_index.m3u8";
    //const char *uri = "http://devimages.apple.com/iphone/samples/bipbop/bipbopall.m3u8";
    //const char *uri = "http://10.18.19.52:8000/HLS_V5_TEST/index.m3u8";
    const char *uri = argv[1];
    void *hls_ctx;
    dthls_open(&hls_ctx, uri);
    dt_av_pkt_t pkt;
    dthls_read_packet(hls_ctx, &pkt);
    dthls_close(hls_ctx);
    printf("Exit HLS Unit Test\n");
    printf("===========================================\n");
}
