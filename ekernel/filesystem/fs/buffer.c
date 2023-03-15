/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
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
#include "fs.h"
#include "file.h"
#include "buffer_head.h"
#include "errno.h"
#include "page_pool.h"
#include "writeback.h"
#include "supports.h"
#include "fsys_debug.h"
#include <port.h>


struct buffer_head *bh_cachep;

extern atomic_t nr_async_pages;

void balance_dirty(void);
void refile_buffer(struct buffer_head *);
int osync_inode_buffers(struct inode *);
int osync_inode_data_buffers(struct inode *);

#define NR_SIZES 7
static int buffersize_index[65] =
{
    -1,  0,  1, -1,  2, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1, -1,
        4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        6
    };

#define BUFSIZE_INDEX(X) ((int) buffersize_index[(X)>>9])
#define MAX_BUF_PER_PAGE (PAGE_CACHE_SIZE / 512)
#define NR_RESERVED (10*MAX_BUF_PER_PAGE)
#define MAX_UNUSED_BUFFERS NR_RESERVED+20 /* don't ever have more than this 
                         number of unused buffer heads */

struct buffer_head bh_map[MAX_BUF_PER_PAGE * NR_TOTAL_PAGE];

/* Anti-deadlock ordering:
 *  lru_list_lock > hash_table_lock > free_list_lock > unused_list_lock
 */

#define BH_ENTRY(list) list_entry((list), struct buffer_head, b_inode_buffers)

/*
 * Hash table gook..
 */
static unsigned int bh_hash_mask;
static unsigned int bh_hash_shift;
static struct buffer_head **hash_table;
//static rwlock_t hash_table_lock = {0};//RW_LOCK_UNLOCKED;
static int bh_hash_entrys = 512;

static struct buffer_head *lru_list[NR_LIST];
static int nr_buffers_type[NR_LIST];
static unsigned long size_buffers_type[NR_LIST];

struct bh_free_head
{
    struct buffer_head *list;
};
static struct bh_free_head free_list[NR_SIZES];

static int grow_buffers(int size);
static void __refile_buffer(struct buffer_head *);

/* This is used by some architectures to estimate available memory. */
atomic_t buffermem_pages = ATOMIC_INIT(0);

/* Here is the parameter block for the bdflush process. If you add or
 * remove any of the parameters, make sure to update kernel/sysctl.c
 * and the documentation at linux/Documentation/sysctl/vm.txt.
 */

#define N_PARAM 9

/* The dummy values in this structure are left in there for compatibility
 * with old programs that play with the /proc entries.
 */
const union bdflush_param
{
    struct
    {
        int nfract; /* Percentage of buffer cache dirty to
                   activate bdflush */
        int dummy1; /* old "ndirty" */
        int dummy2; /* old "nrefill" */
        int dummy3; /* unused */
        int interval;   /* jiffies delay between kupdate flushes */
        int age_buffer; /* Time for normal buffer to age before we flush it */
        int nfract_sync;/* Percentage of buffer cache dirty to
                   activate bdflush synchronously */
        int dummy4; /* unused */
        int dummy5; /* unused */
    } b_un;
    unsigned int data[N_PARAM];
} bdf_prm = {{30, 64, 64, 256, 15 * HZ, 30 * HZ, 60, 0, 0}};

/* These are the min and max parameter values that we will allow to be assigned */
int bdflush_min[N_PARAM] = {  0,  10,    5,   25,  0,   1 * HZ,   0, 0, 0};
int bdflush_max[N_PARAM] = {100, 50000, 20000, 20000, 10000 * HZ, 6000 * HZ, 100, 0, 0};

void unlock_buffer(struct buffer_head *bh)
{
    clear_bit(BH_Wait_IO, &bh->b_state);
    clear_bit(BH_Lock, &bh->b_state);
    //  smp_mb__after_clear_bit();
    //  if (waitqueue_active(&bh->b_wait))
    //      wake_up(&bh->b_wait);
}

/*
 * Rewrote the wait-routines to use the "new" wait-queue functionality,
 * and getting rid of the cli-sti pairs. The wait-queue routines still
 * need cli-sti, but now it's just a couple of 386 instructions or so.
 *
 * Note that the real wait_on_buffer() is an inline function that checks
 * if 'b_wait' is set before calling this, so that the queues aren't set
 * up unnecessarily.
 */
void __wait_on_buffer(struct buffer_head *bh)
{
    //  struct task_struct *tsk = current;
    //  DECLARE_WAITQUEUE(wait, tsk);
    //
    //  get_bh(bh);
    //  add_wait_queue(&bh->b_wait, &wait);
    //  do {
    //      run_task_queue(&tq_disk);
    //      set_task_state(tsk, TASK_UNINTERRUPTIBLE);
    //      if (!buffer_locked(bh))
    //          break;
    //      schedule();
    //  } while (buffer_locked(bh));
    //  tsk->state = TASK_RUNNING;
    //  remove_wait_queue(&bh->b_wait, &wait);
    //  put_bh(bh);
}

/*
 * Default synchronous end-of-IO handler..  Just mark it up-to-date and
 * unlock the buffer. This is what ll_rw_block uses too.
 */
void end_buffer_io_sync(struct buffer_head *bh, int uptodate)
{
    mark_buffer_uptodate(bh, uptodate);
    unlock_buffer(bh);
    put_bh(bh);
}

/*
 * The buffers have been marked clean and locked.  Just submit the dang
 * things..
 */
static void write_locked_buffers(struct buffer_head **array, unsigned int count)
{
    fs_log_verbose("write_locked_buffers() enter(count:%d)...\n", count);
    do
    {
        struct buffer_head *bh = *array++;
        bh->b_end_io = end_buffer_io_sync;
        submit_bh(WRITE, bh);
    } while (--count);
}

/*
 * Write some buffers from the head of the dirty queue.
 *
 * This must be called with the LRU lock held, and will
 * return without it!
 */
#define NRSYNC (32)
static int write_some_buffers(struct super_block *sb)
{
    struct buffer_head *next;
    struct buffer_head *array[NRSYNC];
    unsigned int count;
    int nr;

    next = lru_list[BUF_DIRTY];
    nr = nr_buffers_type[BUF_DIRTY];
    count = 0;
    while (next && --nr >= 0)
    {
        struct buffer_head *bh = next;
        next = bh->b_next_free;

        if (sb && bh->b_sb != sb)
        {
            continue;
        }
        if (test_and_set_bit(BH_Lock, &bh->b_state))
        {
            continue;
        }
        if (atomic_set_buffer_clean(bh))
        {
            __refile_buffer(bh);
            get_bh(bh);
            array[count++] = bh;
            if (count < NRSYNC)
            {
                continue;
            }

            write_locked_buffers(array, count);
            if (sb)
            {
                esFSYS_pioctrl(sb->s_part, PART_IOC_USR_FLUSHCACHE, 0, NULL);
            }
            return -EAGAIN;
        }
        unlock_buffer(bh);
        __refile_buffer(bh);
    }

    if (count)
    {
        write_locked_buffers(array, count);
        if (sb)
        {
            esFSYS_pioctrl(sb->s_part, PART_IOC_USR_FLUSHCACHE, 0, NULL);
        }
    }
    return 0;
}

/*
 * Write out all buffers on the dirty list.
 */
static void write_unlocked_buffers(struct super_block *sb)
{
    int needw = 1;

    while (needw)
    {
        needw = write_some_buffers(sb);
    }
}

/*
 * Wait for a buffer on the proper list.
 *
 * This must be called with the LRU lock held, and
 * will return with it released.
 */
static int wait_for_buffers(struct super_block *sb, int index, int refile)
{
    struct buffer_head *next;
    int nr;

    next = lru_list[index];
    nr = nr_buffers_type[index];
    while (next && --nr >= 0)
    {
        struct buffer_head *bh = next;
        next = bh->b_next_free;

        if (!buffer_locked(bh))
        {
            if (refile)
            {
                __refile_buffer(bh);
            }
            continue;
        }
        if (sb && bh->b_sb != sb)
        {
            continue;
        }

        //      get_bh(bh);
        //      wait_on_buffer (bh);
        //      put_bh(bh);
        return -EAGAIN;
    }
    return 0;
}

static   void wait_for_some_buffers(struct super_block *sb)
{
    wait_for_buffers(sb, BUF_LOCKED, 1);
}

static int wait_for_locked_buffers(struct super_block *sb, int index, int refile)
{
    wait_for_buffers(sb, index, refile);
    return 0;
}

/* Call sync_buffers with wait!=0 to ensure that the call does not
 * return until all buffer writes have completed.  Sync() may return
 * before the writes have finished; fsync() may not.
 */

/* Godamity-damn.  Some buffers (bitmaps for filesystems)
 * spontaneously dirty themselves without ever brelse being called.
 * We will ultimately want to put these in a separate list, but for
 * now we search all of the lists for dirty buffers.
 */
int sync_buffers(struct super_block *sb, int wait)
{
    int err = 0;

    /* One pass for no-wait, three for wait:
     * 0) write out all dirty, unlocked buffers;
     * 1) wait for all dirty locked buffers;
     * 2) write out all dirty, unlocked buffers;
     * 2) wait for completion by waiting for all buffers to unlock.
     */
    write_unlocked_buffers(sb);
    if (wait)
    {
        err = wait_for_locked_buffers(sb, BUF_DIRTY, 0);
        write_unlocked_buffers(sb);
        err |= wait_for_locked_buffers(sb, BUF_LOCKED, 1);
    }
    return err;
}

int fsync_dev(struct super_block *sb)
{
    struct writeback_control wbc =
    {
        .sync_mode  = WB_SYNC_NONE,
        .older_than_this = 0,
        .nr_to_write    = 0,
        .nonblocking    = 1,
        .for_kupdate    = 1,
        .range_cyclic   = 1,
    };

    sync_buffers(sb, 0);

    sync_part_inodes(sb, &wbc);
    sync_supers();

    sync_buffers(sb, 1);

    return 0;
}

/*
 *  filp may be NULL if called via the msync of a vma.
 */

__s32 file_fsync(struct file *filp, struct dentry *dentry, __s32 datasync)
{
    struct inode *inode = dentry->d_inode;
    struct super_block *sb = inode->i_sb;

    /* sync the inode to buffers */
    write_inode_now(inode, 0);

    /* sync the superblock to buffers */
    if (sb->s_op && sb->s_op->write_super)
    {
        sb->s_op->write_super(sb);
    }

    /* .. finally sync the buffers to disk */
    sync_buffers(sb, 1);

    return 0;
}

/* After several hours of tedious analysis, the following hash
 * function won.  Do not mess with it... -DaveM
 */
#define _hashfn(dev,block)  \
    ((((dev)<<(bh_hash_shift - 6)) ^ ((dev)<<(bh_hash_shift - 9))) ^ \
     (((block)<<(bh_hash_shift - 6)) ^ ((block) >> 13) ^ \
      ((block) << (bh_hash_shift - 12))))
#define hash(dev,block) hash_table[(_hashfn((uintptr_t) (dev),block) & bh_hash_mask)]

static   void __hash_link(struct buffer_head *bh, struct buffer_head **head)
{
    if ((bh->b_next = *head) != NULL)
    {
        bh->b_next->b_pprev = &bh->b_next;
    }
    *head = bh;
    bh->b_pprev = head;
}

static   void __hash_unlink(struct buffer_head *bh)
{
    if (bh->b_pprev)
    {
        if (bh->b_next)
        {
            bh->b_next->b_pprev = bh->b_pprev;
        }
        *(bh->b_pprev) = bh->b_next;
        bh->b_pprev = NULL;
    }
}

static void __insert_into_lru_list(struct buffer_head *bh, int blist)
{
    struct buffer_head **bhp = &lru_list[blist];

    if (!*bhp)
    {
        *bhp = bh;
        bh->b_prev_free = bh;
    }
    bh->b_next_free = *bhp;
    bh->b_prev_free = (*bhp)->b_prev_free;
    (*bhp)->b_prev_free->b_next_free = bh;
    (*bhp)->b_prev_free = bh;
    nr_buffers_type[blist]++;
    size_buffers_type[blist] += bh->b_size;
}

static void __remove_from_lru_list(struct buffer_head *bh, int blist)
{
    if (bh->b_prev_free || bh->b_next_free)
    {
        bh->b_prev_free->b_next_free = bh->b_next_free;
        bh->b_next_free->b_prev_free = bh->b_prev_free;
        if (lru_list[blist] == bh)
        {
            lru_list[blist] = bh->b_next_free;
        }
        if (lru_list[blist] == bh)
        {
            lru_list[blist] = NULL;
        }
        bh->b_next_free = bh->b_prev_free = NULL;
        nr_buffers_type[blist]--;
        size_buffers_type[blist] -= bh->b_size;
    }
}

static void __remove_from_free_list(struct buffer_head *bh, int index)
{
    if (bh->b_next_free == bh)
    {
        free_list[index].list = NULL;
    }
    else
    {
        bh->b_prev_free->b_next_free = bh->b_next_free;
        bh->b_next_free->b_prev_free = bh->b_prev_free;
        if (free_list[index].list == bh)
        {
            free_list[index].list = bh->b_next_free;
        }
    }
    bh->b_next_free = bh->b_prev_free = NULL;
}

/* must be called with both the hash_table_lock and the lru_list_lock
   held */
static void __remove_from_queues(struct buffer_head *bh)
{
    __hash_unlink(bh);
    __remove_from_lru_list(bh, bh->b_list);
}

static void __insert_into_queues(struct buffer_head *bh)
{
    struct buffer_head **head = &hash(bh->b_sb, bh->b_blocknr);

    __hash_link(bh, head);
    __insert_into_lru_list(bh, bh->b_list);
}

/* This function must only run if there are no other
 * references _anywhere_ to this buffer head.
 */
static void put_last_free(struct buffer_head *bh)
{
    struct bh_free_head *head = &free_list[BUFSIZE_INDEX(bh->b_size)];
    struct buffer_head **bhp = &head->list;

    bh->b_state = 0;

    bh->b_sb = (void *)B_FREE;
    if (!*bhp)
    {
        *bhp = bh;
        bh->b_prev_free = bh;
    }
    bh->b_next_free = *bhp;
    bh->b_prev_free = (*bhp)->b_prev_free;
    (*bhp)->b_prev_free->b_next_free = bh;
    (*bhp)->b_prev_free = bh;
}

/*
 * Why like this, I hear you say... The reason is race-conditions.
 * As we don't lock buffers (unless we are reading them, that is),
 * something might happen to it while we sleep (ie a read-error
 * will force it bad). This shouldn't really happen currently, but
 * the code is ready.
 */
struct buffer_head *get_hash_table(struct super_block *sb, int block, int size)
{
    struct buffer_head *bh = hash(sb, block);

    for (; bh; bh = bh->b_next)
        if (bh->b_blocknr == block  &&
            bh->b_size    == size   &&
            bh->b_sb     == sb)
        {
            break;
        }
    if (bh)
    {
        get_bh(bh);
    }

    return bh;
}

void buffer_insert_inode_queue(struct buffer_head *bh, struct inode *inode)
{
    if (bh->b_inode)
    {
        list_del(&bh->b_inode_buffers);
    }
    bh->b_inode = inode;
    list_add(&bh->b_inode_buffers, &inode->i_dirty_buffers);
}

void buffer_insert_inode_data_queue(struct buffer_head *bh, struct inode *inode)
{
    if (bh->b_inode)
    {
        list_del(&bh->b_inode_buffers);
    }
    bh->b_inode = inode;
    list_add(&bh->b_inode_buffers, &inode->i_dirty_data_buffers);
}

/* The caller must have the lru_list lock before calling the
   remove_inode_queue functions.  */
static void __remove_inode_queue(struct buffer_head *bh)
{
    bh->b_inode = NULL;
    list_del(&bh->b_inode_buffers);
}

static   void remove_inode_queue(struct buffer_head *bh)
{
    if (bh->b_inode)
    {
        __remove_inode_queue(bh);
    }
}

int inode_has_buffers(struct inode *inode)
{
    int ret;

    ret = !list_empty(&inode->i_dirty_buffers) || !list_empty(&inode->i_dirty_data_buffers);

    return ret;
}

/* If invalidate_buffers() will trash dirty buffers, it means some kind
   of fs corruption is going on. Trashing dirty data always imply losing
   information that was supposed to be just stored on the physical layer
   by the user.

   Thus invalidate_buffers in general usage is not allwowed to trash dirty
   buffers. For example ioctl(FLSBLKBUF) expects dirty data to be preserved.

   NOTE: In the case where the user removed a removable-media-disk even if
   there's still dirty data not synced on disk (due a bug in the device driver
   or due an error of the user), by not destroying the dirty buffers we could
   generate corruption also on the next media inserted, thus a parameter is
   necessary to handle this case in the most safe way possible (trying
   to not corrupt also the new disk inserted with the data belonging to
   the old now corrupted disk). Also for the ramdisk the natural thing
   to do in order to release the ramdisk memory is to destroy dirty buffers.

   These are two special cases. Normal usage imply the device driver
   to issue a sync on the device (without waiting I/O completion) and
   then an invalidate_buffers call that doesn't trash dirty buffers.

   For handling cache coherency with the blkdev pagecache the 'update' case
   is been introduced. It is needed to re-read from disk any pinned
   buffer. NOTE: re-reading from disk is destructive so we can do it only
   when we assume nobody is changing the buffercache under our I/O and when
   we think the disk contains more recent information than the buffercache.
   The update == 1 pass marks the buffers we need to update, the update == 2
   pass does the actual I/O. */
void invalidate_buffers(struct super_block *sb, int destroy_dirty_buffers, int update)
{
    int i, nlist, slept;
    struct buffer_head *bh, * bh_next;

retry:
    slept = 0;
    for (nlist = 0; nlist < NR_LIST; nlist++)
    {
        bh = lru_list[nlist];
        if (!bh)
        {
            continue;
        }
        for (i = nr_buffers_type[nlist]; i > 0 ; bh = bh_next, i--)
        {
            bh_next = bh->b_next_free;

            /* Another device? */
            if (bh->b_sb != sb)
            {
                continue;
            }
            /* Not hashed? */
            if (!bh->b_pprev)
            {
                continue;
            }
            if (buffer_locked(bh))
            {
                get_bh(bh);
                wait_on_buffer(bh);
                slept = 1;
                put_bh(bh);
            }

            /* All buffers in the lru lists are mapped */
            if (!buffer_mapped(bh))
            {
                BUG();
            }
            if (!atomic_read(&bh->b_count))
            {
                if (destroy_dirty_buffers || !buffer_dirty(bh))
                {
                    remove_inode_queue(bh);
                    __remove_from_queues(bh);
                    put_last_free(bh);
                }
            }
            else if (update)
            {
                if ((update == 2) ^ buffer_uptodate(bh)  &&
                    (update == 2) ^ buffer_req(bh))
                {
                    atomic_inc(&bh->b_count);

                    if (update == 2)
                    {
                        ll_rw_block(READ, 1, &bh);
                        wait_on_buffer(bh);
                    }
                    else
                    {
                        lock_buffer(bh);
                        clear_bit(BH_Uptodate, &bh->b_state);
                        clear_bit(BH_Req, &bh->b_state);
                        unlock_buffer(bh);
                    }

                    atomic_dec(&bh->b_count);
                    goto retry;
                }
            }

            if (slept)
            {
                goto out;
            }
        }
    }
out:
    if (slept)
    {
        goto retry;
    }
}

static void free_more_memory(void)
{
    balance_dirty();
}

void init_buffer(struct buffer_head *bh, bh_end_io_t *handler, void *private)
{
    bh->b_inode = NULL;
    bh->b_list = BUF_CLEAN;
    bh->b_end_io = handler;
    //  bh->b_private = private;
}

static void end_buffer_io_async(struct buffer_head *bh, int uptodate)
{
    struct page *page;
    struct buffer_head *tmp;

    mark_buffer_uptodate(bh, uptodate);

    /* This is a temporary buffer used for page I/O. */
    page = bh->b_page;

    /*
     * Be _very_ careful from here on. Bad things can happen if
     * two buffer heads end IO at almost the same time and both
     * decide that the page is now completely done.
     *
     * Async buffer_heads are here only as labels for IO, and get
     * thrown away once the IO for this page is complete.  IO is
     * deemed complete once all buffers have been visited
     * (b_count==0) and are now unlocked. We must make sure that
     * only the _last_ buffer that decrements its count is the one
     * that unlock the page..
     */
    mark_buffer_async(bh, 0);
    unlock_buffer(bh);
    tmp = bh->b_this_page;
    while (tmp != bh)
    {
        if (buffer_async(tmp) && buffer_locked(tmp))
        {
            goto still_busy;
        }
        tmp = tmp->b_this_page;
    }

    /*
     * if none of the buffers had errors then we can set the
     * page uptodate:
     */
    SetPageUptodate(page);

    /*
     * Run the hooks that have to be done when a page I/O has completed.
     */
    if (PageTestandClearDecrAfter(page))
    {
        atomic_dec(&nr_async_pages);
    }

    UnlockPage(page);

    return;

still_busy:
    return;
}

void set_buffer_async_io(struct buffer_head *bh)
{
    bh->b_end_io = end_buffer_io_async ;
    mark_buffer_async(bh, 1);
}

/*
 * Synchronise all the inode's dirty buffers to the disk.
 *
 * We have conflicting pressures: we want to make sure that all
 * initially dirty buffers get waited on, but that any subsequently
 * dirtied buffers don't.  After all, we don't want fsync to last
 * forever if somebody is actively writing to the file.
 *
 * Do this in two main stages: first we copy dirty buffers to a
 * temporary inode list, queueing the writes as we go.  Then we clean
 * up, waiting for those writes to complete.
 *
 * During this second stage, any subsequent updates to the file may end
 * up refiling the buffer on the original inode's dirty list again, so
 * there is a chance we will end up with a buffer queued for write but
 * not yet completed on that list.  So, as a final cleanup we go through
 * the osync code to catch these locked, dirty buffers without requeuing
 * any newly dirty buffers for write.
 */

int fsync_inode_buffers(struct inode *inode)
{
    struct buffer_head *bh;
    struct inode tmp;
    int err = 0, err2;

    INIT_LIST_HEAD(&tmp.i_dirty_buffers);

    while (!list_empty(&inode->i_dirty_buffers))
    {
        bh = BH_ENTRY(inode->i_dirty_buffers.next);
        list_del(&bh->b_inode_buffers);
        if (!buffer_dirty(bh) && !buffer_locked(bh))
        {
            bh->b_inode = NULL;
        }
        else
        {
            bh->b_inode = &tmp;
            list_add(&bh->b_inode_buffers, &tmp.i_dirty_buffers);
            if (buffer_dirty(bh))
            {
                get_bh(bh);
                ll_rw_block(WRITE, 1, &bh);
                brelse(bh);
            }
        }
    }

    while (!list_empty(&tmp.i_dirty_buffers))
    {
        bh = BH_ENTRY(tmp.i_dirty_buffers.prev);
        remove_inode_queue(bh);
        get_bh(bh);
        wait_on_buffer(bh);
        if (!buffer_uptodate(bh))
        {
            err = -EIO;
        }
        brelse(bh);
    }

    err2 = osync_inode_buffers(inode);

    if (err)
    {
        return err;
    }
    else
    {
        return err2;
    }
}

int fsync_inode_data_buffers(struct inode *inode)
{
    struct buffer_head *bh;
    struct inode tmp;
    int err = 0, err2;

    INIT_LIST_HEAD(&tmp.i_dirty_data_buffers);

    while (!list_empty(&inode->i_dirty_data_buffers))
    {
        bh = BH_ENTRY(inode->i_dirty_data_buffers.next);
        list_del(&bh->b_inode_buffers);
        if (!buffer_dirty(bh) && !buffer_locked(bh))
        {
            bh->b_inode = NULL;
        }
        else
        {
            bh->b_inode = &tmp;
            list_add(&bh->b_inode_buffers, &tmp.i_dirty_data_buffers);
            if (buffer_dirty(bh))
            {
                get_bh(bh);
                ll_rw_block(WRITE, 1, &bh);
                brelse(bh);
            }
        }
    }

    while (!list_empty(&tmp.i_dirty_data_buffers))
    {
        bh = BH_ENTRY(tmp.i_dirty_data_buffers.prev);
        remove_inode_queue(bh);
        get_bh(bh);
        wait_on_buffer(bh);
        if (!buffer_uptodate(bh))
        {
            err = -EIO;
        }
        brelse(bh);
    }

    err2 = osync_inode_data_buffers(inode);

    if (err)
    {
        return err;
    }
    else
    {
        return err2;
    }
}

/*
 * osync is designed to support O_SYNC io.  It waits synchronously for
 * all already-submitted IO to complete, but does not queue any new
 * writes to the disk.
 *
 * To do O_SYNC writes, just queue the buffer writes with ll_rw_block as
 * you dirty the buffers, and then use osync_inode_buffers to wait for
 * completion.  Any other dirty buffers which are not yet queued for
 * write will not be flushed to disk by the osync.
 */

int osync_inode_buffers(struct inode *inode)
{
    struct buffer_head *bh;
    struct list_head *list;
    int err = 0;

repeat:

    for (list = inode->i_dirty_buffers.prev;
         bh = BH_ENTRY(list), list != &inode->i_dirty_buffers;
         list = bh->b_inode_buffers.prev)
    {
        if (buffer_locked(bh))
        {
            get_bh(bh);
            wait_on_buffer(bh);
            if (!buffer_uptodate(bh))
            {
                err = -EIO;
            }
            brelse(bh);
            goto repeat;
        }
    }

    return err;
}

int osync_inode_data_buffers(struct inode *inode)
{
    struct buffer_head *bh;
    struct list_head *list;
    int err = 0;

repeat:

    for (list = inode->i_dirty_data_buffers.prev;
         bh = BH_ENTRY(list), list != &inode->i_dirty_data_buffers;
         list = bh->b_inode_buffers.prev)
    {
        if (buffer_locked(bh))
        {
            get_bh(bh);
            wait_on_buffer(bh);
            if (!buffer_uptodate(bh))
            {
                err = -EIO;
            }
            brelse(bh);
            goto repeat;
        }
    }

    return err;
}


/*
 * Invalidate any and all dirty buffers on a given inode.  We are
 * probably unmounting the fs, but that doesn't mean we have already
 * done a sync().  Just drop the buffers from the inode list.
 */
void invalidate_inode_buffers(struct inode *inode)
{
    struct list_head *entry;

    while ((entry = inode->i_dirty_buffers.next) != &inode->i_dirty_buffers)
    {
        remove_inode_queue(BH_ENTRY(entry));
    }
    while ((entry = inode->i_dirty_data_buffers.next) != &inode->i_dirty_data_buffers)
    {
        remove_inode_queue(BH_ENTRY(entry));
    }
}


/*
 * Ok, this is getblk, and it isn't very clear, again to hinder
 * race-conditions. Most of the code is seldom used, (ie repeating),
 * so it should be much more efficient than it looks.
 *
 * The algorithm is changed: hopefully better, and an elusive bug removed.
 *
 * 14.02.92: changed it to sync dirty buffers a bit: better performance
 * when the filesystem starts to get full of dirty blocks (I hope).
 */
struct buffer_head *__getblk(struct super_block *sb, int block, int size)
{
    struct buffer_head *bh;
    int isize;

repeat:
    bh = get_hash_table(sb, block, size);
    if (bh)
    {
        goto out;
    }

    isize = BUFSIZE_INDEX(size);
    bh = free_list[isize].list;
    if (bh)
    {
        __remove_from_free_list(bh, isize);
        atomic_set(&bh->b_count, 1);
    }

    /*
     * OK, FINALLY we know that this buffer is the only one of
     * its kind, we hold a reference (b_count>0), it is unlocked,
     * and it is clean.
     */
    if (bh)
    {
        init_buffer(bh, NULL, NULL);
        bh->b_sb = sb;
        bh->b_blocknr = block;
        bh->b_state = 1 << BH_Mapped;

        /* Insert the buffer into the regular lists */
        __insert_into_queues(bh);
out:
        //      touch_buffer(bh);
        mark_page_accessed(bh);

        return bh;
    }

    /*
     * If we block while refilling the free list, somebody may
     * create the buffer first ... search the hashes again.
     */
    if (!grow_buffers(size))
    {
        free_more_memory();
    }

    /* FIXME: getblk should fail if there's no enough memory */
    goto repeat;
}

/* -1 -> no need to flush
    0 -> async flush
    1 -> sync flush (wait for I/O completion) */
static int balance_dirty_state(void)
{
    unsigned long dirty, tot, hard_dirty_limit, soft_dirty_limit;

    dirty = size_buffers_type[BUF_DIRTY] >> PAGE_SHIFT;
    tot = nr_free_buffer_pages();

    dirty *= 100;
    soft_dirty_limit = tot * bdf_prm.b_un.nfract;
    hard_dirty_limit = tot * bdf_prm.b_un.nfract_sync;

    /* First, check for the "real" dirty limit. */
    if (dirty > soft_dirty_limit)
    {
        if (dirty > hard_dirty_limit)
        {
            return 1;
        }
        return 0;
    }

    return -1;
}

/*
 * if a new dirty buffer is created we need to balance bdflush.
 *
 * in the future we might want to make bdflush aware of different
 * pressures on different devices - thus the (currently unused)
 * 'dev' parameter.
 */
void balance_dirty(void)
{
    int state = balance_dirty_state();

    if (state < 0)
    {
        return;
    }

    /* If we're getting into imbalance, start write-out */
    write_some_buffers(NODEV);

    /*
     * And if we're _really_ out of balance, wait for
     * some of the dirty/locked buffers ourselves and
     * start bdflush.
     * This will throttle heavy writers.
     */
    if (state > 0)
    {
        wait_for_some_buffers(NODEV);
    }
}

void __mark_dirty(struct buffer_head *bh)
{
    bh->b_flushtime = jiffies + bdf_prm.b_un.age_buffer;
    fs_log_debug("__mark_dirty::bh->b_flushtime:%x\n", bh->b_flushtime);
    refile_buffer(bh);
}

/* atomic version, the user must call balance_dirty() by hand
   as soon as it become possible to block */
void __mark_buffer_dirty(struct buffer_head *bh)
{
    if (!atomic_set_buffer_dirty(bh))
    {
        __mark_dirty(bh);
    }
}

void mark_buffer_dirty(struct buffer_head *bh)
{
    if (!atomic_set_buffer_dirty(bh))
    {
        __mark_dirty(bh);
        balance_dirty();
    }
}

void mark_buffer_dirty_inode(struct buffer_head *bh, struct inode *inode)
{
    mark_buffer_dirty(bh);
}

/*
 * A buffer may need to be moved from one buffer list to another
 * (e.g. in case it is not shared any more). Handle this.
 */
static void __refile_buffer(struct buffer_head *bh)
{
    int dispose = BUF_CLEAN;
    if (buffer_locked(bh))
    {
        dispose = BUF_LOCKED;
    }
    if (buffer_dirty(bh))
    {
        dispose = BUF_DIRTY;
    }
    if (dispose != bh->b_list)
    {
        __remove_from_lru_list(bh, bh->b_list);
        bh->b_list = dispose;
        if (dispose == BUF_CLEAN)
        {
            remove_inode_queue(bh);
        }
        __insert_into_lru_list(bh, dispose);
    }
}

void refile_buffer(struct buffer_head *bh)
{
    __refile_buffer(bh);
}

/*
 * Release a buffer head
 */
void __brelse(struct buffer_head *buf)
{
    if (atomic_read(&buf->b_count))
    {
        put_bh(buf);
        return;
    }
    fs_log_error("VFS: brelse: Trying to free free buffer\n");
}

/*
 * bforget() is like brelse(), except it puts the buffer on the
 * free list if it can.. We can NOT free the buffer if:
 *  - there are other users of it
 *  - it is locked and thus can have active IO
 */
void __bforget(struct buffer_head *buf)
{
    /* grab the lru lock here to block bdflush. */
    if (!atomic_dec_and_test(&buf->b_count) || buffer_locked(buf))
    {
        goto in_use;
    }
    __hash_unlink(buf);
    remove_inode_queue(buf);
    __remove_from_lru_list(buf, buf->b_list);
    put_last_free(buf);
    return;

in_use:
    return;
}

/**
 *  bread() - reads a specified block and returns the bh
 *  @block: number of block
 *  @size: size (in bytes) to read
 *
 *  Reads a specified block, and returns buffer head that
 *  contains it. It returns NULL if the block was unreadable.
 */
struct buffer_head *__bread(struct super_block *sb, int block, int size)
{
    struct buffer_head *bh;

    debug_timerstart(getblktime);
    bh = __getblk(sb, block, size);
    debug_timerend(getblktime);

    if (buffer_uptodate(bh))
    {
        return bh;
    }

    debug_timerstart(bufdevicetime);
    ll_rw_block(READ, 1, &bh);
    debug_timerend(bufdevicetime);
    debug_timerprint("one bread----");
    debug_timershow(bufdevicetime);

    wait_on_buffer(bh);
    if (buffer_uptodate(bh))
    {
        return bh;
    }
    brelse(bh);
    return NULL;
}

/*
 * Note: the caller should wake up the buffer_wait list if needed.
 */
static   void __put_unused_buffer_head(struct buffer_head *bh)
{
    if (bh->b_inode)
    {
        BUG();
    }
    bh->b_blocknr = (unsigned long) - 1;
    bh->b_this_page = NULL;
    bh->b_next_free = NULL;
}

/*
 * Reserve NR_RESERVED buffer heads for async IO requests to avoid
 * no-buffer-head deadlock.  Return NULL on failure; waiting for
 * buffer heads is now handled in create_buffers().
 */
static struct buffer_head *get_unused_buffer_head(struct page *page, int pb_index)
{
    struct buffer_head *bh;

    bh = &bh_map[mem_map_index(page) * MAX_BUF_PER_PAGE + pb_index];

    return bh;
}

void set_bh_page(struct buffer_head *bh, struct page *page, unsigned long offset)
{
    bh->b_page = page;
    if (offset >= PAGE_SIZE)
    {
        BUG();
    }
    //  if (PageHighMem(page))
    //      /*
    //       * This catches illegal uses and preserves the offset:
    //       */
    //      bh->b_data = (char *)(0 + offset);
    //  else
    bh->b_data = page_address(page) + offset;
}

/*
 * Create the appropriate buffers when given a page for data area and
 * the size of each buffer.. Use the bh->b_this_page linked list to
 * follow the buffers created.  Return NULL if unable to create more
 * buffers.
 * The async flag is used to differentiate async IO (paging, swapping)
 * from ordinary buffer allocations, and only async requests are allowed
 * to sleep waiting for buffer heads.
 */
static struct buffer_head *create_buffers(struct page *page, unsigned long size, int async)
{
    struct buffer_head *bh, *head;
    int offset, buffer_index;

try_again:
    head = NULL;
    offset = PAGE_SIZE;
    buffer_index = 0;
    while ((offset -= size) >= 0)
    {
        bh = get_unused_buffer_head(page, buffer_index++);
        if (!bh)
        {
            goto no_grow;
        }

        bh->b_sb = (void *)B_FREE; /* Flag as unused */
        bh->b_this_page = head;
        head = bh;

        bh->b_state = 0;
        bh->b_next_free = NULL;
        bh->b_pprev = NULL;
        atomic_set(&bh->b_count, 0);
        bh->b_size = size;

        set_bh_page(bh, page, offset);

        bh->b_list = BUF_CLEAN;
        bh->b_end_io = NULL;
    }
    return head;
    /*
     * In case anything failed, we just free everything we got.
     */
no_grow:
    if (head)
    {
        do
        {
            bh = head;
            head = head->b_this_page;
            __put_unused_buffer_head(bh);
        } while (head);

        /* Wake up any waiters ... */
        //      wake_up(&buffer_wait);
    }

    /*
     * Return failure for non-async IO requests.  Async IO requests
     * are not allowed to fail, so we have to wait until buffer heads
     * become available.  But we don't want tasks sleeping with
     * partially complete buffers, so all were released above.
     */
    if (!async)
    {
        return NULL;
    }

    /* We're _really_ low on memory. Now we just
     * wait for old buffer heads to become free due to
     * finishing IO.  Since this is an async request and
     * the reserve list is empty, we're sure there are
     * async buffer heads in use.
     */
    //  run_task_queue(&tq_disk);

    free_more_memory();
    goto try_again;
}

/*
 * Try to increase the number of buffers available: the size argument
 * is used to determine what kind of buffers we want.
 */
static int grow_buffers(int size)
{
    struct page *page;
    struct buffer_head *bh, *tmp;
    struct buffer_head *insert_point;
    int isize;

    if ((size & 511) || (size > PAGE_SIZE))
    {
        fs_log_error("VFS: grow_buffers: size = %d\n", size);
        return 0;
    }

    debug_show_page_info("grow buffer->");
    page = alloc_page(GFP_NOFS);
    if (!page)
    {
        goto out;
    }
    LockPage(page);
    bh = create_buffers(page, size, 0);
    if (!bh)
    {
        goto no_buffer_head;
    }

    isize = BUFSIZE_INDEX(size);

    insert_point = free_list[isize].list;
    tmp = bh;
    while (1)
    {
        if (insert_point)
        {
            tmp->b_next_free = insert_point->b_next_free;
            tmp->b_prev_free = insert_point;
            insert_point->b_next_free->b_prev_free = tmp;
            insert_point->b_next_free = tmp;
        }
        else
        {
            tmp->b_prev_free = tmp;
            tmp->b_next_free = tmp;
        }
        insert_point = tmp;
        if (tmp->b_this_page)
        {
            tmp = tmp->b_this_page;
        }
        else
        {
            break;
        }
    }
    tmp->b_this_page = bh;
    free_list[isize].list = bh;

    page->buffers = bh;
    page->flags &= ~(1 << PG_referenced);
    lru_cache_add(page);
    UnlockPage(page);
    atomic_inc(&buffermem_pages);
    return 1;

no_buffer_head:
    UnlockPage(page);
    page_cache_release(page);
out:
    return 0;
}

static int sync_page_buffers(struct buffer_head *bh, unsigned int gfp_mask)
{
    struct buffer_head *p = bh;
    int tryagain = 1;

    do
    {
        if (buffer_dirty(p) || buffer_locked(p))
        {
            if (test_and_set_bit(BH_Wait_IO, &p->b_state))
            {
                if (buffer_dirty(p))
                {
                    ll_rw_block(WRITE, 1, &p);
                    tryagain = 0;
                }
                else if (buffer_locked(p))
                {
                    if (gfp_mask & __GFP_WAIT)
                    {
                        wait_on_buffer(p);
                        tryagain = 1;
                    }
                    else
                    {
                        tryagain = 0;
                    }
                }
            }
            else
            {
                tryagain = 0;
            }
        }
        p = p->b_this_page;
    } while (p != bh);

    return tryagain;
}

/*
 * Can the buffer be thrown out?
 */
#define BUFFER_BUSY_BITS    ((1<<BH_Dirty) | (1<<BH_Lock))
#define buffer_busy(bh)     (atomic_read(&(bh)->b_count) | ((bh)->b_state & BUFFER_BUSY_BITS))

/*
 * try_to_free_buffers() checks if all the buffers on this particular page
 * are unused, and free's the page if so.
 *
 * Wake up bdflush() if this fails - if we're running low on memory due
 * to dirty buffers, we need to flush them out as quickly as possible.
 *
 * NOTE: There are quite a number of ways that threads of control can
 *       obtain a reference to a buffer head within a page.  So we must
 *   lock out all of these paths to cleanly toss the page.
 */
int try_to_free_buffers(struct page *page, unsigned int gfp_mask)
{
    struct buffer_head *tmp, * bh = page->buffers;
    int index = BUFSIZE_INDEX(bh->b_size);

cleaned_buffers_try_again:
    tmp = bh;
    do
    {
        if (buffer_busy(tmp))
        {
            goto busy_buffer_page;
        }
        tmp = tmp->b_this_page;
    } while (tmp != bh);

    tmp = bh;
    do
    {
        struct buffer_head *p = tmp;
        tmp = tmp->b_this_page;

        /* The buffer can be either on the regular
         * queues or on the free list..
         */
        if (p->b_sb != (void *)B_FREE)
        {
            remove_inode_queue(p);
            __remove_from_queues(p);
        }
        else
        {
            __remove_from_free_list(p, index);
        }
        __put_unused_buffer_head(p);
    } while (tmp != bh);

    /* Wake up anyone waiting for buffer heads */
    //  wake_up(&buffer_wait);

    /* And free the page */
    page->buffers = NULL;
    //  page_cache_release(page);
    return 1;

busy_buffer_page:
    /* Uhhuh, start writeback so that we don't end up with all dirty pages */
    if (gfp_mask & __GFP_IO)
    {
        if (sync_page_buffers(bh, gfp_mask))
        {
            /* no IO or waiting next time */
            gfp_mask = 0;
            goto cleaned_buffers_try_again;
        }
    }

    return 0;
}

/* ===================== Init ======================= */

/*
 * allocate the hash table and init the free list
 * Use gfp() for the hash table to decrease TLB misses, use
 * SLAB cache for buffer heads.
 */
int buffer_init(void)
{
    int  i;

    /* The buffer cache hash table is less important these days,
     * trim it a bit.
     */
    //  mempages >>= 14;
    //
    //  mempages *= sizeof(struct buffer_head *);
    //
    //  for (order = 0; (1 << order) < mempages; order++)
    //      ;

    /* try to allocate something until we get it or we're asking
       for something that is really too small */

    //  do {
    //      unsigned long tmp;
    //
    //      nr_hash = (PAGE_SIZE << order) / sizeof(struct buffer_head *);
    //      bh_hash_mask = (nr_hash - 1);
    //
    //      tmp = nr_hash;
    //      bh_hash_shift = 0;
    //      while((tmp >>= 1UL) != 0UL)
    //          bh_hash_shift++;
    //
    //      hash_table = (struct buffer_head **)
    //          __get_free_pages(GFP_ATOMIC, order);
    //  } while (hash_table == NULL && --order > 0);

    hash_table = alloc_large_system_hash("bcache hashtable",
                                         sizeof(struct buffer_head *),
                                         bh_hash_entrys,
                                         0, 0,
                                         &bh_hash_shift,
                                         &bh_hash_mask,
                                         0);

    if (!hash_table)
    {
        fs_log_critical("Failed to allocate buffer hash table\n");
    }

    /* Setup hash chains. */
    for (i = 0; i < bh_hash_entrys; i++)
    {
        hash_table[i] = NULL;
    }

    /* Setup free lists. */
    for (i = 0; i < NR_SIZES; i++)
    {
        free_list[i].list = NULL;
    }

    /* Setup lru lists. */
    for (i = 0; i < NR_LIST; i++)
    {
        lru_list[i] = NULL;
    }

    return 0;
}

void block_sync_page(struct page *page)
{
    //  run_task_queue(&tq_disk);
    //  return 0;
}

/*
 * If an error happens during the make_request, this function
 * has to be recalled. It marks the buffer as clean and not
 * uptodate, and it notifys the upper layer about the end
 * of the I/O.
 */
static inline void buffer_IO_error(struct buffer_head *bh)
{
    mark_buffer_clean(bh);
    /*
     * b_end_io has to clear the BH_Uptodate bitflag in the error case!
     */
    bh->b_end_io(bh, 0);
}

int submit_bh(int rw, struct buffer_head *bh)
{
    struct super_block *sb;
    __u32  sector_num;
    __u32  ret;

    sb = bh->b_sb;

    sector_num = bh->b_size >> sb->s_blocksize_bits;
    if (rw == WRITE)
    {
        ret = esFSYS_pwrite(bh->b_data, bh->b_blocknr, sector_num, sb->s_part);
    }
    else
    {
        ret = esFSYS_pread(bh->b_data, bh->b_blocknr, sector_num, sb->s_part);

#if FSYS_DEBUG_ON
        if (debug_para.inuse && debug_para.ino)
        {
            if (debug_dump_mems(bh->b_data, debug_para.ino->debug_dentry->d_name.name,
                                debug_para.pos, 512))
            {
                fs_log_info("nand logic blk: %d\n", bh->b_blocknr);
            }
        }
#endif
    }

    if (ret != sector_num)
    {
        //IO error
        buffer_IO_error(bh);
    }
    else
    {
        //request data succeeded
        bh->b_end_io(bh, 1);
    }

    return 0;
}

/**
 * ll_rw_block: low-level access to block devices (DEPRECATED)
 * @rw: whether to %READ or %WRITE or %SWRITE or maybe %READA (readahead)
 * @nr: number of &struct buffer_heads in the array
 * @bhs: array of pointers to &struct buffer_head
 *
 * ll_rw_block() takes an array of pointers to &struct buffer_heads, and
 * requests an I/O operation on them, either a %READ or a %WRITE.  The third
 * %SWRITE is like %WRITE only we make sure that the *current* data in buffers
 * are sent to disk. The fourth %READA option is described in the documentation
 * for generic_make_request() which ll_rw_block() calls.
 *
 * This function drops any buffer that it cannot get a lock on (with the
 * BH_Lock state bit) unless SWRITE is required, any buffer that appears to be
 * clean when doing a write request, and any buffer that appears to be
 * up-to-date when doing read request.  Further it marks as clean buffers that
 * are processed for writing (the buffer cache won't assume that they are
 * actually clean until the buffer gets unlocked).
 *
 * ll_rw_block sets b_end_io to simple completion handler that marks
 * the buffer up-to-date (if approriate), unlocks the buffer and wakes
 * any waiters.
 *
 * All of the buffers must be for the same device, and must also be a
 * multiple of the current approved size for the device.
 */
void ll_rw_block(int rw, int nr, struct buffer_head *bhs[])
{
    int i;

    if (!nr)
    {
        return;
    }

    for (i = 0; i < nr; i++)
    {
        struct buffer_head *bh = bhs[i];

        /* Only one thread can actually submit the I/O. */
        if (test_and_set_bit(BH_Lock, &bh->b_state))
        {
            continue;
        }

        /* We have the buffer lock */
        atomic_inc(&bh->b_count);
        bh->b_end_io = end_buffer_io_sync;

        switch (rw)
        {
            case WRITE:
                if (!atomic_set_buffer_clean(bh))
                    /* Hmmph! Nothing to write */
                {
                    goto end_io;
                }
                __mark_buffer_clean(bh);
                break;

            case READA:
            case READ:
                if (buffer_uptodate(bh))
                    /* Hmmph! Already have it */
                {
                    goto end_io;
                }
                break;
            default:
                BUG();
end_io:
                bh->b_end_io(bh, test_bit(BH_Uptodate, &bh->b_state));
                continue;
        }

        submit_bh(rw, bh);
    }
    return;
}
/*
 * For a data-integrity writeout, we need to wait upon any in-progress I/O
 * and then start new I/O and then wait upon it.  The caller must have a ref on
 * the buffer_head.
 */
int sync_dirty_buffer(struct buffer_head *bh)
{
    int ret = 0;

    WARN_ON(atomic_read(&bh->b_count) < 1);
    lock_buffer(bh);
    if (test_clear_buffer_dirty(bh))
    {
        get_bh(bh);
        bh->b_end_io = end_buffer_io_sync;
        ret = submit_bh(WRITE, bh);
        wait_on_buffer(bh);
        //      if (buffer_eopnotsupp(bh)) {
        //          clear_buffer_eopnotsupp(bh);
        //          ret = -EOPNOTSUPP;
        //      }
        if (!ret && !buffer_uptodate(bh))
        {
            ret = -EIO;
        }
    }
    else
    {
        unlock_buffer(bh);
    }
    return ret;
}

void *alloc_large_system_hash(const char *tablename,
                              unsigned long bucketsize,
                              unsigned long numentries,
                              int scale,
                              int flags,
                              unsigned int *_hash_shift,
                              unsigned int *_hash_mask,
                              unsigned long limit)
{
    unsigned long log2qty = 0;
    void *table = NULL;

    while (numentries > 0)
    {
        numentries >>= 1;
        log2qty++;
    }
    log2qty--;

    table = malloc(bucketsize << log2qty);

    if (!table)
    {
        return NULL;
    }

    if (_hash_shift)
    {
        *_hash_shift = log2qty;
    }
    if (_hash_mask)
    {
        *_hash_mask = (1 << log2qty) - 1;
    }

    return table;
}

/* --------------------------  DEBUG ------------------------------*/
#if     FSYS_DEBUG_ON
void debug_show_all_bh_list(char *info)
{
    debug_show_bh_list(lru_list[BUF_DIRTY], "BUF_DIRTY list", 0);
    debug_show_bh_list(lru_list[BUF_LOCKED], "BUF_LOCKED list", 0);
    debug_show_bh_list(lru_list[BUF_CLEAN], "BUF_CLEAN list", 0);
    debug_show_bh_list(free_list[BUFSIZE_INDEX(512)].list, " 512byte free buffer", 0);
}
#endif  /* FSYS_DEBUG_ON */