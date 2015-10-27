#ifndef HLS_MACRO_H
#define HLS_MACRO_H

/*************************************
** Common
*************************************/
#define TRUE            1
#define FALSE           0
#define MIN(x,y)       ((x)<(y)?(x):(y))
#define MAX(x,y)       ((x)>(y)?(x):(y))
#define HLS_MIN(x,y)       ((x)<(y)?(x):(y))
#define HLS_MAX(x,y)       ((x)>(y)?(x):(y))

#define MAX_URI_SIZE 4096
#define HLS_MAX_CACHE_SIZE 10*1024*1024


// Return Value
#define HLS_ERROR_UNKOWN -1
#define HLS_ERROR_NONE 0

#endif
