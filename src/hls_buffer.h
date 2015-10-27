#ifndef HLS_BUFFER_T
#define HLS_BUFFER_T

#include "hls_lock.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    uint8_t *data;
    int size;
    int level;
    uint8_t *rd_ptr;
    uint8_t *wr_ptr;
    hls_lock_t mutex;
} hls_buffer_t;

int hls_buf_init(hls_buffer_t * dbt, int size);
int hls_buf_reinit(hls_buffer_t * dbt);
int hls_buf_release(hls_buffer_t * dbt);
int hls_buf_space(hls_buffer_t * dbt);
int hls_buf_level(hls_buffer_t * dbt);
int hls_buf_get(hls_buffer_t * dbt, uint8_t * out, int size);
int hls_buf_put(hls_buffer_t * dbt, uint8_t * in, int size);
#endif
