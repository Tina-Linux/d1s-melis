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