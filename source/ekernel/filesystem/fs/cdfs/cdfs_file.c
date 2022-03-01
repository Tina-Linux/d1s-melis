/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : cdfs_file.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-16
* Descript: isofs file handling functions.
* Update  : date                auther      ver     notes
*           2011-3-16 13:52:48  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "fs.h"
#include "err.h"
#include "buffer_head.h"
#include "fsys_debug.h"
#include "cdfs.h"

static __s32 isofs_file_read(struct file *filp, char *buf,
                             __u32 len, __s64 *ppos)
{
    sector_t phy;
    int count, chunk_size;
    __s64 pos;
    int offset;
    unsigned long max_blks;
    int sb_blksize, sb_blksize_bits;
    struct buffer_head *bh;
    struct super_block *sb;
    struct inode *ino;
    int    read_sector_nr;

    sb = filp->f_dentry->d_sb;
    ino = filp->f_dentry->d_inode;

    sb_blksize = sb->s_blocksize;
    sb_blksize_bits = sb->s_blocksize_bits;

    offset = *ppos % sb_blksize;
    pos = *ppos;

    if (*ppos >= ino->i_size)
    {
        goto out;
    }

    len = *ppos + len < ino->i_size ? len : ino->i_size - *ppos;
    if (offset)
    {
        max_blks = (len + sb_blksize - 1) >> sb_blksize_bits;
        if (isofs_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0) || !phy)
        {
            goto out;
        }

        bh = __bread(sb, phy, sb_blksize);
        if (bh == NULL)
        {
            /* read block device sector failed */
            goto out;
        }
        chunk_size = len <= sb_blksize - offset ? len : sb_blksize - offset;
        debug_timerstart(memcpytime);
        memcpy(buf, bh->b_data + offset, chunk_size);
        debug_timerend(memcpytime);
        buf += chunk_size;
        brelse(bh);
        len -= chunk_size;
        pos += chunk_size;
    }
    if (((__u32)buf >= CONFIG_DRAM_VIRTBASE) && ((__u32)buf <= (CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_SIZE)))
    {
        struct buffer_head *bh;
        int i, offset;
        int dio_startphy, dio_blks;
        char *dio_startbuf;

        while ((max_blks = (len >> sb_blksize_bits)))
        {
            if (isofs_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0) || !phy)
            {
                goto out;
            }

            dio_startphy = phy;
            dio_startbuf = buf;
            dio_blks = 0;
            for (offset = i = 0; i < max_blks; offset += sb_blksize, i++)
            {
                debug_timerstart(hashtime);
                bh = get_hash_table(sb, phy++, sb_blksize);
                debug_timerend(hashtime);
                if (bh)
                {
                    if (dio_blks)
                    {
                        debug_timerstart(diodevicetime);
                        read_sector_nr = esFSYS_pread(dio_startbuf, dio_startphy, dio_blks, sb->s_part);
                        if (read_sector_nr != dio_blks)
                        {
                            /* read block device sector failed */
                            goto out;
                        }
                        debug_timerend(diodevicetime);
                        dio_blks = 0;
                    }
                    memcpy(buf + offset, bh->b_data, sb_blksize);
                    put_bh(bh);
                    dio_startphy = phy;
                    dio_startbuf = buf + offset + sb_blksize;
                }
                else
                {
                    dio_blks++;
                }
            }
            if (dio_blks)
            {
                debug_timerstart(diodevicetime);
                read_sector_nr = esFSYS_pread(dio_startbuf, dio_startphy, dio_blks, sb->s_part);
                if (read_sector_nr != dio_blks)
                {
                    /* read block device sector failed */
                    goto out;
                }
                debug_timerend(diodevicetime);
                dio_blks = 0;
            }
            chunk_size = max_blks << sb_blksize_bits;
            buf += chunk_size;
            len -= chunk_size;
            pos += chunk_size;
        }
    }
    else
    {
        while ((max_blks = (len >> sb_blksize_bits)))
        {
            if (isofs_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0) || !phy)
            {
                goto out;
            }

            chunk_size = sb_blksize * max_blks;
            while (max_blks--)
            {
                bh = __bread(sb, phy++, sb_blksize);
                if (bh == NULL)
                {
                    /* read block device sector failed */
                    goto out;
                }
                debug_timerstart(memcpytime);
                memcpy(buf, bh->b_data, sb_blksize);
                debug_timerend(memcpytime);
                buf += sb_blksize;
                brelse(bh);
            }
            len -= chunk_size;
            pos += chunk_size;
        }
    }
    if (len)
    {
        max_blks = (len + sb_blksize - 1) >> sb_blksize_bits;
        if (isofs_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0) || !phy)
        {
            goto out;
        }

        bh = __bread(sb, phy, sb_blksize);
        if (bh == NULL)
        {
            /* read block device sector failed */
            goto out;
        }

        debug_timerstart(memcpytime);
        memcpy(buf,  bh->b_data, len);
        debug_timerend(memcpytime);
        brelse(bh);

        pos += len;
        buf += len;
    }

out:
    chunk_size = pos - *ppos ;
    debug_dump_mems(buf - chunk_size, ino->debug_dentry->d_name.name, *ppos, chunk_size);
    *ppos = pos;
    return chunk_size;
}

const struct file_operations isofs_file_operations =
{
    .llseek     = generic_file_llseek,
    .read       = isofs_file_read,
};

