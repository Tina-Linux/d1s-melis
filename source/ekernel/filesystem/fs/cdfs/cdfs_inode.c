/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : cdfs_inode.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-16
* Descript: isofs inode handling functions.
* Update  : date                auther      ver     notes
*           2011-3-16 13:53:48  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "slab.h"
#include "nls.h"
#include "fsys_debug.h"
#include "err.h"
#include "cdfs.h"
#include "cdfs_compress.h"

static kmem_cache_t *isofs_inode_cachep;

struct inode *isofs_alloc_inode(struct super_block *sb)
{
    struct iso_inode_info *ei;
    ei = kmem_cache_alloc(isofs_inode_cachep, GFP_KERNEL);
    if (!ei)
    {
        return NULL;
    }
    return &ei->vfs_inode;
}

void isofs_destroy_inode(struct inode *inode)
{
    kmem_cache_free(isofs_inode_cachep, ISOFS_I(inode));
}

static void iso_init_once(void *foo, kmem_cache_t *cachep, unsigned long flags)
{
    struct iso_inode_info *ei = foo;

    inode_init_once(&ei->vfs_inode);
}

int isofs_init_inodecache(void)
{
    isofs_inode_cachep = kmem_cache_create("isofs_inode_cache",
                                           sizeof(struct iso_inode_info),
                                           0, SLAB_HWCACHE_ALIGN,
                                           /*(SLAB_RECLAIM_ACCOUNT|
                                           SLAB_MEM_SPREAD),*/
                                           iso_init_once, NULL);
    if (isofs_inode_cachep == NULL)
    {
        return -ENOMEM;
    }
    return 0;
}

void isofs_destroy_inodecache(void)
{
    kmem_cache_destroy(isofs_inode_cachep);
}

/*
 * Get a set of blocks; filling in buffer_heads if already allocated
 * or getblk() if they are not.  Returns the number of blocks inserted
 * (0 == error.)
 */
int isofs_get_blocks(struct inode *inode,
                     sector_t iblock,
                     sector_t *phys,
                     unsigned long *max_blocks)
{
    unsigned long b_off;
    unsigned offset, sect_size;
    unsigned int firstext;
    unsigned long nextblk, nextoff;
    unsigned long map_blocks = 0;
    int section;
    struct iso_inode_info *ei = ISOFS_I(inode);
    sector_t last_iblock;

    /* we can't walk beyond the end of the file */
    last_iblock = (inode->i_size + ISOFS_BUFFER_SIZE(inode) - 1)
                  >> ISOFS_BUFFER_BITS(inode);
    if (iblock > last_iblock)
    {
        fs_log_debug("%s: block >= EOF (%ld, %ld)\n",
                     __func__, iblock, (unsigned long)inode->i_size);
        return -EIO;
    }

    b_off = iblock;
    offset = 0;
    firstext = ei->i_first_extent;
    sect_size = ei->i_section_size >> ISOFS_BUFFER_BITS(inode);
    nextblk = ei->i_next_section_block;
    nextoff = ei->i_next_section_offset;
    section = 0;
    /*
     * FIXME : if we encount critical preformance problem,
     *         we should cache every file section information,
     *         the implement can use one file sections table
     *         in iso_inode_info for this,
     *         this method can locate iblock number location more faster.
     *
     *  Sunny, 2009-11-11
     */

    /* first step : walk to  goal section,
     * every time will walk from the first section location
     */
    while (nextblk && (b_off >= (offset + sect_size)))
    {
        struct inode *ninode;

        offset += sect_size;
        ninode = isofs_iget(inode->i_sb, nextblk, nextoff);
        if (!ninode)
        {
            return -EIO;
        }
        firstext  = ISOFS_I(ninode)->i_first_extent;
        sect_size = ISOFS_I(ninode)->i_section_size >> ISOFS_BUFFER_BITS(ninode);
        nextblk   = ISOFS_I(ninode)->i_next_section_block;
        nextoff   = ISOFS_I(ninode)->i_next_section_offset;
        iput(ninode);

        /* max section number limit */
        if (++section > 100)
        {
            fs_log_debug("%s: More than 100 file sections ?!?"
                         " aborting...\n", __func__);
            fs_log_debug("%s: block=%ld firstext=%u sect_size=%u "
                         "nextblk=%lu nextoff=%lu\n", __func__,
                         iblock, firstext, (unsigned) sect_size,
                         nextblk, nextoff);
            return -EIO;
        }
    }

    /* second step : walk to goal block */
    *phys = firstext + b_off - offset;

    /*
     * all left blocks in last section
     * FIXME : maybe too big
     *
     * we can't get blocks number beyond
     * the value of *max_blocks.
     * By Sunny 2010.3.11
     */
    map_blocks = sect_size - (b_off - offset);
    *max_blocks = min_t(unsigned long, map_blocks, *max_blocks);

    return 0;
}

/*
 * Used by the standard interfaces.
 */
int isofs_get_block(struct inode *inode,
                    int iblock,
                    unsigned long *max_blocks,
                    sector_t *phys,
                    int create)
{
    if (create)
    {
        fs_log_debug("%s: Kernel tries to allocate a block\n", __func__);
        return -EROFS;
    }

    return isofs_get_blocks(inode, iblock, phys, max_blocks);
}

static sector_t isofs_bmap(struct inode *inode, sector_t block)
{
    int error;
    unsigned long blocks;
    sector_t phys;

    blocks = 1;
    error = isofs_get_block(inode, block, &blocks, &phys, 0);
    if (!error)
    {
        return phys;
    }
    return 0;
}

struct buffer_head *isofs_bread(struct inode *inode, sector_t block)
{
    sector_t blknr = isofs_bmap(inode, block);
    if (!blknr)
    {
        return NULL;
    }
    return sb_bread(inode->i_sb, blknr);
}

//static inline void test_and_set_uid(uid_t *p, uid_t value)
//{
//  if (value)
//      *p = value;
//}
//
//static inline void test_and_set_gid(gid_t *p, gid_t value)
//{
//       if (value)
//                *p = value;
//}

static int isofs_read_level3_size(struct inode *inode)
{
    unsigned long bufsize = ISOFS_BUFFER_SIZE(inode);
    int high_sierra = ISOFS_SB(inode->i_sb)->s_high_sierra;
    struct buffer_head *bh = NULL;
    unsigned long block, offset, block_saved, offset_saved;
    int i = 0;
    int more_entries = 0;
    struct iso_directory_record *tmpde = NULL;
    struct iso_inode_info *ei = ISOFS_I(inode);

    inode->i_size = 0;

    /* The first 16 blocks are reserved as the System Area.  Thus,
     * no inodes can appear in block 0.  We use this to flag that
     * this is the last section. */
    ei->i_next_section_block = 0;
    ei->i_next_section_offset = 0;

    block = ei->i_iget5_block;
    offset = ei->i_iget5_offset;

    do
    {
        struct iso_directory_record *de;
        unsigned int de_len;

        if (!bh)
        {
            bh = sb_bread(inode->i_sb, block);
            if (!bh)
            {
                goto out_noread;
            }
        }
        de = (struct iso_directory_record *)(bh->b_data + offset);
        de_len = *(unsigned char *) de;

        if (de_len == 0)
        {
            brelse(bh);
            bh = NULL;
            ++block;
            offset = 0;
            continue;
        }

        block_saved = block;
        offset_saved = offset;
        offset += de_len;

        /* Make sure we have a full directory entry */
        if (offset >= bufsize)
        {
            int slop = bufsize - offset + de_len;
            if (!tmpde)
            {
                tmpde = kmalloc(256, GFP_KERNEL);
                if (!tmpde)
                {
                    goto out_nomem;
                }
            }
            memcpy(tmpde, de, slop);
            offset &= bufsize - 1;
            block++;
            brelse(bh);
            bh = NULL;
            if (offset)
            {
                bh = sb_bread(inode->i_sb, block);
                if (!bh)
                {
                    goto out_noread;
                }
                //              memcpy((void *)tmpde+slop, bh->b_data, offset);
                memcpy((char *)tmpde + slop, bh->b_data, offset);
            }
            de = tmpde;
        }

        inode->i_size += isonum_733(de->size);
        if (i == 1)
        {
            ei->i_next_section_block = block_saved;
            ei->i_next_section_offset = offset_saved;
        }

        more_entries = de->flags[-high_sierra] & 0x80;

        i++;
        if (i > 100)
        {
            goto out_toomany;
        }
    } while (more_entries);
out:
    kfree(tmpde);
    if (bh)
    {
        brelse(bh);
    }
    return 0;

out_nomem:
    if (bh)
    {
        brelse(bh);
    }
    return -ENOMEM;

out_noread:
    fs_log_info("ISOFS: unable to read i-node block %lu\n", block);
    kfree(tmpde);
    return -EIO;

out_toomany:
    fs_log_info("%s: More than 100 file sections ?!?, aborting...\n"
                "isofs_read_level3_size: inode=%lu\n",
                __func__, inode->i_ino);
    goto out;
}

void isofs_read_inode(struct inode *inode)
{
    struct super_block *sb = inode->i_sb;
    struct isofs_sb_info *sbi = ISOFS_SB(sb);
    unsigned long bufsize = ISOFS_BUFFER_SIZE(inode);
    unsigned long block;
    int high_sierra = sbi->s_high_sierra;
    struct buffer_head *bh = NULL;
    struct iso_directory_record *de;
    struct iso_directory_record *tmpde = NULL;
    unsigned int de_len;
    unsigned long offset;
    struct iso_inode_info *ei = ISOFS_I(inode);

    block = ei->i_iget5_block;
    bh = sb_bread(inode->i_sb, block);
    if (!bh)
    {
        goto out_badread;
    }

    offset = ei->i_iget5_offset;

    de = (struct iso_directory_record *)(bh->b_data + offset);
    de_len = *(unsigned char *) de;

    if (offset + de_len > bufsize)
    {
        int frag1 = bufsize - offset;

        tmpde = kmalloc(de_len, GFP_KERNEL);
        if (tmpde == NULL)
        {
            fs_log_info("%s: out of memory\n", __func__);
            goto fail;
        }
        memcpy(tmpde, bh->b_data + offset, frag1);
        brelse(bh);
        bh = sb_bread(inode->i_sb, ++block);
        if (!bh)
        {
            goto out_badread;
        }
        memcpy((char *)tmpde + frag1, bh->b_data, de_len - frag1);
        de = tmpde;
    }

    inode->i_ino = isofs_get_ino(ei->i_iget5_block,
                                 ei->i_iget5_offset,
                                 ISOFS_BUFFER_BITS(inode));

    /* Assume it is a normal-format file unless told otherwise */
    ei->i_file_format = isofs_file_normal;

    if (de->flags[-high_sierra] & 2)
    {
        inode->i_mode = S_IRUGO | S_IXUGO | S_IFDIR;
        inode->i_nlink = 1; /*
                     * Set to 1.  We know there are 2, but
                     * the find utility tries to optimize
                     * if it is 2, and it screws up.  It is
                     * easier to give 1 which tells find to
                     * do it the hard way.
                     */
    }
    else
    {
        /* Everybody gets to read the file. */
        inode->i_mode = sbi->s_mode;
        inode->i_nlink = 1;
        inode->i_mode |= S_IFREG;
    }
    //  inode->i_uid = sbi->s_uid;
    //  inode->i_gid = sbi->s_gid;
    inode->i_blocks = 0;

    ei->i_format_parm[0] = 0;
    ei->i_format_parm[1] = 0;
    ei->i_format_parm[2] = 0;

    ei->i_section_size = isonum_733(de->size);
    if (de->flags[-high_sierra] & 0x80)
    {
        if (isofs_read_level3_size(inode))
        {
            goto fail;
        }
    }
    else
    {
        ei->i_next_section_block = 0;
        ei->i_next_section_offset = 0;
        inode->i_size = isonum_733(de->size);
    }

    /*
     * Some dipshit decided to store some other bit of information
     * in the high byte of the file length.  Truncate size in case
     * this CDROM was mounted with the cruft option.
     */

    if (sbi->s_cruft == 'y')
    {
        inode->i_size &= 0x00ffffff;
    }

    if (de->interleave[0])
    {
        fs_log_debug("ISOFS: Interleaved files not (yet) supported.\n");
        inode->i_size = 0;
    }

    /* I have no idea what file_unit_size is used for, so
     * we will flag it for now */
    if (de->file_unit_size[0] != 0)
    {
        fs_log_debug("ISOFS: File unit size != 0,"
                     "for ISO file (%ld).\n", inode->i_ino);
    }

    /* I have no idea what other flag bits are used for, so
       we will flag it for now */
    if ((de->flags[-high_sierra] & ~2) != 0)
    {
        fs_log_debug("ISOFS: Unusual flag settings for ISO file "
                     "(%ld %x).\n",
                     inode->i_ino, de->flags[-high_sierra]);
    }

    inode->i_mtime.tv_sec =
        inode->i_atime.tv_sec =
            inode->i_ctime.tv_sec = iso_date(de->date, high_sierra);
    inode->i_mtime.tv_nsec =
        inode->i_atime.tv_nsec =
            inode->i_ctime.tv_nsec = 0;

    ei->i_first_extent = (isonum_733(de->extent) +
                          isonum_711(de->ext_attr_length));

    /* Set the number of blocks for stat() - should be done before RR */
    inode->i_blocks = (inode->i_size + 511) >> 9;

    /*
     * Now test for possible Rock Ridge extensions which will override
     * some of these numbers in the inode structure.
     */
    if (!high_sierra)
    {
        parse_rock_ridge_inode(de, inode);
        /* if we want uid/gid set, override the rock ridge setting */
        //      test_and_set_uid(&inode->i_uid, sbi->s_uid);
        //      test_and_set_gid(&inode->i_gid, sbi->s_gid);
    }

    /* Install the inode operations vector */
    if (S_ISREG(inode->i_mode))
    {
        inode->i_fop = &isofs_file_operations;
        /*
                switch (ei->i_file_format) {
        #ifdef CONFIG_ZISOFS
                case isofs_file_compressed:
                    inode->i_data.a_ops = &zisofs_aops;
                    break;
        #endif
                default:
                    //inode->i_data.a_ops = &isofs_aops;
                    break;
                }
        */
    }
    else if (S_ISDIR(inode->i_mode))
    {
        inode->i_op = &isofs_dir_inode_operations;
        inode->i_fop = &isofs_dir_operations;
    }
    else if (S_ISLNK(inode->i_mode))
    {
        /* not support under epdk */
        //      inode->i_op = &page_symlink_inode_operations;
        //      inode->i_data.a_ops = &isofs_symlink_aops;
        inode->i_op = NULL;
        inode->i_fop = NULL;
    }
    else
    {
        /* XXX - parse_rock_ridge_inode() had already set i_rdev. */
        init_special_inode(inode, inode->i_mode, 0);
    }

out:
    kfree(tmpde);
    if (bh)
    {
        brelse(bh);
    }
    return;

out_badread:
    fs_log_warning("ISOFS: unable to read i-node block\n");
fail:
    make_bad_inode(inode);
    goto out;
}

struct isofs_iget5_callback_data
{
    unsigned long block;
    unsigned long offset;
};

static int isofs_iget5_test(struct inode *ino, void *data)
{
    struct iso_inode_info *i = ISOFS_I(ino);
    struct isofs_iget5_callback_data *d =
        (struct isofs_iget5_callback_data *)data;
    return (i->i_iget5_block == d->block)
           && (i->i_iget5_offset == d->offset);
}

static int isofs_iget5_set(struct inode *ino, void *data)
{
    struct iso_inode_info *i = ISOFS_I(ino);
    struct isofs_iget5_callback_data *d =
        (struct isofs_iget5_callback_data *)data;
    i->i_iget5_block = d->block;
    i->i_iget5_offset = d->offset;
    return 0;
}

/* Store, in the inode's containing structure, the block and block
 * offset that point to the underlying meta-data for the inode.  The
 * code below is otherwise similar to the iget() code in
 * include/linux/fs.h */
struct inode *isofs_iget(struct super_block *sb,
                         unsigned long block,
                         unsigned long offset)
{
    unsigned long hashval;
    struct inode *inode;
    struct isofs_iget5_callback_data data;

    if (offset >= 1ul << sb->s_blocksize_bits)
    {
        return NULL;
    }

    data.block = block;
    data.offset = offset;

    hashval = (block << sb->s_blocksize_bits) | offset;

    inode = iget5_locked(sb, hashval,
                         &isofs_iget5_test,
                         &isofs_iget5_set,
                         &data);

    if (inode && (inode->i_state & I_NEW))
    {
        sb->s_op->read_inode(inode);
        unlock_new_inode(inode);
    }
    return inode;
}

