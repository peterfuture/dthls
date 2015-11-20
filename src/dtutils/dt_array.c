/*
 * =====================================================================================
 *
 *    Filename   :  dt_array.c
 *    Description:
 *    Version    :  1.0
 *    Created    :  2015年11月19日 14时13分59秒
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s
 *    Email      :  peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "dt_array.h"

int dt_dynarray_add_nofree(void *tab_ptr, int *nb_ptr, void *elem)
{
    void **tab;
    memcpy(&tab, tab_ptr, sizeof(tab));

    DT_DYNARRAY_ADD(INT_MAX, sizeof(*tab), tab, *nb_ptr, {
        tab[*nb_ptr] = elem;
        memcpy(tab_ptr, &tab, sizeof(tab));
    }, {
        return DTERROR(ENOMEM);
    });
    return 0;
}

void dt_dynarray_add(void *tab_ptr, int *nb_ptr, void *elem)
{
    void **tab;
    memcpy(&tab, tab_ptr, sizeof(tab));

    DT_DYNARRAY_ADD(INT_MAX, sizeof(*tab), tab, *nb_ptr, {
        tab[*nb_ptr] = elem;
        memcpy(tab_ptr, &tab, sizeof(tab));
    }, {
        *nb_ptr = 0;
        dt_freep(tab_ptr);
    });
}

void *dt_dynarray2_add(void **tab_ptr, int *nb_ptr, size_t elem_size,
                       const uint8_t *elem_data)
{
    uint8_t *tab_elem_data = NULL;

    DT_DYNARRAY_ADD(INT_MAX, elem_size, *tab_ptr, *nb_ptr, {
        tab_elem_data = (uint8_t *)*tab_ptr + (*nb_ptr) * elem_size;
        if (elem_data)
            memcpy(tab_elem_data, elem_data, elem_size);
    }, {
        dt_freep(tab_ptr);
        *nb_ptr = 0;
    });
    return tab_elem_data;
}
