/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : buffer_head.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: everything to do with buffer_heads.
*           code is extracted from linux.
*
* Update  : date                auther      ver     notes
*           2011-3-15 15:30:36  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __BUFFER_HEAD_H__
#define __BUFFER_HEAD_H__

#include "fs.h"
#include "supports.h"
#include "atomic.h"
#include "list.h"

#define CONFIG_BLOCK

#ifdef CONFIG_BLOCK

enum bh_state_bits
{
    BH_Uptodate,    /* 1 if the buffer contains valid data */
    BH_Dirty,   /* 1 if the buffer is dirty */
    BH_Lock,    /* 1 if the buffer is locked */
    BH_Req,     /* 0 if the buffer has been invalidated */
    BH_Mapped,  /* 1 if the buffer has a disk mapping */
    BH_New,     /* 1 if the buffer is new and not yet written out */
    BH_Async,   /* 1 if the buffer is under end_buffer_io_async I/O */
    BH_Wait_IO, /* 1 if we should throttle on this buffer */
    BH_Refered,

    BH_PrivateStart,/* not a state bit, but the first bit available
             * for private allocation by other entities
             */
};

#define MAX_BUF_PER_PAGE (PAGE_CACHE_SIZE / 512)

#define NODEV           0
#define B_FREE          0xffffffff

struct page;
struct buffer_head;
struct address_space;
typedef void (bh_end_io_t)(struct buffer_head *bh, int uptodate);

/*
 * Historically, a buffer_head was used to map a single block
 * within a page, and of course as the unit of I/O through the
 * filesystem and block layers.  Nowadays the basic I/O unit
 * is the bio, and buffer_heads are used for extracting block
 * mappings (via a get_block_t call), for tracking state within
 * a page (via a page_mapping) and for wrapping bio submission
 * for backward compatibility reasons (e.g. submit_bh).
 */
struct buffer_head
{
    /* First cache line: */
    struct buffer_head *b_next; /* Hash queue list */
    unsigned long b_blocknr;    /* block number */
    unsigned short b_size;      /* block size */
    unsigned short b_list;      /* List that this buffer appears */
    struct super_block *b_sb;

    atomic_t b_count;       /* users using this block */
    //  kdev_t b_rdev;          /* Real device */
    unsigned long b_state;      /* buffer state bitmap (see above) */
    unsigned long b_flushtime;  /* Time when (dirty) buffer should be written */

    struct buffer_head *b_next_free;/* lru/free list linkage */
    struct buffer_head *b_prev_free;/* doubly linked list of buffers */
    struct buffer_head *b_this_page;/* circular list of buffers in one page */
    //  struct buffer_head *b_reqnext;  /* request queue */

    struct buffer_head **b_pprev;   /* doubly linked list of hash-queue */
    char *b_data;           /* pointer to data block */
    struct page *b_page;        /* the page this bh is mapped to */
    void (*b_end_io)(struct buffer_head *bh, int uptodate); /* I/O completion */
    //  void *b_private;        /* reserved for b_end_io */

    //  unsigned long b_rsector;    /* Real buffer location on disk */
    //  wait_queue_head_t b_wait;

    struct inode        *b_inode;
    struct list_head     b_inode_buffers;   /* doubly linked list of inode dirty buffers */
};

typedef void (bh_end_io_t)(struct buffer_head *bh, int uptodate);
void init_buffer(struct buffer_head *, bh_end_io_t *, void *);

#define __buffer_state(bh, state)   (((bh)->b_state & (1UL << BH_##state)) != 0)

#define buffer_uptodate(bh) __buffer_state(bh,Uptodate)
#define buffer_dirty(bh)    __buffer_state(bh,Dirty)
#define buffer_locked(bh)   __buffer_state(bh,Lock)
#define buffer_req(bh)      __buffer_state(bh,Req)
#define buffer_mapped(bh)   __buffer_state(bh,Mapped)
#define buffer_new(bh)      __buffer_state(bh,New)
#define buffer_async(bh)    __buffer_state(bh,Async)
#define Buffer_Refered(bh)  __buffer_state(bh,Refered)

#define bh_offset(bh)       ((unsigned long)(bh)->b_data & ~PAGE_MASK)

extern void set_bh_page(struct buffer_head *bh, struct page *page, unsigned long offset);

#define touch_buffer(bh)    SetPageReferenced(bh->b_page)

/*
 * Emit the buffer bitops functions.   Note that there are also functions
 * of the form "mark_buffer_foo()".  These are higher-level functions which
 * do something in addition to setting a b_state bit.
 */
/* If we *know* page->private refers to buffer_heads */
#define page_buffers(page)                  \
    ({                          \
        BUG_ON(!PagePrivate(page));         \
        ((struct buffer_head *)page_private(page)); \
    })
#define page_has_buffers(page)  PagePrivate(page)

/*
 * Declarations
 */

void mark_buffer_dirty(struct buffer_head *bh);
void init_buffer(struct buffer_head *, bh_end_io_t *, void *);
void set_bh_page(struct buffer_head *bh,
                 struct page *page, unsigned long offset);
int try_to_free_buffers(struct page *, unsigned int);
struct buffer_head *alloc_page_buffers(struct page *page, unsigned long size,
                                       int retry);
//void create_empty_buffers(struct page *, __hdle,
//          unsigned long b_state);
void end_buffer_read_sync(struct buffer_head *bh, int uptodate);
void end_buffer_write_sync(struct buffer_head *bh, int uptodate);
void invalidate_buffers(struct super_block *, int destroy_dirty_buffers, int update);

/* Things to do with buffers at mapping->private_list */
void mark_buffer_dirty_inode(struct buffer_head *bh, struct inode *inode);
int inode_has_buffers(struct inode *);
void invalidate_inode_buffers(struct inode *);
int remove_inode_buffers(struct inode *inode);
int sync_mapping_buffers(struct address_space *mapping);
void unmap_underlying_metadata(struct buffer_head *);

void mark_buffer_async_write(struct buffer_head *bh);
void __wait_on_buffer(struct buffer_head *);
//wait_queue_head_t *bh_waitq_head(struct buffer_head *bh);
int fsync_super(struct super_block *);
struct buffer_head *__getblk(struct super_block *, int block,
                             int size);
void __brelse(struct buffer_head *);
void __bforget(struct buffer_head *);
//void __breadahead(struct block_device *, int block, unsigned int size);
struct buffer_head *__bread(struct super_block *, int block, int size);
void invalidate_bh_lrus(void);
void free_buffer_head(struct buffer_head *bh);
void unlock_buffer(struct buffer_head *bh);
void __lock_buffer(struct buffer_head *bh);
void ll_rw_block(int, int, struct buffer_head *bh[]);
int sync_dirty_buffer(struct buffer_head *bh);
int submit_bh(int, struct buffer_head *);

extern int buffer_heads_over_limit;

///*
// * Generic address_space_operations implementations for buffer_head-backed
// * address_spaces.
// */
//void block_invalidatepage(struct page *page, unsigned long offset);
//int block_write_full_page(struct page *page, get_block_t *get_block,
//              struct writeback_control *wbc);
//int block_read_full_page(struct page*, get_block_t*);
//int block_prepare_write(struct page*, unsigned, unsigned, get_block_t*);
//int cont_prepare_write(struct page*, unsigned, unsigned, get_block_t*,
//              __s64 *);
//int generic_cont_expand(struct inode *inode, __s64 size);
//int generic_cont_expand_simple(struct inode *inode, __s64 size);
//int block_commit_write(struct page *page, unsigned from, unsigned to);
//void block_sync_page(struct page *);
//int generic_block_bmap(struct address_space *, int, get_block_t *);
//int generic_commit_write(struct file *, struct page *, unsigned, unsigned);
//int block_truncate_page(struct address_space *, __s64, get_block_t *);
//int file_fsync(struct file *, struct dentry *, int);
//int nobh_prepare_write(struct page*, unsigned, unsigned, get_block_t*);
//int nobh_commit_write(struct file *, struct page *, unsigned, unsigned);
//int nobh_truncate_page(struct address_space *, __s64);
//int nobh_writepage(struct page *page, get_block_t *get_block,
//                        struct writeback_control *wbc);


/*
 * inline definitions
 */

//static inline void attach_page_buffers(struct page *page,
//      struct buffer_head *head)
//{
//  page_cache_get(page);
//  SetPagePrivate(page);
//  set_page_private(page, (unsigned long)head);
//}

#define BUF_CLEAN   0
#define BUF_LOCKED  1   /* Buffers scheduled for write */
#define BUF_DIRTY   2   /* Dirty buffers, not yet scheduled for write */
#define NR_LIST     3

static inline void get_bh(struct buffer_head *bh)
{
    atomic_inc(&bh->b_count);
}

static inline void put_bh(struct buffer_head *bh)
{
    //        smp_mb__before_atomic_dec();
    atomic_dec(&bh->b_count);
}

/*
 * This is called by bh->b_end_io() handlers when I/O has completed.
 */
static inline void mark_buffer_uptodate(struct buffer_head *bh, int on)
{
    if (on)
    {
        set_bit(BH_Uptodate, &bh->b_state);
    }
    else
    {
        clear_bit(BH_Uptodate, &bh->b_state);
    }
}

static inline void set_buffer_uptodate(struct buffer_head *bh)
{
    set_bit(BH_Uptodate, &(bh)->b_state);
}
static inline void set_buffer_refered(struct buffer_head *bh)
{
    set_bit(BH_Refered, &(bh)->b_state);
}
static inline void clear_buffer_refered(struct buffer_head *bh)
{
    clear_bit(BH_Refered, &(bh)->b_state);
}
#define atomic_set_buffer_clean(bh) test_and_clear_bit(BH_Dirty, &(bh)->b_state)
void refile_buffer(struct buffer_head *);
static inline void __mark_buffer_clean(struct buffer_head *bh)
{
    refile_buffer(bh);
}

static inline void mark_buffer_clean(struct buffer_head *bh)
{
    if (atomic_set_buffer_clean(bh))
    {
        __mark_buffer_clean(bh);
    }
}


#define atomic_set_buffer_dirty(bh) test_and_set_bit(BH_Dirty, &(bh)->b_state)

static inline int test_set_buffer_dirty(struct buffer_head *bh)
{
    return test_and_set_bit(BH_Dirty, &(bh)->b_state);
}
static inline int test_clear_buffer_dirty(struct buffer_head *bh)
{
    return test_and_clear_bit(BH_Dirty, &(bh)->b_state);
}
static inline int test_set_buffer_locked(struct buffer_head *bh)
{
    return test_and_set_bit(BH_Lock, &(bh)->b_state);
}

static inline void mark_buffer_async(struct buffer_head *bh, int on)
{
    if (on)
    {
        set_bit(BH_Async, &bh->b_state);
    }
    else
    {
        clear_bit(BH_Async, &bh->b_state);
    }
}

static inline void brelse(struct buffer_head *bh)
{
    if (bh)
    {
        __brelse(bh);
    }
}

static inline void bforget(struct buffer_head *bh)
{
    if (bh)
    {
        __bforget(bh);
    }
}

static inline struct buffer_head *sb_bread(struct super_block *sb, int block)
{
    return __bread(sb, block,   sb->s_blocksize);
}

static inline void sb_breadahead(struct super_block *sb, int block)
{
    //  __breadahead(sb->s_bdev, block, sb->s_blocksize);
}

static inline struct buffer_head *sb_getblk(struct super_block *sb, int block)
{
    return __getblk(sb, block, sb->s_blocksize);
}

/*
 * Calling wait_on_buffer() for a zero-ref buffer is illegal, so we call into
 * __wait_on_buffer() just to trip a debug check.  Because debug code in inline
 * functions is bloaty.
 */
static inline void wait_on_buffer(struct buffer_head *bh)
{
    //  might_sleep();
    //  if (buffer_locked(bh) || atomic_read(&bh->b_count) == 0)
    //      __wait_on_buffer(bh);
}

static inline void lock_buffer(struct buffer_head *bh)
{
    //  might_sleep();
    //  if (test_set_buffer_locked(bh))
    //      __lock_buffer(bh);
}

extern int __set_page_dirty_buffers(struct page *page);

extern struct buffer_head *get_hash_table(struct super_block *sb, int block, int size);

#endif /* CONFIG_BLOCK */
#endif /* __BUFFER_HEAD_H__ */
