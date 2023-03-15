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
#include "exfatfs.h"
#include "exfat_bitmap.h"
#include "buffer_head.h"
#include "errno.h"
#include "fsys_debug.h"
#include "endians.h"


/**
 * hweightN - returns the hamming weight of a N-bit word
 * @x: the word to weigh
 *
 * The Hamming Weight of a number is the total number of bits set in it.
 */
static unsigned int hweight32(unsigned int w)
{
    unsigned int res = w - ((w >> 1) & 0x55555555);
    res = (res & 0x33333333) + ((res >> 2) & 0x33333333);
    res = (res + (res >> 4)) & 0x0F0F0F0F;
    res = res + (res >> 8);
    return (res + (res >> 16)) & 0x000000FF;
}

static unsigned long hweight64(__u64 w)
{
#if BITS_PER_LONG == 32
    return hweight32((unsigned int)(w >> 32)) + hweight32((unsigned int)w);
#elif BITS_PER_LONG == 64

    __u64 res = w - ((w >> 1) & 0x5555555555555555ul);
    res = (res & 0x3333333333333333ul) + ((res >> 2) & 0x3333333333333333ul);
    res = (res + (res >> 4)) & 0x0F0F0F0F0F0F0F0Ful;
    res = res + (res >> 8);
    res = res + (res >> 16);
    return (res + (res >> 32)) & 0x00000000000000FFul;
#endif
}


static inline unsigned long hweight_long(unsigned long w)
{
    return sizeof(w) == 4 ? hweight32(w) : hweight64(w);
}

/* Carry-Save Adder */
#define CSA(h, l, a, b, c)  do {        \
        unsigned long u = a ^ b;        \
        unsigned long v = c;            \
        h = (a & b) | (u & v);          \
        l = u ^ v;              \
    } while (0)

/* TODO: maybe, should merge to bitmap_weight(). */
static int hweight_array(const unsigned long *bitmap, int n)
{
    unsigned long ones, twos, twosA, twosB, fours, foursA, foursB, eights;
    int counts, i;

    counts = 0;
    ones = twos = fours = 0;
    for (i = 0; i <= n - 8; i += 8)
    {
        CSA(twosA, ones, ones, bitmap[i], bitmap[i + 1]);
        CSA(twosB, ones, ones, bitmap[i + 2], bitmap[i + 3]);
        CSA(foursA, twos, twos, twosA, twosB);
        CSA(twosA, ones, ones, bitmap[i + 4], bitmap[i + 5]);
        CSA(twosB, ones, ones, bitmap[i + 6], bitmap[i + 7]);
        CSA(foursB, twos, twos, twosA, twosB);
        CSA(eights, fours, fours, foursA, foursB);
        counts += hweight_long(eights);
    }
    counts = 8 * counts + 4 * hweight_long(fours)
             + 2 * hweight_long(twos) + hweight_long(ones);
    for (; i < n; i++)
    {
        counts += hweight_long(bitmap[i]);
    }
    return counts;
}

/* Hack: want bitmap_weight_le() */
static int exfat_hweight_le(const void *buf, int bits)
{
    const unsigned long *bitmap = buf;
    int counts;

    /* counts = bitmap_weight(bitmap, bits & ~(BITS_PER_LONG - 1)); */
    counts = hweight_array(bitmap, BIT_WORD(bits & ~(BITS_PER_LONG - 1)));
    if (bits & (BITS_PER_LONG - 1))
    {
        unsigned long word;
        word = lelong_to_cpu(*(__lelong *)(&bitmap[BIT_WORD(bits)]));
        counts += hweight_long(word & BITMAP_LAST_WORD_MASK(bits));
    }
    return counts;
}

int exfat_count_free_clusters(struct inode *inode)
{
    int err;
    __s64 pos;
    sector_t blocknr;
    unsigned long blocks;
    struct super_block *sb = inode->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct buffer_head *bh;
    u32 bits, bits_left, used_bits;
    u32 sb_blksize;
    u8  sb_blksize_bits;

    sb_blksize = sb->s_blocksize;
    sb_blksize_bits = sb->s_blocksize_bits;

    /* FIXME: readahead all data of bitmap */
    pos = 0;
    used_bits = 0;
    bits_left = sbi->total_clusters;
    blocks = 0;
    while (bits_left > 0)
    {

        /* readahead all left block number */
        if (!blocks)
        {
            blocks = (inode->i_size - pos + (sb_blksize - 1)) >> sb_blksize_bits;
            err = exfat_get_block(inode, pos >> sb_blksize_bits, &blocks, &blocknr, 0);
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
        /*
         * read one block once
         */
        bh = sb_bread(sb, blocknr);
        if (NULL == bh || NULL == bh->b_data)
        {
            fs_log_error("%s : sb_bread fail\n", __func__);
            err = -EIO;
            goto error;
        }

        bits = min_t(u32, bits_left, sb_blksize * 8);  /* 1 byte : 8 bits */

        used_bits += exfat_hweight_le((void *)(bh->b_data), bits);

        brelse(bh);

        bits_left -= bits;
        pos += sb_blksize;
        blocknr++;
        blocks--;
    }
    sbi->free_clusters = sbi->total_clusters - used_bits;
error:
    fs_log_info("%s: free cluster number %u\n", __func__, sbi->free_clusters);

    /* when encount error
     * should continue or stop
     */
    return err;
}

int exfat_setup_bitmap(struct super_block *sb, u32 clusnr, u64 i_size)
{
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct inode *inode;
    int err;

    if (!clusnr)
    {
        fs_log_error("exFAT: free space bitmap isn't available\n");
        return -EINVAL;
    }
    /* check bitmap inode size */
    if (i_size < DIV_ROUND_UP(sbi->total_clusters, 8))
    {
        fs_log_error("exFAT: free space bitmap is too small\n");
        return -EINVAL;
    }

    inode = exfat_new_internal_inode(sb, EXFAT_BITMAP_INO, 0,
                                     clusnr, i_size);
    if (!inode)
    {
        return -ENOMEM;
    }

    err = exfat_count_free_clusters(inode);
    if (err)
    {
        return err;
    }

    sbi->bitmap_inode = inode;

    return 0;
}

void exfat_free_bitmap(struct exfat_sb_info *sbi)
{
    if (sbi->bitmap_inode)
    {
        exfat_free_internal_inode(sbi->bitmap_inode);
        sbi->bitmap_inode = NULL;
    }
}

#if defined CONFIG_FSYS_EXFAT_RW
/**
 * exfat_bit_set - set a bit in a field of bits
 * @bitmap: field of bits
 * @bit:    bit to set
 * @new_value:  value to set bit to (0 or 1)
 *
 * Set the bit @bit in the @bitmap to @new_value. Ignore all errors.
 */
void exfat_bit_set(u8 *bitmap, const u64 bit, const u8 new_value)
{
    if (!bitmap || new_value > 1)
    {
        return;
    }
    if (!new_value)
    {
        bitmap[bit >> 3] &= ~(1 << (bit & 7));
    }
    else
    {
        bitmap[bit >> 3] |= (1 << (bit & 7));
    }
}

void exfat_set_bits(u8 *bitmap,  u64 offset,
                    int bits_nr, u8 value)
{
    /* maybe other method : for fast bit operation */
    while (bits_nr)
    {
        exfat_bit_set(bitmap, offset, value);
        offset++;
        bits_nr--;
    }
}

/**
 * exfat_bit_get - get value of a bit in a field of bits
 * @bitmap: field of bits
 * @bit:    bit to get
 *
 * Get and return the value of the bit @bit in @bitmap (0 or 1).
 * Return -1 on error.
 */
char exfat_bit_get(const u8 *bitmap, const u64 bit)
{
    if (!bitmap)
    {
        return -1;
    }
    return (bitmap[bit >> 3] >> (bit & 7)) & 1;
}
#endif  /* FSYS_EXFAT_RW */