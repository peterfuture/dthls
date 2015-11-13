#include <time.h>
#include <sys/time.h>
#include "dt_queue.h"

#include <stdlib.h>

static _node_t *get_node_link_nth(dt_queue_t * qu, uint32_t n);

static void lock_queue(dt_queue_t * qu)
{
    if (unlikely(NULL == qu)) {
        return;
    }
    pthread_mutex_lock(&qu->mutex);
}

static void unlock_queue(dt_queue_t * qu)
{
    if (unlikely(NULL == qu)) {
        return;
    }
    pthread_mutex_unlock(&qu->mutex);
}

#if 0
static int wakeup_on_queue(dt_queue_t * qu)
{
    if (unlikely(NULL == qu)) {
        return -1;
    }
    return pthread_cond_broadcast(&qu->cond);
}

int wait_on_queue(dt_queue_t * qu)
{
    if (unlikely(NULL == qu)) {
        return -1;
    }

    pthread_cond_wait(&qu->cond, &qu->mutex);
    pthread_mutex_unlock(&qu->mutex);
    return 0;
}

/* timeout unit : millisecond */
int wait_on_dt_queue_timeout(dt_queue_t * qu, int timeout)
{
    if (unlikely(NULL == qu)) {
        return -1;
    }

    if (0 == timeout) {
        pthread_cond_wait(&qu->cond, &qu->mutex);
        pthread_mutex_unlock(&qu->mutex);
    } else {
        struct timespec abstime;
        struct timeval tv;
        long s;

        gettimeofday(&tv, NULL);
        s = timeout * 1000 + tv.tv_usec;
        tv.tv_sec += s / 1000000;
        tv.tv_usec = s % 1000000;

        abstime.tv_sec = tv.tv_sec;
        abstime.tv_nsec = tv.tv_usec * 1000;

        pthread_cond_timedwait(&qu->cond, &qu->mutex, &abstime);
        pthread_mutex_unlock(&qu->mutex);
    }
    return 0;
}
#endif

/**
 * @brief create a queue
 * return queue
 */
dt_queue_t *dt_queue_new(void)
{
    dt_queue_t *q = (dt_queue_t *) malloc(sizeof(dt_queue_t));
    q->head = q->tail = NULL;
    q->length = 0;
    pthread_mutex_init(&q->mutex, NULL);
    //pthread_cond_init(&q->cond,NULL);
    return q;
}

/**
 * @brief delete a queue
 */
void dt_queue_free(dt_queue_t * qu, free_func func)
{
    void *data;

    if (unlikely(NULL == qu)) {
        return;
    }

    while ((data = dt_queue_pop_tail(qu))) {
        if (func) {
            func(data);
        }
        free(data);
    }

    pthread_mutex_destroy(&qu->mutex);
    //pthread_cond_destroy(&qu->cond);

    free(qu);
}

void dt_queue_flush(dt_queue_t * qu, free_func func)
{
    void *data;

    if (unlikely(NULL == qu)) {
        return;
    }

    while ((data = dt_queue_pop_tail(qu))) {
        if (func) {
            func(data);
        }
        free(data);
    }
}

/**
 * @brief get length of queue
 */
uint32_t dt_queue_length(dt_queue_t * qu)
{
    if (unlikely(NULL == qu)) {
        return 0;
    }
    return qu->length;
}

/**
 * @brief push head of queue
 * @param qu    queue
 * @param data  node data
 */
void dt_queue_push_head(dt_queue_t * qu, void *data)
{
    _node_t *node = NULL;

    if (unlikely(NULL == qu)) {
        return;
    }

    lock_queue(qu);
    node = (_node_t *) malloc(sizeof(_node_t));
    node->data = data;
    node->next = qu->head;
    node->prev = NULL;

    if (qu->head) {
        qu->head->prev = node;
    }
    qu->head = node;
    if (NULL == qu->tail) {
        qu->tail = qu->head;
    }
    qu->length++;
    unlock_queue(qu);
    //wakeup_on_queue(qu);
}

/**
 * @brief push tail of queue
 * @param qu    queue
 * @param data  node data
 */
void dt_queue_push_tail(dt_queue_t * qu, void *data)
{
    _node_t *node = NULL;

    if (unlikely(NULL == qu)) {
        return;
    }

    lock_queue(qu);
    node = (_node_t *) malloc(sizeof(_node_t));
    node->data = data;
    node->next = NULL;
    node->prev = qu->tail;

    if (qu->tail) {
        qu->tail->next = node;
    }
    qu->tail = node;
    if (NULL == qu->head) {
        qu->head = qu->tail;
    }
    qu->length++;
    //printf("queue in length:%d \n",qu->length);
    unlock_queue(qu);
    //wakeup_on_queue(qu);
}

/**
 * @brief push data at the nth of queue
 * @param qu    queue
 * @param data  node data
 */
void dt_queue_push_nth(dt_queue_t * qu, void *data, uint32_t n)
{
    _node_t *node, *p;

    if (unlikely(NULL == qu)) {
        return;
    }

    lock_queue(qu);
    if (unlikely(n > qu->length)) {
        unlock_queue(qu);
        return dt_queue_push_tail(qu, data);
    }
    if (unlikely(n <= 1)) {
        unlock_queue(qu);
        return dt_queue_push_head(qu, data);
    }

    node = (_node_t *) malloc(sizeof(_node_t));
    node->data = data;

    p = get_node_link_nth(qu, n - 1);

    node->next = p->next;
    node->prev = p;
    p->next = node;
    node->next->prev = node;

    qu->length++;

    unlock_queue(qu);
    //wakeup_on_queue(qu);
}

/**
 * @brief pop data at the head of queue
 * @param qu    queue
 * @return  queue node data
 */
void *dt_queue_pop_head(dt_queue_t * qu)
{
    _node_t *p;
    void *data = NULL;

    lock_queue(qu);
    if (unlikely(NULL == qu) || unlikely(NULL == qu->head)) {
        goto end;
    }
    //printf("queue out head length:%d \n",qu->length);

    p = qu->head;
    qu->head = p->next;
    if (qu->head) {
        qu->head->prev = NULL;
    } else {
        qu->tail = NULL;
    }

    qu->length--;
    data = p->data;
    free(p);
end:
    unlock_queue(qu);
    return data;
}

/**
 * @brief pop data at the head of queue
 * @param qu    queue
 * @return  queue node data
 */
void *dt_queue_pre_pop_head(dt_queue_t * qu)
{
    _node_t *p;
    void *data = NULL;

    lock_queue(qu);
    if (unlikely(NULL == qu) || unlikely(NULL == qu->head)) {
        goto end;
    }
    p = qu->head;
    data = p->data;
end:
    unlock_queue(qu);
    return data;
}

/**
 * @brief pop data at the tail of queue
 * @param qu    queue
 * @return  queue node data
 */
void *dt_queue_pop_tail(dt_queue_t * qu)
{
    _node_t *p;
    void *data = NULL;

    lock_queue(qu);
    if (unlikely(NULL == qu) || unlikely(NULL == qu->tail)) {
        goto end;
    }

    p = qu->tail;
    qu->tail = p->prev;
    if (qu->tail) {
        qu->tail->next = NULL;
    } else {
        qu->head = NULL;
    }

    qu->length--;
    data = p->data;
    free(p);

    //printf("queue out tail length:%d \n",qu->length);
end:
    unlock_queue(qu);
    return data;
}

/**
 * @brief pop data at the nth of queue
 * @param qu    queue
 * @return  queue node data
 */
void *dt_queue_pop_nth(dt_queue_t * qu, uint32_t n)
{
    _node_t *p, *q;
    void *data = NULL;

    lock_queue(qu);
    if (unlikely(NULL == qu)) {
        goto end;
    }

    if (n > qu->length) {
        unlock_queue(qu);
        return dt_queue_pop_tail(qu);
    }
    if (n <= 1) {
        unlock_queue(qu);
        return dt_queue_pop_head(qu);
    }

    p = get_node_link_nth(qu, n);

    q = p->prev;
    q->next = p->next;
    p->next->prev = q;

    qu->length--;
    data = p->data;
    free(p);

end:
    unlock_queue(qu);
    return data;
}

/**
 * @brief peek data at the head of queue
 * @param qu    queue
 * @return  queue node data
 */
void *dt_queue_peek_head(dt_queue_t * qu)
{
    if (unlikely(NULL == qu)) {
        return NULL;
    }

    return qu->head ? qu->head->data : NULL;
}

/**
 * @brief peek data at the tail of queue
 * @param qu    queue
 * @return  queue node data
 */
void *dt_queue_peek_tail(dt_queue_t * qu)
{
    if (unlikely(NULL == qu)) {
        return NULL;
    }

    return qu->tail ? qu->tail->data : NULL;
}

/**
 * @brief peek data at the nth of queue
 * @param qu    queue
 * @return  queue node data
 */
void *dt_queue_peek_nth(dt_queue_t * qu, uint32_t n)
{
    _node_t *p;

    if (unlikely(NULL == qu)) {
        return NULL;
    }

    if (n > qu->length) {
        return dt_queue_peek_tail(qu);
    }
    if (n <= 1) {
        return dt_queue_peek_head(qu);
    }

    p = get_node_link_nth(qu, n);

    return p->data;
}

static _node_t *get_node_link_nth(dt_queue_t * qu, uint32_t n)
{
    _node_t *p = NULL;

    if (unlikely(NULL == qu)) {
        return NULL;
    }

    if (n < qu->length / 2) {
        p = qu->head;
        n = n - 1;
        while (n--) {
            p = p->next;
        }
    } else {
        p = qu->tail;
        n = qu->length - n;
        while (n--) {
            p = p->prev;
        }
    }
    return p;
}
