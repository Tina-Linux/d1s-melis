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
#ifndef _EXFAT_CACHE_H_
#define _EXFAT_CACHE_H_

#define EXFAT_CACHE_VALID       0   /* special case for valid cache */

/* This must be > 0. */
#define EXFAT_MAX_CACHE         8

#define exfat_cache_entry(x)    list_entry(x, struct exfat_cache, cache_list)

struct exfat_cache
{
    struct list_head cache_list;
    u32 iclusnr;        /* cluster number on data of inode. */
    u32 clusnr;         /* cluster number on disk. */
    u32 len;            /* number of contiguous clusters */
};

struct exfat_cache_id
{
    u32 id;         /* cache generation id */
    u32 iclusnr;        /* cluster number on data of inode. */
    u32 clusnr;         /* cluster number on disk. */
    u32 len;            /* number of contiguous clusters */
};

static inline void exfat_cache_init(struct exfat_cache_id *cid,
                                    u32 iclusnr, u32 clusnr)
{
    cid->id = EXFAT_CACHE_VALID;
    cid->iclusnr = iclusnr;
    cid->clusnr = clusnr;
    cid->len = 1;
}

static inline int exfat_cache_contiguous(struct exfat_cache_id *cid, u32 clusnr)
{
    return (cid->clusnr + cid->len) == clusnr;
}

int  exfat_init_cache_cachep(void);
void exfat_destroy_cache_cachep(void);
int  exfat_cache_lookup(struct inode *inode, u32 iclusnr,
                        struct exfat_cache_id *cid);
void exfat_cache_add(struct inode *inode, struct exfat_cache_id *new);
void exfat_cache_inval(struct inode *inode);
void exfat_cache_inode_init_once(struct exfat_inode_info *exi);

#endif /* _EXFAT_CACHE_H_ */