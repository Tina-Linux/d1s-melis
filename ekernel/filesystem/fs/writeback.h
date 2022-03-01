/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : writeback.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: code is extracted from linux.
* Update  : date                auther      ver     notes
*           2011-3-15 15:50:31  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __WRITEBACK_H__
#define __WRITEBACK_H__

extern struct list_head inode_in_use;
extern struct list_head inode_unused;

/*
 * fs/fs-writeback.c
 */
enum writeback_sync_modes
{
    WB_SYNC_NONE,   /* Don't wait on anything */
    WB_SYNC_ALL,    /* Wait on every mapping */
    WB_SYNC_HOLD,   /* Hold the inode on sb_dirty for sys_sync() */
};

/*
 * A control structure which tells the writeback code what to do.  These are
 * always on the stack, and hence need no locking.  They are always initialised
 * in a manner such that unspecified fields are set to zero.
 */
struct writeback_control
{
    enum writeback_sync_modes sync_mode;
    unsigned long *older_than_this; /* If !NULL, only write back inodes
                       older than this */
    long nr_to_write;       /* Write this many pages, and decrement
                       this for each page written */
    long pages_skipped;     /* Pages which were not written */

    /*
     * For a_ops->writepages(): is start or end are non-zero then this is
     * a hint that the filesystem need only write out the pages inside that
     * byterange.  The byte at `end' is included in the writeout request.
     */
    __s64 range_start;
    __s64 range_end;

    unsigned nonblocking: 1;    /* Don't get stuck on request queues */
    unsigned encountered_congestion: 1; /* An output: a queue is full */
    unsigned for_kupdate: 1;    /* A kupdate writeback */
    unsigned for_reclaim: 1;    /* Invoked from the page allocator */
    unsigned for_writepages: 1; /* This is a writepages() call */
    unsigned range_cyclic: 1;   /* range_start is cyclic */

    void *fs_private;       /* For use by ->writepages() */
};

/*
 * fs/fs-writeback.c
 */
void writeback_inodes(struct writeback_control *wbc);
void wake_up_inode(struct inode *inode);
int inode_wait(void *);
void sync_part_inodes(struct super_block *sb, struct writeback_control *wbc);
void sync_parts_inodes(struct writeback_control *wbc);

//void balance_dirty_inodes(int write_chunk);

#endif  /* __WRITEBACK_H__ */
