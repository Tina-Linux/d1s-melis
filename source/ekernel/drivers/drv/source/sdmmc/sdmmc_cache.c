/*
 * =====================================================================================
 *
 *       Filename:  sdmmc_cache.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2020-04-29 17:35:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zeng Zhijin
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>

#include <typedef.h>
#include <log.h>
#include <common/list.h>

#include "cache_hash.h"
#include "sdmmc_cache.h"

#include <rtconfig.h>
#include <pthread.h>
#include <semaphore.h>
#include <ktimer.h>

#ifndef CONFIG_SDMMC_CACHE_WRITEBACK_THREAD_PRIO
#define CONFIG_SDMMC_CACHE_WRITEBACK_THREAD_PRIO (22)
#endif

#define CONFIG_SDMMC_CACHE_DIRECTLY_SIZE (32 * 1024)

#define HASHTABLE_SIZE   (256)
#define HASHTABLE_MAX  (256)
#define HASH_VALUE(d, b)   ((unsigned long)((((long)d) << (sizeof(long)*8 - 6)) | (b)))

#define CACHE_READAHEAD_SIZE     (32 * 1024)
#define WRITEBACK_BLOCKS_MAX   (256)
#define MAX_IO_VECOTR  WRITEBACK_BLOCKS_MAX

#define CACHE_BIT(x) (1 << x)

#define CACHE_SIZE_MASK    0x000fffff
#define CACHE_DIRTY        0x00100000
#define CACHE_BUSY         0x00200000
#define CACHE_USED         0x00400000

#define CACHE_BLOCK_SIZE(b) ((b)->flags & CACHE_SIZE_MASK)

#define BCM_STATE_DEINIT     0
#define BCM_STATE_INITED     1

#define MS_TO_NS(x) (x * 1000 * 1000LL)

#define MAX_BLOCK_SIZE_ORDERS   16

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

struct block_cache_manager
{
    struct list_head    list_ready;
    struct list_head    list_lockdown;

    int                 flags;
    uint32_t            cache_heap_size;
    cache_hash_table    *hash_table;
    pthread_mutex_t     lock;
};

struct cache_entry
{
    void *dev;
    int  block_offset;
    int  flags;
    int  ref;

    struct cache_entry  *hash_list_next_element;
    struct list_head    list;
    struct list_head    cache_heap_list;
};

struct iovec
{
    void *iov_base;
    size_t iov_len;
};

struct cache_entry_key
{
    void  *dev;
    int   block_offset;
};

struct cache_heap
{
    uint32_t byte_size;
    void     *heap_ptr;

    uint32_t block_size;
    uint32_t total_ents;
    int      free_ents;
    struct list_head  free_ents_list;
};

static pthread_t pth_flusher = 0;
static sem_t flusher_sem;

static struct cache_heap  g_CacheHeaps[MAX_BLOCK_SIZE_ORDERS];
static struct block_cache_manager g_bcm;

extern int msleep(int);
extern int clock_gettime(clockid_t clockid, struct timespec *tp);

static int alloc_cache_entrys(struct cache_entry **ents, int nCount, int block_size, int *isBlocked);
static void free_cache_entry(struct cache_entry *ent);

static void *get_cache_data(struct cache_entry *ent)
{
    return (ent + 1);
}

static void block_cache_init_lock(pthread_mutex_t *lock)
{
    pthread_mutex_init(lock, NULL);
}

static void block_cache_destroy_lock(pthread_mutex_t *lock)
{
    pthread_mutex_destroy(lock);
}

static void block_cache_lock(pthread_mutex_t *lock)
{
    pthread_mutex_lock(lock);
}

static void block_cache_unlock(pthread_mutex_t *lock)
{
    pthread_mutex_unlock(lock);
}

static unsigned long cache_entry_hash(void *_v, const void *_key, unsigned long range)
{
    struct cache_entry *ent = (struct cache_entry *)_v;
    struct cache_entry_key *key = (struct cache_entry_key *)_key;
    unsigned long hash;

    if (ent)
    {
        hash = HASH_VALUE(ent->dev, ent->block_offset);
    }
    else
    {
        hash = HASH_VALUE(key->dev, key->block_offset);
    }

    return hash % HASHTABLE_MAX;
}

static ssize_t read_vector(__hdle fd, const struct iovec *vector, int start_block, int count)
{
    char *buffer = NULL;
    char *buffer_start = NULL;
    size_t bytes;
    ssize_t bytes_read = 0;
    ssize_t sectors_read = 0;
    ssize_t total_sectors_read = 0;
    int i;

    bytes = 0;
    for (i = 0; i < count; ++i)
    {
        bytes += vector[i].iov_len;
    }

    buffer = (char *) malloc(bytes);
    if (buffer == NULL)
    {
        return -ENOMEM;
    }

    buffer_start = buffer;
    sectors_read = sdmmc_dev_phy_read(buffer, start_block, bytes >> 9, fd);
    if (sectors_read <= 0)
    {
        goto read_vector_free;
    }

    bytes_read = sectors_read * 512;
    total_sectors_read += sectors_read;
    bytes = bytes_read;

    for (i = 0; i < count; ++i)
    {
        size_t copy_size = MIN(vector[i].iov_len, bytes);

        memcpy(vector[i].iov_base, buffer, copy_size);

        buffer += copy_size;
        bytes -= copy_size;
        if (bytes == 0)
        {
            break;
        }
    }
read_vector_free:
    if (buffer_start)
    {
        free(buffer_start);
    }
    return total_sectors_read;
}

static ssize_t read_vector_block(__hdle fd, int start_block, struct iovec *iov, int count)
{
    int sblock = start_block;
    size_t finsh_blocks = 0;
    ssize_t ret;
    struct iovec *tmpiov;
    int i, n;

    i = 0;
    tmpiov = iov;
    while (i < count)
    {
        if (i + MAX_IO_VECOTR < count)
        {
            n = MAX_IO_VECOTR;
        }
        else
        {
            n = (count - i);
        }

        ret = read_vector(fd, tmpiov, sblock, n);
        if (ret < 0 || ret != n)
        {
            __err("read %d block but just return %d!", n, ret);
            return -1;
        }
        sblock += ret;
        finsh_blocks += ret;
        i += n;
        tmpiov += n;
    }

    return finsh_blocks;
}

static ssize_t write_vector(__hdle fd, const struct iovec *vector, int start_block, int count)
{
    char *buffer = NULL;
    char *bufferpos = NULL;
    size_t bytes, copy_bytes;
    ssize_t sector_written = 0;
    ssize_t total_sector_written = 0;
    int i;

    bytes = 0;
    for (i = 0; i < count; ++i)
    {
        bytes += vector[i].iov_len;
    }

    buffer = (char *) malloc(bytes);
    if (buffer == NULL)
    {
        return -ENOMEM;
    }

    copy_bytes = bytes;
    bufferpos = buffer;
    for (i = 0; i < count; ++i)
    {
        size_t copy_size = MIN(vector[i].iov_len, copy_bytes);

        memcpy(bufferpos, vector[i].iov_base, copy_size);

        bufferpos += copy_size;
        copy_bytes -= copy_size;
        if (copy_bytes == 0)
        {
            break;
        }
    }

    sector_written = sdmmc_dev_phy_write(buffer, start_block, bytes >> 9, fd);
    if (sector_written >= 0)
    {
        total_sector_written += sector_written;
    }

    if (buffer)
    {
        free(buffer);
    }
writedv_out:
    return total_sector_written;
}

static ssize_t write_vector_block(__hdle fd, int start_block, struct iovec *iov, int count)
{
    int sblock = start_block;
    size_t finsh_blocks = 0;
    ssize_t ret;
    struct iovec *tmpiov;
    int i, n;

    i = 0;
    tmpiov = iov;
    while (i < count)
    {
        if (i + MAX_IO_VECOTR < count)
        {
            n = MAX_IO_VECOTR;
        }
        else
        {
            n = (count - i);
        }

        ret = write_vector(fd, tmpiov, sblock, n);
        if (ret < 0 || ret != n)
        {
            __err("write %d block but just return %d!", n, ret);
            return -1;
        }
        sblock += ret;
        finsh_blocks += ret;

        i += n;
        tmpiov += n;
    }

    return finsh_blocks;
}

static int cache_entry_cmp(const void *first, const void *second)
{
    struct cache_entry *ent1 = *(struct cache_entry **) first;
    struct cache_entry *ent2 = *(struct cache_entry **) second;

    if (ent1 == NULL || ent2 == NULL)
    {
        return -1;
    }

    if (ent1->dev == ent2->dev)
    {
        return ent1->block_offset - ent2->block_offset;
    }
    else
    {
        return ent1->dev - ent2->dev;
    }
}

static int cache_entry_compare(void *_cache_entry, const void *_key)
{
    struct cache_entry *ent = (struct cache_entry *)_cache_entry;
    struct cache_entry_key *key = (struct cache_entry_key *)_key;

    if ((ent->dev == key->dev) && (ent->block_offset == key->block_offset))
    {
        return 0;
    }

    return -1;
}

static void cache_list_add_to_head(struct list_head *list, struct cache_entry *ent)
{
    if (!list_empty(&ent->list))
    {
        return;
    }

    list_add(&ent->list, list);
}

static void cache_list_add_to_tail(struct list_head *list, struct cache_entry *ent)
{
    if (!list_empty(&ent->list))
    {
        return;
    }

    list_add_tail(&ent->list, list);
}

static void cache_list_remove_from_list(struct list_head *list,  struct cache_entry *ent)
{
    list_del_init(&ent->list);
}

static struct cache_entry *lookup_cache_entry(__hdle dev, int block_offset)
{
    struct cache_entry *ent = NULL;
    int i;
    int hasWarned = 0;
    struct cache_entry_key key;

    for (i = 0 ; ; ++i)
    {
        key.dev = dev;
        key.block_offset = block_offset;
        ent = cache_hash_lookup(g_bcm.hash_table, &key);
        if (NULL == ent)
        {
            return (NULL);
        }

        if ((ent->flags & CACHE_BUSY) == 0 && (ent->dev != dev || ent->block_offset != block_offset))
        {
            __err("Error: invalid cache entry by cache_hash_lookup, lookup %p::%ld but get %p::%ld!!!\n",
                  dev, block_offset, ent->dev, ent->block_offset);
            return (NULL);
        }

        if ((ent->flags & CACHE_BUSY) == 0)
        {
            break;
        }

        block_cache_unlock(&g_bcm.lock);
        if (i > 1000)
        {
            if (hasWarned == 0)
            {
                __msg("the block(%p::%ld) is busy. Please check your code!\n", dev, block_offset);
                hasWarned = 1;
            }
            msleep(1);
            i = 0;
        }
        block_cache_lock(&g_bcm.lock);
    }

    return (ent);
}

static int flush_cache_entry_list(struct cache_entry **ents, int n_ents)
{
    struct iovec rw_iovecs[WRITEBACK_BLOCKS_MAX];
    int rw_iovec_cnt = 0;
    struct cache_entry *first_ent;
    struct cache_entry *prev_ent;
    int error = 0;
    int i;

    prev_ent = NULL;
    first_ent = ents[0];

    if (n_ents > WRITEBACK_BLOCKS_MAX)
    {
        __err("can not flush %d blocks, WRITEBACK_BLOCKS_MAX is %d\n", n_ents, WRITEBACK_BLOCKS_MAX);
        return -1;
    }

    for (i = 0 ; i < n_ents && error >= 0  ; ++i)
    {
        struct cache_entry *ent = ents[i];

        if (ent->ref > 0)
        {
            __msg("the block is locked, so not flushing\n");
            continue;
        }

        if ((ent->flags & CACHE_DIRTY) == 0)
        {
            continue;
        }

        RT_ASSERT(ent->dev != NULL);

        if (i > 0 && ents[i - 1]->dev == ents[i]->dev &&
            ents[i - 1]->block_offset >= ents[i]->block_offset)
        {
            __err("ents[%d]->block_offset(%d) >= ents[%d]->block_offsetBlock(%d).\n",
                  i - 1, i, ents[i - 1]->block_offset, ents[i]->block_offset);
            return -1;
        }

        RT_ASSERT(0 == ent->ref);

        /*
         * ensure block physical continuity
         */
        if (NULL != prev_ent &&
            (prev_ent->dev != ent->dev ||
             prev_ent->block_offset != (ent->block_offset - 1)))
        {
            if (prev_ent->block_offset - first_ent->block_offset != rw_iovec_cnt - 1)
            {
                __err("prev_ent->block_offset(%d) - first_ent->block_offset(%d) != rw_iovec_cnt(%d) - 1",
                      prev_ent->block_offset, first_ent->block_offset, rw_iovec_cnt);

                return -1;
            }

            error = write_vector_block(first_ent->dev, first_ent->block_offset,
                                       rw_iovecs, rw_iovec_cnt);

            first_ent = ent;
            rw_iovec_cnt = 0;
        }
        rw_iovecs[rw_iovec_cnt].iov_base = get_cache_data(ent);
        rw_iovecs[rw_iovec_cnt].iov_len  = CACHE_BLOCK_SIZE(ent);
        rw_iovec_cnt++;

        if (NULL == prev_ent)
        {
            first_ent = ent;
        }
        prev_ent = ent;
    }

    if (rw_iovec_cnt > 0)
    {
        if (prev_ent->block_offset - first_ent->block_offset != rw_iovec_cnt - 1)
        {
            __err("prev_ent->block_offset(%d) - first_ent->block_offset(%d) != rw_iovec_cnt(%d) - 1",
                  prev_ent->block_offset, first_ent->block_offset, rw_iovec_cnt);
            return -1;
        }

        error = write_vector_block(first_ent->dev, first_ent->block_offset,
                                   rw_iovecs, rw_iovec_cnt);
        rw_iovec_cnt = 0;
    }

    for (i = 0 ; i < n_ents ; ++i)
    {
        struct cache_entry *ent = ents[i];
        int nBlock = ent->block_offset;

        if ((ent->flags & CACHE_DIRTY) == 0)
        {
            continue;
        }
        if (ent->ref > 0)
        {
            continue;
        }
        ent->flags &= ~CACHE_DIRTY;
    }

    if (error < 0)
    {
        __err("failed!!, error = %d\n", error);
    }
    return error;
}

static int release_cache_entrys(void)
{
    struct cache_entry   *ents[WRITEBACK_BLOCKS_MAX];
    struct cache_entry   *ent;
    int nCount = 0;
    int dirty_cnt = 0;
    int i;

    struct list_head *pos;

    list_for_each_prev(pos, &g_bcm.list_ready)
    {
        ent = list_entry(pos, struct cache_entry, list);
        RT_ASSERT(0 == ent->ref);

        if (ent->flags & CACHE_BUSY)
        {
            continue;
        }
        if (ent->dev == NULL)
        {
            continue;
        }
        if (ent->flags & CACHE_DIRTY)
        {
            dirty_cnt++;
        }

        ent->flags |= CACHE_BUSY;
        ents[nCount++] = ent;

        if (nCount >= WRITEBACK_BLOCKS_MAX)
        {
            break;
        }
    }
    if (dirty_cnt > 0)
    {
        qsort(ents, nCount, sizeof(struct cache_entry *), cache_entry_cmp);
        block_cache_unlock(&g_bcm.lock);
        if (flush_cache_entry_list(ents, nCount) < 0)
        {
            block_cache_lock(&g_bcm.lock);
            for (i = 0 ; i < nCount ; ++i)
            {
                ent = ents[i];
                ent->flags &= ~CACHE_BUSY;
            }
            return -1;
        }
        block_cache_lock(&g_bcm.lock);
    }

    for (i = 0 ; i < nCount ; ++i)
    {
        ent = ents[i];
        cache_list_remove_from_list(&g_bcm.list_ready, ent);
        cache_hash_remove(g_bcm.hash_table, ent);
        free_cache_entry(ent);
    }
    return nCount;
}

static int read_cache_entry_list(__hdle dev, int block_offset, struct cache_entry **ents, int nCount, int bsize)
{
    int error = 0;
    int i;
    struct iovec rw_iovecs[WRITEBACK_BLOCKS_MAX];

    RT_ASSERT(nCount <= WRITEBACK_BLOCKS_MAX);

    for (i = 0 ; i < nCount ; ++i)
    {
        RT_ASSERT(ents[i]->flags & CACHE_BUSY);
        RT_ASSERT((ents[i]->flags & CACHE_DIRTY) == 0);

        rw_iovecs[i].iov_base = get_cache_data(ents[i]);
        rw_iovecs[i].iov_len  = bsize;
    }

    if (read_vector_block(dev, block_offset, rw_iovecs, nCount) < 0)
    {
        return -1;
    }

    return nCount;
}

static struct cache_entry *do_get_empty_cache_entry(__hdle dev, int block_offset, int bsize)
{
    struct cache_entry *ent;

again:
    ent = lookup_cache_entry(dev, block_offset);

    if (NULL != ent)
    {
        ent->ref++;

        if (1 == ent->ref)
        {
            cache_list_remove_from_list(&g_bcm.list_ready, ent);
            cache_list_add_to_head(&g_bcm.list_lockdown, ent);
        }
    }
    else
    {
        int isBlocked;

        ent = NULL;
        if (alloc_cache_entrys(&ent, 1, bsize, &isBlocked) < 1)
        {
            __err("failed to alloc cache block\n");
            return NULL;
        }

        if (ent == NULL)
        {
            return NULL;
        }

        ent->dev = dev;
        ent->block_offset = block_offset;
        ent->flags &= ~CACHE_BUSY;
        ent->ref++;
        cache_hash_insert(g_bcm.hash_table, ent);

        cache_list_add_to_head(&g_bcm.list_lockdown, ent);
    }

    return (ent);
}

static void *get_block(__hdle dev, int block_offset, int bsize)
{
    struct cache_entry *ents[ WRITEBACK_BLOCKS_MAX ];
    struct cache_entry *ent = NULL;
    struct cache_entry_key key;
    int readahead_block_max_cnt = CACHE_READAHEAD_SIZE / bsize;
    int alloced_ent_cnt = 0;
    int i;
    int read_err;

    if (readahead_block_max_cnt > WRITEBACK_BLOCKS_MAX)
    {
        readahead_block_max_cnt = WRITEBACK_BLOCKS_MAX;
    }

    if (dev == NULL)
    {
        __log("invalid device %p", dev);
        return NULL;
    }

    block_cache_lock(&g_bcm.lock);

    ent = lookup_cache_entry(dev, block_offset);

    if (NULL != ent)
    {
        ent->ref++;
        if (1 == ent->ref)
        {
            cache_list_remove_from_list(&g_bcm.list_ready, ent);
            cache_list_add_to_head(&g_bcm.list_lockdown, ent);
        }
    }
    else
    {
        struct cache_entry protected_ent;
        int rw_cnt;
        int request_block;
        int isBlocked;

        request_block = block_offset;

        for (rw_cnt = 1 ; rw_cnt < readahead_block_max_cnt ; ++rw_cnt)
        {
            key.dev = dev;
            key.block_offset = block_offset + rw_cnt;
            if (cache_hash_lookup(g_bcm.hash_table, &key) != NULL)
            {
                break;
            }
        }

        for (; rw_cnt < readahead_block_max_cnt && block_offset > 0 ; ++rw_cnt)
        {
            key.dev = dev;
            key.block_offset = block_offset - 1;
            if (cache_hash_lookup(g_bcm.hash_table, &key) != NULL)
            {
                break;
            }
            block_offset--;
        }

        memset(&protected_ent, 0, sizeof(protected_ent));

        protected_ent.flags |= CACHE_BUSY;
        protected_ent.dev   = dev;
        protected_ent.block_offset = request_block;
        cache_hash_insert(g_bcm.hash_table, &protected_ent);

        alloced_ent_cnt = alloc_cache_entrys(ents, rw_cnt, bsize, &isBlocked);

        cache_hash_remove(g_bcm.hash_table, &protected_ent);

        if (isBlocked)
        {
            block_offset = request_block;
            for (rw_cnt = 1 ; rw_cnt < alloced_ent_cnt ; ++rw_cnt)
            {
                key.dev = dev;
                key.block_offset = block_offset + rw_cnt;
                if (cache_hash_lookup(g_bcm.hash_table, &key) != NULL)
                {
                    break;
                }
            }

            for (; rw_cnt < alloced_ent_cnt && block_offset > 0 ; ++rw_cnt)
            {
                key.dev = dev;
                key.block_offset = block_offset - 1;
                if (cache_hash_lookup(g_bcm.hash_table, &key) != NULL)
                {
                    break;
                }
                block_offset--;
            }
        }

        if (alloced_ent_cnt == 0)
        {
            __err("allock cache entry failed!\n");
            return NULL;
        }

        if (alloced_ent_cnt < rw_cnt)
        {
            int max_blocks_cnt = (int)(block_offset  + rw_cnt - request_block);

            if (alloced_ent_cnt <= max_blocks_cnt)
            {
                block_offset = request_block;
            }
            else
            {
                block_offset = request_block + max_blocks_cnt - alloced_ent_cnt;
            }
            rw_cnt = alloced_ent_cnt;
        }
        else if (alloced_ent_cnt > rw_cnt)
        {
            for (i = rw_cnt ; i < alloced_ent_cnt ; ++i)
            {
                free_cache_entry(ents[i]);
            }
            alloced_ent_cnt = rw_cnt;
        }

        ent = NULL;
        for (i = 0 ; i < alloced_ent_cnt ; ++i)
        {
            struct cache_entry *tempent = ents[i];
            tempent->flags |= CACHE_BUSY;
            tempent->dev = dev;
            tempent->block_offset = block_offset + i;
            cache_hash_insert(g_bcm.hash_table, tempent);

            if (tempent->block_offset == request_block)
            {
                tempent->ref++;
                ent = tempent;
                cache_list_add_to_head(&g_bcm.list_lockdown, ent);
            }
            else
            {
                cache_list_add_to_head(&g_bcm.list_ready, tempent);
            }
        }

        block_cache_unlock(&g_bcm.lock);
        read_err = read_cache_entry_list(dev, block_offset, ents, rw_cnt, bsize);

        block_cache_lock(&g_bcm.lock);
        if (read_err < 0)
        {
            ent = NULL;
            for (i = 0 ; i < alloced_ent_cnt ; ++i)
            {
                struct cache_entry *tempent = ents[i];
                tempent->flags &= ~CACHE_BUSY;
                tempent->dev = dev;
                tempent->block_offset = block_offset + i;
                cache_hash_remove(g_bcm.hash_table, tempent);

                if (tempent->block_offset == request_block)
                {
                    tempent->ref = 0;
                    cache_list_remove_from_list(&g_bcm.list_lockdown, tempent);
                }
                else
                {
                    cache_list_remove_from_list(&g_bcm.list_ready, tempent);
                }
            }
        }
        else
        {
            for (i = 0 ; i < alloced_ent_cnt ; ++i)
            {
                struct cache_entry *tempent = ents[i];
                tempent->flags &= ~CACHE_BUSY;
            }
            alloced_ent_cnt = 0;
        }
    }

    for (i = 0 ; i < alloced_ent_cnt ; ++i)
    {
        free_cache_entry(ents[i]);
    }

    block_cache_unlock(&g_bcm.lock);

    return ((NULL != ent) ? get_cache_data(ent) : NULL);
}

static int put_block(__hdle dev, int block_offset)
{
    struct cache_entry *ent;

    if (dev == NULL)
    {
        __err("invalid device handle %p\n", dev);
        return (-EINVAL);
    }

    block_cache_lock(&g_bcm.lock);

    ent = lookup_cache_entry(dev, block_offset);
    if (NULL != ent)
    {
        RT_ASSERT(ent->ref > 0);
        ent->ref--;

        cache_list_remove_from_list(&g_bcm.list_lockdown, ent);
        if (0 == ent->ref)
        {
            cache_list_add_to_head(&g_bcm.list_ready, ent);
        }
        else
        {
            cache_list_add_to_head(&g_bcm.list_lockdown, ent);
        }
    }

    block_cache_unlock(&g_bcm.lock);

    return 0;
}

static int read_phys_blocks(__hdle dev, int block_offset, void *pBuffer, unsigned int nBlockCount, int bsize)
{
    int error = 0;

    if (dev == NULL)
    {
        __err("invalid device handle %p\n", dev);
        return (-EINVAL);
    }

    error = sdmmc_dev_phy_read(pBuffer, (__u32)block_offset, (__u32)nBlockCount, dev);

    if (error > 0)
    {
        uint32_t i;
        block_cache_lock(&g_bcm.lock);
        for (i = 0 ; i < nBlockCount ; ++i)
        {
            struct cache_entry *ent = lookup_cache_entry(dev, block_offset + i);
            if (ent != NULL)
            {
                memcpy(((uint8_t *)pBuffer) + i * bsize, get_cache_data(ent), bsize);

                if (ent->flags & CACHE_DIRTY)
                {
                    ent->flags &= ~CACHE_DIRTY;
                }
            }
        }
        block_cache_unlock(&g_bcm.lock);
    }

    return error;
}

int sdmmc_cache_read(void *pBuffer, int block_offset, int nBlockCount, int bsize, __hdle dev)
{
    int error = 0;
    int i;

    if (nBlockCount * bsize > CONFIG_SDMMC_CACHE_DIRECTLY_SIZE)
    {
        error = -1;
        if (read_phys_blocks(dev, block_offset, pBuffer, nBlockCount, bsize) == nBlockCount)
        {
            error =  0;
        }
        return error;
    }

    for (i = 0 ; i < nBlockCount ; ++i)
    {
        struct cache_entry *ent = NULL;
        void *pData = NULL;

        block_cache_lock(&g_bcm.lock);

        ent = lookup_cache_entry(dev, block_offset + i);
        if (ent)
        {
            pData = get_cache_data(ent);
            memcpy(((char *)pBuffer) + i * bsize, pData, bsize);
            if ((ent->flags & CACHE_DIRTY) == 0)
            {
                if (0 == ent->ref)
                {
                    cache_list_remove_from_list(&g_bcm.list_ready, ent);
                    cache_list_add_to_head(&g_bcm.list_ready, ent);
                }
                else
                {
                    cache_list_remove_from_list(&g_bcm.list_lockdown, ent);
                    cache_list_add_to_head(&g_bcm.list_lockdown, ent);
                }
            }
            block_cache_unlock(&g_bcm.lock);
            continue;
        }
        block_cache_unlock(&g_bcm.lock);

        pData = get_block(dev, block_offset + i, bsize);

        if (NULL != pData)
        {
            memcpy(((char *)pBuffer) + i * bsize, pData, bsize);
            put_block(dev, block_offset + i);
        }
        else
        {
            __err("failed to get block(%d) data\n", block_offset + i);
            error = -EIO;
            break;
        }
    }
    return (error);
}

static int write_phys_blocks(__hdle dev, int block_offset, const void *pBuffer, unsigned  int nBlockCount, int bsize)
{
    int error;

    if (dev == NULL)
    {
        __log("invalid device handle %p\n", dev);
        return (-EINVAL);
    }

    error = sdmmc_dev_phy_write((void *)pBuffer, (__u32)block_offset, (__u32)nBlockCount, dev);

    if (error > 0)
    {
        uint32_t i;
        block_cache_lock(&g_bcm.lock);
        for (i = 0 ; i < nBlockCount ; ++i)
        {
            struct cache_entry *ent = lookup_cache_entry(dev, block_offset + i);
            if (ent != NULL)
            {
                memcpy(get_cache_data(ent), ((uint8_t *)pBuffer) + i * bsize, bsize);

                if (ent->flags & CACHE_DIRTY)
                {
                    ent->flags &= ~CACHE_DIRTY;
                }
            }
        }
        block_cache_unlock(&g_bcm.lock);
    }
    return error;
}

int sdmmc_cache_write(void *pBuffer, int block_offset, int nBlockCount, int bsize, __hdle dev)
{
    int error = 0;
    int i;

    if (nBlockCount * bsize > CONFIG_SDMMC_CACHE_DIRECTLY_SIZE)
    {
        error = -1;
        if (write_phys_blocks(dev, block_offset, pBuffer, nBlockCount, bsize) == nBlockCount)
        {
            error = 0;
        }
        /* To ensure write dirty data as soon as possible */
        /* sem_post(&flusher_sem); */
        return error;
    }

    block_cache_lock(&g_bcm.lock);

    for (i = 0 ; i < nBlockCount ; ++i)
    {
        struct cache_entry *ent;

        /* to avoid one entry repeatedly write and lose data when power off  */
        ent = lookup_cache_entry(dev, block_offset + i);
        if (ent)
        {
            memcpy(get_cache_data(ent), ((char *)pBuffer) + i * bsize, bsize);
            if ((ent->flags & CACHE_DIRTY) == 0)
            {
                ent->flags |= CACHE_DIRTY;
            }
            continue;
        }

        ent = do_get_empty_cache_entry(dev, block_offset + i, bsize);
        if (NULL != ent)
        {
            /* block_cache_unlock(&g_bcm.lock); */
            memcpy(get_cache_data(ent), ((char *)pBuffer) + i * bsize, bsize);
            /* block_cache_lock(&g_bcm.lock); */

            if ((ent->flags & CACHE_DIRTY) == 0)
            {
                ent->flags |= CACHE_DIRTY;
            }

            RT_ASSERT(ent->ref > 0);
            ent->ref--;
            cache_list_remove_from_list(&g_bcm.list_lockdown, ent);
            if (0 == ent->ref)
            {
                cache_list_add_to_head(&g_bcm.list_ready, ent);
            }
            else
            {
                cache_list_add_to_head(&g_bcm.list_lockdown, ent);
            }
        }
        else
        {
            __err("failed to get block(%d) data\n", block_offset + i);
            error = -EIO;
            break;
        }
    }
    block_cache_unlock(&g_bcm.lock);
    return (error);
}

int flush_block_cache(int wait)
{
    struct cache_entry *ents[WRITEBACK_BLOCKS_MAX];
    struct cache_entry *ent;
    int i = 0;
    struct list_head *pos;

retry_write_normal_list:
    block_cache_lock(&g_bcm.lock);

    list_for_each(pos, &g_bcm.list_ready)
    {
        ent = list_entry(pos, struct cache_entry, list);
        RT_ASSERT(0 == ent->ref);

        if (ent->flags & CACHE_BUSY)
        {
            continue;
        }

        if ((ent->flags & CACHE_DIRTY) == 0)
        {
            continue;
        }
        ent->flags |= CACHE_BUSY;

        if (i >= WRITEBACK_BLOCKS_MAX)
        {
            int j;

            qsort(ents, WRITEBACK_BLOCKS_MAX, sizeof(struct cache_entry *), cache_entry_cmp);
            block_cache_unlock(&g_bcm.lock);
            flush_cache_entry_list(ents, WRITEBACK_BLOCKS_MAX);
            block_cache_lock(&g_bcm.lock);
            for (j = 0 ; j < WRITEBACK_BLOCKS_MAX ; ++j)
            {
                ents[j]->flags &= ~CACHE_BUSY;
            }

            i = 0;
        }
        ents[i++] = ent;
    }
    if (i > 0)
    {
        int j;

        qsort(ents, i, sizeof(struct cache_entry *), cache_entry_cmp);
        block_cache_unlock(&g_bcm.lock);
        flush_cache_entry_list(ents, i);
        block_cache_lock(&g_bcm.lock);

        for (j = 0 ; j < i ; ++j)
        {
            ents[j]->flags &= ~CACHE_BUSY;
        }
    }
    block_cache_unlock(&g_bcm.lock);

    block_cache_lock(&g_bcm.lock);

    if (wait)
    {
        if (list_empty(&g_bcm.list_lockdown))
        {
            block_cache_unlock(&g_bcm.lock);
            return 0;
        }
        else
        {
            block_cache_unlock(&g_bcm.lock);
            msleep(100);
            goto retry_write_normal_list;
        }
    }

    block_cache_unlock(&g_bcm.lock);
    return 0;
}

static int get_cache_order(int block_size)
{
    int order;

    for (order = 0; order < MAX_BLOCK_SIZE_ORDERS; order++)
    {
        if ((1 << order) == block_size)
        {
            break;
        }
    }
    if (order == MAX_BLOCK_SIZE_ORDERS)
    {
        __err("invalid block size: %d\n", block_size);
        return (0);
    }
    return order;
}

static int init_heap(struct cache_heap *psHeap)
{
    uint32_t  heap_buf_size;
    char *temp_ptr;
    int  nCnt = 0;

    psHeap->byte_size = g_bcm.cache_heap_size * 1024;

    psHeap->heap_ptr = malloc(psHeap->byte_size);
    if (psHeap->heap_ptr == NULL)
    {
        __err("alloc memory for heap failed!");
        return -1;
    }

    temp_ptr = psHeap->heap_ptr;
    heap_buf_size = psHeap->byte_size;
    INIT_LIST_HEAD(&psHeap->free_ents_list);

    while (heap_buf_size >= sizeof(struct cache_entry) + psHeap->block_size)
    {
        struct cache_entry *ent = (struct cache_entry *)temp_ptr;
        list_add_tail(&ent->cache_heap_list, &psHeap->free_ents_list);

        temp_ptr     += sizeof(struct cache_entry) + psHeap->block_size;
        heap_buf_size -= sizeof(struct cache_entry) + psHeap->block_size;
        nCnt++;
    }
    psHeap->free_ents = nCnt;
    psHeap->total_ents = nCnt;
    return 0;
}

static void deinit_heap(void)
{
    int i;
    struct cache_heap *psHeap;
    for (i = 0; i < MAX_BLOCK_SIZE_ORDERS; i++)
    {
        psHeap = &g_CacheHeaps[i];
        if (psHeap->heap_ptr != NULL)
        {
            free(psHeap->heap_ptr);
            psHeap->heap_ptr = NULL;
        }
    }
    memset(g_CacheHeaps, 0, sizeof(g_CacheHeaps));
}

static int count_free_num_blocks(struct cache_heap *psHeap, int num)
{
    (void)num;
    return psHeap->free_ents;
}

static int alloc_cache_entrys(struct cache_entry **ents, int nCount, int block_size, int *isBlocked)
{
    int order = get_cache_order(block_size);
    struct cache_heap *psHeap = &g_CacheHeaps[order];
    struct cache_entry *ent;
    int free_blocks_cnt;
    int alloc_cnt = 0;
    int flag = 0;
    int error;
    int lowlevel;

    struct list_head *pos, *n;

    if (isBlocked)
    {
        *isBlocked = 0;
    }

    if (psHeap->heap_ptr == NULL)
    {
        psHeap->block_size  = block_size;
        init_heap(psHeap);
    }

    lowlevel = ((psHeap->total_ents / 2) > WRITEBACK_BLOCKS_MAX) ? (psHeap->total_ents / 2) : (WRITEBACK_BLOCKS_MAX);

again:
    free_blocks_cnt = count_free_num_blocks(psHeap, nCount);
    if (free_blocks_cnt < nCount)
    {
        error = release_cache_entrys();
        if (error <= 0)
        {
            sem_post(&flusher_sem);
            goto out;
        }

        if (isBlocked)
        {
            *isBlocked = 1;
        }

        free_blocks_cnt = count_free_num_blocks(psHeap, nCount);
        if (free_blocks_cnt <= lowlevel)
        {
            sem_post(&flusher_sem);
        }
    }

    if (list_empty(&psHeap->free_ents_list))
    {
        goto again;
    }

    if (free_blocks_cnt < nCount)
    {
        goto again;
    }

    alloc_cnt = 0;
    list_for_each_safe(pos, n, &psHeap->free_ents_list)
    {
        ent = list_entry(pos, struct cache_entry, cache_heap_list);
        ent->dev = NULL;
        ent->block_offset = -1;
        ent->ref = 0;
        ent->flags = block_size | CACHE_USED;
        list_del_init(&ent->cache_heap_list);
        psHeap->free_ents--;
        INIT_LIST_HEAD(&ent->list);
        ent->hash_list_next_element = NULL;

        ents[alloc_cnt++] = ent;
        if (alloc_cnt >= nCount)
        {
            break;
        }
    }
out:
    return alloc_cnt;
}

static void free_cache_entry(struct cache_entry *ent)
{
    int block_size = CACHE_BLOCK_SIZE(ent);
    int order = get_cache_order(block_size);
    struct cache_heap *psHeap = &g_CacheHeaps[order];

    ent->flags &= CACHE_SIZE_MASK;
    list_add(&ent->cache_heap_list, &psHeap->free_ents_list);
    psHeap->free_ents++;
}

static void *cache_writeback_thread(void *pData)
{
    struct cache_entry *ent;
    struct list_head *p;
    int freeblocks = 0;

    int i = 0;
    int order = get_cache_order(512);
    int writeback_undirty_flag = 0;
    struct cache_heap *psHeap = &g_CacheHeaps[order];
    struct timespec abs_timeout;
    struct cache_entry *ents[WRITEBACK_BLOCKS_MAX];
    __hdle dev;
    int lowlevel;

    lowlevel = ((psHeap->total_ents / 4) > WRITEBACK_BLOCKS_MAX) ? (psHeap->total_ents / 4) : (WRITEBACK_BLOCKS_MAX);

    for (;;)
    {
        dev = NULL;
        if (i < WRITEBACK_BLOCKS_MAX)
        {
            clock_gettime(CLOCK_REALTIME, &abs_timeout);
            long long tv_nsec = abs_timeout.tv_nsec + MS_TO_NS(10000);

            abs_timeout.tv_sec += (long)(0xffffffff & (tv_nsec / (1000 * 1000 * 1000LL)));
            abs_timeout.tv_nsec = (long)(0xffffffff & (tv_nsec % (1000 * 1000 * 1000LL)));

            sem_timedwait(&flusher_sem, &abs_timeout);
        }
        i = 0;

        block_cache_lock(&g_bcm.lock);

        freeblocks = count_free_num_blocks(psHeap, lowlevel);
        if (freeblocks < lowlevel)
        {
            writeback_undirty_flag = 1;
        }
        else
        {
            writeback_undirty_flag = 0;
        }

        list_for_each_prev(p, &g_bcm.list_ready)
        {
            ent = list_entry(p, struct cache_entry, list);
            RT_ASSERT(0 == ent->ref);

            if (ent->flags & CACHE_BUSY)
            {
                continue;
            }

            if ((ent->flags & CACHE_DIRTY) == 0)
            {
                continue;
            }
            if (dev != NULL && ent->dev != dev)
            {
                continue;
            }
            dev = ent->dev;

            ent->flags |= CACHE_BUSY;
            ents[i++] = ent;
            if (i >= WRITEBACK_BLOCKS_MAX)
            {
                break;
            }
        }

        if (i < WRITEBACK_BLOCKS_MAX)
        {
            list_for_each_prev(p, &g_bcm.list_ready)
            {
                ent = list_entry(p, struct cache_entry, list);
                RT_ASSERT(0 == ent->ref);

                if (ent->flags & CACHE_BUSY)
                {
                    continue;
                }

                if ((ent->flags & CACHE_DIRTY) == 0 && writeback_undirty_flag == 0)
                {
                    continue;
                }
                if (dev != NULL && ent->dev != dev)
                {
                    continue;
                }
                dev = ent->dev;

                ent->flags |= CACHE_BUSY;
                ents[i++] = ent;
                if (i >= WRITEBACK_BLOCKS_MAX)
                {
                    break;
                }
            }
        }

        if (i > 0)
        {
            int j;

            qsort(ents, i, sizeof(struct cache_entry *), cache_entry_cmp);

            block_cache_unlock(&g_bcm.lock);
            if (flush_cache_entry_list(ents, i) < 0)
            {
                block_cache_lock(&g_bcm.lock);
                for (j = 0 ; j < i ; ++j)
                {
                    ents[j]->flags &= ~CACHE_BUSY;
                }
                block_cache_unlock(&g_bcm.lock);
                msleep(1000);
                continue;
            }
            block_cache_lock(&g_bcm.lock);
            for (j = 0 ; j < i ; ++j)
            {
                ents[j]->flags &= ~CACHE_BUSY;

                cache_list_remove_from_list(&g_bcm.list_ready, ents[j]);
                cache_hash_remove(g_bcm.hash_table, ents[j]);
                free_cache_entry(ents[j]);
            }
        }

        block_cache_unlock(&g_bcm.lock);
    }
    return NULL;
}

int discard_block_cache(void)
{
    struct list_head *pos;
    struct list_head *n;
    struct cache_entry *ent;

    if (g_bcm.hash_table)
    {
        block_cache_lock(&g_bcm.lock);

        list_for_each_safe(pos, n, &g_bcm.list_ready)
        {
            ent = list_entry(pos, struct cache_entry, list);
            ent->flags &= ~CACHE_BUSY;
            list_del_init(&ent->list);
            INIT_LIST_HEAD(&ent->list);
            cache_hash_remove(g_bcm.hash_table, ent);
            free_cache_entry(ent);
        }

        list_for_each_safe(pos, n, &g_bcm.list_lockdown)
        {
            ent = list_entry(pos, struct cache_entry, list);
            ent->flags &= ~CACHE_BUSY;
            list_del_init(&ent->list);
            INIT_LIST_HEAD(&ent->list);
            cache_hash_remove(g_bcm.hash_table, ent);
            free_cache_entry(ent);
        }

        cache_hash_deinit(g_bcm.hash_table);
        g_bcm.hash_table = NULL;
        /* deinit_heap(); */
        block_cache_unlock(&g_bcm.lock);
        block_cache_destroy_lock(&g_bcm.lock);
        g_bcm.flags = BCM_STATE_DEINIT;
    }
    return 0;
}

int block_cache_manager_deinit(void)
{
    struct cache_entry *ent;
    struct list_head *pos;

    flush_block_cache(1);

    list_for_each_prev(pos, &g_bcm.list_lockdown)
    {
        ent = list_entry(pos, struct cache_entry, list);
    }

    cache_hash_deinit(g_bcm.hash_table);
    g_bcm.hash_table = NULL;
    deinit_heap();
    g_bcm.flags = BCM_STATE_DEINIT;
    return 0;
}

int block_cache_manager_init(int cache_heap_size)
{
    memset(&g_bcm, 0, sizeof(struct block_cache_manager));

    g_bcm.hash_table = cache_hash_init(HASHTABLE_MAX,
                                       offsetof(struct cache_entry, hash_list_next_element),
                                       cache_entry_compare,
                                       cache_entry_hash);

    block_cache_init_lock(&g_bcm.lock);
    g_bcm.cache_heap_size = cache_heap_size;

    INIT_LIST_HEAD(&g_bcm.list_ready);
    INIT_LIST_HEAD(&g_bcm.list_lockdown);

    if (pth_flusher == 0)
    {
        pthread_attr_t attr;
        struct sched_param param;

        memset(&param, 0, sizeof(struct sched_param));
        memset(&attr, 0, sizeof(pthread_attr_t));

        param.sched_priority = CONFIG_SDMMC_CACHE_WRITEBACK_THREAD_PRIO;
        pthread_attr_init(&attr);
        pthread_attr_setschedparam(&attr, &param);

        sem_init(&flusher_sem, 0, 0);

        pthread_create(&pth_flusher, &attr, cache_writeback_thread, NULL);
        pthread_setname_np(pth_flusher, "cache-flusher");
    }

    g_bcm.flags = BCM_STATE_INITED;

    return 0;
}

#if defined(RT_USING_FINSH) && defined(CONFIG_SDMMC_CACHE_INFO_CMD)
static int cmd_sdmmc_cache(int argc, char **argv)
{
    struct list_head *p;
    struct cache_entry *ent;
    int num_busy = 0;
    int num_dirty = 0;
    int num_total = 0;
    int i = 0;

    if (g_bcm.flags != BCM_STATE_INITED)
    {
        printf("global block cache manager has no inited!!\n");
        return 0;
    }

    block_cache_lock(&g_bcm.lock);

    list_for_each_prev(p, &g_bcm.list_lockdown)
    {
        ent = list_entry(p, struct cache_entry, list);
        num_total++;
    }
    printf("list_lockdown: total %d\n", num_total);

    num_busy = 0;
    num_dirty = 0;
    num_total = 0;

    list_for_each_prev(p, &g_bcm.list_ready)
    {
        ent = list_entry(p, struct cache_entry, list);

        if (ent->flags & CACHE_BUSY)
        {
            num_busy++;
        }
        if (ent->flags & CACHE_DIRTY)
        {
            num_dirty++;
        }
        num_total++;
    }
    printf("list_ready: total %d, dirty %d, buys %d\n", num_total, num_dirty, num_busy);

    printf("\nheap information:\n");
    for (i = 0; i < sizeof(g_CacheHeaps) / sizeof(g_CacheHeaps[0]); i++)
    {
        if (g_CacheHeaps[i].heap_ptr == NULL)
        {
            continue;
        }
        printf("order = %d, bytes size = %ld (bytes), heap_ptr = %p, total_entry_num = %ld, free_entry_num = %d\n",
               i,
               g_CacheHeaps[i].byte_size,
               g_CacheHeaps[i].heap_ptr,
               g_CacheHeaps[i].total_ents,
               g_CacheHeaps[i].free_ents);
    }

    block_cache_unlock(&g_bcm.lock);
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_sdmmc_cache, __cmd_sdmmc_cache, dump sdmmc cache information);
#endif
