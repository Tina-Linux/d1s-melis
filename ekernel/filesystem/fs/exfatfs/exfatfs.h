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
#ifndef __EXFATFS_H__
#define __EXFATFS_H__

#include "fs.h"
#include "exfat_fs.h"

/* exfat_debug.c : config debug info */
//#define   EXFAT_DEBUG

#ifdef  EXFAT_DEBUG
#define EXFAT_DEBUG0_ON(cond, format...)    do {        \
        if (cond) {                 \
            printk("---- DEBUG_ON: %s:%u -----\n",  \
                   __func__, __LINE__);     \
            BUG();                  \
        }                       \
    } while (0)

#define EXFAT_DEBUG_ON(cond, format...) do {        \
        if (cond) {                 \
            printk("---- DEBUG_ON: %s:%u -----\n",  \
                   __func__, __LINE__);     \
            printk(format);             \
            BUG();                  \
        }                       \
    } while (0)

#else

#define EXFAT_DEBUG0_ON(cond, format...)

#define EXFAT_DEBUG_ON(cond, format...)

#endif      /* EXFAT_DEBUG */

/* Current limitations */
#define EXFAT_MIN_BLKSIZE   (1 << EXFAT_MIN_BLKBITS)
#define EXFAT_MIN_BLKBITS   9           /* 512 */
#define EXFAT_MAX_BLKBITS   12          /* 4096 */
#define EXFAT_MAX_CLUS_BITS 25          /* 32MB */

/* Number of buffers for maximum size of a directory entry */
#define EXFAT_DE_MAX_SUB_CHUNKS  255
#define EXFAT_DE_MAX_CHUNKS      (1 + EXFAT_DE_MAX_SUB_CHUNKS)
#define EXFAT_DE_MAX_SIZE        (EXFAT_DE_MAX_CHUNKS << EXFAT_CHUNK_BITS)
#define EXFAT_DE_MAX_BH          (DIV_ROUND_UP(EXFAT_DE_MAX_SIZE, EXFAT_MIN_BLKSIZE) + 1)

/* Reverved inode numbers */
#define EXFAT_ROOT_INO      1
#define EXFAT_BITMAP_INO    2
#define EXFAT_UPCASE_INO    3
#define EXFAT_RESERVED_INO  EXFAT_UPCASE_INO

#define EXFAT_HASH_BITS     8
#define EXFAT_HASH_SIZE     (1UL << EXFAT_HASH_BITS)

struct upcase;

struct exfat_mount_opts
{
    uid_t       uid;
    gid_t       gid;
    umode_t     fmode;
    umode_t     dmode;
    //  struct nls_table *nls;
};

struct exfat_sb_info
{
    //  spinlock_t  inode_hash_lock;
    struct hlist_head inode_hashtable[EXFAT_HASH_SIZE];

    sector_t    fat_blocknr;        /* start blocknr of FAT */
    u32     fat_block_counts;       /* number of FAT blocks */
    u32     fpb;                    /* FAT-entries per block */
    u8      fpb_bits;               /* bits of FAT-entries per block */
    u8      clus_bits;              /* bits of cluster size */
    u8      bpc_bits;               /* bits of block per cluster */
    unsigned int    bpc;            /* block per cluster */
    unsigned long   clus_size;      /* cluster size */

    u8      cpbb_bits;              /* bits of cluster per bitmap block */
    u32     cpbb;                   /* cluster number per bitmap block */

    u32     chunk_per_block;        /* chunks number per block */

    sector_t    clus_blocknr;       /* start blocknr of cluster */
    u32     rootdir_clusnr;         /* start clusnr of rootdir */
    u32     total_clusters;         /* number of total clusters */
    u32     free_clusters;          /* number of free clusters, -1 undefined */

    /* maybe add to improve performance */
    //sector_t bitmap_first_blocknr;    /* bitmap first block number */

    u16     state;                  /* state of this volume */
    u32     serial_number;          /* serial number */

    struct inode    *bitmap_inode;  /* inode for bitmap */
    struct upcase   *upcase;        /* upper-case table info */

    struct exfat_mount_opts opts;
};

struct exfat_inode_info
{
    struct hlist_node i_hash;

    //  spinlock_t  cache_lru_lock;     /* lock for caches */
    struct list_head cache_lru;     /* LRU list for caches */
    int     nr_caches;              /* number of caches */
    u32     cache_valid_id;         /* For detecting the race of
                                     * truncate and get_cluster */

    /* NOTE: phys_size is 64bits, so must hold ->i_mutex to access */
    loff_t      phys_size;          /* physically allocated size */

    u32         clusnr;
    __le16      attrib;
    u8          data_flag;

    /* FIXME: de_blocknr is big. should rethink more small, and more
     * faster way */
    sector_t    de_blocknr[EXFAT_DE_MAX_BH];/* blocknr of this entry */
    unsigned long   de_offset;      /* start offset in buffer */
    unsigned long   de_size;        /* size of this entry */

    /* ????? add to improve performance */
    //u32 last_clusnr;

    struct inode    vfs_inode;
};

struct exfat_parse_iter_data
{
    unsigned long   left;
    unsigned long   bh_offset;
    unsigned int    bh_index;
};

struct exfat_parse_data
{
    unsigned long   size;                       /* size of this entry */
    struct buffer_head *bhs[EXFAT_DE_MAX_BH];   /* buffers for this entry */
    unsigned long   bh_offset;                  /* start offset in buffer */
    int             nr_bhs;                     /* number of buffer heads */
};

struct exfat_slot_info
{
    int                         nr_slots;   /* chunks number */
    int                         is_dir;     /* chunks make of a directory or not */
    int                         clusnr;     /* first cluster number or zero */
    struct exfat_parse_data     pd;         /* parse data structure */
    struct exfat_chunk_dirent  *dirent;     /* pointer of dirent chunk */
    struct exfat_chunk_data    *data;       /* pointer of data chunk */
};

static inline struct exfat_sb_info *EXFAT_SB(struct super_block *sb)
{
    return sb->s_fs_info;
}

static inline struct exfat_inode_info *EXFAT_I(struct inode *inode)
{
    return container_of(inode, struct exfat_inode_info, vfs_inode);
}

static inline sector_t exfat_clus_to_blknr(struct exfat_sb_info *sbi, u32 clusnr)
{
    EXFAT_DEBUG_ON(clusnr < EXFAT_START_ENT, "clusnr 0x%08x\n", clusnr);
    return sbi->clus_blocknr +
           ((sector_t)(clusnr - EXFAT_START_ENT) << sbi->bpc_bits);
}

/* Is the FAT entry free? */
static inline int exfat_ent_free(struct exfat_sb_info *sbi, u32 clusnr)
{
    return clusnr == EXFAT_ENT_FREE;
}

/* Is the FAT entry bad cluster? */
static inline int exfat_ent_bad(struct exfat_sb_info *sbi, u32 clusnr)
{
    return clusnr == EXFAT_ENT_BAD;
}

/* Is the FAT entry EOF? */
static inline int exfat_ent_eof(struct exfat_sb_info *sbi, u32 clusnr)
{
    return clusnr == EXFAT_ENT_EOF;
}

/* Is the FAT entry valid as normal entry? */
static inline int exfat_ent_valid(struct exfat_sb_info *sbi, u32 clusnr)
{
    return (clusnr - EXFAT_START_ENT) < sbi->total_clusters;
}

/* Is the FAT entry contiguous */
static inline int exfat_ent_contiguous(int prev, int next)
{
    return ((prev + 1) == next);
}

/* helper for printk */
typedef unsigned long long  llu;

/* exfat_dir.c */
extern struct dentry_operations exfat_dentry_ops;
extern const struct inode_operations exfat_dir_inode_ops;
extern const struct file_operations exfat_dir_ops;

int  exfat_find(struct inode *dir, struct qstr *qname,
                struct exfat_slot_info *sinfo);

struct dentry *exfat_lookup(struct inode *dir, struct dentry *dentry,
                            struct nameidata *nd);

unsigned int exfat_subdirs(struct inode *dir);

int     exfat_read_rootdir(struct inode *inode);
void    pd_init(struct exfat_parse_data *pd);
void    pd_dirty(struct exfat_parse_data *pd);
void    pd_brelse(struct exfat_parse_data *pd);
void   *pd_iter_first_de(struct exfat_parse_data *pd,
                         struct exfat_parse_iter_data *pd_iter);
void   *pd_iter_next_de(struct exfat_parse_data *pd,
                        struct exfat_parse_iter_data *pd_iter);

/* exfat_file.c */
extern const struct inode_operations exfat_file_inode_ops;
extern const struct file_operations exfat_file_ops;
void exfat_truncate(struct inode *inode);


/* exfat_inode.c */
int exfat_get_block(struct inode *inode,
                    int iblock,
                    unsigned long *max_blocks,
                    sector_t *phys,
                    int create);
void exfat_hash_init(struct super_block *sb);
void exfat_detach(struct inode *inode);
void exfat_attach(struct inode *inode, struct exfat_parse_data *pd);
struct inode *exfat_ilookup(struct super_block *sb, sector_t blocknr,
                            unsigned long offset);
struct inode *exfat_build_inode(struct super_block *sb,
                                struct exfat_parse_data *pd,
                                struct exfat_chunk_dirent *dirent,
                                struct exfat_chunk_data *data);
struct inode *exfat_new_internal_inode(struct super_block *sb,
                                       unsigned long ino, u16 attrib,
                                       u32 clusnr, u64 size);
struct inode *exfat_rootdir_iget(struct super_block *sb);
void   exfat_free_internal_inode(struct inode *inode);

/* exfat_untils.c */
u32 exfat_checksum32(u32 sum, const void *__buf, int size);
u16 exfat_checksum16(u16 sum, const void *__buf, int size);
int exfat_conv_u2c(struct nls_table *nls,
                   const __le16 **uni, unsigned int *uni_left,
                   unsigned char **out, unsigned int *out_left);
int exfat_conv_c2u(struct nls_table *nls,
                   const unsigned char **in, unsigned int *in_left,
                   __wchar_t **uni, unsigned int *uni_left);
void exfat_time_exfat2unix(struct timespec *ts, __le16 time, __le16 date,
                           u8 time_cs);
void exfat_time_unix2exfat(struct timespec *ts, __le16 *time, __le16 *date,
                           u8 *time_cs);

/* exfat_super.c */
__s32     exfat_fill_super(struct super_block *sb, void *data, __s32 silent);
int     exfat_init_inodecache(void);
void    exfat_destroy_inodecache(void);
int     exfat_sync_inode(struct inode *inode);
int     exfat_flush_inodes(struct super_block *sb,
                           struct inode *i1, struct inode *i2);

/* exfat_namei.c */
int exfat_sync_bhs(struct buffer_head **bhs, int nr_bhs);

#endif /* !__EXFATFS_H__ */