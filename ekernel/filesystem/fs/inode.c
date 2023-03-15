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
#include "dcache.h"
#include "writeback.h"
#include "hash.h"
#include "slab.h"
#include "buffer_head.h"
#include "fsys_debug.h"


//extern int inode_has_buffers(struct inode *);
int shrink_icache_memory(int nr);

/*
 * New inode.c implementation.
 *
 * This implementation has the basic premise of trying
 * to be extremely low-overhead and SMP-safe, yet be
 * simple enough to be "obviously correct".
 *
 * Famous last words.
 */

/* inode dynamic allocation 1999, Andrea Arcangeli <andrea@suse.de> */

/* #define INODE_PARANOIA 1 */
/* #define INODE_DEBUG 1 */

/*
 * Inode lookup is no longer as critical as it used to be:
 * most of the lookups are going to be through the dcache.
 */
#define I_HASHBITS  i_hash_shift
#define I_HASHMASK  i_hash_mask

static unsigned int i_hash_mask ;
static unsigned int i_hash_shift ;

/*
 * Each inode can be on two separate lists. One is
 * the hash list of the inode, used for lookups. The
 * other linked list is the "type" list:
 *  "in_use" - valid inode, i_count > 0, i_nlink > 0
 *  "dirty"  - as "in_use" but also dirty
 *  "unused" - valid inode, i_count = 0
 *
 * A "dirty" list is maintained for each super block,
 * allowing for low-overhead inode sync() operations.
 */

struct list_head inode_in_use = LIST_HEAD_INIT(inode_in_use);
struct list_head inode_unused = LIST_HEAD_INIT(inode_unused);
static struct hlist_head *inode_hashtable ;

///*
// * A simple spinlock to protect the list manipulations.
// *
// * NOTE! You also have to own the lock if you change
// * the i_state of an inode while it is in use..
// */
//spinlock_t inode_lock = {0};//SPIN_LOCK_UNLOCKED;

///*
// * iprune_mutex provides exclusion between the kswapd or try_to_free_pages
// * icache shrinking path, and the umount path.  Without this exclusion,
// * by the time prune_icache calls iput for the inode whose pages it has
// * been invalidating, or by the time it calls clear_inode & destroy_inode
// * from its final dispose_list, the struct super_block they refer to
// * (for inode->i_sb->s_op) may already have been freed and reused.
// */
//static DEFINE_MUTEX(iprune_mutex);

/*
 * Statistics gathering..
 */
struct inodes_stat_t inodes_stat;

static kmem_cache_t *inode_cachep;
static void init_once(void *foo, kmem_cache_t *p, unsigned long flags);
static struct inode *alloc_inode(struct super_block *sb)
{
    static struct inode_operations empty_iops = {0};
    static const struct file_operations empty_fops = {0};
    struct inode *inode;

    if (sb->s_op->alloc_inode)
    {
        inode = sb->s_op->alloc_inode(sb);
    }
    else
    {
        inode = (struct inode *) kmem_cache_alloc(inode_cachep, GFP_KERNEL);
        init_once(inode, NULL, 0);
    }

    if (inode)
    {
        inode->i_sb = sb;
        inode->i_blkbits = sb->s_blocksize_bits;
        inode->i_flags = 0;
        atomic_set(&inode->i_count, 1);
        inode->i_op = &empty_iops;
        inode->i_fop = &empty_fops;
        inode->i_nlink = 1;
        inode->i_size = 0;
        inode->i_blocks = 0;
        inode->i_bytes = 0;
        inode->i_generation = 0;
        inode->dirtied_when = 0;

        inode->i_private = NULL;
    }
    return inode;
}

void destroy_inode(struct inode *inode)
{
    //  BUG_ON(inode_has_buffers(inode));
    //  security_inode_free(inode);
    if (inode->i_sb->s_op->destroy_inode)
    {
        inode->i_sb->s_op->destroy_inode(inode);
    }
    else
    {
        kmem_cache_free(inode_cachep, (inode));
    }
}


/*
 * These are initializations that only need to be done
 * once, because the fields are idempotent across use
 * of the inode, so let the slab aware of that.
 */
void inode_init_once(struct inode *inode)
{
    memset(inode, 0, sizeof(*inode));

    INIT_HLIST_NODE(&inode->i_hash);
    INIT_LIST_HEAD(&inode->i_dentry);
    INIT_LIST_HEAD(&inode->i_devices);
    INIT_LIST_HEAD(&inode->i_dirty_buffers);
    INIT_LIST_HEAD(&inode->i_dirty_data_buffers);
    //  mutex_init(&inode->i_mutex);
    //  init_rwsem(&inode->i_alloc_sem);
    //  INIT_RADIX_TREE(&inode->i_data.page_tree, GFP_ATOMIC);
    //  rwlock_init(&inode->i_data.tree_lock);
    //  spin_lock_init(&inode->i_data.i_mmap_lock);
    //  INIT_LIST_HEAD(&inode->i_data.private_list);
    //  spin_lock_init(&inode->i_data.private_lock);
    //  INIT_RAW_PRIO_TREE_ROOT(&inode->i_data.i_mmap);
    //  INIT_LIST_HEAD(&inode->i_data.i_mmap_nonlinear);
    //  spin_lock_init(&inode->i_lock);
    //  i_size_ordered_init(inode);
    //#ifdef CONFIG_INOTIFY
    //  INIT_LIST_HEAD(&inode->inotify_watches);
    //  mutex_init(&inode->inotify_mutex);
    //#endif
}

static void init_once(void *foo, kmem_cache_t *p, unsigned long flags)
{
    struct inode *inode = (struct inode *) foo;

    inode_init_once(inode);
}

struct inode_operations bad_inode_ops = {0};
int is_bad_inode(struct inode *inode)
{
    return (inode->i_op == &bad_inode_ops);
}

/* ????make one inode for bad inode */
void make_bad_inode(struct inode *inode)
{
    remove_inode_hash(inode);

    inode->i_mode = S_IFREG;
    //  inode->i_atime = inode->i_mtime = inode->i_ctime =
    //      current_fs_time(inode->i_sb);
    inode->i_op = &bad_inode_ops;
    //  inode->i_fop = &bad_file_ops;
}

//static   void write_inode(struct inode *inode, int sync)
//{
//  if (inode->i_sb && inode->i_sb->s_op && inode->i_sb->s_op->write_inode && !is_bad_inode(inode))
//      inode->i_sb->s_op->write_inode(inode, sync);
//}

/*
 * inode_lock must be held
 */
void __iget(struct inode *inode)
{
    if (atomic_read(&inode->i_count))
    {
        atomic_inc(&inode->i_count);
        return;
    }
    atomic_inc(&inode->i_count);
    if (!(inode->i_state & (I_DIRTY | I_LOCK)))
    {
        list_move(&inode->i_list, &inode_in_use);
    }
    inodes_stat.nr_unused--;
}

/**
 * clear_inode - clear an inode
 * @inode: inode to clear
 *
 * This is called by the filesystem to tell us
 * that the inode is no longer useful. We just
 * terminate it with extreme prejudice.
 */
void clear_inode(struct inode *inode)
{
    //  might_sleep();
    //  invalidate_inode_buffers(inode);

    BUG_ON(inode->i_data.nrpages);
    BUG_ON(!(inode->i_state & I_FREEING));
    BUG_ON(inode->i_state & I_CLEAR);
    //  wait_on_inode(inode);
    //  DQUOT_DROP(inode);
    if (inode->i_sb->s_op->clear_inode)
    {
        inode->i_sb->s_op->clear_inode(inode);
    }
    //  if (S_ISBLK(inode->i_mode) && inode->i_bdev)
    //      bd_forget(inode);
    //  if (S_ISCHR(inode->i_mode) && inode->i_cdev)
    //      cd_forget(inode);
    inode->i_state = I_CLEAR;
}

/*
 * dispose_list - dispose of the contents of a local list
 * @head: the head of the list to free
 *
 * Dispose-list gets a local list with local inodes in it, so it doesn't
 * need to worry about list corruption and SMP locks.
 */
static void dispose_list(struct list_head *head)
{
    int nr_disposed = 0;

    while (!list_empty(head))
    {
        struct inode *inode;

        inode = list_first_entry(head, struct inode, i_list);
        list_del(&inode->i_list);

        //      if (inode->i_data.nrpages)
        //          truncate_inode_pages(&inode->i_data, 0);
        clear_inode(inode);

        hlist_del_init(&inode->i_hash);
        list_del_init(&inode->i_sb_list);

        //      wake_up_inode(inode);
        destroy_inode(inode);
        nr_disposed++;
    }
    inodes_stat.nr_inodes -= nr_disposed;
}

/*
 * Invalidate all inodes for a device.
 */
static int invalidate_list(struct list_head *head, struct list_head *dispose)
{
    struct list_head *next;
    int busy = 0, count = 0;

    next = head->next;
    for (;;)
    {
        struct list_head *tmp = next;
        struct inode *inode;

        next = next->next;
        if (tmp == head)
        {
            break;
        }
        inode = list_entry(tmp, struct inode, i_sb_list);
        invalidate_inode_buffers(inode);
        if (!atomic_read(&inode->i_count))
        {
            list_move(&inode->i_list, dispose);
            inode->i_state |= I_FREEING;
            count++;
            continue;
        }
        else if (atomic_read(&inode->i_count) == 1
                 && inode->i_sb->s_root
                 && inode == inode->i_sb->s_root->d_inode)
        {
            continue;
        }

        busy = 1;
    }
    /* only unused inodes may be cached with i_count zero */
    inodes_stat.nr_unused -= count;
    return busy;
}

/**
 *  invalidate_inodes   - discard the inodes on a device
 *  @sb: superblock
 *
 *  Discard all of the inodes for a given superblock. If the discard
 *  fails because there are busy inodes then a non zero value is returned.
 *  If the discard is successful all the inodes have been discarded.
 */
int invalidate_inodes(struct super_block *sb)
{
    int busy;
    LIST_HEAD(throw_away);

    //  mutex_lock(&iprune_mutex);
    busy = invalidate_list(&sb->s_inodes, &throw_away);

    dispose_list(&throw_away);
    //  mutex_unlock(&iprune_mutex);

    return busy;
}

static int can_unuse(struct inode *inode)
{
    if (inode->i_state)
    {
        return 0;
    }
    if (inode_has_buffers(inode))
    {
        return 0;
    }
    if (atomic_read(&inode->i_count))
    {
        return 0;
    }
    return 1;
}

/*
 * Scan `goal' inodes on the unused list for freeable ones. They are moved to
 * a temporary list and then are freed outside inode_lock by dispose_list().
 *
 * Any inodes which are pinned purely because of attached pagecache have their
 * pagecache removed.  We expect the final iput() on that inode to add it to
 * the front of the inode_unused list.  So look for it there and if the
 * inode is still freeable, proceed.  The right inode is found 99.9% of the
 * time in testing on a 4-way.
 *
 * If the inode has metadata buffers attached to mapping->private_list then
 * try to remove them.
 */
static void prune_icache(int nr_to_scan)
{
    LIST_HEAD(freeable);
    int nr_pruned = 0;
    int nr_scanned;
    unsigned long reap = 0;

    for (nr_scanned = 0; nr_scanned < nr_to_scan; nr_scanned++)
    {
        struct inode *inode;

        if (list_empty(&inode_unused))
        {
            break;
        }

        inode = list_entry(inode_unused.prev, struct inode, i_list);

        if (inode->i_state || atomic_read(&inode->i_count))
        {
            list_move(&inode->i_list, &inode_unused);
            continue;
        }
        if (inode_has_buffers(inode))
        {
            if (inode != list_entry(inode_unused.next,
                                    struct inode, i_list))
            {
                continue;    /* wrong inode or list_empty */
            }
            if (!can_unuse(inode))
            {
                continue;
            }
        }
        list_move(&inode->i_list, &freeable);
        inode->i_state |= I_FREEING;
        nr_pruned++;
    }
    inodes_stat.nr_unused -= nr_pruned;
    dispose_list(&freeable);
}

/*
 * shrink_icache_memory() will attempt to reclaim some unused inodes.  Here,
 * "unused" means that no dentries are referring to the inodes: the files are
 * not open and the dcache references to those inodes have already been
 * reclaimed.
 *
 * This function is passed the number of inodes to scan, and it returns the
 * total number of remaining possibly-reclaimable inodes.
 */
int shrink_icache_memory(int nr)
{
    if (nr)
    {
        /*
         * Nasty deadlock avoidance.  We may hold various FS locks,
         * and we don't want to recurse into the FS that called us
         * in clear_inode() and friends..
         */
        //      if (!(gfp_mask & __GFP_FS))
        //          return -1;
        prune_icache(nr);
    }
    return 0;
    //  return (inodes_stat.nr_unused / 100) * sysctl_vfs_cache_pressure;
}

//static void __wait_on_freeing_inode(struct inode *inode);
/*
 * Called with the inode lock held.
 * NOTE: we are not increasing the inode-refcount, you must call __iget()
 * by hand after calling find_inode now! This simplifies iunique and won't
 * add any additional branch in the common code.
 */
static struct inode *find_inode(struct super_block *sb, struct hlist_head *head, int (*test)(struct inode *, void *), void *data)
{
    struct hlist_node *node;
    struct inode *inode = NULL;

repeat:
    hlist_for_each(node, head)
    {
        inode = hlist_entry(node, struct inode, i_hash);
        if (inode->i_sb != sb)
        {
            continue;
        }
        if (!test(inode, data))
        {
            continue;
        }

        /*
         * If we try to find an inode in the inode hash while it is being
         * deleted, we have to wait until the filesystem completes its
         * deletion before reporting that it isn't found.  This function waits
         * until the deletion _might_ have completed.  Callers are responsible
         * to recheck inode state.
         *
         * Sunny : not care under epdk now, 2009.11.09
         */
        if (inode->i_state & (I_FREEING | I_CLEAR | I_WILL_FREE))
        {
            //          __wait_on_freeing_inode(inode);
            goto repeat;
        }
        break;
    }
    return node ? inode : NULL;
}

/*
 * find_inode_fast is the fast path version of find_inode, see the comment at
 * iget_locked for details.
 */
static struct inode *find_inode_fast(struct super_block *sb, struct hlist_head *head, unsigned long ino)
{
    struct hlist_node *node;
    struct inode *inode = NULL;

repeat:
    hlist_for_each(node, head)
    {
        inode = hlist_entry(node, struct inode, i_hash);
        if (inode->i_ino != ino)
        {
            continue;
        }
        if (inode->i_sb != sb)
        {
            continue;
        }

        /*
         * If we try to find an inode in the inode hash while it is being
         * deleted, we have to wait until the filesystem completes its
         * deletion before reporting that it isn't found.  This function waits
         * until the deletion _might_ have completed.  Callers are responsible
         * to recheck inode state.
         *
         * Sunny : not care under epdk now, 2009.11.09
         */
        if (inode->i_state & (I_FREEING | I_CLEAR | I_WILL_FREE))
        {
            //          __wait_on_freeing_inode(inode);
            goto repeat;
        }
        break;
    }
    return node ? inode : NULL;
}

/**
 *  new_inode   - obtain an inode
 *  @sb: superblock
 *
 *  Allocates a new inode for given superblock.
 */
static unsigned int last_ino;
struct inode *new_inode(struct super_block *sb)
{
    /*
     * On a 32bit, non LFS stat() call, glibc will generate an EOVERFLOW
     * error if st_ino won't fit in target struct field. Use 32bit counter
     * here to attempt to avoid that.
     */
    struct inode *inode;

    //  spin_lock_prefetch(&inode_lock);

    inode = alloc_inode(sb);
    if (inode)
    {
        inodes_stat.nr_inodes++;
        list_add(&inode->i_list, &inode_in_use);
        list_add(&inode->i_sb_list, &sb->s_inodes);
        inode->i_ino = ++last_ino;
        inode->i_state = 0;
    }
    return inode;
}

struct inode *new_inode_nocache(struct super_block *sb)
{
    struct inode *inode;

    inode = alloc_inode(sb);
    if (inode)
    {
        inodes_stat.nr_inodes++;
        list_add(&inode->i_list, &inode_in_use);
        list_add(&inode->i_sb_list, &sb->s_inodes);
        inode->i_ino = ++last_ino;
        inode->i_state = 0;
    }
    return inode;
}

void unlock_new_inode(struct inode *inode)
{
    /*
     * This is special!  We do not need the spinlock
     * when clearing I_LOCK, because we're guaranteed
     * that nobody else tries to do anything about the
     * state of the inode when it is locked, as we
     * just created it (so there can be no old holders
     * that haven't tested I_LOCK).
     */
    inode->i_state &= ~(I_LOCK | I_NEW);
    //  wake_up_inode(inode);
}

/*
 * This is called without the inode lock held.. Be careful.
 *
 * We no longer cache the sb_flags in i_flags - see fs.h
 *  -- rmk@arm.uk.linux.org
 */
static struct inode *get_new_inode(struct super_block *sb,
                                   struct hlist_head *head,
                                   int (*test)(struct inode *, void *),
                                   int (*set)(struct inode *, void *),
                                   void *data)
{
    struct inode *inode;

    inode = alloc_inode(sb);
    if (inode)
    {
        struct inode *old;

        /* We released the lock, so.. */
        old = find_inode(sb, head, test, data);
        if (!old)
        {
            if (set(inode, data))
            {
                goto set_failed;
            }

            inodes_stat.nr_inodes++;
            list_add(&inode->i_list, &inode_in_use);
            list_add(&inode->i_sb_list, &sb->s_inodes);
            hlist_add_head(&inode->i_hash, head);
            inode->i_state = I_LOCK | I_NEW;

            /* Return the locked inode with I_NEW set, the
             * caller is responsible for filling in the contents
             */
            return inode;
        }

        /*
         * Uhhuh, somebody else created the same inode under
         * us. Use the old inode instead of the one we just
         * allocated.
         */
        __iget(old);
        destroy_inode(inode);
        inode = old;
        //      wait_on_inode(inode);
    }
    return inode;

set_failed:
    destroy_inode(inode);
    return NULL;
}

/*
 * get_new_inode_fast is the fast path version of get_new_inode, see the
 * comment at iget_locked for details.
 */
static struct inode *get_new_inode_fast(struct super_block *sb, struct hlist_head *head, unsigned long ino)
{
    struct inode *inode;

    inode = alloc_inode(sb);
    if (inode)
    {
        struct inode *old;

        /* We released the lock, so.. */
        old = find_inode_fast(sb, head, ino);
        if (!old)
        {
            inode->i_ino = ino;
            inodes_stat.nr_inodes++;
            list_add(&inode->i_list, &inode_in_use);
            list_add(&inode->i_sb_list, &sb->s_inodes);
            hlist_add_head(&inode->i_hash, head);
            inode->i_state = I_LOCK | I_NEW;

            /* Return the locked inode with I_NEW set, the
             * caller is responsible for filling in the contents
             */
            return inode;
        }

        /*
         * Uhhuh, somebody else created the same inode under
         * us. Use the old inode instead of the one we just
         * allocated.
         */
        __iget(old);
        destroy_inode(inode);
        inode = old;
        //      wait_on_inode(inode);
    }
    return inode;
}
static unsigned long hash(struct super_block *sb, unsigned long hashval)
{
    unsigned long tmp;

    tmp = (hashval * (unsigned long)sb) ^ (GOLDEN_RATIO_PRIME + hashval) /
          L1_CACHE_BYTES;
    tmp = tmp ^ ((tmp ^ GOLDEN_RATIO_PRIME) >> I_HASHBITS);
    return tmp & I_HASHMASK;
}

/**
 *  iunique - get a unique inode number
 *  @sb: superblock
 *  @max_reserved: highest reserved inode number
 *
 *  Obtain an inode number that is unique on the system for a given
 *  superblock. This is used by file systems that have no natural
 *  permanent inode numbering system. An inode number is returned that
 *  is higher than the reserved limit but unique.
 *
 *  BUGS:
 *  With a large number of inodes live on the file system this function
 *  currently becomes quite slow.
 */
int iunique(struct super_block *sb, int max_reserved)
{
    /*
     * On a 32bit, non LFS stat() call, glibc will generate an EOVERFLOW
     * error if st_ino won't fit in target struct field. Use 32bit counter
     * here to attempt to avoid that.
     */
    static unsigned int counter;
    struct inode *inode;
    struct hlist_head *head;
    int res;

    do
    {
        if (counter <= max_reserved)
        {
            counter = max_reserved + 1;
        }
        res = counter++;
        head = inode_hashtable + hash(sb, res);
        inode = find_inode_fast(sb, head, res);
    } while (inode != NULL);

    return res;
}

struct inode *igrab(struct inode *inode)
{
    if (!(inode->i_state & (I_FREEING | I_CLEAR | I_WILL_FREE)))
    {
        __iget(inode);
    }
    else
        /*
         * Handle the case where s_op->clear_inode is not been
         * called yet, and somebody is calling igrab
         * while the inode is getting freed.
         */
    {
        inode = NULL;
    }
    return inode;
}

/**
 * ifind - internal function, you want ilookup5() or iget5().
 * @sb:     super block of file system to search
 * @head:   the head of the list to search
 * @test:   callback used for comparisons between inodes
 * @data:   opaque data pointer to pass to @test
 * @wait:   if true wait for the inode to be unlocked, if false do not
 *
 * ifind() searches for the inode specified by @data in the inode
 * cache. This is a generalized version of ifind_fast() for file systems where
 * the inode number is not sufficient for unique identification of an inode.
 *
 * If the inode is in the cache, the inode is returned with an incremented
 * reference count.
 *
 * Otherwise NULL is returned.
 *
 * Note, @test is called with the inode_lock held, so can't sleep.
 */
static struct inode *ifind(struct super_block *sb,
                           struct hlist_head *head,
                           int (*test)(struct inode *, void *),
                           void *data, const int wait)
{
    struct inode *inode;

    inode = find_inode(sb, head, test, data);
    if (inode)
    {
        __iget(inode);
        //      if (wait)
        //          wait_on_inode(inode);
        return inode;
    }
    return NULL;
}

/**
 * ifind_fast - internal function, you want ilookup() or iget().
 * @sb:     super block of file system to search
 * @head:       head of the list to search
 * @ino:    inode number to search for
 *
 * ifind_fast() searches for the inode @ino in the inode cache. This is for
 * file systems where the inode number is sufficient for unique identification
 * of an inode.
 *
 * If the inode is in the cache, the inode is returned with an incremented
 * reference count.
 *
 * Otherwise NULL is returned.
 */
static struct inode *ifind_fast(struct super_block *sb,
                                struct hlist_head *head, unsigned long ino)
{
    struct inode *inode;

    inode = find_inode_fast(sb, head, ino);
    if (inode)
    {
        __iget(inode);
        //      wait_on_inode(inode);
        return inode;
    }
    return NULL;
}
//
///**
// * ilookup5_nowait - search for an inode in the inode cache
// * @sb:       super block of file system to search
// * @hashval:  hash value (usually inode number) to search for
// * @test: callback used for comparisons between inodes
// * @data: opaque data pointer to pass to @test
// *
// * ilookup5() uses ifind() to search for the inode specified by @hashval and
// * @data in the inode cache. This is a generalized version of ilookup() for
// * file systems where the inode number is not sufficient for unique
// * identification of an inode.
// *
// * If the inode is in the cache, the inode is returned with an incremented
// * reference count.  Note, the inode lock is not waited upon so you have to be
// * very careful what you do with the returned inode.  You probably should be
// * using ilookup5() instead.
// *
// * Otherwise NULL is returned.
// *
// * Note, @test is called with the inode_lock held, so can't sleep.
// */
//struct inode *ilookup5_nowait(struct super_block *sb, unsigned long hashval,
//      int (*test)(struct inode *, void *), void *data)
//{
//  struct hlist_head *head = inode_hashtable + hash(sb, hashval);
//
//  return ifind(sb, head, test, data, 0);
//}
//
///**
// * ilookup5 - search for an inode in the inode cache
// * @sb:       super block of file system to search
// * @hashval:  hash value (usually inode number) to search for
// * @test: callback used for comparisons between inodes
// * @data: opaque data pointer to pass to @test
// *
// * ilookup5() uses ifind() to search for the inode specified by @hashval and
// * @data in the inode cache. This is a generalized version of ilookup() for
// * file systems where the inode number is not sufficient for unique
// * identification of an inode.
// *
// * If the inode is in the cache, the inode lock is waited upon and the inode is
// * returned with an incremented reference count.
// *
// * Otherwise NULL is returned.
// *
// * Note, @test is called with the inode_lock held, so can't sleep.
// */
//struct inode *ilookup5(struct super_block *sb, unsigned long hashval,
//      int (*test)(struct inode *, void *), void *data)
//{
//  struct hlist_head *head = inode_hashtable + hash(sb, hashval);
//
//  return ifind(sb, head, test, data, 1);
//}
//
///**
// * ilookup - search for an inode in the inode cache
// * @sb:       super block of file system to search
// * @ino:  inode number to search for
// *
// * ilookup() uses ifind_fast() to search for the inode @ino in the inode cache.
// * This is for file systems where the inode number is sufficient for unique
// * identification of an inode.
// *
// * If the inode is in the cache, the inode is returned with an incremented
// * reference count.
// *
// * Otherwise NULL is returned.
// */
//struct inode *ilookup(struct super_block *sb, unsigned long ino)
//{
//  struct hlist_head *head = inode_hashtable + hash(sb, ino);
//
//  return ifind_fast(sb, head, ino);
//}
//
/**
 * iget5_locked - obtain an inode from a mounted file system
 * @sb:     super block of file system
 * @hashval:    hash value (usually inode number) to get
 * @test:   callback used for comparisons between inodes
 * @set:    callback used to initialize a new struct inode
 * @data:   opaque data pointer to pass to @test and @set
 *
 * This is iget() without the read_inode() portion of get_new_inode().
 *
 * iget5_locked() uses ifind() to search for the inode specified by @hashval
 * and @data in the inode cache and if present it is returned with an increased
 * reference count. This is a generalized version of iget_locked() for file
 * systems where the inode number is not sufficient for unique identification
 * of an inode.
 *
 * If the inode is not in cache, get_new_inode() is called to allocate a new
 * inode and this is returned locked, hashed, and with the I_NEW flag set. The
 * file system gets to fill it in before unlocking it via unlock_new_inode().
 *
 * Note both @test and @set are called with the inode_lock held, so can't sleep.
 */
struct inode *iget5_locked(struct super_block *sb, unsigned long hashval,
                           int (*test)(struct inode *, void *),
                           int (*set)(struct inode *, void *), void *data)
{
    struct hlist_head *head = inode_hashtable + hash(sb, hashval);
    struct inode *inode;

    inode = ifind(sb, head, test, data, 1);
    if (inode)
    {
        return inode;
    }
    /*
     * get_new_inode() will do the right thing, re-trying the search
     * in case it had to block at any point.
     */
    return get_new_inode(sb, head, test, set, data);
}

/**
 * iget_locked - obtain an inode from a mounted file system
 * @sb:     super block of file system
 * @ino:    inode number to get
 *
 * This is iget() without the read_inode() portion of get_new_inode_fast().
 *
 * iget_locked() uses ifind_fast() to search for the inode specified by @ino in
 * the inode cache and if present it is returned with an increased reference
 * count. This is for file systems where the inode number is sufficient for
 * unique identification of an inode.
 *
 * If the inode is not in cache, get_new_inode_fast() is called to allocate a
 * new inode and this is returned locked, hashed, and with the I_NEW flag set.
 * The file system gets to fill it in before unlocking it via
 * unlock_new_inode().
 *
 * FIXME : without wait_on_inode portion now, 2009.10.29, By sunny
 */
struct inode *iget_locked(struct super_block *sb, unsigned long ino)
{
    struct hlist_head *head = inode_hashtable + hash(sb, ino);
    struct inode *inode;

    inode = ifind_fast(sb, head, ino);
    if (inode)
    {
        return inode;
    }
    /*
     * get_new_inode_fast() will do the right thing, re-trying the search
     * in case it had to block at any point.
     */
    return get_new_inode_fast(sb, head, ino);
}

/**
 *  __insert_inode_hash - hash an inode
 *  @inode: unhashed inode
 *  @hashval: unsigned long value used to locate this object in the
 *      inode_hashtable.
 *
 *  Add an inode to the inode hash for this superblock.
 */
void __insert_inode_hash(struct inode *inode, unsigned long hashval)
{
    struct hlist_head *head = inode_hashtable + hash(inode->i_sb, hashval);
    hlist_add_head(&inode->i_hash, head);
}

/**
 *  remove_inode_hash - remove an inode from the hash
 *  @inode: inode to unhash
 *
 *  Remove an inode from the superblock.
 */
void remove_inode_hash(struct inode *inode)
{
    //  spin_lock(&inode_lock);
    hlist_del_init(&inode->i_hash);
    //  spin_unlock(&inode_lock);
}

/*
 * Tell the filesystem that this inode is no longer of any interest and should
 * be completely destroyed.
 *
 * We leave the inode in the inode hash table until *after* the filesystem's
 * ->delete_inode completes.  This ensures that an iget (such as nfsd might
 * instigate) will always find up-to-date information either in the hash or on
 * disk.
 *
 * I_FREEING is set so that no-one will take a new reference to the inode while
 * it is being deleted.
 */
void generic_delete_inode(struct inode *inode)
{
    const struct super_operations *op = inode->i_sb->s_op;

    list_del_init(&inode->i_list);
    list_del_init(&inode->i_sb_list);
    inode->i_state |= I_FREEING;
    inodes_stat.nr_inodes--;

    //  security_inode_delete(inode);

    if (op->delete_inode)
    {
        void (*delete)(struct inode *) = op->delete_inode;
        //      if (!is_bad_inode(inode))
        //          DQUOT_INIT(inode);
        /* Filesystems implementing their own
         * s_op->delete_inode are required to call
         * truncate_inode_pages and clear_inode()
         * internally */
        delete (inode);
    }
    else
    {
        //      truncate_inode_pages(&inode->i_data, 0);
        clear_inode(inode);
    }
    hlist_del_init(&inode->i_hash);
    //  wake_up_inode(inode);
    BUG_ON(inode->i_state != I_CLEAR);
    destroy_inode(inode);
}

static void generic_forget_inode(struct inode *inode)
{
    struct super_block *sb = inode->i_sb;

    if (!hlist_unhashed(&inode->i_hash))
    {
        if (!(inode->i_state & (I_DIRTY | I_LOCK)))
        {
            list_move(&inode->i_list, &inode_unused);
        }
        inodes_stat.nr_unused++;
        if (sb->s_flags & MS_ACTIVE)
        {
            return;
        }
        inode->i_state |= I_WILL_FREE;
        write_inode_now(inode, 1);
        inode->i_state &= ~I_WILL_FREE;
        inodes_stat.nr_unused--;
        hlist_del_init(&inode->i_hash);
    }
    list_del_init(&inode->i_list);
    list_del_init(&inode->i_sb_list);
    inode->i_state |= I_FREEING;
    inodes_stat.nr_inodes--;
    //  if (inode->i_data.nrpages)
    //      truncate_inode_pages(&inode->i_data, 0);
    clear_inode(inode);
    //  wake_up_inode(inode);
    destroy_inode(inode);
}

/*
 * Normal UNIX filesystem behaviour: delete the
 * inode when the usage count drops to zero, and
 * i_nlink is zero.
 */
void generic_drop_inode(struct inode *inode)
{
    if (!inode->i_nlink)
    {
        generic_delete_inode(inode);
    }
    else
    {
        generic_forget_inode(inode);
    }
}


/*
 * Called when we're dropping the last reference
 * to an inode.
 *
 * Call the FS "drop()" function, defaulting to
 * the legacy UNIX filesystem behaviour..
 *
 * NOTE! NOTE! NOTE! We're called with the inode lock
 * held, and the drop function is supposed to release
 * the lock!
 */
static   void iput_final(struct inode *inode)
{
    const struct super_operations *op = inode->i_sb->s_op;
    void (*drop)(struct inode *) = generic_drop_inode;

    if (op && op->drop_inode)
    {
        drop = op->drop_inode;
    }
    drop(inode);
}

/**
 *  iput    - put an inode
 *  @inode: inode to put
 *
 *  Puts an inode, dropping its usage count. If the inode use count hits
 *  zero, the inode is then freed and may also be destroyed.
 *
 *  Consequently, iput() can sleep.
 */
void iput(struct inode *inode)
{
    if (inode)
    {
        const struct super_operations *op = inode->i_sb->s_op;

        BUG_ON(inode->i_state == I_CLEAR);

        if (op && op->put_inode)
        {
            op->put_inode(inode);
        }

        if (atomic_dec_and_test(&inode->i_count))
        {
            iput_final(inode);
        }
    }
}

/**
 *  touch_atime -   update the access time
 *  @mnt: mount the inode is accessed on
 *  @dentry: dentry accessed
 *
 *  Update the accessed time on an inode and mark it for writeback.
 *  This function automatically handles read only file systems and media,
 *  as well as the "noatime" flag and inode specific "noatime" markers.
 */
void touch_atime(struct dentry *dentry)
{
    struct inode *inode = dentry->d_inode;
    struct timespec now;

    if (inode->i_flags & S_NOATIME)
    {
        return;
    }
    if (IS_NOATIME(inode))
    {
        return;
    }
    if ((inode->i_sb->s_flags & MS_NODIRATIME) && S_ISDIR(inode->i_mode))
    {
        return;
    }
    now = current_fs_time(inode->i_sb);
    inode->i_atime.tv_sec = now.tv_sec;
    inode->i_atime.tv_nsec = now.tv_nsec;
    mark_inode_dirty_sync(inode);
}

/**
 *  file_update_time    -   update mtime and ctime time
 *  @file: file accessed
 *
 *  Update the mtime and ctime members of an inode and mark the inode
 *  for writeback.  Note that this function is meant exclusively for
 *  usage in the file write path of filesystems, and filesystems may
 *  choose to explicitly ignore update via this function with the
 *  S_NOCTIME inode flag, e.g. for network filesystem where these
 *  timestamps are handled by the server.
 */

void file_update_time(struct file *file)
{
    struct inode *inode = file->f_dentry->d_inode;
    struct timespec now;
    int sync_it = 0;

    if (IS_NOCMTIME(inode))
    {
        return;
    }
    if (IS_RDONLY(inode))
    {
        return;
    }

    if (sync_it)
    {
        mark_inode_dirty_sync(inode);
    }
}


int inode_needs_sync(struct inode *inode)
{
    if (IS_SYNC(inode))
    {
        return 1;
    }
    if (S_ISDIR(inode->i_mode) && IS_DIRSYNC(inode))
    {
        return 1;
    }
    return 0;
}

int inode_wait(void *word)
{
    //  schedule();
    return 0;
}

/**
 *  __mark_inode_dirty -    internal function
 *  @inode: inode to mark
 *  @flags: what kind of dirty (i.e. I_DIRTY_SYNC)
 *  Mark an inode as dirty. Callers should use mark_inode_dirty or
 *      mark_inode_dirty_sync.
 *
 * Put the inode on the super block's dirty list.
 *
 * CAREFUL! We mark it dirty unconditionally, but move it onto the
 * dirty list only if it is hashed or if it refers to a blockdev.
 * If it was not hashed, it will never be added to the dirty list
 * even if it is later hashed, as it will have been marked dirty already.
 *
 * In short, make sure you hash any inodes _before_ you start marking
 * them dirty.
 *
 * This function *must* be atomic for the I_DIRTY_PAGES case -
 * set_page_dirty() is called under spinlock in several places.
 *
 * Note that for blockdevs, inode->dirtied_when represents the dirtying time of
 * the block-special inode (/dev/hda1) itself.  And the ->dirtied_when field of
 * the kernel-internal blockdev inode represents the dirtying time of the
 * blockdev's pages.  This is why for I_DIRTY_PAGES we always use
 * page->mapping->host, so the page-dirtying time is recorded in the internal
 * blockdev inode.
 */
void __mark_inode_dirty(struct inode *inode, int flags)
{
    struct super_block *sb = inode->i_sb;

    /*
     * Don't do this for I_DIRTY_PAGES - that doesn't actually
     * dirty the inode itself
     */
    if (flags & (I_DIRTY_SYNC | I_DIRTY_DATASYNC))
    {
        if (sb->s_op->dirty_inode)
        {
            sb->s_op->dirty_inode(inode);
        }
    }

    /* avoid the locking if we can */
    if ((inode->i_state & flags) == flags)
    {
        return;
    }

    if ((inode->i_state & flags) != flags)
    {
        const int was_dirty = inode->i_state & I_DIRTY;

        inode->i_state |= flags;

        /*
         * If the inode is locked, just update its dirty state.
         * The unlocker will place the inode on the appropriate
         * superblock list, based upon its state.
         */
        if (inode->i_state & I_LOCK)
        {
            goto out;
        }

        if (inode->i_state & (I_FREEING | I_CLEAR))
        {
            goto out;
        }

        /*
         * If the inode was already on s_dirty or s_io, don't
         * reposition it (that would break s_dirty time-ordering).
         */
        if (!was_dirty)
        {
            inode->dirtied_when = jiffies;
            list_move(&inode->i_list, &sb->s_dirty);
        }
    }
out:
    return;
}

static int write_inode(struct inode *inode, int sync)
{
    fs_log_verbose("write_inode() enter(name:%s)...\n", inode->debug_dentry->d_name.name);
    if (inode->i_sb->s_op->write_inode && !is_bad_inode(inode))
    {
        return inode->i_sb->s_op->write_inode(inode, sync);
    }
    return 0;
}

/*
 * Write a single inode's dirty pages and inode data out to disk.
 * If `wait' is set, wait on the writeout.
 *
 * The whole writeout design is quite complex and fragile.  We want to avoid
 * starvation of particular inodes when others are being redirtied, prevent
 * livelocks, etc.
 *
 * Called under inode_lock.
 */
static int __writeback_single_inode(struct inode *inode, struct writeback_control *wbc)
{
    unsigned dirty;
    struct super_block *sb = inode->i_sb;
    int wait = wbc->sync_mode == WB_SYNC_ALL;
    int ret = 0;

    if (!atomic_read(&inode->i_count))
    {
        WARN_ON(!(inode->i_state & (I_WILL_FREE | I_FREEING)));
    }
    else
    {
        WARN_ON(inode->i_state & I_WILL_FREE);
    }

    if (!wait && (inode->i_state & I_LOCK))
    {
        list_move(&inode->i_list, &inode->i_sb->s_dirty);

        return 0;
    }

    BUG_ON(inode->i_state & I_LOCK);

    /* Set I_LOCK, reset I_DIRTY */
    dirty = inode->i_state & I_DIRTY;
    inode->i_state |= I_LOCK;
    inode->i_state &= ~I_DIRTY;

    /* Don't write the inode if only I_DIRTY_PAGES was set */
    if (dirty & (I_DIRTY_SYNC | I_DIRTY_DATASYNC))
    {
        int err = write_inode(inode, wait);
        if (ret == 0)
        {
            ret = err;
        }
    }

    inode->i_state &= ~I_LOCK;
    if (!(inode->i_state & I_FREEING))
    {
        if (inode->i_state & I_DIRTY)
        {
            /*
             * Someone redirtied the inode while were writing back
             * the pages.
             */
            list_move(&inode->i_list, &sb->s_dirty);
        }
        else if (atomic_read(&inode->i_count))
        {
            /*
             * The inode is clean, inuse
             */
            list_move(&inode->i_list, &inode_in_use);
        }
        else
        {
            /*
             * The inode is clean, unused
             */
            list_move(&inode->i_list, &inode_unused);
        }
    }
    return ret;
}

/*
 * Write out a superblock's list of dirty inodes.  A wait will be performed
 * upon no inodes, all inodes or the final one, depending upon sync_mode.
 *
 * If older_than_this is non-NULL, then only write out inodes which
 * had their first dirtying at a time earlier than *older_than_this.
 *
 * If we're a pdlfush thread, then implement pdflush collision avoidance
 * against the entire list.
 *
 * WB_SYNC_HOLD is a hack for sys_sync(): reattach the inode to sb->s_dirty so
 * that it can be located for waiting on in __writeback_single_inode().
 *
 * Called under inode_lock.
 *
 * If `bdi' is non-zero then we're being asked to writeback a specific queue.
 * This function assumes that the blockdev superblock's inodes are backed by
 * a variety of queues, so all inodes are searched.  For other superblocks,
 * assume that all inodes are backed by the same queue.
 *
 * FIXME: this linear search could get expensive with many fileystems.  But
 * how to fix?  We need to go from an address_space to all inodes which share
 * a queue with that address_space.  (Easy: have a global "dirty superblocks"
 * list).
 *
 * The inodes to be written are parked on sb->s_io.  They are moved back onto
 * sb->s_dirty as they are selected for writing.  This way, none can be missed
 * on the writer throttling path, and we get decent balancing between many
 * throttled threads: we don't want them all piling up on __wait_on_inode.
 */
void sync_part_inodes(struct super_block *sb,  struct writeback_control *wbc)
{
    unsigned long start = jiffies;  /* livelock avoidance */

    if (!wbc->for_kupdate || list_empty(&sb->s_io))
    {
        list_splice_init(&sb->s_dirty, &sb->s_io);
    }

    while (!list_empty(&sb->s_io))
    {
        struct inode *inode = list_entry(sb->s_io.prev,
                                         struct inode, i_list);

        /* Was this inode dirtied after sync_sb_inodes was called? */
        if (time_after(inode->dirtied_when, start))
        {
            break;
        }

        /* Was this inode dirtied too recently? */
        if (wbc->older_than_this && time_after(inode->dirtied_when,
                                               *wbc->older_than_this))
        {
            break;
        }

        BUG_ON(inode->i_state & I_FREEING);
        __iget(inode);
        __writeback_single_inode(inode, wbc);
        if (wbc->sync_mode == WB_SYNC_HOLD)
        {
            inode->dirtied_when = jiffies;
            list_move(&inode->i_list, &sb->s_dirty);
        }
        iput(inode);
        //      if (wbc->nr_to_write <= 0)
        //          break;
    }
    return;     /* Leave any unwritten inodes on s_io */
}

/**
 * sync_inodes - writes all inodes to disk
 * @wait: wait for completion
 *
 * sync_inodes() goes through each super block's dirty inode list, writes the
 * inodes out, waits on the writeout and puts the inodes back on the normal
 * list.
 *
 * This is for sys_sync().  fsync_dev() uses the same algorithm.  The subtle
 * part of the sync functions is that the blockdev "superblock" is processed
 * last.  This is because the write_inode() function of a typical fs will
 * perform no I/O, but will mark buffers in the blockdev mapping as dirty.
 * What we want to do is to perform all that dirtying first, and then write
 * back all those inode blocks via the blockdev mapping in one sweep.  So the
 * additional (somewhat redundant) sync_blockdev() calls here are to make
 * sure that really happens.  Because if we call sync_inodes_sb(wait=1) with
 * outstanding dirty inodes, the writeback goes block-at-a-time within the
 * filesystem's write_inode().  This is extremely slow.
 */
void sync_parts_inodes(struct writeback_control *wbc)
{
    struct super_block *sb;

    list_for_each_entry(sb, &super_blocks, s_list)
    {
        if (sb->s_syncing)
        {
            continue;
        }
        sb->s_syncing = 1;
        if (sb->s_root)
        {
            sync_part_inodes(sb, wbc);
        }
        sb->s_syncing = 0;
    }
}

/**
 * write_inode_now  -   write an inode to disk
 * @inode: inode to write to disk
 * @sync: whether the write should be synchronous or not
 *
 * This function commits an inode to disk immediately if it is dirty. This is
 * primarily needed by knfsd.
 *
 * The caller must either have a ref on the inode or must have set I_WILL_FREE.
 */
int write_inode_now(struct inode *inode, int sync)
{
    int ret;
    struct writeback_control wbc =
    {
        .nr_to_write = LONG_MAX,
        .sync_mode = WB_SYNC_ALL,
        .range_start = 0,
        .range_end = LLONG_MAX,
    };

    ret = __writeback_single_inode(inode, &wbc);

    return ret;
}

/*
 * If we try to find an inode in the inode hash while it is being
 * deleted, we have to wait until the filesystem completes its
 * deletion before reporting that it isn't found.  This function waits
 * until the deletion _might_ have completed.  Callers are responsible
 * to recheck inode state.
 *
 * It doesn't matter if I_LOCK is not set initially, a call to
 * wake_up_inode() after removing from the hash list will DTRT.
 *
 * This is called with inode_lock held.
 */
//static void __wait_on_freeing_inode(struct inode *inode)
//{
//  wait_queue_head_t *wq;
//  DEFINE_WAIT_BIT(wait, &inode->i_state, __I_LOCK);
//  wq = bit_waitqueue(&inode->i_state, __I_LOCK);
//  prepare_to_wait(wq, &wait.wait, TASK_UNINTERRUPTIBLE);
//  spin_unlock(&inode_lock);
//  schedule();
//  finish_wait(wq, &wait.wait);
//  spin_lock(&inode_lock);
//}

static unsigned long ihash_entries = DENTRY_INODE_CNT;

void inode_init(unsigned long mempages)
{
    struct inode *inode;
    int loop;

    /* inode slab cache */
    inode_cachep = kmem_cache_create("inode_cache",
                                     sizeof(struct inode),
                                     0,
                                     SLAB_HWCACHE_ALIGN,
                                     init_once,
                                     NULL);
    //  set_shrinker(DEFAULT_SEEKS, shrink_icache_memory);
    //
    //  /* Hash may have been set up in inode_init_early */
    //  if (!hashdist)
    //      return;
    //

    inode_hashtable =
        alloc_large_system_hash("Inode-cache",
                                sizeof(struct hlist_head),
                                ihash_entries,
                                14,
                                0,
                                &i_hash_shift,
                                &i_hash_mask,
                                0);

    for (loop = 0; loop < (1 << i_hash_shift); loop++)
    {
        INIT_HLIST_HEAD(&inode_hashtable[loop]);
    }
}

/* epdk not support type special file
 * just fill NULL
 */
void init_special_inode(struct inode *inode, umode_t mode, dev_t rdev)
{
    inode->i_mode = mode;
    if (S_ISCHR(mode))
    {
        //      inode->i_fop = &def_chr_fops;
        inode->i_op = NULL;
        inode->i_fop = NULL;
        //      inode->i_rdev = rdev;
    }
    else if (S_ISBLK(mode))
    {
        //      inode->i_fop = &def_blk_fops;
        inode->i_op = NULL;
        inode->i_fop = NULL;
        //      inode->i_rdev = rdev;
    }
    else if (S_ISFIFO(mode))
    {
        //      inode->i_fop = &def_fifo_fops;
        inode->i_op = NULL;
        inode->i_fop = NULL;
    }
    else if (S_ISSOCK(mode))
    {
        //      inode->i_fop = &bad_sock_fops;
        inode->i_op = NULL;
        inode->i_fop = NULL;
    }
    else
    {
        fs_log_error("init_special_inode: bogus i_mode (%o)\n", mode);
    }
}

/* ---------------------------debug --------------------------------*/
#if     FSYS_DEBUG_ON
void debug_show_all_inode(char *info)
{
    struct super_block *sb;

    fs_log_info("%s", info);

    list_for_each_entry(sb, &super_blocks, s_list)
    {
        debug_show_sb_inode(&sb->s_inodes, "sb->s_inodes inode", 0);

        debug_show_lru_inode(&sb->s_dirty, "lru, &sb->s_dirty inodes:", 0);
        debug_show_lru_inode(&inode_in_use, "lru, in use inodes:", 0);
        debug_show_lru_inode(&inode_unused, "lru, unused inodes:", 0);
    }
}
#endif  /* FSYS_DEBUG_ON */