/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : exfat_upcase.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-16
* Descript: exfat file system upcase handing functions.
* Update  : date                auther      ver     notes
*           2011-3-16 14:28:54  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "exfatfs.h"
#include "exfat_upcase.h"
#include "buffer_head.h"
#include "page_pool.h"
#include "slab.h"
#include "errno.h"
#include "endians.h"
#include "fsys_libs.h"
#include "fsys_debug.h"

/*
 * exFAT upper case conversion table.
 *
 * The table is 16bits array, and 16bits value is unicode of
 * upper-case letter.
 *
 * There is a special value - 0xffff. It is used for specifying the
 * range has no upper-case letter, and following 16bits of special
 * value is length of the range. so the up-case conversion table can
 * reduce size much more
 *
 * E.g:
 *
 * offset:   dst unicode : src unicode
 *  0000h:  0000 (U+0000):   U+0000
 *  0002h:  0001 (U+0001):   U+0001
 * ...
 *  0b0ch:  0556 (U+0556):   U+0586
 *  0b0eh:  ffff         :
 *  0b10h:  17f6         :
 *  0b12h:  2c63 (U+2c63):   U+1d7d (U+0587 + 17f6h)
 *  0b14h:  1d7e (U+1d7e):   U+1d7e
 * ...
 */

#define MAX_UPCASE_SIZE     (0x10000 * sizeof(u16))
#define MAX_CODE            0xffff
#define UPCASE_HOLE         0xffff
#define VMALLOC_THRESHOLD   8192

struct upcase_range
{
    u16 *tbl;           /* upper-case table in range */
    u16 start;          /* start of unicode in range */
    u16 end;            /* end of unicode in range */
};

struct upcase
{
    u16 *table;                     /* upper-case table */
    int table_size;                 /* upper-case table size */
    int     nr_range;               /* number of ranges */
    struct upcase_range ranges[0];  /* upper-case table ranges */
};

static u16 *exfat_upcase_table_alloc(int size)
{
    u16 *table;

    /*
     * exFAT up-case table allocate in page_allocate mode
     */
    table = (u16 *)palloc(size);

    return table;
}

static void exfat_upcase_table_free(const u16 *table, int size)
{
    /*
     * exFAT up-case table allocate in page_allocate mode
     */
    pfree((void *)table, size);
}

static struct upcase *exfat_upcase_alloc(int range_cnt)
{
    int size = sizeof(struct upcase)
               + sizeof(struct upcase_range) * range_cnt;

    return kmalloc(size, GFP_KERNEL);
}

static int exfat_parse_upcase_table(struct inode *inode, u32 checksum)
{
    struct super_block *sb = inode->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    __s64 pos;
    sector_t blocknr;
    unsigned long blocks;
    unsigned long block_size;
    unsigned long blocksize_bits;
    struct upcase *upcase;
    u16 *tbl;
    loff_t left;
    u32 csum;
    u16 uc, src_start, src_len, tbl_pos, tbl_end;
    int i, err, size, overflow, is_hole, range_cnt;

    /* for faster read purpose , if tbl is Palloc , setup DMA read directly  */
    tbl = exfat_upcase_table_alloc(inode->i_size);
    if (!tbl)
    {
        return -ENOMEM;
    }

    /* FIXME: readahead all data of upper-case table */

    csum = 0;
    uc = src_start = src_len = 0;
    overflow = tbl_pos = is_hole = range_cnt = 0;
    tbl_end = (inode->i_size / sizeof(u16)) - 1;

    pos = 0;
    blocks = 0;
    left = inode->i_size;
    block_size = sb->s_blocksize;
    blocksize_bits = sb->s_blocksize_bits;
    while (left > 0 && !overflow)
    {
        struct buffer_head *bh;
        char *kaddr;

        /* readahead left block number of up-case table */
        if (!blocks)
        {
            blocks = (left + (block_size - 1)) >> blocksize_bits;

            err = exfat_get_block(inode, pos >> blocksize_bits, &blocks, &blocknr, 0);
            if (err)
            {
                fs_log_error("%s : get block number have error\n", __func__);
                goto error;
            }
            if (!blocknr)
            {
                fs_log_error("%s : get invalid block number\n", __func__);
                err = -EINVAL;
                goto error;
            }
        }
        bh = sb_bread(sb, blocknr);
        if (NULL == bh || NULL == bh->b_data)
        {
            fs_log_error("%s : sb_bread fail\n", __func__);
            err = -EIO;
            goto error;
        }

        kaddr = bh->b_data;
        size = min(left, (loff_t)block_size);
        pos += size;

        for (i = 0; i < size; i += sizeof(u16))
        {

            uc = le16_to_cpu(*(__le16 *)(kaddr + i));
            left -= sizeof(u16);

            /*
             * check if a range end when encount up-case hole
             */
            if (is_hole)
            {
                u16 skiplen = uc;
                u16 end_pos = tbl_end - range_cnt * 2;

                if (src_start + src_len + skiplen >= MAX_CODE)
                {
                    overflow = 1;
                    break;
                }

                /*
                 * Save range temporarily at end of table.
                 * UPCASE_HOLE and skiplen aren't saved into
                 * tbl, so there is space for those.
                 */
                tbl[end_pos] = src_start;
                tbl[end_pos - 1] = src_len;

                /* If it's bogus empty range, ignore it. */
                if (src_len)
                {
                    range_cnt++;
                }

                /* parse another new range begin */
                is_hole = 0;
                src_start += src_len + skiplen;
                src_len = 0;
            }
            else if (uc == UPCASE_HOLE && left >= sizeof(u16))
            {
                is_hole = 1;
            }
            else
            {
                tbl[tbl_pos] = uc;
                tbl_pos++;
                src_len++;
            }
        }
        csum = exfat_checksum32(csum, (void *)kaddr, size);
        brelse(bh);
        blocknr++;
        blocks--;
    }

    err = 0;
    if (overflow)
    {
        fs_log_error("invalid upper-case table");
        goto error;
    }
    else if (checksum != csum)
    {
        fs_log_error("checksum failed for upper-case table"
                     " (0x%08x != 0x%08x)", checksum, csum);
        goto error;
    }
    else if (!range_cnt && !src_len)
    {
        fs_log_error("upper-case table is empty");
        goto error;
    }

    err = -ENOMEM;
    upcase = exfat_upcase_alloc(range_cnt + !!src_len);
    if (!upcase)
    {
        goto error;
    }

    upcase->table = tbl;
    upcase->table_size = inode->i_size;
    upcase->nr_range = range_cnt + !!src_len;

    tbl_pos = 0;
    for (i = 0; i < range_cnt; i++)
    {
        u16 end_pos = tbl_end - i * 2;
        u16 start = upcase->table[end_pos];
        u16 len = upcase->table[end_pos - 1];

        upcase->ranges[i].tbl = tbl + tbl_pos;
        upcase->ranges[i].start = start;
        upcase->ranges[i].end = start + len - 1;

        tbl_pos += len;
    }
    if (src_len)
    {
        upcase->ranges[i].tbl = tbl + tbl_pos;
        upcase->ranges[i].start = src_start;
        upcase->ranges[i].end = src_start + src_len - 1;
    }

    sbi->upcase = upcase;

    return 0;

error:
    exfat_upcase_table_free(tbl, inode->i_size);
    return err;
}

int exfat_setup_upcase(struct super_block *sb,
                       u32 checksum, u32 clusnr, u64 i_size)
{
    struct inode *inode;
    int err;

    /*
     * NOTE: even if upper-case table is invalid, we continue to
     * work without it as read-only filesystem.
     */
    if (!clusnr)
    {
        fs_log_error("couldn't find upper-case table");
        return 0;
    }
    if (i_size > MAX_UPCASE_SIZE)
    {
        fs_log_error("upper-case table size is too big");
        return 0;
    }
    if (i_size & (sizeof(u16) - 1))
    {
        /* i_size is strange, fix up */
        i_size &= ~(u64)sizeof(u16);
    }
    if (!i_size)
    {
        fs_log_error("upper-case table size is zero");
        return 0;
    }
    inode = exfat_new_internal_inode(sb, EXFAT_UPCASE_INO, 0,
                                     clusnr, i_size);
    if (!inode)
    {
        return -ENOMEM;
    }

    /* Parse upper-case data, then make table for exfat_towupper(). */
    err = exfat_parse_upcase_table(inode, checksum);
    exfat_free_internal_inode(inode);
    return err;
}

void exfat_free_upcase(struct exfat_sb_info *sbi)
{
    if (sbi->upcase)
    {
        struct upcase *upcase = sbi->upcase;
        exfat_upcase_table_free(upcase->table, upcase->table_size);
        kfree(upcase);
        sbi->upcase = NULL;
    }
}

__wchar_t exfat_towupper(struct exfat_sb_info *sbi, __wchar_t wc)
{
    struct upcase *upcase = sbi->upcase;
    int i;

    if (!upcase)
    {
        return wc;
    }

    for (i = 0; i < upcase->nr_range; i++)
    {
        if (wc < upcase->ranges[i].start)
        {
            break;
        }
        if (wc > upcase->ranges[i].end)
        {
            continue;
        }
        return upcase->ranges[i].tbl[wc - upcase->ranges[i].start];
    }
    return wc;
}
