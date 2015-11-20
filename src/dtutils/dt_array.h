/*
 * =====================================================================================
 *
 *    Filename   :  dt_array.h
 *    Description:
 *    Version    :  1.0
 *    Created    :  2015年11月19日 13时47分02秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s (), peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#ifndef DT_ARRAY_H
#define DT_ARRAY_H

#include "dt_error.h"
#include "dt_mem.h"

int dt_dynarray_add_nofree(void *tab_ptr, int *nb_ptr, void *elem);
void dt_dynarray_add(void *tab_ptr, int *nb_ptr, void *elem);
void *dt_dynarray2_add(void **tab_ptr, int *nb_ptr, size_t elem_size, const uint8_t *elem_data);

/**
 * Add an element of to a dynamic array.
 *
 * The array is reallocated when its number of elements reaches powers of 2.
 * Therefore, the amortized cost of adding an element is constant.
 *
 * In case of success, the pointer to the array is updated in order to
 * point to the new grown array, and the size is incremented.
 *
 * @param av_size_max  maximum size of the array, usually the MAX macro of
 *                     the type of the size
 * @param av_elt_size  size of the elements in the array, in bytes
 * @param av_array     pointer to the array, must be a lvalue
 * @param av_size      size of the array, must be an integer lvalue
 * @param av_success   statement to execute on success; at this point, the
 *                     size variable is not yet incremented
 * @param av_failure   statement to execute on failure; if this happens, the
 *                     array and size are not changed; the statement can end
 *                     with a return or a goto
 */
#define DT_DYNARRAY_ADD(av_size_max, av_elt_size, av_array, av_size, \
                        av_success, av_failure) \
    do { \
        size_t av_size_new = (av_size); \
        if (!((av_size) & ((av_size) - 1))) { \
            av_size_new = (av_size) ? (av_size) << 1 : 1; \
            if (av_size_new > (av_size_max) / (av_elt_size)) { \
                av_size_new = 0; \
            } else { \
                void *av_array_new = \
                    dt_realloc((av_array), av_size_new * (av_elt_size)); \
                if (!av_array_new) \
                    av_size_new = 0; \
                else \
                    (av_array) = av_array_new; \
            } \
        } \
        if (av_size_new) { \
            { av_success } \
            (av_size)++; \
        } else { \
            av_failure \
        } \
    } while (0)



#ifdef __GNUC__
#define dt_array_add(tab, nb_ptr, elem)\
do {\
    __typeof__(tab) _tab = (tab);\
    __typeof__(elem) _elem = (elem);\
    (void)sizeof(**_tab == _elem); /* check that types are compatible */\
    dt_dynarray_add(_tab, nb_ptr, _elem);\
} while(0)
#else
#define dt_array_add(tab, nb_ptr, elem)\
do {\
    dt_dynarray_add((tab), nb_ptr, (elem));\
} while(0)
#endif

#endif
