/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : exfat_file.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-16
* Descript: exfat file system file handing functions.
* Update  : date                auther      ver     notes
*           2011-3-16 14:26:02  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "exfatfs.h"
#include "exfat_cluster.h"
#include "buffer_head.h"
#include "errno.h"
#include "fsys_debug.h"

__s32 exfat_generic_ioctl(struct inode *inode, struct file *filp,
                          __u32 cmd, __u32 aux, void *buffer)
{
    return 0;
}


static __s32 exfat_getattr(struct dentry *dentry, struct kstat *stat)
{
    struct inode *inode = dentry->d_inode;
    generic_fillattr(inode, stat);
    stat->blksize = EXFAT_SB(inode->i_sb)->clus_size;
    return 0;
}

static __s32 exfat_file_read(struct file *filp, char *buf, __u32 len, __s64 *ppos)
{
    sector_t phy;
    int count, chunk_size;
    __s64 pos;
    int offset;
    unsigned long   max_blks;
    int sb_blksize, sb_blksize_bits;
    struct buffer_head *bh;
    struct super_block *sb;
    struct inode *ino;
    int read_sector_nr;
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
        if (exfat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0) || !phy)
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
    if (((uintptr_t)buf >= CONFIG_DRAM_VIRTBASE) && ((uintptr_t)buf <= (CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_SIZE)))
    {
        struct buffer_head *bh;
        int i, offset;
        int dio_startphy, dio_blks;
        char *dio_startbuf;

        while ((max_blks = (len >> sb_blksize_bits)))
        {
            if (exfat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0) || !phy)
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
            if (exfat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0) || !phy)
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
        if (exfat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0) || !phy)
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

#if defined CONFIG_FSYS_EXFAT_RW
static int exfat_cont_expand(struct inode *inode, __s64 expand_size)
{
    sector_t phy;
    int count, err = 0;
    unsigned long  max_blks = 0;
    __u64 pos, offset;
    int sb_blksize, sb_blksize_bits;
    struct buffer_head *bh;
    struct super_block *sb;
    struct inode *ino;

    struct exfat_sb_info *sbi;
    struct exfat_inode_info *exi;

    count = expand_size - inode->i_size ;
    sb  = inode->i_sb ;
    ino = inode;
    sb_blksize = sb->s_blocksize;
    sb_blksize_bits = sb->s_blocksize_bits;
    /* expand pos ,at the file tail */
    pos = ino->i_size;

    sbi = EXFAT_SB(sb);
    exi = EXFAT_I(inode);

    offset = pos % sb_blksize;
    if (offset)
    {
        pos += sb_blksize - offset ;
        count -= sb_blksize - offset;
        if (pos > ino->i_size)
        {
            ino->i_size = pos;
        }
    }

    while ((max_blks = (count >> sb_blksize_bits)))
    {
        if (count > 16 * 1024 * 1024)
        {
            max_blks = 16 * 1024 * 1024 >> sb_blksize_bits;
        }
        err = exfat_get_block(ino, pos >> sb_blksize_bits,  &max_blks, &phy, 1);
        if (err  || !phy)
        {
            goto out;
        }

        count -= max_blks * sb_blksize;
        pos += max_blks * sb_blksize;

        if (pos > ino->i_size)
        {
            ino->i_size = pos;
        }
    }

    if (count)
    {
        max_blks = (count + sb_blksize - 1) >> sb_blksize_bits;
        err = exfat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1);
        if (err || !phy)
        {
            goto out;
        }
        pos += count;
        count -= max_blks * sb_blksize;
        if (pos > ino->i_size)
        {
            ino->i_size = pos;
        }
    }

out:
    return err;
}

static __s32 exfat_setattr(struct dentry *dentry, struct iattr *attr)
{
    struct exfat_sb_info *sbi = EXFAT_SB(dentry->d_sb);
    struct inode *inode = dentry->d_inode;
    unsigned int ia_valid;
    int error = 0;

    if (attr->ia_valid & ATTR_SIZE)
    {
        if (attr->ia_size > inode->i_size)
        {
            error = exfat_cont_expand(inode, attr->ia_size);
            /* if error or we do not do anthing more, goto out */
            if (error || attr->ia_valid == ATTR_SIZE)
            {
                goto out;
            }
            /* there is something to do ,like change time */
            attr->ia_valid &= ~ATTR_SIZE;
        }
        else
        {
            if (attr->ia_size  <  inode->i_size)
            {
                /* set inode size to ia_size, then we truncate the inode size to it, exfat_truncate actually to free fat entry */
                inode->i_size = attr->ia_size;
                exfat_truncate(inode);
            }
        }
    }

    /* set atime、mtime、ctime */
    if (attr->ia_valid & ATTR_ATIME)
    {
        inode->i_atime = timespec_trunc(attr->ia_atime, inode->i_sb->s_time_gran);
    }
    if (attr->ia_valid & ATTR_MTIME)
    {
        inode->i_mtime = timespec_trunc(attr->ia_mtime, inode->i_sb->s_time_gran);
    }
    if (attr->ia_valid & ATTR_CTIME)
    {
        inode->i_ctime = timespec_trunc(attr->ia_ctime, inode->i_sb->s_time_gran);
    }
    /* we have change the time ,makr inode dirty */
    mark_inode_dirty(inode);
out:
    return error;
}

static __s64 exfat_file_llseek(struct file *file, __s64 offset, __s32 origin)
{
    long long retval;

    if (file == NULL)
    {
        __err("invalid parameter!");
        return -EINVAL;
    }

    struct inode *inode = file->f_dentry->d_inode;

    switch (origin)
    {
        case SEEK_END:
            offset += inode->i_size;
            break;
        case SEEK_CUR:
            offset += file->f_pos;
    }
    retval = -EINVAL;
    if (offset >= 0 && offset <= inode->i_size)
    {
        if (offset != file->f_pos)
        {
            file->f_pos = offset;
        }
        retval = 0;
    }
    else
    {
        if (offset > inode->i_size)
        {
            int error = exfat_cont_expand(inode, offset);
            if (error)
            {
                file->f_err = retval;
                __err("seek file %s error.", file->f_dentry->d_name.name);
                return -EINVAL;
            }
            file->f_pos = offset;
            mark_inode_dirty(inode);
            retval = 0;
        }
    }
#ifdef FSEEK_LEGACY
    return retval;
#else
    return offset;
#endif
}

static __s32 exfat_file_write(struct file *filp, const char *buf, __u32 len, __s64 *ppos)
{
    struct super_block *sb = filp->f_dentry->d_sb;
    struct inode  *ino = filp->f_dentry->d_inode;
    struct exfat_inode_info *exi = EXFAT_I(filp->f_dentry->d_inode);
    struct exfat_sb_info *sbi = EXFAT_SB(filp->f_dentry->d_sb);
    struct buffer_head *bh = NULL;
    int res, count, chunk_size, offset;
    sector_t phy;
    int sb_blksize, sb_blksize_bits;
    unsigned long max_blks;
    __s64 pos;

    sb_blksize = sb->s_blocksize;
    sb_blksize_bits = sb->s_blocksize_bits;

    offset = *ppos & (sb_blksize - 1);
    pos = *ppos;
    if (pos > ino->i_size)
    {
        goto out;
    }

    debug_dump_mems(buf, ino->debug_dentry->d_name.name, *ppos, len);

    if (offset)
    {
        max_blks = (len + sb_blksize - 1) >> sb_blksize_bits;
        if (exfat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1) || !phy)
        {
            goto out;
        }

        bh = __bread(sb, phy, sb_blksize);
        if (bh == NULL)
        {
            /* read block device sector failed */
            goto out;
        }
        chunk_size = min(len, (__u32)(sb_blksize - offset));
        debug_timerstart(memcpytime);
        memcpy(bh->b_data + offset, buf, chunk_size);
        debug_timerend(memcpytime);
        buf += chunk_size;
        mark_buffer_dirty(bh);
        brelse(bh);

        len -= chunk_size;
        pos += chunk_size;
        if (pos > ino->i_size)
        {
            ino->i_size = pos;
            if (ino->i_size > exi->phys_size)
            {
                exi->phys_size = ino->i_size;
            }
        }
    }
    if (((uintptr_t)buf >= CONFIG_DRAM_VIRTBASE) && ((uintptr_t)buf <= (CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_SIZE)))
    {
        int i, buf_off;
        while ((max_blks = (len >> sb_blksize_bits)))
        {
            if (exfat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1) || !phy)
            {
                goto out;
            }

            debug_timerstart(diodevicetime);
            esFSYS_pwrite(buf, phy, max_blks, sb->s_part);
            debug_timerend(diodevicetime);

            debug_timerstart(hashtime);
            for (buf_off = i = 0; i < max_blks; buf_off += sb_blksize, i++)
            {
                bh = get_hash_table(sb, phy++, sb_blksize);
                if (bh)
                {
                    memcpy(bh->b_data, buf + buf_off, sb_blksize);
                    mark_buffer_clean(bh);
                    put_bh(bh);
                }
            }
            debug_timerend(hashtime);

            chunk_size = sb_blksize * max_blks;
            buf += chunk_size;
            len -= chunk_size;
            pos += chunk_size;
            if (pos > ino->i_size)
            {
                ino->i_size = pos;
                if (ino->i_size > exi->phys_size)
                {
                    exi->phys_size = ino->i_size;
                }
            }
        }
    }
    else
    {
        while ((max_blks = (len >> sb_blksize_bits)))
        {
            if (exfat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1) || !phy)
            {
                goto out;
            }

            chunk_size = sb_blksize * max_blks;
            while (max_blks--)
            {
                bh = __getblk(sb, phy++, sb_blksize);
                debug_timerstart(memcpytime);
                memcpy(bh->b_data, buf, sb_blksize);
                debug_timerend(memcpytime);
                buf += sb_blksize;
                mark_buffer_dirty(bh);
                exfat_sync_bhs(&bh, 1);
                brelse(bh);
            }
            len -= chunk_size;
            pos += chunk_size;
            if (pos > ino->i_size)
            {
                ino->i_size = pos;
                if (ino->i_size > exi->phys_size)
                {
                    exi->phys_size = ino->i_size;
                }
            }
        }
    }
    if (len)
    {
        max_blks = (len + sb_blksize - 1) >> sb_blksize_bits;
        if (exfat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1) || !phy)
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
        memcpy(bh->b_data, buf, len);
        debug_timerend(memcpytime);
        mark_buffer_dirty(bh);
        brelse(bh);
        pos += len;
        if (pos > ino->i_size)
        {
            ino->i_size = pos;
            if (ino->i_size > exi->phys_size)
            {
                exi->phys_size = ino->i_size;
            }
        }
    }
out:
    /* update file size, can improve performance a little */
    //if (pos > ino->i_size) {
    //  ino->i_size = pos;
    //    if(ino->i_size > exi->phys_size) {
    //      exi->phys_size = ino->i_size;
    //    }
    //}
    chunk_size = pos - *ppos ;
    *ppos = pos;
    return chunk_size;
}


static __s32 exfat_file_release(struct inode *inode, struct file *filp)
{
    if ((filp->f_mode & FMODE_WRITE))
    {
        exfat_flush_inodes(inode->i_sb, inode, NULL);
        //      congestion_wait(WRITE, HZ/10);
    }
    return 0;
}

void exfat_truncate(struct inode *inode)
{
    struct super_block *sb = inode->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct exfat_inode_info *exi = EXFAT_I(inode);
    int iclus, nr_clus, err;

    /* This protects against truncating a file bigger than it was then
     * trying to write into the hole. */
    //if (exi->phys_size > inode->i_size)
    //  exi->phys_size = inode->i_size;

    /* get logic start cluster number */
    iclus = (inode->i_size + (sbi->clus_size - 1)) >> sbi->clus_bits;

    /* FIXME : cluster number maybe calc by inode->i_blocks */
    nr_clus = ((exi->phys_size + sbi->clus_size - 1) >> sbi->clus_bits) - iclus;
    err = exfat_free(inode, iclus, nr_clus);
    if (err)
    {
        fs_log_error("exFAT truncate free space failed\n");
        return ;
    }
    /* update inode physically allocated size here */
    exi->phys_size = inode->i_size;
    mark_inode_dirty(inode);
    exfat_flush_inodes(inode->i_sb, inode, NULL);
}
#endif  /* FSYS_EXFAT_RW */

const struct file_operations exfat_file_ops =
{
#if defined CONFIG_FSYS_EXFAT_RW
    .llseek     = exfat_file_llseek,
#else
    .llseek     = generic_file_llseek,
#endif
    .read       = exfat_file_read,
    .ioctl      = exfat_generic_ioctl,
    .fsync      = file_fsync,
#if defined CONFIG_FSYS_EXFAT_RW
    .write      = exfat_file_write,
    .release    = exfat_file_release,
#endif
};

const struct inode_operations exfat_file_inode_ops =
{
    .getattr    = exfat_getattr,
#if defined CONFIG_FSYS_EXFAT_RW
    .setattr    = exfat_setattr,
    .truncate   = exfat_truncate,
#endif
};

