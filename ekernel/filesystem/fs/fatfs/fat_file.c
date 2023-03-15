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
#include "fatfs.h"
#include "buffer_head.h"
#include "errno.h"
#include "fsys_debug.h"

#if     FSYS_DEBUG_ON
extern int __debug_dump_fatchunk(struct inode *inode);
#else
#define __debug_dump_fatchunk(inode)
#endif


__s32 fat_generic_ioctl(struct inode *inode, struct file *filp,
                        __u32 cmd, __u32 aux, void *buffer)
{
    struct msdos_sb_info *sbi = MSDOS_SB(inode->i_sb);

    switch (cmd)
    {
        case FS_IOC_USR_DEBUG_FATCHUNK:
            __debug_dump_fatchunk(inode);
            return 0;

        case FAT_IOCTL_GET_ATTRIBUTES:
        {
            __u32 attr;

            if (inode->i_ino == MSDOS_ROOT_INO)
            {
                attr = ATTR_DIR;
            }
            else
            {
                attr = fat_attr(inode);
            }

            *((int *)buffer) = attr;
            return 0;
        }
        default:
            return -ENOTTY; /* Inappropriate ioctl for device */
    }
}

__s32 fat_getattr(struct dentry *dentry, struct kstat *stat)
{
    struct inode *inode = dentry->d_inode;
    generic_fillattr(inode, stat);
    stat->blksize = MSDOS_SB(inode->i_sb)->cluster_size;
    return 0;
}

static __s32 fat_file_read(struct file *filp, char *buf, __u32 len, __s64 *ppos)
{
    int phy, err = 0;
    int count, chunk_size;
    __s64 pos;
    int offset, max_blks;
    int sb_blksize, sb_blksize_bits;
    struct buffer_head *bh;
    struct super_block *sb;
    struct inode *ino;
    int    read_sector_nr;
    struct timespec ts;

    ts = CURRENT_TIME_SEC;

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
        err = __fat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0);
        if (err || !phy)
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
            err = __fat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0);
            if (err || !phy)
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
            //            debug_timerstart(diodevicetime);
            //            (*part_ops).read(buf, phy, max_blks, part);
            //            debug_timerend(diodevicetime);
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
            err = __fat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0);
            if (err || !phy)
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
        err = __fat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0);
        if (err || !phy)
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

    ino->i_atime = ts;

out:
    chunk_size = pos - *ppos ;
    debug_dump_mems(buf - chunk_size, ino->debug_dentry->d_name.name, *ppos, chunk_size);
    *ppos = pos;
    fs_err = err;
    return chunk_size;
}

#if defined CONFIG_FSYS_FAT_RW
static __s32 fat_file_release(struct inode *inode, struct file *filp)
{
    if ((filp->f_mode & FMODE_WRITE) &&
        MSDOS_SB(inode->i_sb)->options.flush)
    {
        fat_flush_inodes(inode->i_sb, inode, NULL);
        //      congestion_wait(WRITE, HZ/10);
    }
    return 0;
}
static int fat_cont_expand(struct inode *inode, __s64 expand_size)
{
    int phy, count, err = 0;
    int  max_blks = 0;
    __u64 pos, offset;
    int sb_blksize, sb_blksize_bits;
    struct buffer_head *bh;
    struct super_block *sb;
    struct inode *ino;
    struct msdos_inode_info *ms_inode = MSDOS_I(inode);
    struct timespec ts;

    ts = CURRENT_TIME_SEC;

    count = expand_size - inode->i_size ;
    sb  = inode->i_sb ;
    ino = inode;            //获取文件的node
    sb_blksize = sb->s_blocksize;               //扇区大小
    sb_blksize_bits = sb->s_blocksize_bits; //扇区大小，位来表示
    //expand pos ,at the file tail
    pos = ino->i_size;

    offset = pos % sb_blksize;
    if (offset)
    {
        pos += sb_blksize - offset ;
        count -= sb_blksize - offset;
        if (pos > ino->i_size)
        {
            ino->i_size = pos;
            if (ino->i_size > ms_inode->mmu_private)
            {
                ms_inode->mmu_private = ino->i_size;
            }
        }
    }

    while ((max_blks = (count >> sb_blksize_bits)))
    {
        //max_blks = (len + sb_blksize -1)>>sb_blksize_bits;
        //分配规则是一次最多分配16M(一个扇区的fat数)
        if (count > 16 * 1024 * 1024)
        {
            max_blks = 16 * 1024 * 1024 >> sb_blksize_bits;
        }
        err = __fat_get_block(ino, pos >> sb_blksize_bits,  &max_blks, &phy, 1);
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
    //if have something
    if (count)
    {
        max_blks = (count + sb_blksize - 1) >> sb_blksize_bits;
        err = __fat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1);
        if (err || !phy)
        {
            goto out;
        }
        pos += count;
        count -= max_blks * sb_blksize;
        if (pos > ino->i_size)
        {
            ino->i_size = pos;
            if (ino->i_size > ms_inode->mmu_private)
            {
                ms_inode->mmu_private = ino->i_size;
            }
        }
    }

    ino->i_ctime = ino->i_mtime = ts;
out:

    return err;
}

__s32 fat_setattr(struct dentry *dentry, struct iattr *attr)
{
    struct msdos_sb_info *sbi = MSDOS_SB(dentry->d_sb);
    struct inode *inode = dentry->d_inode;
    unsigned int ia_valid;
    int error = 0;

    /*
     * Expand the file. Since inode_setattr() updates ->i_size
     * before calling the ->truncate(), but FAT needs to fill the
     * hole before it. XXX: this is no longer true with new truncate
     * sequence.
     */
    if (attr->ia_valid & ATTR_SIZE)
    {
        if (attr->ia_size > inode->i_size)
        {
            error = fat_cont_expand(inode, attr->ia_size);
            //if error or we do not do anthing more, goto out
            if (error || attr->ia_valid == ATTR_SIZE)
            {
                goto out;
            }
            //there is something to do ,like change time
            attr->ia_valid &= ~ATTR_SIZE;
        }
        else
        {
            if (attr->ia_size  <  inode->i_size)
            {
                //set inode size to ia_size, then we truncate the inode size to it, fat_truncate actually to free fat entry
                inode->i_size = attr->ia_size;
                fat_truncate(inode);
            }
        }
    }

    //set atime、mtime、ctime
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
    //we have change the time ,makr inode dirty
    mark_inode_dirty(inode);
out:
    return error;
}

static __s32 fat_file_write(struct file *filp, const char *buf, __u32 len, __s64 *ppos)
{
    int phy, err = 0;
    int res, count, chunk_size;
    __s64 pos;
    int offset, max_blks;
    int sb_blksize, sb_blksize_bits;
    struct buffer_head *bh;
    struct super_block *sb;
    struct inode *ino;
    //debug
    struct msdos_inode_info *ms_inode = MSDOS_I(filp->f_dentry->d_inode);
    struct msdos_sb_info *ms_sbi = MSDOS_SB(filp->f_dentry->d_sb);
    int    write_sector_nr;
    struct timespec ts;

    ts = CURRENT_TIME_SEC;

    sb = filp->f_dentry->d_sb;
    ino = filp->f_dentry->d_inode;

    sb_blksize = sb->s_blocksize;
    sb_blksize_bits = sb->s_blocksize_bits;

    offset = *ppos % sb_blksize;
    pos = *ppos;

    if (pos > ino->i_size)
    {
        goto out;
    }

    debug_dump_mems(buf, ino->debug_dentry->d_name.name, *ppos, len);

    if (offset)
    {
        max_blks = (len + sb_blksize - 1) >> sb_blksize_bits;
        err = __fat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1);
        if (err || !phy)
        {
            goto out;
        }

        bh = __bread(sb, phy, sb_blksize);
        if (bh == NULL)
        {
            /* read block device sector failed */
            goto out;
        }

        chunk_size = (len <= sb_blksize - offset ? len : sb_blksize - offset);

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
            if (ino->i_size > ms_inode->mmu_private)
            {
                ms_inode->mmu_private = ino->i_size;
            }
        }
    }

    if (((uintptr_t)buf >= CONFIG_DRAM_VIRTBASE) && ((uintptr_t)buf <= (CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_SIZE)))
    {
        struct buffer_head *bh;
        int i, buf_off;

        while ((max_blks = (len >> sb_blksize_bits)))
        {
            err = __fat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1);
            if (err || !phy)
            {
                goto out;
            }

            debug_timerstart(diodevicetime);
            write_sector_nr = esFSYS_pwrite(buf, phy, max_blks, sb->s_part);
            if (write_sector_nr != max_blks)
            {
                /* write block device sector failed */
                goto out;
            }
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
            }
        }
    }
    else
    {
        while ((max_blks = (len >> sb_blksize_bits)))
        {
            err = __fat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1);
            if (err || !phy)
            {
                goto out;
            }

            chunk_size = sb_blksize * max_blks;
            while (max_blks--)
            {
                bh = __getblk(sb, phy++, sb_blksize);
                if (bh == NULL)
                {
                    /* read block device sector failed */
                    goto out;
                }
                debug_timerstart(memcpytime);
                memcpy(bh->b_data, buf, sb_blksize);
                debug_timerend(memcpytime);
                buf += sb_blksize;
                mark_buffer_dirty(bh);
                fat_sync_bhs(&bh, 1);
                brelse(bh);
            }

            len -= chunk_size;
            pos += chunk_size;
            if (pos > ino->i_size)
            {
                ino->i_size = pos;
            }
        }
    }

    if (len)
    {
        max_blks = (len + sb_blksize - 1) >> sb_blksize_bits;
        err = __fat_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1);
        if (err || !phy)
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
        //buf += len;
        if (pos > ino->i_size)
        {
            ino->i_size = pos;
            if (ino->i_size > ms_inode->mmu_private)
            {
                ms_inode->mmu_private = ino->i_size;
            }
        }
    }

    ino->i_ctime = ino->i_mtime = ts;
out:
    if (IS_DIRSYNC(ino))
    {
        (void)fat_sync_inode(ino);
    }
    else
    {
        mark_inode_dirty(ino);
    }
    chunk_size = pos - *ppos ;
    *ppos = pos;
    fs_err = err;
    return chunk_size;
}

/* Free all clusters after the skip'th cluster. */
static int fat_free(struct inode *inode, int skip)
{
    struct super_block *sb = inode->i_sb;
    int err, wait, free_start, i_start, i_logstart;

    if (MSDOS_I(inode)->i_start == 0)
    {
        return 0;
    }

    fat_cache_inval_inode(inode);

    wait = IS_DIRSYNC(inode);
    i_start = free_start = MSDOS_I(inode)->i_start;
    i_logstart = MSDOS_I(inode)->i_logstart;

    /* First, we write the new file size. */
    if (!skip)
    {
        MSDOS_I(inode)->i_start = 0;
        MSDOS_I(inode)->i_logstart = 0;
    }
    MSDOS_I(inode)->i_attrs |= ATTR_ARCH;
    inode->i_ctime = inode->i_mtime = CURRENT_TIME_SEC;
    if (wait)
    {
        err = fat_sync_inode(inode);
        if (err)
        {
            MSDOS_I(inode)->i_start = i_start;
            MSDOS_I(inode)->i_logstart = i_logstart;
            return err;
        }
    }
    else
    {
        mark_inode_dirty(inode);
    }

    /* Write a new EOF, and get the remaining cluster chain for freeing. */
    if (skip)
    {
        struct fat_entry fatent;
        int ret, fclus, dclus;

        ret = fat_get_cluster(inode, skip - 1, &fclus, &dclus, NULL);
        if (ret < 0)
        {
            return ret;
        }
        else if (ret == FAT_ENT_EOF)
        {
            return 0;
        }

        fatent_init(&fatent);
        ret = fat_ent_read(inode, &fatent, dclus);
        if (ret == FAT_ENT_EOF)
        {
            fatent_brelse(&fatent);
            return 0;
        }
        else if (ret == FAT_ENT_FREE)
        {
            fat_fs_panic(sb,
                         "%s: invalid cluster chain (i_pos %d)",
                         __FUNCTION__, (int)MSDOS_I(inode)->i_pos);
            ret = -EIO;
        }
        else if (ret > 0)
        {
            err = fat_ent_write(inode, &fatent, FAT_ENT_EOF, wait);
            if (err)
            {
                ret = err;
            }
        }
        fatent_brelse(&fatent);
        if (ret < 0)
        {
            return ret;
        }

        free_start = ret;
    }
    inode->i_blocks = skip << (MSDOS_SB(sb)->cluster_bits - 9);

    /* Freeing the remained cluster chain */
    return fat_free_clusters(inode, free_start);
}

void fat_truncate(struct inode *inode)
{
    struct msdos_sb_info *sbi = MSDOS_SB(inode->i_sb);
    const unsigned int cluster_size = sbi->cluster_size;
    int nr_clusters;

    /*
     * This protects against truncating a file bigger than it was then
     * trying to write into the hole.
     */
    if (MSDOS_I(inode)->mmu_private > inode->i_size)
    {
        MSDOS_I(inode)->mmu_private = inode->i_size;
    }

    nr_clusters = (inode->i_size + (cluster_size - 1)) >> sbi->cluster_bits;

    fat_free(inode, nr_clusters);
    fat_flush_inodes(inode->i_sb, inode, NULL);
}

__s64 fat_file_llseek(struct file *file, __s64 offset, __s32 origin)
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
            int error = fat_cont_expand(inode, offset);
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
#endif

const struct file_operations fat_file_operations =
{
#if defined CONFIG_FSYS_FAT_RW
    .llseek     = fat_file_llseek,
#else
    .llseek     = generic_file_llseek,
#endif
    .read       = fat_file_read,
    .ioctl      = fat_generic_ioctl,
#if defined CONFIG_FSYS_FAT_RW
    .write      = fat_file_write,
    .release    = fat_file_release,
    .fsync      = file_fsync,
#endif
};

const struct inode_operations fat_file_inode_operations =
{
    .getattr    = fat_getattr,
#if defined CONFIG_FSYS_FAT_RW
    .setattr    = fat_setattr,
    .truncate   = fat_truncate,
#endif
};