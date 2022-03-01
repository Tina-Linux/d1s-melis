/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : exfat_bitmap.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-16
* Descript: exfat file system bitmap handing functions.
* Update  : date                auther      ver     notes
*           2011-3-16 14:20:06  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __EXFAT_BITMAP_H__
#define __EXFAT_BITMAP_H__

#define  BITS_PER_LONG   32

#if BITS_PER_LONG == 64
typedef __le64          __lelong;
#define lelong_to_cpu(x)    le64_to_cpu(x);
#else
typedef __le32          __lelong;
#define lelong_to_cpu(x)    le32_to_cpu(x);
#endif

#define BITMAP_LAST_WORD_MASK(nbits)                    \
    (                                   \
                                        ((nbits) % BITS_PER_LONG) ?                 \
                                        (1UL<<((nbits) % BITS_PER_LONG))-1 : ~0UL       \
    )

#define BIT_WORD(nr)        ((nr) / BITS_PER_LONG)

int  exfat_setup_bitmap(struct super_block *sb, u32 clusnr, u64 i_size);
void exfat_free_bitmap(struct exfat_sb_info *sbi);
int  exfat_count_free_clusters(struct inode *inode);

void exfat_bit_set(u8 *bitmap, const u64 bit, const u8 new_value);
void exfat_set_bits(u8 *bitmap,  u64 offset,
                    int bits_nr, u8 value);
char exfat_bit_get(const u8 *bitmap, const u64 bit);

#endif /* __EXFAT_BITMAP_H__ */
