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
#include "exfat_cache.h"
#include "exfat_cluster.h"
#include "buffer_head.h"
#include "page_pool.h"
#include "slab.h"
#include "errno.h"
#include "endians.h"
#include "fsys_libs.h"
#include "fsys_debug.h"

#if defined CONFIG_FSYS_EXFAT_RW
int exfat_add_clusters(struct inode *inode, int nr_newclus)
{
    int err;
    int *cluster;
    u8  data_flag;

    /*
     * NOTE : if nr_newclus too large, kmalloc will fail,
     * the max valid size for kmalloc is 8K under epdk now.
     * so use system malloc to replace kmalloc.
     * By sunny  2010.1.29
     */
    //  cluster = (int *)kmalloc(sizeof(int) * nr_newclus, 0);
    cluster = (int *)malloc(sizeof(int) * nr_newclus);
    if (!cluster)
    {
        fs_log_error("allocate memory by system malloc failed\n");
        return -ENOMEM;
    }
    err = exfat_alloc_clusters(inode, cluster, nr_newclus, &data_flag);
    if (err)
    {
        //      kfree(cluster);
        free(cluster);
        return err;
    }
    /* FIXME: this cluster should be added after data of this
     * cluster is writed */
    err = exfat_chain_add(inode, *cluster, nr_newclus, data_flag);
    if (err)
    {
        exfat_free_clusters(inode, *cluster, nr_newclus, data_flag);
    }
    //  kfree(cluster);
    free(cluster);
    return err;
}
#endif  /* FSYS_EXFAT_RW */

int exfat_get_block(struct inode *inode,
                    int iblock,
                    unsigned long *max_blocks,
                    sector_t *phys,
                    int create)
{
    struct super_block *sb = inode->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct exfat_inode_info *exi = EXFAT_I(inode);
    struct exfat_clus_map cmap;
    unsigned long blocksize, mapped_blocks;
    sector_t blocknr, last_iblock;
    u32 iclusnr, last_iclusnr, clus_len;
    int nr_newclus, blk_in_alloced_clus, err, offset;

    err = 0;
    blocksize = 1 << inode->i_blkbits;
    last_iblock = (exi->phys_size + (blocksize - 1)) >> inode->i_blkbits;
    if (!create && iblock >= last_iblock)
    {
        return -EIO;
    }
    /* Find the available blocks already allocating */
    last_iclusnr = (last_iblock + (sbi->bpc - 1)) >> sbi->bpc_bits;
    iclusnr = iblock >> sbi->bpc_bits;
    offset = iblock & (sbi->bpc - 1);
    mapped_blocks = 0;
    clus_len = (*max_blocks + (sbi->bpc - 1)) >> sbi->bpc_bits;
    if (iclusnr < last_iclusnr)
    {

        /* Some clusters are available */
        err = exfat_get_cluster(inode, iclusnr, clus_len, &cmap);
        if (err)
        {
            return err;
        }
        if (!exfat_ent_valid(sbi, cmap.clusnr))
        {
            fs_log_error("exFAT: unexpected FAT entry"
                         "(start cluster 0x%08x, entry 0x%08x)\n",
                         exi->clusnr, cmap.clusnr);
            return -EIO;
        }
        blocknr = exfat_clus_to_blknr(sbi, cmap.clusnr) + offset;
        mapped_blocks = (cmap.len << sbi->bpc_bits) - offset;
        if (iblock < last_iblock)
        {
            /* Some blocks are available */
            unsigned long avail_blocks = last_iblock - iblock;

            mapped_blocks = min(mapped_blocks, avail_blocks);
            mapped_blocks = min(mapped_blocks, *max_blocks);

            *phys = blocknr;
            *max_blocks = mapped_blocks;
            return 0;
        }
        /* We can allocate blocks in this last cluster */
    }
    /* Allocate new blocks
     * new blocks can allocate from:
     * 1.blocks in the last cluster,
     * 2.new blocks in new cluster. */
#if defined CONFIG_FSYS_EXFAT_RW
    if (!create)
    {
        err = -EIO;
        goto error;
    }
    if (iblock != exi->phys_size >> inode->i_blkbits)
    {
        fs_log_error("iblock %llu, phys_size %lld\n",
                     (llu)iblock, exi->phys_size);
        err = -EIO;
        goto error;
    }
    blk_in_alloced_clus = offset ? min((unsigned long)(sbi->bpc - offset), *max_blocks) : 0;
    nr_newclus = FSYS_ALIGN((*max_blocks - blk_in_alloced_clus), sbi->bpc) >> sbi->bpc_bits;
    if (nr_newclus)
    {

        /* add new clusters would be desirable. */
        err = exfat_add_clusters(inode, nr_newclus);
        if (err)
        {
            goto error;
        }
    }
    /* update inode physically allocated size here */
    exi->phys_size += ((blk_in_alloced_clus << sb->s_blocksize_bits) +
                       (nr_newclus << sbi->clus_bits));

    /* available blocks on this cluster */
    err = exfat_get_cluster(inode, iclusnr, clus_len, &cmap);
    if (err)
    {
        goto error;
    }
    if (!exfat_ent_valid(sbi, cmap.clusnr))
    {
        fs_log_error("exFAT: unexpected FAT entry"
                     "(start cluster %x, entry %x)\n",
                     exi->clusnr, cmap.clusnr);
        err = -EIO;
        goto error;
    }
    blocknr = exfat_clus_to_blknr(sbi, cmap.clusnr) + offset;
    mapped_blocks = (cmap.len << sbi->bpc_bits) - offset;
    *max_blocks = min(mapped_blocks, *max_blocks);
    *phys = blocknr;
    return 0;

error:
#endif  /* exfat_add_clusters */
    return err;
}

/* See comment in fs/fat/inode.c */
/* FIXME: rethink those hash stuff */
void exfat_hash_init(struct super_block *sb)
{
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    int i;

    //  spin_lock_init(&sbi->inode_hash_lock);
    for (i = 0; i < EXFAT_HASH_SIZE; i++)
    {
        INIT_HLIST_HEAD(&sbi->inode_hashtable[i]);
    }
}

/* maybe use fs/hash.h hash_long() directly */
#define GOLDEN_RATIO_PRIME_32 0x9e370001UL

static inline u32 hash_32(u32 val, unsigned int bits)
{
    /* On some cpus multiply is faster, on others gcc will do shifts */

    u32 hash = val * GOLDEN_RATIO_PRIME_32;

    /* High bits are more random, so use them. */
    return hash >> (32 - bits);
}

static inline u32 exfat_hash(struct super_block *sb, sector_t blocknr,
                             unsigned long offset)
{
    u32 key = (((loff_t)blocknr << sb->s_blocksize_bits) | offset)
              >> EXFAT_CHUNK_BITS;
    return hash_32(key, EXFAT_HASH_BITS);
}

void exfat_attach(struct inode *inode, struct exfat_parse_data *pd)
{
    struct super_block *sb = inode->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct exfat_inode_info *exi = EXFAT_I(inode);
    u32 hashval = exfat_hash(sb, pd->bhs[0]->b_blocknr, pd->bh_offset);
    struct hlist_head *head = sbi->inode_hashtable + hashval;
    int i;

    //  spin_lock(&sbi->inode_hash_lock);
    for (i = 0; i < pd->nr_bhs; i++)
    {
        exi->de_blocknr[i] = pd->bhs[i]->b_blocknr;
    }
    exi->de_offset = pd->bh_offset;
    exi->de_size = pd->size;
    hlist_add_head(&exi->i_hash, head);
    //  spin_unlock(&sbi->inode_hash_lock);
}

static void __exfat_detach(struct exfat_inode_info *exi)
{
    exi->de_blocknr[0] = 0;
    exi->de_offset = -1;
    exi->de_size = -1;
}

void exfat_detach(struct inode *inode)
{
    struct exfat_sb_info *sbi = EXFAT_SB(inode->i_sb);
    struct exfat_inode_info *exi = EXFAT_I(inode);

    //  spin_lock(&sbi->inode_hash_lock);
    __exfat_detach(exi);
    hlist_del_init(&exi->i_hash);
    //  spin_unlock(&sbi->inode_hash_lock);
}

struct inode *exfat_ilookup(struct super_block *sb, sector_t blocknr,
                            unsigned long offset)
{
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    u32 hashval = exfat_hash(sb, blocknr, offset);
    struct hlist_head *head = sbi->inode_hashtable + hashval;
    struct hlist_node *_p;
    struct exfat_inode_info *exi;
    struct inode *inode = NULL;

    //  spin_lock(&sbi->inode_hash_lock);
    hlist_for_each_entry(exi, _p, head, i_hash)
    {
        EXFAT_DEBUG_ON(exi->vfs_inode.i_sb != sb,
                       "vfs.sb %p, sb %p\n",
                       exi->vfs_inode.i_sb, sb);
        if (exi->de_blocknr[0] != blocknr)
        {
            continue;
        }
        if (exi->de_offset != offset)
        {
            continue;
        }
        inode = igrab(&exi->vfs_inode);
        if (inode)
        {
            break;
        }
    }
    //  spin_unlock(&sbi->inode_hash_lock);
    return inode;
}

static void exfat_fill_inode(struct inode *inode, unsigned long ino,
                             struct exfat_chunk_dirent *dirent,
                             struct exfat_chunk_data *data)
{
    struct super_block *sb = inode->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct exfat_inode_info *exi = EXFAT_I(inode);

    inode->i_ino = ino;
    //inode->i_nlink = 1;   /*  Windows use 1 even if directory.*/
    inode->i_version++;

    /*  inode->i_generation = get_seconds(); FIXME: don't support nfs? */
    inode->i_generation = 0; //get_seconds();

    /* FIXME: inode->i_size is "signed", have to check overflow? */
    inode->i_size = le64_to_cpu(data->size);

    /* FIXME: sane time conversion is needed */
    exfat_time_exfat2unix(&inode->i_ctime, dirent->crtime, dirent->crdate,
                          dirent->crtime_cs);
    exfat_time_exfat2unix(&inode->i_mtime, dirent->mtime, dirent->mdate,
                          dirent->mtime_cs);
    exfat_time_exfat2unix(&inode->i_atime, dirent->atime, dirent->adate, 0);

    inode->i_blocks = (inode->i_size + (sb->s_blocksize - 1)) >> sb->s_blocksize_bits;

    exi->phys_size = inode->i_size;
    exi->clusnr = le32_to_cpu(data->clusnr);
    exi->attrib = dirent->attrib;
    exi->data_flag = data->flag;
    if (dirent->attrib & cpu_to_le16(EXFAT_ATTR_DIR))
    {
        inode->i_mode = S_IFDIR | sbi->opts.dmode;
        inode->i_op = &exfat_dir_inode_ops;
        inode->i_fop = &exfat_dir_ops;
        inode->i_nlink = exfat_subdirs(inode) + 1;      /* ????? */
    }
    else
    {
        inode->i_mode = S_IFREG | sbi->opts.fmode;
        inode->i_op = &exfat_file_inode_ops;
        inode->i_fop = &exfat_file_ops;
        inode->i_nlink = 1;
    }
#if 0
    /* FIXME: separate S_NOCMTIME to S_NOCTIME and S_NOMTIME,
     * then exFAT/FAT can use S_NOCTIME. */
    inode->i_flags |= S_NOCTIME;

    /* FIXME: how handle these attrib? */
    if (dirent->attrib & cpu_to_le16(EXFAT_ATTR_SYSTEM))
    {
        if (sbi->opts.sys_immutable)
        {
            inode->i_flags |= S_IMMUTABLE;
        }
    }
    if (dirent->attrib & cpu_to_le16(EXFAT_ATTR_RO))
    {
        if (sbi->opts.sys_immutable)
        {
            inode->i_flags |= S_IMMUTABLE;
        }
    }
#endif

    __exfat_detach(exi);
}

struct inode *exfat_build_inode(struct super_block *sb,
                                struct exfat_parse_data *pd,
                                struct exfat_chunk_dirent *dirent,
                                struct exfat_chunk_data *data)
{
    struct inode *inode;
    unsigned long ino;

    /* FIXME: this can use ilookup5 (with inode_lock). Which one is fast? */
    inode = exfat_ilookup(sb, pd->bhs[0]->b_blocknr, pd->bh_offset);
    if (inode)
    {
        return inode;
    }

    inode = new_inode(sb);
    if (!inode)
    {
        return ERR_PTR(-ENOMEM);
    }

    ino = iunique(sb, EXFAT_RESERVED_INO);

    inode->i_version = 1;

    exfat_fill_inode(inode, ino, dirent, data);

    exfat_attach(inode, pd);
    insert_inode_hash(inode);

    return inode;
}
/*
 * allocate exfat internal inode, it just belong to exfat,
 * can't add to VFS managemant list
 */
static struct inode *exfat_alloc_internal_inode(struct super_block *sb)
{
    static struct inode_operations empty_iops = {0};
    static const struct file_operations empty_fops = {0};
    struct exfat_inode_info *exi;
    struct inode *inode;

    /* allocate inode directly, not use slab allocater */
    exi = (struct exfat_inode_info *)calloc(1, sizeof(struct exfat_inode_info));
    if (!exi)
    {
        fs_log_error("no memory\n");
        return NULL;
    }
    /* initi new inode */
    INIT_HLIST_NODE(&exi->i_hash);
    exfat_cache_inode_init_once(exi);
    inode_init_once(&exi->vfs_inode);

    inode = &exi->vfs_inode;
    inode->i_sb = sb;
    inode->i_blkbits = sb->s_blocksize_bits;
    inode->i_flags = 0;
    atomic_set(&inode->i_count, 1);
    inode->i_op = &empty_iops;
    inode->i_fop = &empty_fops;
    inode->i_nlink = 1;
    inode->i_size = 0;
    inode->i_blocks = 0;
    inode->i_bytes = 0;
    inode->i_generation = 0;
    inode->dirtied_when = 0;
    inode->i_private = NULL;
    return inode;
}
/*
 * free a internal inode
 */
void exfat_free_internal_inode(struct inode *inode)
{
    struct exfat_inode_info *exi = EXFAT_I(inode);
    free(exi);
}
struct inode *exfat_new_internal_inode(struct super_block *sb,
                                       unsigned long ino, u16 attrib,
                                       u32 clusnr, u64 size)
{
    struct inode *inode;
    struct exfat_chunk_dirent dirent =
    {
        .attrib = cpu_to_le16(attrib),
    };

    /* all internal inode data flag type is "EXFAT_DATA_NORMAL"
     * must be careful for this
     */
    struct exfat_chunk_data data =
    {
        .flag   = EXFAT_DATA_NORMAL,
        .clusnr = cpu_to_le32(clusnr),
        .size   = cpu_to_le64(size),
    };

    inode = exfat_alloc_internal_inode(sb);
    if (inode)
    {
        exfat_fill_inode(inode, ino, &dirent, &data);
        /* The internal inode doesn't have timestamp */
        inode->i_flags |= S_NOATIME | S_NOCMTIME;
    }
    return inode;
}
/*
 * allocate root directory inode for exfat
 * root directory of exfat just like any normal directory deal
 */
struct inode *exfat_alloc_root_inode(struct super_block *sb,
                                     unsigned long ino, u16 attrib,
                                     u32 clusnr, u64 size)
{
    struct inode *inode;
    struct exfat_chunk_dirent dirent =
    {
        .attrib = cpu_to_le16(attrib),
    };

    /* root directory data flag type is "EXFAT_DATA_NORMAL"
     * be careful for this
     */
    struct exfat_chunk_data data =
    {
        .flag   = EXFAT_DATA_NORMAL,
        .clusnr = cpu_to_le32(clusnr),
        .size   = cpu_to_le64(size),
    };
    /* allocate inode from VFS,
     * root directory inode treat as an normal inode
     * add to normal managemant list
     * all the works been done under "new_inode()" under VFS
     */
    inode = new_inode(sb);
    if (inode)
    {
        exfat_fill_inode(inode, ino, &dirent, &data);

        /* The internal inode doesn't have timestamp */
        inode->i_flags |= S_NOATIME | S_NOCMTIME;
    }
    return inode;
}
struct inode *exfat_rootdir_iget(struct super_block *sb)
{
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct inode *inode;
    struct exfat_clus_map cmap;
    int err;

    err = -ENOMEM;
    inode = exfat_alloc_root_inode(sb, EXFAT_ROOT_INO, EXFAT_ATTR_DIR,
                                   sbi->rootdir_clusnr, 0);
    if (!inode)
    {
        goto error;
    }
    /* Get last iclusnr to set ->i_size */
    err = exfat_get_cluster(inode, EXFAT_ENT_EOF, 0, &cmap);
    if (err)
    {
        goto error_iput;
    }
    if (!exfat_ent_eof(sbi, cmap.clusnr))
    {
        fs_log_error("exFAT: found invalid FAT entry 0x%x"
                     " for root directory\n", cmap.clusnr);
        err = -EIO;
        goto error_iput;
    }
    inode->i_size = cmap.iclusnr << sbi->clus_bits;
    EXFAT_I(inode)->phys_size = inode->i_size;
    inode->i_blocks = (inode->i_size + (sbi->clus_size - 1)) >> 9;
    inode->i_nlink = exfat_subdirs(inode);
    insert_inode_hash(inode);
    return inode;

error_iput:
    iput(inode);
error:
    return ERR_PTR(err);
}