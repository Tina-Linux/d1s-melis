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
#include "udf_decl.h"
#include "udf_fs.h"
#include "err.h"
#include "buffer_head.h"
#include "fsys_debug.h"
#include "udf_fs_i.h"
#include "udf_sb.h"

__s32 udf_ioctl(struct inode *inode, struct file *filp,
                __u32 cmd, __u32 aux, void *buffer)
{
    //long old_block, new_block;
    int result = -EINVAL;

    //if (file_permission(filp, MAY_READ) != 0) {
    //    udf_debug("no permission to access inode %lu\n",
    //          inode->i_ino);
    //    return -EPERM;
    //}
    //
    //if (!arg) {
    //    udf_debug("invalid argument to udf_ioctl\n");
    //    return -EINVAL;
    //}
    //
    //switch (cmd) {
    //case UDF_GETVOLIDENT:
    //    if (copy_to_user((char __user *)arg,
    //             UDF_SB(inode->i_sb)->s_volume_ident, 32))
    //        return -EFAULT;
    //    else
    //        return 0;
    //case UDF_RELOCATE_BLOCKS:
    //    if (!capable(CAP_SYS_ADMIN))
    //        return -EACCES;
    //    if (get_user(old_block, (long __user *)arg))
    //        return -EFAULT;
    //    result = udf_relocate_blocks(inode->i_sb,
    //                    old_block, &new_block);
    //    if (result == 0)
    //        result = put_user(new_block, (long __user *)arg);
    //    return result;
    //case UDF_GETEASIZE:
    //    result = put_user(UDF_I(inode)->i_lenEAttr, (int __user *)arg);
    //    break;
    //case UDF_GETEABLOCK:
    //    result = copy_to_user((char __user *)arg,
    //                  UDF_I(inode)->i_ext.i_data,
    //                  UDF_I(inode)->i_lenEAttr) ? -EFAULT : 0;
    //    break;
    //}

    return result;
}

static __s32 udf_adinicb_file_read(struct file *filp, char *buf,
                                   __u32 len, __s64 *ppos)
{
    unsigned int  copy;
    struct inode *inode = filp->f_dentry->d_inode;
    struct udf_inode_info *iinfo = UDF_I(inode);

    copy = min(len, (__u32)inode->i_size);
    memcpy(buf, iinfo->i_ext.i_data + iinfo->i_lenEAttr + *ppos, copy);
    *ppos += copy;

    return 0;
}


static __s32 udf_file_read(struct file *filp, char *buf,
                           __u32 len, __s64 *ppos)
{
    sector_t phy;
    int count, chunk_size;
    __s64 pos;
    int offset, max_blks;
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
        if (udf_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0) || !phy)
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
            if (udf_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0) || !phy)
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
                esFSYS_pread(dio_startbuf, dio_startphy, dio_blks, sb->s_part);
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
            if (udf_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0) || !phy)
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
        if (udf_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 0) || !phy)
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

#if defined CONFIG_FSYS_UDFFS_RW
static __s32 udf_release_file(struct inode *inode, struct file *filp)
{
    if (filp->f_mode & FMODE_WRITE)
    {
        //      mutex_lock(&inode->i_mutex);
        //      lock_kernel();
        udf_discard_prealloc(inode);
        udf_truncate_tail_extent(inode);
        //      unlock_kernel();
        //      mutex_unlock(&inode->i_mutex);
    }
    return 0;
}

static __s32 udf_adinicb_file_write(struct file *filp, const char *buf,
                                    __u32 len, __s64 *ppos)
{
    struct inode *inode = filp->f_dentry->d_inode;
    struct udf_inode_info *iinfo = UDF_I(inode);
    unsigned int copy;

    /* FIXME : you can't write length bigger then inode size */
    copy = min(len, (__u32)inode->i_size);
    if (copy + *ppos > inode->i_size)
    {
        copy = inode->i_size - *ppos;
    }
    memcpy(iinfo->i_ext.i_data + iinfo->i_lenEAttr + *ppos, buf, copy);
    mark_inode_dirty(inode);

    if (IS_DIRSYNC(inode))
    {
        udf_sync_inode(inode);
    }
    return 0;
}

static __s32 udf_sync_bhs(struct buffer_head **bhs, __s32 nr_bhs)
{
    int i, err = 0;

    ll_rw_block(WRITE, nr_bhs, bhs);
    for (i = 0; i < nr_bhs; i++)
    {
        wait_on_buffer(bhs[i]);
        //      if (buffer_eopnotsupp(bhs[i])) {
        //          clear_buffer_eopnotsupp(bhs[i]);
        //          err = -EOPNOTSUPP;
        //      } else
        if (!err && !buffer_uptodate(bhs[i]))
        {
            err = -EIO;
        }
    }
    return err;
}


static __s32 udf_file_write(struct file *filp, const char *buf,
                            __u32 len, __s64 *ppos)
{
    struct super_block *sb = filp->f_dentry->d_sb;
    struct inode  *ino = filp->f_dentry->d_inode;
    struct buffer_head *bh = NULL;
    int res, count, chunk_size, offset;
    sector_t phy;
    int max_blks, sb_blksize, sb_blksize_bits;
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
        if (udf_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1) || !phy)
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
    }
    if (((__u32)buf >= CONFIG_DRAM_VIRTBASE) && ((__u32)buf <= (CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_SIZE)))
    {
        int i, buf_off;
        while ((max_blks = (len >> sb_blksize_bits)))
        {
            if (udf_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1) || !phy)
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
        }
    }
    else
    {
        while ((max_blks = (len >> sb_blksize_bits)))
        {
            if (udf_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1) || !phy)
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
                udf_sync_bhs(&bh, 1);
                brelse(bh);
            }
            len -= chunk_size;
            pos += chunk_size;
        }
    }
    if (len)
    {
        max_blks = (len + sb_blksize - 1) >> sb_blksize_bits;
        if (udf_get_block(ino, pos >> sb_blksize_bits, &max_blks, &phy, 1) || !phy)
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
    }
out:
    chunk_size = pos - *ppos ;
    *ppos = pos;
    return chunk_size;
}
#endif

/* Allocation Descriptor in icb type file */
const struct file_operations udf_adinicb_file_opts =
{
    .llseek     = generic_file_llseek,
    .read       = udf_adinicb_file_read,
    .ioctl      = udf_ioctl,
#if defined CONFIG_FSYS_UDFFS_RW
    .write      = udf_adinicb_file_write,
    .release    = udf_release_file,
    .fsync      = file_fsync,
#endif
};

const struct file_operations udf_file_opts =
{
    .llseek     = generic_file_llseek,
    .read       = udf_file_read,
    .ioctl      = udf_ioctl,
#if defined CONFIG_FSYS_UDFFS_RW
    .write      = udf_file_write,
    .release    = udf_release_file,
    .fsync      = file_fsync,
#endif
};

const struct inode_operations udf_file_inode_operations =
{
#if defined CONFIG_FSYS_UDFFS_RW
    .truncate = udf_truncate,
#endif
};