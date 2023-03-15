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