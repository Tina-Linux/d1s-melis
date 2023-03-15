/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "adb.h"
#include "adb_queue.h"
#include "adb_shell.h"
#include "adb_rb.h"
#include "adb_ev.h"
#include "misc.h"
#include "mqueue.h"
#include "pthread_internal.h"

#include <hal_gpio.h>
#include <hal_time.h>
#include <hal_thread.h>
#include <hal_sem.h>
#include <hal_event.h>

int clock_gettime(clockid_t clockid, struct timespec *tp);
int msleep(unsigned int msecs);

adb_schd_t adb_schd_init(int mask)
{
    adb_schd_t schd = NULL;

    schd = adb_malloc(sizeof(adb_schd));
    if (!schd)
    {
        fatal("no memory");
    }
    pthread_cond_init(&schd->cond, NULL);
    pthread_mutex_init(&schd->mutex, NULL);
    schd->event = 0;
    schd->mask = mask;
    return schd;
}

int adb_schd_wait(adb_schd_t schd)
{
    int event = 0;
    pthread_mutex_lock(&schd->mutex);
    if (!(schd->event & schd->mask))
    {
        pthread_cond_wait(&schd->cond, &schd->mutex);
    }
    event = schd->event;
    schd->event = 0;
    pthread_mutex_unlock(&schd->mutex);
    return event;
}

void adb_schd_wakeup(adb_schd_t schd, int event)
{
    pthread_mutex_lock(&schd->mutex);
    schd->event |= event;
    if ((schd->event & schd->mask) != 0)
    {
        pthread_cond_signal(&schd->cond);
    }
    pthread_mutex_unlock(&schd->mutex);
}

void adb_schd_release(adb_schd_t schd)
{
    pthread_cond_destroy(&schd->cond);
    pthread_mutex_destroy(&schd->mutex);
    adb_free(schd);
}

int adb_enqueue(adb_queue q, void **data, int ms)
{

    int ret;

    if (q == NULL)
    {
        printf("recv q NULL\n");
        return -1;
    }

    mq_timedsend(q, (const char *)data, sizeof(void *), 0, NULL);
    return 0;


}

int adb_dequeue(adb_queue q, void **data, int ms)
{
    int ret;
    struct timespec spec;
    struct timespec tmp;

    if (q == NULL)
    {
        printf("recv q NULL\n");
        return -1;
    }
    if (ms == 0)
    {
        mq_receive(q, (char *)data, sizeof(void *), 0);
    }
    else
    {
        memset(&spec, 0, sizeof(struct timespec));
        memset(&tmp, 0, sizeof(tmp));

        clock_gettime(CLOCK_REALTIME, &spec);

        tmp.tv_sec = ms / 1000;
        tmp.tv_nsec = (ms * 1000 * 1000) - tmp.tv_sec * (1000 * 1000 * 1000);

        spec.tv_sec += tmp.tv_sec;
        spec.tv_nsec += tmp.tv_nsec;

        if (spec.tv_nsec > (1000 * 1000 * 1000))
        {
            spec.tv_sec ++;
            spec.tv_nsec -= (1000 * 1000 * 1000);
        }

        while (1) {
            ret = mq_timedreceive(q, (char *)data, sizeof(void *), 0, &spec);
            if (ret == sizeof(void *))
                break;
            msleep(2);
        }
    }
    return 0;
}

adb_queue adb_queue_init(void)
{
    adb_queue q = NULL;

    struct mq_attr mq_stat;
    memset(&mq_stat, 0, sizeof(struct mq_attr));

    mq_stat.mq_maxmsg = ADB_QUEUE_NUMBER;
    mq_stat.mq_msgsize = sizeof(void *);

    q = mq_open("adb-queue", O_RDWR | O_CREAT, 0777, &mq_stat);
    if (!q)
    {
        printf("queue create failed\n");
        return NULL;
    }

    return q;
}

void adb_queue_release(adb_queue queue)
{
    if (!queue)
    {
        return;
    }
    mq_close(queue);
    mq_unlink("adb-queue");
}

typedef struct queue_event
{
    int event;
    void *data;
} queue_event;

int adb_enqueue_event(adb_queue_ev *aqe, int event, void *data, int ms)
{
    long ret;
    queue_event ev;

    memset(&ev, 0, sizeof(queue_event));
    ev.event = event;
    ev.data = data;

    while (1)
    {
        ret = mq_timedsend(aqe->queue, (const char *)&ev, sizeof(queue_event), 0, NULL);
        if (ret == 0)
        {
            break;
        }
    }
    return 0;
}

int adb_dequeue_event(adb_queue_ev *aqe, int *event, void **data, int ms)
{
    int ret;
    queue_event ev;
    struct timespec spec;
    struct timespec tmp;

    if (aqe->queue == NULL)
    {
        printf("recv q NULL\n");
        return -1;
    }

    if (ms == 0)
    {
        ret = mq_receive(aqe->queue, (char *)&ev, sizeof(queue_event), NULL);
    }
    else
    {
        memset(&spec, 0, sizeof(struct timespec));
        memset(&tmp, 0, sizeof(tmp));

        clock_gettime(CLOCK_REALTIME, &spec);

        tmp.tv_sec = ms / 1000;
        tmp.tv_nsec = (ms * 1000 * 1000) - tmp.tv_sec * (1000 * 1000 * 1000);

        spec.tv_sec += tmp.tv_sec;
        spec.tv_nsec += tmp.tv_nsec;

        if (spec.tv_nsec > (1000 * 1000 * 1000))
        {
            spec.tv_sec ++;
            spec.tv_nsec -= (1000 * 1000 * 1000);
        }

        ret = mq_timedreceive(aqe->queue, (char *)&ev, sizeof(queue_event), NULL, &spec);
    }

    if (ret <= 0)
    {
        return -1;
    }
    if (event)
    {
        *event = ev.event;
    }
    *data = ev.data;

    return 0;
}

adb_queue_ev *adb_queue_event_init(void)
{
    adb_queue_ev *q = NULL;

    q = adb_malloc(sizeof(adb_queue_ev));
    if (!q)
    {
        fatal("no memory");
    }
    memset(q, 0, sizeof(adb_queue_ev));
    struct mq_attr mq_stat;
    memset(&mq_stat, 0, sizeof(struct mq_attr));

    mq_stat.mq_maxmsg = ADB_QUEUE_NUMBER;
    mq_stat.mq_msgsize = sizeof(queue_event);

    q->queue = mq_open("adb-queue-event", O_RDWR | O_CREAT, 0777, &mq_stat);
    if (!q)
    {
        printf("queue create failed\n");
        return NULL;
    }
    return q;
}

void adb_queue_event_release(adb_queue_ev *aqe)
{
    if (!aqe)
    {
        return;
    }
    if (!aqe->queue)
    {
        mq_close(aqe->queue);
        mq_unlink("adb-queue-event");
    }
    adb_free(aqe);
}

adb_rb *adb_ringbuffer_init(int size)
{
    adb_rb *rb = NULL;

    if (size <= 0)
    {
        return NULL;
    }
    rb = adb_calloc(1, sizeof(adb_rb));
    if (!rb)
    {
        fatal("no memory");
    }
    rb->buffer = adb_malloc(size);
    if (!rb->buffer)
    {
        fatal("no memory");
    }
    rb->length = size;
    rb->start = 0;
    rb->end = 0;
    pthread_mutex_init(&rb->mutex, NULL);
    pthread_cond_init(&rb->cond, NULL);
    rb->ev = adb_event_init();
    return rb;
}

void adb_ringbuffer_release(adb_rb *rb)
{
    if (!rb)
    {
        return;
    }
    if (rb->buffer)
    {
        adb_free(rb->buffer);
    }
    pthread_mutex_destroy(&rb->mutex);
    pthread_cond_destroy(&rb->cond);
    adb_event_release(rb->ev);
    adb_free(rb);
    return;
}

int adb_ringbuffer_get(adb_rb *rb, void *buf, int size, int timeout)
{
    int len, cross = 0;

    if (!rb)
    {
        return -1;
    }
    pthread_mutex_lock(&rb->mutex);
    if (rb->isfull)
    {
        len = rb->length;
        goto cal_actual_size;
    }
    if (rb->end - rb->start == 0 && timeout >= 0)
    {
        pthread_mutex_unlock(&rb->mutex);
        adb_event_get(NULL, rb, ADB_EV_WRITE, timeout);
        pthread_mutex_lock(&rb->mutex);
    }
    len = rb->end - rb->start;
    if (len == 0)
    {
        pthread_mutex_unlock(&rb->mutex);
        return 0;
    }
    else if (len < 0)
    {
        len += rb->length;
    }
    else  if (len > rb->length)
    {
        printf("len=%d, error\n", len);
    }
cal_actual_size:
    len = len > size ? size : len;
    if (rb->start + len >= rb->length)
    {
        cross = 1;
    }
    if (cross != 0)
    {
        int first = rb->length - rb->start;
        memcpy(buf, rb->buffer + rb->start, first);
        memcpy(buf + first, rb->buffer, len - first);
        rb->start = len - first;
    }
    else
    {
        memcpy(buf, rb->buffer + rb->start, len);
        rb->start += len;
    }
    if (rb->isfull && len != 0)
    {
        rb->isfull = 0;
    }
    pthread_mutex_unlock(&rb->mutex);
    return len;
}

int adb_ringbuffer_put(adb_rb *rb, const void *buf, int size)
{
    int len, cross = 0;

    if (!rb)
    {
        return -1;
    }
    if (rb->isfull)
    {
        adb_event_set(rb->ev, ADB_EV_WRITE);
        return 0;
    }
    /*pthread_mutex_lock(&rb->mutex);*/
    if (rb->start > rb->end)
    {
        len = rb->start - rb->end;
    }
    else
    {
        len = rb->length - (rb->end - rb->start);
    }
    len = len > size ? size : len;
    if (rb->end + len > rb->length)
    {
        cross = 1;
    }
    if (cross != 0)
    {
        int first = rb->length - rb->end;
        memcpy(rb->buffer + rb->end, buf, first);
        memcpy(rb->buffer, buf + first, len - first);
        rb->end = len - first;
    }
    else
    {
        memcpy(rb->buffer + rb->end, buf, len);
        rb->end += len;
        rb->end %= rb->length;
    }
    if (rb->end == rb->start && len != 0)
    {
        rb->isfull = 1;
    }
    if (rb->isfull || rb->start != rb->end)
    {
        adb_event_set(rb->ev, ADB_EV_WRITE);
    }
    return len;
}

struct adb_ev
{
    hal_event_t handle;
};

adb_ev *adb_event_init(void)
{
    adb_ev *ev = adb_calloc(1, sizeof(adb_ev));
    if (!ev)
    {
        fatal("no memory");
    }
    ev->handle = hal_event_create();
    if (!ev->handle)
    {
        fatal("Event Group Create failed");
    }
    return ev;
}

void adb_event_release(adb_ev *ev)
{
    hal_event_delete(ev->handle);
    adb_free(ev);
}

int adb_event_set(adb_ev *ev, int bits)
{
    hal_event_set_bits(ev->handle, bits);
    return 0;
}

int adb_event_get(shell_xfer_t *xfer, adb_rb *rb, int bits, int ms)
{
    uint32_t uxBits = 0;
    uint32_t xTicksToWait = MS_TO_OSTICK(ms);
	adb_ev *ev = NULL;
	shell_xfer_t *xfer_tmp = NULL;

	if (xfer)
	{
		ev = xfer->ev_handle;
		xfer_tmp = xfer;
	} else {
		ev = rb->ev;
		xfer_tmp = rb->xfer;
	}
		
    if (!ms)
    {
        xTicksToWait = 0xfffffff;
    }
    else if (ms < 0)
    {
        xTicksToWait = 0;
    }

    do
    {
        if (xfer_tmp && xfer_tmp->recv_close)
        {
            return -1;
        }
        uxBits = hal_event_wait(ev->handle, bits, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, xTicksToWait);
        if (!(uxBits & bits))
        {
            if (ms > 0)
            {
                return -1;
            }
            else if (ms < 0)
            {
                return 0;
            }
            continue;
        }
        break;
    } while (!ms);

    return uxBits;
}

#ifdef MEM_DEBUG

#if 0
#define MEMLOG(fmt, arg...)               printf("<mem-debug>[%s:%u] "fmt"\n", __FUNCTION__, __LINE__, ##arg)
#else
#define MEMLOG(fmt, arg...)
#endif

typedef struct
{
    void *ptr;
    void *ret_addr;
    int32_t match_num;
} ptr_t;

typedef struct
{
    char name[32];
    uint32_t type;
    uint32_t count;
    ptr_t *ptr_s;
    uint32_t match;
    pthread_mutex_t mutex;
} memleak_t;

enum
{
    MEMLEAK_MALLOC = 0,
    MEMLEAK_CALLOC,
    MEMLEAK_REALLOC,
    MEMLEAK_STRDUP,
    MEMLEAK_SCANDIR,
    MEMLEAK_FREE,
};
const memleak_t gMemLeak_init[] =
{
    {"malloc", MEMLEAK_MALLOC, 0, NULL, 0, PTHREAD_MUTEX_INITIALIZER},
    {"calloc", MEMLEAK_CALLOC, 0, NULL, 0, PTHREAD_MUTEX_INITIALIZER},
    {"realloc", MEMLEAK_REALLOC, 0, NULL, 0, PTHREAD_MUTEX_INITIALIZER},
    {"strdup", MEMLEAK_STRDUP, 0, NULL, 0, PTHREAD_MUTEX_INITIALIZER},
    {"scandir", MEMLEAK_SCANDIR, 0, NULL, 0, PTHREAD_MUTEX_INITIALIZER},
    {"free", MEMLEAK_FREE, 0, NULL, 0, PTHREAD_MUTEX_INITIALIZER},
};

memleak_t gMemLeak[] =
{
    {"malloc", MEMLEAK_MALLOC, 0, NULL, 0, PTHREAD_MUTEX_INITIALIZER},
    {"calloc", MEMLEAK_CALLOC, 0, NULL, 0, PTHREAD_MUTEX_INITIALIZER},
    {"realloc", MEMLEAK_REALLOC, 0, NULL, 0, PTHREAD_MUTEX_INITIALIZER},
    {"strdup", MEMLEAK_STRDUP, 0, NULL, 0, PTHREAD_MUTEX_INITIALIZER},
    {"scandir", MEMLEAK_SCANDIR, 0, NULL, 0, PTHREAD_MUTEX_INITIALIZER},
    {"free", MEMLEAK_FREE, 0, NULL, 0, PTHREAD_MUTEX_INITIALIZER},
};

#if 1
#define memleak_add_count(pointer, type) \
    do { \
        pthread_mutex_lock(&gMemLeak[type].mutex); \
        gMemLeak[type].count++; \
        gMemLeak[type].ptr_s = realloc(gMemLeak[type].ptr_s, sizeof(ptr_t)*gMemLeak[type].count); \
        gMemLeak[type].ptr_s[gMemLeak[type].count-1].ptr = pointer; \
        gMemLeak[type].ptr_s[gMemLeak[type].count-1].ret_addr = __builtin_return_address(0); \
        gMemLeak[type].ptr_s[gMemLeak[type].count-1].match_num = -1; \
        pthread_mutex_unlock(&gMemLeak[type].mutex); \
    } while (0)
#else
static void memleak_add_count(void *ptr, uint32_t type)
{
    gMemLeak[type].count++;
    gMemLeak[type].ptr_s = realloc(gMemLeak[type].ptr_s, sizeof(ptr_t) * gMemLeak[type].count);
    gMemLeak[type].ptr_s[gMemLeak[type].count - 1].ptr = ptr;
    gMemLeak[type].ptr_s[gMemLeak[type].count - 1].ret_addr = __builtin_return_address(0);
    gMemLeak[type].ptr_s[gMemLeak[type].count - 1].match_num = -1;
}
#endif

void *malloc_wrapper(size_t size)
{
    void *ptr = NULL;
    ptr = malloc(size);
    memleak_add_count(ptr, MEMLEAK_MALLOC);
    return ptr;
}

void *calloc_wrapper(size_t nmemb, size_t size)
{
    void *ptr = NULL;

    ptr = calloc(nmemb, size);
    memleak_add_count(ptr, MEMLEAK_CALLOC);
    MEMLOG("--------calloc ptr:%p\n", ptr);
    return ptr;
}

void *realloc_wrapper(void *ptr, size_t size)
{
    void *p = NULL;
    uint32_t i;

    p = realloc(ptr, size);
    for (i = 0; i < gMemLeak[MEMLEAK_REALLOC].count; i++)
    {
        if (ptr == gMemLeak[MEMLEAK_REALLOC].ptr_s[i].ptr)
        {
            gMemLeak[MEMLEAK_REALLOC].ptr_s[i].ptr = p;
            MEMLOG("--------realloc ptr:%p(instead)\n", p);
            return p;
        }
    }
    memleak_add_count(p, MEMLEAK_REALLOC);
    MEMLOG("--------realloc ptr:%p\n", p);
    return p;
}

char *strdup_wrapper(const char *s)
{
    char *ptr = NULL;

    ptr = strdup(s);
    memleak_add_count(ptr, MEMLEAK_STRDUP);
    MEMLOG("--------strdup ptr:%p\n", ptr);
    return ptr;
}
#if 0
int scandir_wrapper(const char *dirp, struct dirent ***namelist,
                    int (*filter)(const struct dirent *),
                    int (*compar)(const struct dirent **, const struct dirent **))
{
    struct dirent **ptr = NULL;
    int ret, i;

    ret = scandir(dirp, namelist, filter, compar);
    ptr = *namelist;
    if (ret == 0)
    {
        return ret;
    }
    for (i = 0; i < ret; i++)
    {
        MEMLOG("--------scandir ptr:%p\n", ptr[i]);
        memleak_add_count(ptr[i], MEMLEAK_SCANDIR);
    }
    memleak_add_count(ptr, MEMLEAK_SCANDIR);
    MEMLOG("--------scandir ptr:%p\n", ptr);
    return ret;
}
#endif

void free_wrapper(void *ptr)
{
    MEMLOG("--------free ptr:%p\n", ptr);
    memleak_add_count(ptr, MEMLEAK_FREE);
    return free(ptr);
}

static void memleak_print_notmatch(uint32_t type)
{
    int i;
    memleak_t *alloc = &gMemLeak[type];
    for (i = 0; i < alloc->count; i++)
    {
        if (alloc->ptr_s[i].match_num < 0)
        {
            printf("ptr:%p, return addr:%p\n",
                   alloc->ptr_s[i].ptr, alloc->ptr_s[i].ret_addr);
        }
    }
}

static int memleak_doublefree(void)
{
    memleak_t *free = &gMemLeak[MEMLEAK_FREE];
    int doublefree = 0;
    int i, j;

    for (i = 0; i < free->count; i++)
    {
        for (j = i + 1; j < free->count; j++)
        {
#if 0
            if (free->ptr_s[i].ptr == free->ptr_s[j].ptr)
            {
#else
            if (free->ptr_s[i].match_num == free->ptr_s[j].match_num)
            {
#endif
                printf("match_num=%d, ptr=%p double free...return addr:%p\n",
                       free->ptr_s[i].match_num,
                       free->ptr_s[i].ptr,
                       free->ptr_s[i].ret_addr);
                doublefree++;
            }
        }
    }
    if (doublefree != 0)
    {
        printf("double free count=%d\n", doublefree);
    }
}

static int memleak_match(uint32_t type)
{
    if (gMemLeak[type].count == gMemLeak[type].match)
    {
        return 0;
    }
    else
    {
        printf("Not match! type:%u, count:%u, match:%u\n", type, gMemLeak[type].count, gMemLeak[type].match);
        memleak_print_notmatch(type);
        return -1;
    }
}

static void memleak_caculate(void)
{
    uint32_t i, j, k;
    int ret;
    int found;

    memleak_t *free = &gMemLeak[MEMLEAK_FREE];
    for (i = 0; i < free->count; i++)
    {
        if (free->ptr_s[i].match_num >= 0)
        {
            continue;
        }
        found = 0;
        for (j = MEMLEAK_MALLOC; j < MEMLEAK_FREE && !found; j++)
        {
            memleak_t *alloc = &gMemLeak[j];
            for (k = 0; k < alloc->count && !found; k++)
            {
                if (alloc->ptr_s[k].match_num >= 0)
                {
                    continue;
                }
                if (alloc->ptr_s[k].ptr == free->ptr_s[i].ptr)
                {
                    alloc->ptr_s[k].match_num = i;
                    free->ptr_s[i].match_num = k;
                    alloc->match++;
                    free->match++;
                    found = 1;
                }
            }
        }
    }
    /*memleak_doublefree();*/
    ret = memleak_match(MEMLEAK_MALLOC);
    ret += memleak_match(MEMLEAK_CALLOC);
    ret += memleak_match(MEMLEAK_REALLOC);
    ret += memleak_match(MEMLEAK_STRDUP);
    ret += memleak_match(MEMLEAK_SCANDIR);
    ret += memleak_match(MEMLEAK_FREE);
    if (!ret)
    {
        printf("=========================\n");
        printf("=   alloc, free match   =\n");
        printf("=========================\n");
    }
    else
    {
        printf("=========================\n");
        printf("= alloc, free not match =\n");
        printf("=========================\n");
        /*exit(-1);*/
    }
}

void memleak_exit(void)
{
    uint32_t i;

    memleak_t *mem = &gMemLeak[MEMLEAK_FREE];
    for (i = MEMLEAK_MALLOC; i < MEMLEAK_FREE + 1; i++)
    {
        mem = &gMemLeak[i];
        if (mem->ptr_s != NULL)
        {
            free(mem->ptr_s);
            mem->ptr_s = NULL;
        }
        pthread_mutex_destroy(&mem->mutex);
    }
    memcpy(gMemLeak, gMemLeak_init, sizeof(gMemLeak_init));
}

void memleak_print(void)
{
    memleak_caculate();
#if 0
    printf("---------------\n");
    printf("malloc count:%u\n", gMemLeak[MEMLEAK_MALLOC].count);
    printf("calloc count:%u\n", calloc_count);
    printf("realloc count:%u\n", realloc_count);
    printf("strdup count:%u\n", strdup_count);
    printf("scandir count:%u\n", scandir_count);
    printf("free count:%u\n", free_count);
    printf("\n");
    printf("total malloc count:%u\n", malloc_count + calloc_count + realloc_count + strdup_count + scandir_count);
    printf("---------------\n");
#endif
}

#endif

#ifdef ADBD_RECORD_ALIVE_THREAD
static char *g_alive_thread[32];
static int g_alive_thread_position = -1;
/*lock?*/
void record_alive_thread_print(void)
{
    int i;
    if (g_alive_thread_position < 0)
    {
        return;
    }
    printf("adbd shell command maybe alive[position=%d]:\n", g_alive_thread_position);
    printf("=====Start=====\n");
    for (i = 0; i < ARRAY_SIZE(g_alive_thread); i++)
    {
        if (g_alive_thread[i] != NULL)
        {
            printf("thread name:%s\n", g_alive_thread[i]);
        }
    }
    printf("=====End=====\n");
}

void record_alive_thread_add(const char *name)
{
    char *str;
    str = adb_strdup(name);
    if (!str)
    {
        adbd_err("no memory");
        return;
    }
    if (++g_alive_thread_position == ARRAY_SIZE(g_alive_thread))
    {
        g_alive_thread_position = 0;
    }
    if (g_alive_thread[g_alive_thread_position] != NULL)
    {
        free(g_alive_thread[g_alive_thread_position]);
    }
    g_alive_thread[g_alive_thread_position] = str;
}

/* don't use */
void record_alive_thread_release(void)
{
    int i;
    for (i = 0; i < ARRAY_SIZE(g_alive_thread); i++)
    {
        if (g_alive_thread[i] != NULL)
        {
            free(g_alive_thread[i]);
            g_alive_thread[i] = NULL;
        }
    }
}
#endif

int adb_thread_wait_timeout(adb_thread_t tid, void **retval, int timeout)
{
    int ret = 0;
    pthread_t thread = (pthread_t)tid;
    _pthread_data_t *ptcb;

    if (thread == RT_NULL)
    {
        return -1;
    }

    if (thread == (pthread_t)kthread_self())
    {
        return -1;
    }

    ptcb = _pthread_get_data(thread);

    if (ptcb == NULL)
    {
        return -1;
    }

    if (ptcb->attr.detachstate == PTHREAD_CREATE_DETACHED)
    {
        return -1;
    }

    ret = hal_sem_timedwait(ptcb->joinable_sem, MS_TO_OSTICK(timeout));
    if (ret == RT_EOK)
    {
        if (retval != 0)
        {
            *retval = ptcb->return_value;
        }

        hal_sem_delete(ptcb->joinable_sem);

        if (ptcb->attr.detachstate == PTHREAD_CREATE_JOINABLE)
        {
            if (ptcb->attr.stackaddr == 0)
            {
                free(ptcb->tid->stack_addr);
            }

            free(ptcb->tid);

            free(ptcb);
        }
    }
    else
    {
        return -2;
    }

    return 0;
}


