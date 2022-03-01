/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : dcache.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: dentry cache operations.
*           Notes on the allocation strategy:
*           The dcache is a master of the icache - whenever a dcache entry
*           exists, the inode will always exist. "iput()" is done either when
*           the dcache entry is deleted or garbage collected.
*
* Update  : date                auther      ver     notes
*           2011-3-15 15:32:16  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include <string.h>
#include <stdarg.h>
#include "fs.h"
#include "hash.h"
#include "file.h"
#include "errno.h"
#include "page_pool.h"
#include "slab.h"
#include "limits.h"
#include "atomic.h"
#include "fsys_debug.h"

static kmem_cache_t *dentry_cache;

#define DNAME_INLINE_LEN (sizeof(struct dentry)-offsetof(struct dentry,d_iname))

/*
 * This is the single most critical data structure when it comes
 * to the dcache: the hashtable for lookups. Somebody should try
 * to make this good - I've just made it work.
 *
 * This hash-function tries to avoid losing too many bits of hash
 * information, yet avoid using a prime hash-size or similar.
 */
#define D_HASHBITS     d_hash_shift
#define D_HASHMASK     d_hash_mask

static unsigned int d_hash_mask ;
static unsigned int d_hash_shift ;
static struct hlist_head *dentry_hashtable ;
static struct list_head dentry_unused = LIST_HEAD_INIT(dentry_unused);

/* Statistics gathering. */
struct dentry_stat_t dentry_stat =
{
    .age_limit = 45,
};

static uint32_t m_randseed;

/*  随机函数 */
static inline uint32_t prand(void)
{
    return (m_randseed = 1664525L * m_randseed + 1013904223L) >> 5;
}

/*
 * no dcache_lock, please.  The caller must decrement dentry_stat.nr_dentry
 * inside dcache_lock.
 */
static void d_free(struct dentry *dentry)
{
    if (dentry->d_op && dentry->d_op->d_release)
    {
        dentry->d_op->d_release(dentry);
    }

    if (dname_external(dentry))
    {
        kfree(dentry->d_name.name);
    }
    if (!list_empty(&dentry->d_sb_list))
    {
        list_del_init(&dentry->d_sb_list);
    }
    kmem_cache_free(dentry_cache, dentry);
}

/*
 * Release the dentry's inode, using the filesystem
 * d_iput() operation if defined.
 * Called with dcache_lock and per dentry lock held, drops both.
 */
static void dentry_iput(struct dentry *dentry)
{
    struct inode *inode = dentry->d_inode;
    if (inode)
    {
        dentry->d_inode = NULL;

        if (dentry->d_op && dentry->d_op->d_iput)
        {
            dentry->d_op->d_iput(dentry, inode);
        }
        else
        {
            iput(inode);
        }
    }
}

/**
 * d_kill - kill dentry and return parent
 * @dentry: dentry to kill
 *
 * Called with dcache_lock and d_lock, releases both.  The dentry must
 * already be unhashed and removed from the LRU.
 *
 * If this is the root of the dentry tree, return NULL.
 */
static struct dentry *d_kill(struct dentry *dentry)
{
    dentry_stat.nr_dentry--;    /* For d_free, below */
    /*drops the locks, at that point nobody can reach this dentry */
    dentry_iput(dentry);
    //dput(dentry->d_parent);
    d_free(dentry);
    return NULL;
}

/*
 * This is dput
 *
 * This is complicated by the fact that we do not want to put
 * dentries that are no longer on any hash chain on the unused
 * list: we'd much rather just get rid of them immediately.
 *
 * However, that implies that we have to traverse the dentry
 * tree upwards to the parents which might _also_ now be
 * scheduled for deletion (it may have been only waiting for
 * its last child to go away).
 *
 * This tail recursion is done by hand as we don't want to depend
 * on the compiler to always get this right (gcc generally doesn't).
 * Real recursion would eat up our stack space.
 */

/*
 * dput - release a dentry
 * @dentry: dentry to release
 *
 * Release a dentry. This will drop the usage count and if appropriate
 * call the dentry unlink method as well as removing it from the queues and
 * releasing its resources. If the parent dentries were scheduled for release
 * they too may now get deleted.
 *
 * no dcache lock, please.
 */

void dput(struct dentry *dentry)
{
    if (!dentry)
    {
        return;
    }

repeat:
    if (!atomic_dec_and_test(&dentry->d_count))
    {
        return;
    }

    if (atomic_read(&dentry->d_count))
    {
        return;
    }

    /*
     * AV: ->d_delete() is _NOT_ allowed to block now.
     */
    if (dentry->d_op && dentry->d_op->d_delete)
    {
        if (dentry->d_op->d_delete(dentry))
        {
            goto unhash_it;
        }
    }
    /* Unreachable? Get rid of it */
    if (d_unhashed(dentry))
    {
        goto kill_it;
    }
    if (list_empty(&dentry->d_lru))
    {
        dentry->d_flags |= DCACHE_REFERENCED;
        list_add(&dentry->d_lru, &dentry_unused);
        dentry_stat.nr_unused++;
    }
    return;

unhash_it:
    __d_drop(dentry);
kill_it:
    /* If dentry was on d_lru list
     * delete it from there
     */
    if (!list_empty(&dentry->d_lru))
    {
        list_del(&dentry->d_lru);
        dentry_stat.nr_unused--;
    }
    dentry = d_kill(dentry);
    if (dentry)
    {
        goto repeat;
    }
}

/**
 * d_invalidate - invalidate a dentry
 * @dentry: dentry to invalidate
 *
 * Try to invalidate the dentry if it turns out to be
 * possible. If there are other dentries that can be
 * reached through this one we can't delete it and we
 * return -EBUSY. On success we return 0.
 *
 * no dcache lock.
 */

int d_invalidate(struct dentry *dentry)
{
    /*
     * If it's already been dropped, return OK.
     */
    if (d_unhashed(dentry))
    {
        return 0;
    }

    /*
     * Somebody else still using it?
     *
     * If it's a directory, we can't drop it
     * for fear of somebody re-populating it
     * with children (even though dropping it
     * would make it unreachable from the root,
     * we might still populate it if it was a
     * working directory or similar).
     */
    if (atomic_read(&dentry->d_count) > 1)
    {
        if (dentry->d_inode && S_ISDIR(dentry->d_inode->i_mode))
        {
            return -EBUSY;
        }
    }

    __d_drop(dentry);
    return 0;
}

/*
 * Throw away a dentry - free the inode, dput the parent.  This requires that
 * the LRU list has already been removed.
 *
 * If prune_parents is true, try to prune ancestors as well.
 *
 * Called with dcache_lock, drops it and then regains.
 * Called with dentry->d_lock held, drops it.
 */
static void prune_one_dentry(struct dentry *dentry)
{
    __d_drop(dentry);
    d_kill(dentry);
}

/**
 * prune_dcache - shrink the dcache
 * @count: number of entries to try and free
 * @sb: if given, ignore dentries for other superblocks
 *         which are being unmounted.
 *
 * Shrink the dcache. This is done when we need
 * more memory, or simply when we need to unmount
 * something (at which point we need to unuse
 * all dentries).
 *
 * This function may fail to free any resources if
 * all the dentries are in use.
 */

static void prune_dcache(int count, struct super_block *sb)
{
    for (; count ; count--)
    {
        struct dentry *dentry;
        struct list_head *tmp;

        tmp = dentry_unused.prev;
        if (sb)
        {
            /* Try to find a dentry for this sb, but don't try
             * too hard, if they aren't near the tail they will
             * be moved down again soon
             */
            int skip = count;
            while (skip && tmp != &dentry_unused &&
                   list_entry(tmp, struct dentry, d_lru)->d_sb != sb)
            {
                skip--;
                tmp = tmp->prev;
            }
        }
        if (tmp == &dentry_unused)
        {
            break;
        }
        list_del_init(tmp);
        dentry_stat.nr_unused--;
        dentry = list_entry(tmp, struct dentry, d_lru);

        /*
         * We found an inuse dentry which was not removed from
         * dentry_unused because of laziness during lookup.  Do not free
         * it - just keep it off the dentry_unused list.
         */
        if (atomic_read(&dentry->d_count))
        {
            continue;
        }
        /* If the dentry was recently referenced, don't free it. */
        if (dentry->d_flags & DCACHE_REFERENCED)
        {
            dentry->d_flags &= ~DCACHE_REFERENCED;
            list_add(&dentry->d_lru, &dentry_unused);
            dentry_stat.nr_unused++;
            continue;
        }
        /*
         * If the dentry is not DCACHED_REFERENCED, it is time
         * to remove it from the dcache, provided the super block is
         * NULL (which means we are trying to reclaim memory)
         * or this dentry belongs to the same super block that
         * we want to shrink.
         */
        /*
         * If this dentry is for "my" filesystem, then I can prune it
         * without taking the s_umount lock (I already hold it).
         */
        if (sb && dentry->d_sb == sb)
        {
            prune_one_dentry(dentry);
            continue;
        }

        /*
         * ...otherwise we need to be sure this filesystem isn't being
         * unmounted, otherwise we could race with
         * generic_shutdown_super(), and end up holding a reference to
         * an inode while the filesystem is unmounted.
         * So we try to get s_umount, and make sure s_root isn't NULL.
         * (Take a local copy of s_umount to avoid a use-after-free of
         * `dentry').
         */
        if (dentry->d_sb->s_root != NULL)
        {
            prune_one_dentry(dentry);
            continue;
        }

        /*
         * Insert dentry at the head of the list as inserting at the
         * tail leads to a cycle.
         */
        list_add(&dentry->d_lru, &dentry_unused);
        dentry_stat.nr_unused++;
    }
}

/*
 * destroy the dentries attached to a superblock on unmounting
 * - we don't need to use dentry->d_lock, and only need dcache_lock when
 *   removing the dentry from the system lists and hashes because:
 *   - the superblock is detached from all mountings and open files, so the
 *     dentry trees will not be rearranged by the VFS
 *   - s_umount is write-locked, so the memory pressure shrinker will ignore
 *     any dentries belonging to this superblock that it comes across
 *   - the filesystem itself is no longer permitted to rearrange the dentries
 *     in this superblock
 */
void shrink_dcache_for_umount(struct super_block *sb, int force)
{
    struct dentry *loop, *next;
    unsigned detached = 0;

    list_for_each_entry_safe(loop, next, &sb->s_dentrys, d_sb_list)
    {
        if (atomic_read(&loop->d_count) != 0)
        {
            if (force)
            {
                fs_log_error("Dentry %p{i=%x,n=%s}"
                             " still in use (%d), force destroied!\n",
                             loop,
                             loop->d_inode ? loop->d_inode->i_ino : 0UL,
                             loop->d_name.name,
                             atomic_read(&loop->d_count));
            }
            else
            {
                fs_log_warning("not zero file count: %d of %s\n", atomic_read(&loop->d_count),
                               loop->d_name.name);
                continue;
            }
        }

        if (!list_empty(&loop->d_lru))
        {
            dentry_stat.nr_unused--;
            list_del_init(&loop->d_lru);
        }

        detached++;
        prune_one_dentry(loop);
    }

    dentry_stat.nr_dentry -= detached;
}


/*
 * Scan `nr' dentries and return the number which remain.
 *
 * We need to avoid reentering the filesystem if the caller is performing a
 * GFP_NOFS allocation attempt.  One example deadlock is:
 *
 * ext2_new_block->getblk->GFP->shrink_dcache_memory->prune_dcache->
 * prune_one_dentry->dput->dentry_iput->iput->inode->i_sb->s_op->put_inode->
 * ext2_discard_prealloc->ext2_free_blocks->lock_super->DEADLOCK.
 *
 * In this case we return -1 to tell the caller that we baled.
 */
int shrink_dcache_memory(int nr)
{
    prune_dcache(nr, NULL);
    return 0;
}

static unsigned int p_hash(struct dentry *parent,
                           unsigned int hash)
{
    /* construct complicated hash value calcute, to avoid clash */
    //  hash += ((unsigned int)(parent) ^ GOLDEN_RATIO_PRIME) / L1_CACHE_BYTES;
    hash += (parent->d_name.hash ^ GOLDEN_RATIO_PRIME);
    hash += (parent->d_phash_ ^ GOLDEN_RATIO_PRIME);
    hash += (parent->d_pphash_ ^ GOLDEN_RATIO_PRIME);
    hash = hash ^ ((hash ^ GOLDEN_RATIO_PRIME) >> D_HASHBITS);
    return hash;
}

static struct hlist_head *d_hash(unsigned int perent_hash,
                                 unsigned long hash)
{
    hash += (perent_hash ^ GOLDEN_RATIO_PRIME) / L1_CACHE_BYTES;
    hash = hash ^ ((hash ^ GOLDEN_RATIO_PRIME) >> D_HASHBITS);
    return dentry_hashtable + (hash & D_HASHMASK);
}

/**
 * d_alloc  -   allocate a dcache entry
 * @parent: parent of entry to allocate
 * @name: qstr of the name
 *
 * Allocates a dentry. It returns %NULL if there is insufficient memory
 * available. On a success the dentry is returned. The name passed in is
 * copied and the copy passed in may be reused after this call.
 */
struct dentry *d_alloc(struct dentry *parent, const struct qstr *name)
{
    struct dentry *dentry;
    char *dname;

    dentry = kmem_cache_alloc(dentry_cache, GFP_KERNEL);
    if (!dentry)
    {
        return NULL;
    }

    if (name->len > DNAME_INLINE_LEN - 1)
    {
        dname = kmalloc(name->len + 1, GFP_KERNEL);
        if (!dname)
        {
            kmem_cache_free(dentry_cache, dentry);
            return NULL;
        }
    }
    else
    {
        dname = dentry->d_iname;
    }
    dentry->d_name.name = dname;

    dentry->d_name.len = name->len;
    dentry->d_name.hash = name->hash;
    memcpy(dname, name->name, name->len);
    dname[name->len] = 0;

    atomic_set(&dentry->d_count, 1);
    dentry->d_flags = DCACHE_UNHASHED;
    dentry->d_inode = NULL;
    dentry->d_sb = NULL;
    dentry->d_op = NULL;

    dentry->d_phash_  = 0;
    dentry->d_pphash_ = 0;

    INIT_HLIST_NODE(&dentry->d_hash);
    INIT_LIST_HEAD(&dentry->d_lru);
    INIT_LIST_HEAD(&dentry->d_sb_list);

    if (parent)
    {
        dentry->d_pphash_ = parent->d_phash_;
        dentry->d_sb      = parent->d_sb;
        dentry->d_phash_  = p_hash(parent, name->hash);
        //dentry->d_parent = dget(parent);
        list_add(&dentry->d_sb_list, &dentry->d_sb->s_dentrys);
    }
    else
    {
        dentry->d_pphash_ = (uintptr_t)dentry + prand();
        dentry->d_phash_  = p_hash(dentry, name->hash);
    }
    dentry_stat.nr_dentry++;

    return dentry;
}

/**
 * d_instantiate - fill in inode information for a dentry
 * @entry: dentry to complete
 * @inode: inode to attach to this dentry
 *
 * Fill in inode information in the entry.
 *
 * This turns negative dentries into productive full members
 * of society.
 *
 * NOTE! This assumes that the inode count has been incremented
 * (or otherwise set) by the caller to indicate that it is now
 * in use by the dcache.
 */

void d_instantiate(struct dentry *entry, struct inode *inode)
{
    BUG_ON(!list_empty(&entry->d_alias));

#if FSYS_DEBUG_ON
    if (inode)
    {
        inode->debug_dentry = entry;
    }
#endif

    entry->d_inode = inode;
}

/**
 * d_alloc_root - allocate root dentry
 * @root_inode: inode to allocate the root for
 *
 * Allocate a root ("/") dentry for the inode given. The inode is
 * instantiated and returned. %NULL is returned if there is insufficient
 * memory or the inode passed is %NULL.
 */

struct dentry *d_alloc_root(struct inode *root_inode)
{
    struct dentry *res = NULL;

    if (root_inode)
    {
        static const struct qstr name = { .name = "/", .len = 1 };

        res = d_alloc(NULL, &name);
        if (res)
        {
            res->d_sb = root_inode->i_sb;
            //res->d_parent = res;
            d_instantiate(res, root_inode);
        }
    }
    return res;
}


/**
 * d_splice_alias - splice a disconnected dentry into the tree if one exists
 * @inode:  the inode which may have a disconnected dentry
 * @dentry: a negative dentry which we want to point to the inode.
 *
 * If inode is a directory and has a 'disconnected' dentry (i.e. IS_ROOT and
 * DCACHE_DISCONNECTED), then d_move that in place of the given dentry
 * and return it, else simply d_add the inode to the dentry and return NULL.
 *
 * This is needed in the lookup routine of any filesystem that is exportable
 * (via knfsd) so that we can build dcache paths to directories effectively.
 *
 * If a dentry was found and moved, then it is returned.  Otherwise NULL
 * is returned.  This matches the expected return value of ->lookup.
 *
 */
struct dentry *d_splice_alias(struct inode *inode, struct dentry *dentry)
{
    if (inode && S_ISDIR(inode->i_mode))
    {
        dentry->d_inode = inode;
#if FSYS_DEBUG_ON
        inode->debug_dentry = dentry;
#endif
        d_rehash(dentry);
    }
    else
    {
        d_add(dentry, inode);
    }

    return NULL;
}


/**
 * d_lookup - search for a dentry
 * @parent: parent dentry
 * @name: qstr of name we wish to find
 *
 * Searches the children of the parent dentry for the name in question. If
 * the dentry is found its reference count is incremented and the dentry
 * is returned. The caller must use d_put to free the entry when it has
 * finished using it. %NULL is returned on failure.
 *
 * __d_lookup is dcache_lock free. The hash list is protected using RCU.
 * Memory barriers are used while updating and doing lockless traversal.
 * To avoid races with d_move while rename is happening, d_lock is used.
 *
 * Overflows in memcmp(), while d_move, are avoided by keeping the length
 * and name pointer in one structure pointed by d_qstr.
 *
 * rcu_read_lock() and rcu_read_unlock() are used to disable preemption while
 * lookup is going on.
 *
 * dentry_unused list is not updated even if lookup finds the required dentry
 * in there. It is updated in places such as prune_dcache, shrink_dcache_sb,
 * select_parent and __dget_locked. This laziness saves lookup from dcache_lock
 * acquisition.
 *
 * d_lookup() is protected against the concurrent renames in some unrelated
 * directory using the seqlockt_t rename_lock.
 */
struct dentry *__d_lookup(struct dentry *parent, struct qstr *name)
{
    unsigned int len = name->len;
    unsigned int hash = name->hash;
    const unsigned char *str = name->name;
    struct hlist_head *head = d_hash(parent->d_phash_, hash);
    struct dentry *found = NULL;
    struct hlist_node *node;
    struct dentry *dentry;

    hlist_for_each_entry_rcu(dentry, node, head, d_hash)
    {
        struct qstr *qstr;


        if (dentry->d_name.hash != hash)
        {
            continue;
        }
        if (dentry->d_pphash_ != parent->d_phash_)
        {
            continue;
        }
        if (dentry->d_phash_ != p_hash(parent, name->hash))
        {
            continue;
        }
        /*
         * It is safe to compare names since d_move() cannot
         * change the qstr (protected by d_lock).
         */
        qstr = &dentry->d_name;
        if (parent->d_op && parent->d_op->d_compare)
        {
            if (parent->d_op->d_compare(parent, qstr, name))
            {
                goto next;
            }
        }
        else
        {
            if (qstr->len != len)
            {
                goto next;
            }
            if (memcmp(qstr->name, str, len))
            {
                goto next;
            }
        }

        if (!d_unhashed(dentry))
        {
            atomic_inc(&dentry->d_count);
            found = dentry;
        }
        break;
next:
        continue;
    }

    return found;
}

/**
 * d_hash_and_lookup - hash the qstr then search for a dentry
 * @dir: Directory to search in
 * @name: qstr of name we wish to find
 *
 * On hash failure or on lookup failure NULL is returned.
 */
struct dentry *d_hash_and_lookup(struct dentry *dir, struct qstr *name)
{
    struct dentry *dentry = NULL;

    /*
     * Check for a fs-specific hash function. Note that we must
     * calculate the standard hash first, as the d_op->d_hash()
     * routine may choose to leave the hash value unchanged.
     */
    name->hash = full_name_hash(name->name, name->len);
    if (dir->d_op && dir->d_op->d_hash)
    {
        if (dir->d_op->d_hash(dir, name) < 0)
        {
            goto out;
        }
    }
    dentry = __d_lookup(dir, name);
out:
    return dentry;
}

/*
 * When a file is deleted, we have two options:
 * - turn this dentry into a negative dentry
 * - unhash this dentry and free it.
 *
 * Usually, we want to just turn this into
 * a negative dentry, but if anybody else is
 * currently using the dentry or the inode
 * we can't do that and we fall back on removing
 * it from the hash queues and waiting for
 * it to be deleted later when it has no users
 */

/**
 * d_delete - delete a dentry
 * @dentry: The dentry to delete
 *
 * Turn the dentry into a negative dentry if possible, otherwise
 * remove it from the hash queues so it can be deleted later
 */

void d_delete(struct dentry *dentry)
{
    int isdir = 0;
    /*
     * Are we the only user?
     */
    isdir = S_ISDIR(dentry->d_inode->i_mode);
    if (atomic_read(&dentry->d_count) == 1)
    {
        dentry_iput(dentry);

        /* remove this and other inotify debug checks after 2.6.18 */
        dentry->d_flags &= ~DCACHE_INOTIFY_PARENT_WATCHED;
        return;
    }

    if (!d_unhashed(dentry))
    {
        __d_drop(dentry);
    }
}

static void __d_rehash(struct dentry *entry, struct hlist_head *list)
{

    entry->d_flags &= ~DCACHE_UNHASHED;
    hlist_add_head_rcu(&entry->d_hash, list);
}

static void _d_rehash(struct dentry *entry)
{
    __d_rehash(entry, d_hash(entry->d_pphash_, entry->d_name.hash));
}

/**
 * d_rehash - add an entry back to the hash
 * @entry: dentry to add to the hash
 *
 * Adds a dentry to the hash according to its name.
 */

void d_rehash(struct dentry *entry)
{
    _d_rehash(entry);
}

/*
 * We cannibalize "target" when moving dentry on top of it,
 * because it's going to be thrown away anyway. We could be more
 * polite about it, though.
 *
 * This forceful removal will result in ugly /proc output if
 * somebody holds a file open that got deleted due to a rename.
 * We could be nicer about the deleted file, and let it show
 * up under the name it had before it was deleted rather than
 * under the original name of the file that was moved on top of it.
 */

/*
 * d_move_locked - move a dentry
 * @dentry: entry to move
 * @target: new dentry
 *
 * Update the dcache to reflect the move of a file name. Negative
 * dcache entries should not be moved in this way.
 */
void d_move(struct dentry *dentry, struct dentry *target)
{
    struct hlist_head *list;

    if (!dentry->d_inode)
    {
        fs_log_warning("VFS: moving negative dcache entry\n");
    }

    /* Move the dentry to the target hash queue, if on different bucket */
    if (d_unhashed(dentry))
    {
        goto already_unhashed;
    }

    hlist_del_rcu(&dentry->d_hash);

already_unhashed:
    list = d_hash(target->d_phash_, target->d_name.hash);
    __d_rehash(dentry, list);

    /* Unhash the target: dput() will then get rid of it */
    __d_drop(target);
}

static unsigned long dhash_entries = DENTRY_INODE_CNT;

static void dcache_init(unsigned long mempages)
{
    int loop;

    /*
     * A constructor could be added for stable state like the lists,
     * but it is probably not worth it because of the cache nature
     * of the dcache.
     * If fragmentation is too bad then the SLAB_HWCACHE_ALIGN
     * flag could be removed here, to hint to the allocator that
     * it should not try to get multiple page regions.
     */
    dentry_cache = kmem_cache_create("dentry_cache",
                                     sizeof(struct dentry),
                                     0,
                                     SLAB_HWCACHE_ALIGN,
                                     NULL, NULL);
    if (!dentry_cache)
    {
        fs_log_critical("Cannot create dentry cache");
    }

    dentry_hashtable =
        alloc_large_system_hash("Dentry cache",
                                sizeof(struct hlist_head),
                                dhash_entries,
                                13,
                                0,
                                &d_hash_shift,
                                &d_hash_mask,
                                0);

    for (loop = 0; loop < (1 << d_hash_shift); loop++)
    {
        INIT_HLIST_HEAD(&dentry_hashtable[loop]);
    }

}

/* SLAB cache for __getname() consumers,
 * kernal path name allocate from page pool directly,
 * By sunny at 2010-6-12.
 */
//kmem_cache_t *names_cachep;

/* SLAB cache for file structures */
kmem_cache_t *filp_cachep;

int vfs_caches_init(unsigned long mempages)
{
    filp_cachep = kmem_cache_create("filp", sizeof(struct file), 0,
                                    SLAB_HWCACHE_ALIGN, NULL, NULL);

    dcache_init(mempages);
    inode_init(mempages);
    //  files_init(mempages);
    return 0;
}

/*-----------------------------debug ---------------------------*/
#if     FSYS_DEBUG_ON
void debug_show_all_dentry(char *info)
{
    struct super_block *sb;

    fs_log_info("%s", info);

    list_for_each_entry(sb, &super_blocks, s_list)
    {
        debug_show_sb_dentry(&sb->s_dentrys, "dentry on sb->s_dentry:", 0);
        debug_show_lru_dentry(&dentry_unused, "dentry on unused list", 0);
    }

}
#endif  /* FSYS_DEBUG_ON */
