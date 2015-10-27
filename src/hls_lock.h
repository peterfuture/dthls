#ifndef HLS_LOCK_H
#define HLS_LOCK_H

#include "pthread.h"

#define hls_lock_t         pthread_mutex_t
#define hls_lock_init(x,v) pthread_mutex_init(x,v)
#define hls_lock(x)        pthread_mutex_lock(x)
#define hls_unlock(x)      pthread_mutex_unlock(x)

#endif
