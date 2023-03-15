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
#ifndef __EXFAT_FATENT_H__
#define __EXFAT_FATENT_H__

#include "fstypedef.h"
#include "exfatfs.h"
#include "buffer_head.h"
#include "errno.h"
#include "endians.h"

struct exfat_ent
{
    struct buffer_head *bh;
    u32 offset;
};

static inline void exfat_ent_init(struct exfat_ent *fatent)
{
    fatent->bh = NULL;
}

static inline void exfat_ent_release(struct exfat_ent *fatent)
{
    brelse(fatent->bh);
}

/* FIXME: should be outline those funcs? must rethink those */

static inline int exfat_ent_read(struct inode *inode,
                                 struct exfat_ent *fatent,
                                 u32 clusnr, u32 *result_clusnr)
{
    struct super_block *sb = inode->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    sector_t blocknr;
    __le32 *ent;

    blocknr = sbi->fat_blocknr + (clusnr >> sbi->fpb_bits);
    if (!fatent->bh || fatent->bh->b_blocknr != blocknr)
    {
        brelse(fatent->bh);
        fatent->bh = sb_bread(sb, blocknr);
        if (!fatent->bh)
        {
            return -EIO;
        }
    }

    ent = (__le32 *)fatent->bh->b_data;
    *result_clusnr = le32_to_cpu(ent[clusnr & (sbi->fpb - 1)]);

    return 0;
}

#if defined CONFIG_FSYS_EXFAT_RW
static inline int exfat_ent_prepare(struct inode *inode,
                                    struct exfat_ent *fatent,
                                    u32 clusnr, u32 *result_clusnr)
{
    struct super_block *sb = inode->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    __le32 *ent;

    fatent->bh = sb_bread(sb, sbi->fat_blocknr + (clusnr >> sbi->fpb_bits));
    if (!fatent->bh)
    {
        return -EIO;
    }

    fatent->offset = clusnr & (sbi->fpb - 1);
    ent = (__le32 *)fatent->bh->b_data;
    *result_clusnr = le32_to_cpu(ent[fatent->offset]);

    return 0;
}

static inline int exfat_ent_write(struct inode *inode,
                                  struct exfat_ent *fatent,
                                  u32 new_clusnr, int wait)
{
    int err;
    __le32 *ent = (__le32 *)fatent->bh->b_data;
    ent[fatent->offset] = cpu_to_le32(new_clusnr);
    if (wait)
    {
        err = exfat_sync_bhs(&(fatent->bh), 1);
        if (err)
        {
            return err;
        }
    }
    else
    {
        mark_buffer_dirty(fatent->bh);
    }
    return 0;
    /* maybe should update other FAT cluster chain info */
    //return exfat_mirror_bhs(sb, fatent->bhs, fatent->nr_bhs);
}

/*
 * write contiguous clusters chain
 *
 */
static inline int exfat_ent_write_conti(struct inode *inode,
                                        struct exfat_ent *fatent,
                                        u32 new_clusnr, int nr_clus,
                                        int wait)
{
    int err, index;
    __le32 clusnr;
    __le32 *ent = (__le32 *)fatent->bh->b_data;
    index = fatent->offset;
    clusnr = cpu_to_le32(new_clusnr);
    while (nr_clus--)
    {
        ent[index++] = clusnr++;
    }
    if (wait)
    {
        err = exfat_sync_bhs(&(fatent->bh), 1);
        if (err)
        {
            return err;
        }
    }
    else
    {
        mark_buffer_dirty(fatent->bh);
    }
    return 0;

    /* maybe should update mirror FAT cluster chain here */
    //return exfat_mirror_bhs(sb, fatent->bhs, fatent->nr_bhs);
}
#endif  /*  FSYS_EXFAT_RW */


#endif /* __EXFAT_FATENT_H__ */