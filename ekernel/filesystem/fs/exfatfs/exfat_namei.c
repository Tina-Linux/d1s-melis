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
#include "exfat_upcase.h"
#include "exfat_cluster.h"
#include "slab.h"
#include "buffer_head.h"
#include "errno.h"
#include "endians.h"
#include "nls.h"
#include "page_pool.h"
#include "fsys_libs.h"
#include "fsys_debug.h"

#if defined CONFIG_FSYS_EXFAT_RW

/* Characters that are undesirable in an exFAT file name */
static   __wchar_t exfat_bad_char(__wchar_t w)
{
    return (w < 0x0020)
           || (w == '*') || (w == '?') || (w == '<') || (w == '>')
           || (w == '|') || (w == '"') || (w == ':') || (w == '/')
           || (w == '\\');
}

static int exfat_is_used_badchars(const __wchar_t *s, int len)
{
    int i;

    for (i = 0; i < len; i++)
        if (exfat_bad_char(s[i]))
        {
            return -EINVAL;
        }
    return 0;
}

static int exfat_valid_name(const unsigned char *name, unsigned int len)
{
    if (name[len - 1] == ' ')  /* ??? */
    {
        return -EINVAL;
    }
    if (len >= EXFAT_MAX_NAMELEN)
    {
        return -ENAMETOOLONG;
    }
    return 0;
}

static int exfat_build_slots(struct inode *dir, const unsigned char *name,
                             int len, struct timespec *ts,
                             struct exfat_dir_slot *slots,
                             struct exfat_slot_info *sinfo)
{
    struct exfat_sb_info *sbi = EXFAT_SB(dir->i_sb);
    //struct exfat_mount_opts *opts = &(sbi->opts);
    struct exfat_chunk_dirent *dirent_chunk;
    struct exfat_chunk_data *data_chunk;
    struct exfat_chunk_name *name_chunk;
    unsigned long page;
    u16 cksum, name_cksum, name_hash;
    __wchar_t *uname;
    __le16 time, date;
    u8  time_cs;
    int err, ulen, i, nr_slots;

    err = exfat_valid_name(name, len);
    if (err)
    {
        return err;
    }

    page = __get_free_page(GFP_KERNEL);
    if (!page)
    {
        return -ENOMEM;
    }

    uname = (__wchar_t *)page;
    ulen = len;
    {
        /* maybe rewrite this part ??? */
        __wchar_t *uname_tmp;
        const unsigned char *name_tmp;
        int name_left, uname_left;
        uname_tmp = uname;
        name_tmp = name;
        name_left = len;
        uname_left = len;
        err = exfat_conv_c2u(nls, &name_tmp, &name_left,
                             &uname_tmp, &uname_left);
        if (err)
        {
            goto out_free;
        }
        ulen = len - uname_left;
    }
    err = exfat_is_used_badchars(uname, ulen);
    if (err)
    {
        goto out_free;
    }

    /* calc nr_slots : dirent slot + data slot + name slots */
    nr_slots = (ulen + EXFAT_CHUNK_NAME_SIZE - 1) / EXFAT_CHUNK_NAME_SIZE + 2;
    sinfo->nr_slots = nr_slots;

    /* calc name hash : checksum of upcase name */
    name_hash = 0;
    for (i = 0; i < ulen; i++)
    {
        __wchar_t wchar;
        __le16 uc;
        wchar = exfat_towupper(sbi, uname[i]);
        uc = cpu_to_le16(wchar);
        name_hash = exfat_checksum16(name_hash, &uc, sizeof(uc));
    }
    /* fill dirent chunk : slots[0] */
    exfat_time_unix2exfat(ts, &time, &date, &time_cs);
    dirent_chunk = (struct exfat_chunk_dirent *)(&slots[0]);
    dirent_chunk->type = EXFAT_TYPE_DIRENT;
    dirent_chunk->sub_chunks = nr_slots - 1;
    dirent_chunk->attrib = sinfo->is_dir ? EXFAT_ATTR_DIR : EXFAT_ATTR_ARCH;
    dirent_chunk->xxxx01 = 0;
    dirent_chunk->crdate = date;
    dirent_chunk->mdate = date;
    dirent_chunk->adate = date;
    dirent_chunk->crtime = time;
    dirent_chunk->mtime = time;
    dirent_chunk->atime = time;
    dirent_chunk->crtime_cs = time_cs;
    dirent_chunk->mtime_cs = time_cs;
    memset(dirent_chunk->xxxx02, 0, sizeof(dirent_chunk->xxxx02));

    /* calc dirent chunk checksum :
     * NOTE : checksum not include dirent->checksum */
    cksum = exfat_checksum16(0, dirent_chunk, 2);
    cksum = exfat_checksum16(cksum, (u8 *)dirent_chunk + 4, EXFAT_CHUNK_SIZE - 4);

    /* fill data chunk : slots[1] */
    data_chunk = (struct exfat_chunk_data *)(&slots[1]);
    data_chunk->type = EXFAT_TYPE_DATA;
    data_chunk->flag = sinfo->is_dir ? EXFAT_DATA_CONTIGUOUS : EXFAT_DATA_NORMAL;
    data_chunk->xxxx01 = 0;
    data_chunk->name_len = ulen;
    data_chunk->hash = name_hash;       /* hash of file name */
    data_chunk->xxxx02 = 0;
    data_chunk->size2 = sinfo->is_dir ? 1 << sbi->clus_bits : 0;
    data_chunk->xxxx03 = 0;
    data_chunk->clusnr = sinfo->clusnr;
    data_chunk->size = sinfo->is_dir ? 1 << sbi->clus_bits : 0;

    /* calc data chunk checksum */
    cksum = exfat_checksum16(cksum, data_chunk, sizeof(*data_chunk));

    /* fill name chunks */
    name_cksum = 0;
    for (i = 2; i < nr_slots; i++)
    {
        int len;
        name_chunk = (struct exfat_chunk_name *)(&slots[i]);
        name_chunk->type = EXFAT_TYPE_NAME;
        name_chunk->xxxx01 = 0;
        len = min(ulen, EXFAT_CHUNK_NAME_SIZE);
        memcpy(name_chunk->name, uname, sizeof(__wchar_t) * len);

        /* end of name part set zero */
        if (len && len < EXFAT_CHUNK_NAME_SIZE)
        {
            memset(name_chunk->name + len, 0,
                   sizeof(__wchar_t) * (EXFAT_CHUNK_NAME_SIZE - len));
        }
        /* calc name chunk checksum */
        cksum = exfat_checksum16(cksum, name_chunk, sizeof(*name_chunk));
        name_cksum = exfat_checksum16(name_cksum, name_chunk, sizeof(*name_chunk));
        uname += len;
        ulen -= len;
    }
    /* set checksum of sub_chunks last */
    dirent_chunk->checksum = cksum;

out_free:
    free_page(page);
    return err;
}

static int exfat__get_entry(struct inode *dir, __s64 *pos,
                            struct buffer_head **bh, struct exfat_dir_slot **de)
{
    struct super_block *sb = dir->i_sb;
    sector_t phys, iblock, last_iblock;
    unsigned long mapped_blocks;
    int err, offset;

    last_iblock = dir->i_size >> dir->i_blkbits;
next:
    if (*bh)
    {
        brelse(*bh);    /* ????, rethink this */
    }

    *bh = NULL;
    iblock = *pos >> sb->s_blocksize_bits;
    mapped_blocks = last_iblock - iblock;       /* readahead all blocks */
    err = exfat_get_block(dir, iblock, &mapped_blocks, &phys, 0);
    if (err || !phys)
    {
        return -1;    /* beyond EOF or error */
    }

    *bh = sb_bread(sb, phys);
    if (*bh == NULL)
    {
        fs_log_error("EXFAT: Directory bread(block %u) failed\n",
                     (unsigned long)phys);

        /* skip this block */
        *pos = (iblock + 1) << sb->s_blocksize_bits;
        goto next;
    }
    offset = *pos & (sb->s_blocksize - 1);
    *pos += sizeof(struct exfat_dir_slot);
    *de = (struct exfat_dir_slot *)((*bh)->b_data + offset);

    return 0;
}

static   int exfat_get_entry(struct inode *dir, __s64 *pos,
                             struct buffer_head **bh,
                             struct exfat_dir_slot **de)
{
    /* Fast stuff first */
    if (*bh && *de &&
        (*de - (struct exfat_dir_slot *)(*bh)->b_data) < EXFAT_SB(dir->i_sb)->chunk_per_block - 1)
    {
        *pos += sizeof(struct exfat_dir_slot);
        (*de)++;
        return 0;
    }

    /* read directly */
    return exfat__get_entry(dir, pos, bh, de);
}

int exfat_zeroed_blocks(struct inode *dir, int blknr, int nr)
{
    struct super_block *sb = dir->i_sb;
    struct buffer_head *bh = NULL;
    int err = 0;

    /* Zeroing blocks on same cluster */
    while (nr)
    {
        bh = sb_getblk(sb, blknr);
        if (!bh)
        {
            err = -ENOMEM;
            break;
        }
        memset(bh->b_data, 0, sb->s_blocksize);
        set_buffer_uptodate(bh);
        mark_buffer_dirty(bh);

        /* Note : now exfat max cluster size is 32M,
         * so max block number on same is : 64K(32M/512)
         * under epos, fs allocate buffer size is 128K
         * (too small for this:128K VS 32M)
         * if the buffer head not write directly,
         * the buffer head will not enough for use.
         * so the zeroed buffer head will be write out directly
         * unsed buffer head will be released directly here
         * 2009.7.6 11.20 By sunny
         */
        err = exfat_sync_bhs(&bh, 1);
        brelse(bh);
        if (err)
        {
            break;
        }
        blknr++;
        nr--;
    }
    return err;
    //  if (IS_DIRSYNC(dir)) {
    //      err = exfat_sync_bhs(bhs, n);
    //      if (err)
    //          goto error;
    //  }
    //  for (i = 0; i < n; i++)
    //      brelse(bhs[i]);
    //
    //
    //  return 0;
}

static int exfat_add_new_entries(struct inode *dir, int *nr_cluster,
                                 struct exfat_slot_info *sinfo,  int nr_slots,
                                 u8 *data_flag)
{
    struct super_block *sb = dir->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct exfat_parse_data *pd = &(sinfo->pd);
    int blknr, start_blknr, last_blknr;
    unsigned long size, left;
    int err, i, cluster[2];

    /* The minimum cluster size is 512bytes, and maximum entry
     * size is 32 * (2 + 17)(608bytes) :
     * Max dentry chunks : 1 data_chunk + 1 dirent_chunk + 17 name_chunk (255/15).
     * So, iff the cluster size is 512bytes,
     * we may need two clusters.
     */
    left = nr_slots * sizeof(struct exfat_dir_slot);
    *nr_cluster = (left + (sbi->clus_size - 1)) >> sbi->clus_bits;
    BUG_ON(*nr_cluster > 2);

    err = exfat_alloc_clusters(dir, cluster, *nr_cluster, data_flag);
    if (err)
    {
        fs_log_error("exFAT allocate clusters failed\n");
        return err;
    }
    /* get allocate chunks from new cluster */
    i = size = 0;
    while (i < *nr_cluster)
    {
        start_blknr = blknr = exfat_clus_to_blknr(sbi, cluster[i++]);
        last_blknr = start_blknr + sbi->bpc;

        /* clear new blocks firstly */
        err = exfat_zeroed_blocks(dir, start_blknr, last_blknr - start_blknr);
        if (err)
        {
            goto error;
        }
        while (left && (blknr < last_blknr))
        {
            pd->bhs[pd->nr_bhs] = sb_getblk(sb, blknr);
            if (!pd->bhs[pd->nr_bhs])
            {
                err = -EIO;
                goto error;
            }
            /* all chunks allocated here,
             * set bh offset to bh start */
            if (!pd->nr_bhs)
            {
                pd->bh_offset = 0;
            }
            pd->nr_bhs++;
            size = min(left, sb->s_blocksize);
            left -= size;
            pd->size += size;
            blknr++;
        }
    }
    return cluster[0];

error:
    exfat_free_clusters(dir, cluster[0], *nr_cluster, *data_flag);
    return err;
}

/*
 * get allocate chunks position of one inode
 * Microsoft method : firstly set searh postition to EOD of dir,
 * when seach failed, rewind to direntry begin again,
 * avoid generate fragment quickly
 */
static int exfat_get_alloc_chunk_pos(struct inode *dir, __s64 *pos)
{
    int err = 0;
    *pos = 0;
    return err;
}

int exfat_alloc_chunks(struct inode *dir, struct exfat_slot_info *sinfo)
{
    struct super_block *sb = dir->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct buffer_head *bh, *prev;
    struct exfat_dir_slot *de;
    struct exfat_parse_data *pd = &(sinfo->pd);
    int err, nr_slots, free_slots, i;
    __s64 pos;

    nr_slots = sinfo->nr_slots;
    free_slots = 0;
    bh = prev = NULL;

    /* sanity check */
    if (nr_slots < EXFAT_MIN_CHUNK_NUMBER)
    {
        fs_log_error("exFAT too few number of chunks for allocate\n");
        return -EPERM;
    }
    /* Microsoft method : firstly set searh postition to EOD of dir,
     * when seach failed, rewind to direntry begin again
     * FIXME : seach from direntry begin can generate fragment quickly
     */
    err = exfat_get_alloc_chunk_pos(dir, &pos);

    /* First stage: search free direcotry entries */
    while (exfat_get_entry(dir, &pos, &bh, &de) > -1)
    {

        if (EXFAT_FREE_CHUNK(de->type))
        {
            if (prev != bh)
            {
                /* get first free chunk position offset */
                if (!prev)
                {
                    pd->bh_offset = (pos - EXFAT_CHUNK_SIZE) & (sb->s_blocksize - 1);
                }
                get_bh(bh);
                pd->bhs[pd->nr_bhs] = prev = bh;
                pd->nr_bhs++;

            }
            free_slots++;
            if (free_slots == nr_slots)
            {
                goto found;
            }
        }
        else
        {
            pd_brelse(pd);
            prev = NULL;
            free_slots = 0;
        }
    }
found:
    err = 0;
    nr_slots -= free_slots;
    pd->size = free_slots * sizeof(struct exfat_dir_slot);
    if (nr_slots)
    {
        int cluster, nr_cluster;
        u8  data_flag;

        /* Second stage: allocate the cluster for new entries.
         * And add the cluster to dir.
         */
        cluster = exfat_add_new_entries(dir, &nr_cluster, sinfo,
                                        nr_slots, &data_flag);
        if (cluster < 0)
        {
            err = cluster;
            goto error;
        }
        err = exfat_chain_add(dir, cluster, nr_cluster, data_flag);
        if (err)
        {
            exfat_free_clusters(dir, cluster, nr_cluster, data_flag);
            goto error;
        }
        if (dir->i_size & (sbi->clus_size - 1))
        {
            fs_log_error("exFAT Odd directory size");
            dir->i_size = (dir->i_size + sbi->clus_size - 1)
                          & ~((__s64)sbi->clus_size - 1);
        }
        dir->i_size += nr_cluster << sbi->clus_bits;
        EXFAT_I(dir)->phys_size += nr_cluster << sbi->clus_bits;
    }
    /* NOTE: avoid internal memory leak... */
    if (bh)
    {
        brelse(bh);
    }

    return 0;

error:
    brelse(bh);
    pd_brelse(pd);
    return err;
}

int exfat_sync_bhs(struct buffer_head **bhs, int nr_bhs)
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

int exfat_fill_chunks(struct inode *dir, void *chunks,
                      struct exfat_slot_info *sinfo)
{
    int  i, left, copy, err;
    struct super_block *sb = dir->i_sb;
    struct exfat_parse_iter_data pd_iter;
    struct exfat_parse_data  *pd;
    struct exfat_chunk_dirent *dirent;
    struct exfat_chunk_data *data;
    struct exfat_chunk_name *name;

    pd = &(sinfo->pd);
    left = sinfo->nr_slots;
    copy = EXFAT_CHUNK_SIZE;
    err = 0;

    /* fill dirent chunk */
    dirent = pd_iter_first_de(pd, &pd_iter);
    memcpy(dirent, chunks, copy);
    sinfo->dirent = dirent;
    chunks = (void *)((char *)chunks + copy);
    left--;

    /* fill data chunk */
    data = pd_iter_next_de(pd, &pd_iter);
    memcpy(data, chunks, copy);
    sinfo->data = data;
    chunks = (void *)((char *)chunks + copy);
    left--;

    /* fill name chunks */
    for (i = 0; i < left; i++)
    {
        name = pd_iter_next_de(pd, &pd_iter);
        memcpy(name, chunks, copy);
        chunks = (void *)((char *)chunks + copy);
    }
    /* mark buffer dirty */
    pd_dirty(pd);
    if (IS_DIRSYNC(dir))
    {
        err = exfat_sync_bhs(pd->bhs, pd->nr_bhs);
    }
    return err;
}

int exfat_remove_chunks(struct inode *dir, struct exfat_slot_info *sinfo)
{
    int  i, chunks_nr, err;
    struct exfat_parse_iter_data pd_iter;
    struct exfat_parse_data  *pd;
    struct exfat_chunk_dirent *dirent;
    struct exfat_chunk_data *data;
    struct exfat_chunk_name *name;

    chunks_nr = sinfo->nr_slots;
    pd = &(sinfo->pd);
    err = 0;

    /* sanity check */
    if (chunks_nr < EXFAT_MIN_CHUNK_NUMBER)
    {
        fs_log_error("exFAT too few number of chunks for remove\n");
        return -EPERM;
    }
    /* delete dirent chunk */
    dirent = pd_iter_first_de(pd, &pd_iter);
    EXFAT_DELETE_CHUNK(dirent->type);
    chunks_nr--;

    /* delete data chunk */
    data = pd_iter_next_de(pd, &pd_iter);
    EXFAT_DELETE_CHUNK(data->type);
    chunks_nr--;

    /* delete name chunks */
    for (i = 0; i < chunks_nr; i++)
    {
        name = pd_iter_next_de(pd, &pd_iter);
        EXFAT_DELETE_CHUNK(name->type);
    }
    /* mark buffer dirty */
    pd_dirty(pd);
    if (IS_DIRSYNC(dir))
    {
        err = exfat_sync_bhs(pd->bhs, pd->nr_bhs);
    }
    pd_brelse(pd);
    return err;
}

int exfat_add_chunks(struct inode *dir, void *slots,
                     struct exfat_slot_info *sinfo)
{
    int err = 0;

    /* First stage: allocate free direcotry chunks */
    err = exfat_alloc_chunks(dir, sinfo);
    if (err)
    {
        fs_log_error("allocate chunks failed\n");
        return err;
    }
    /* Second stage: filling the free chunks with new chunks.*/
    err = exfat_fill_chunks(dir, slots, sinfo);
    if (err)
    {
        fs_log_error("fill chunks failed\n");
        goto error;
    }
    return 0;

error:
    exfat_remove_chunks(dir, sinfo);
    return err;
}

/*
 * returns the length of a struct qstr,
 * ignoring trailing dots and space,
 * Note : '\t' is unlawful
 * retalk with peter this ???
 */
static unsigned int exfat_striptail_len(struct qstr *qstr)
{
    unsigned int len = qstr->len;

    while (len && ((qstr->name[len - 1] == '.') ||
                   (qstr->name[len - 1] == ' '))
          )
    {
        len--;
    }
    return len;
}

static int exfat_add_entry(struct inode *dir,
                           struct qstr *qname,
                           struct timespec *ts,
                           struct exfat_slot_info *sinfo)
{
    struct exfat_dir_slot *slots;
    unsigned int len;
    int err, nr_slots;

    len = exfat_striptail_len(qname);
    if (len == 0)
    {
        return -ENOENT;
    }
    /* FIXME : maybe use EXFAT_DE_MAX_CHUNKS, but it too big currently */
    slots = kmalloc(sizeof(*slots) * EXFAT_SLOTS, GFP_KERNEL);
    if (slots == NULL)
    {
        return -ENOMEM;
    }
    err = exfat_build_slots(dir, qname->name, len,
                            ts, slots, sinfo);
    if (err)
    {
        goto out;
    }
    err = exfat_add_chunks(dir, slots, sinfo);
    if (err)
    {
        goto out;
    }
    /* update timestamp */
    dir->i_ctime = dir->i_mtime = dir->i_atime = *ts;  /* update ctime ??? */
    if (IS_DIRSYNC(dir))
    {
        (void)exfat_sync_inode(dir);
    }
    else
    {
        mark_inode_dirty(dir);
    }

out:
    kfree(slots);
    return err;
}

static void exfat_slot_info_init(struct exfat_slot_info *sinfo)
{
    sinfo->nr_slots = 0;
    sinfo->clusnr = 0;
    sinfo->data = NULL;
    sinfo->dirent = NULL;
    pd_init(&(sinfo->pd));
}

static void exfat_slot_info_brelse(struct exfat_slot_info *sinfo)
{
    pd_brelse(&(sinfo->pd));
    exfat_slot_info_init(sinfo);
}

static __s32 exfat_create(struct inode *dir, struct dentry *dentry,
                          __s32 mode, struct nameidata *nd)
{
    struct super_block *sb = dir->i_sb;
    struct inode *inode;
    struct exfat_slot_info sinfo;
    struct timespec ts;
    int err;

    ts = CURRENT_TIME_SEC;
    exfat_slot_info_init(&sinfo);
    sinfo.is_dir = 0;
    sinfo.clusnr = 0;
    err = exfat_add_entry(dir, &dentry->d_name, &ts, &sinfo);
    if (err)
    {
        goto out;
    }
    dir->i_version++;

    inode = exfat_build_inode(sb, &(sinfo.pd),
                              sinfo.dirent, sinfo.data);
    exfat_slot_info_brelse(&sinfo);
    if (IS_ERR(PTR_ERR(inode)))
    {
        err = PTR_ERR(inode);
        goto out;
    }
    inode->i_version++;
    inode->i_mtime = inode->i_atime = inode->i_ctime = ts;
    /* timestamp is already written, so mark_inode_dirty() is unneeded. */

    d_instantiate(dentry, inode);
out:
    return err;
}

/*
 * check directory is empty or not
 */
static int exfat_dir_empty(struct inode *dir)
{
    struct buffer_head *bh = NULL;
    struct exfat_dir_slot *de = NULL;
    __s64 cpos = 0;
    int result = 0;
    while (exfat_get_entry(dir, &cpos, &bh, &de) >= 0)
    {
        if (EXFAT_VALID_CHUNK(de->type))
        {
            result = -ENOTEMPTY;
            break;
        }
    }
    brelse(bh);
    return result;
}

static __s32 exfat_rmdir(struct inode *dir, struct dentry *dentry)
{
    struct inode *inode = dentry->d_inode;
    struct exfat_slot_info sinfo;
    int err;

    err = exfat_dir_empty(inode);
    if (err)
    {
        return err;
    }
    exfat_slot_info_init(&sinfo);
    sinfo.is_dir = 1;
    err = exfat_find(dir, &dentry->d_name, &sinfo);
    if (err)
    {
        return err;
    }
    err = exfat_remove_chunks(dir, &sinfo);
    exfat_slot_info_brelse(&sinfo);
    if (err)
    {
        return err;
    }
    drop_nlink(dir);
    clear_nlink(inode);
    inode->i_mtime = inode->i_atime = CURRENT_TIME_SEC;
    exfat_detach(inode);
    return 0;
}

static __s32 exfat_unlink(struct inode *dir, struct dentry *dentry)
{
    struct inode *inode = dentry->d_inode;
    struct exfat_slot_info sinfo;
    int err;

    exfat_slot_info_init(&sinfo);
    sinfo.is_dir = 0;
    err = exfat_find(dir, &dentry->d_name, &sinfo);
    if (err)
    {
        return err;
    }
    err = exfat_remove_chunks(dir, &sinfo);
    exfat_slot_info_brelse(&sinfo);
    if (err)
    {
        return err;
    }
    clear_nlink(inode);
    inode->i_mtime = inode->i_atime = CURRENT_TIME_SEC;
    exfat_detach(inode);
    return 0;
}

int exfat_alloc_new_dir(struct inode *dir, struct timespec *ts)
{
    struct exfat_sb_info *sbi = EXFAT_SB(dir->i_sb);
    int blknr, cluster, err;
    u8 data_flag;

    err = exfat_alloc_clusters(dir, &cluster, 1, &data_flag);
    if (err)
    {
        return err;
    }
    blknr = exfat_clus_to_blknr(sbi, cluster);
    err = exfat_zeroed_blocks(dir, blknr, sbi->bpc);
    if (err)
    {
        exfat_free_clusters(dir, cluster, 1, data_flag);
        return err;
    }
    return cluster;
}

static __s32 exfat_mkdir(struct inode *dir, struct dentry *dentry, __s32 mode)
{
    struct super_block *sb = dir->i_sb;
    struct inode *inode;
    struct exfat_slot_info sinfo;
    struct timespec ts;
    int err, cluster;

    ts = CURRENT_TIME_SEC;
    cluster = exfat_alloc_new_dir(dir, &ts);
    if (cluster < 0)
    {
        err = cluster;
        goto out;
    }
    exfat_slot_info_init(&sinfo);
    sinfo.clusnr = cluster;
    sinfo.is_dir = 1;
    err = exfat_add_entry(dir, &dentry->d_name, &ts, &sinfo);
    if (err)
    {
        goto out_free;
    }
    dir->i_version++;
    inc_nlink(dir);
    inode = exfat_build_inode(sb, &(sinfo.pd),
                              sinfo.dirent, sinfo.data);
    exfat_slot_info_brelse(&sinfo);
    if (IS_ERR(PTR_ERR(inode)))
    {
        err = PTR_ERR(inode);
        /* the directory was completed, just return a error */
        goto out;
    }
    inode->i_version++;
    inode->i_nlink = 2;
    inode->i_mtime = inode->i_atime = inode->i_ctime = ts;
    /* timestamp is already written, so mark_inode_dirty() is unneeded. */

    //dentry->d_time = dentry->d_parent->d_inode->i_version;
    d_instantiate(dentry, inode);

    return 0;

out_free:
    exfat_free_clusters(dir, cluster, 1, EXFAT_DATA_CONTIGUOUS);
out:
    return err;
}

static __s32 exfat_rename(struct inode *old_dir, struct dentry *old_dentry,
                          struct inode *new_dir, struct dentry *new_dentry)
{
    struct inode *old_inode, *new_inode;
    struct exfat_slot_info old_sinfo, sinfo;
    struct timespec ts;
    int err, is_dir, update_dotdot, corrupt = 0;

    memset(&sinfo, 0, sizeof(struct exfat_slot_info));
    memset(&old_sinfo, 0, sizeof(struct exfat_slot_info));

    old_inode = old_dentry->d_inode;
    new_inode = new_dentry->d_inode;
    err = exfat_find(old_dir, &old_dentry->d_name, &old_sinfo);
    if (err)
    {
        goto out;
    }

    is_dir = S_ISDIR(old_inode->i_mode);

    ts = CURRENT_TIME_SEC;
    if (new_inode)
    {
        if (is_dir)
        {
            err = exfat_dir_empty(new_inode);
            if (err)
            {
                goto out;
            }
        }

        err = exfat_find(new_dir, &new_dentry->d_name, &sinfo);
        if (err)
        {
            goto out;
        }
        exfat_detach(new_inode);
    }
    else
    {
        err = exfat_add_entry(new_dir, &new_dentry->d_name,
                              &ts, &sinfo);
        if (err)
        {
            goto out;
        }
    }
    new_dir->i_version++;

    exfat_detach(old_inode);
    exfat_attach(old_inode, &sinfo.pd);
    if (IS_DIRSYNC(new_dir))
    {
        err = exfat_sync_inode(old_inode);
        if (err)
        {
            goto error_inode;
        }
    }
    else
    {
        mark_inode_dirty(old_inode);
    }

    err = exfat_remove_chunks(old_dir, &old_sinfo);
    if (err)
    {
        goto error_dotdot;
    }
    old_dir->i_version++;
    old_dir->i_ctime = old_dir->i_mtime = ts;
    if (IS_DIRSYNC(old_dir))
    {
        (void)exfat_sync_inode(old_dir);
    }
    else
    {
        mark_inode_dirty(old_dir);
    }

    if (new_inode)
    {
        drop_nlink(new_inode);
        if (is_dir)
        {
            drop_nlink(new_inode);
        }
        new_inode->i_ctime = ts;
    }
out:
    exfat_slot_info_brelse(&sinfo);
    exfat_slot_info_brelse(&old_sinfo);

    return err;

error_dotdot:
    corrupt = 1;
error_inode:
    exfat_detach(old_inode);
    exfat_attach(old_inode, &old_sinfo.pd);
    if (new_inode)
    {
        exfat_attach(new_inode, &sinfo.pd);
        if (corrupt)
        {
            corrupt |= exfat_sync_inode(new_inode);
        }
    }
    else
    {
        int err2 = exfat_remove_chunks(new_dir, &sinfo);
        if (corrupt)
        {
            corrupt |= err2;
        }
    }
    if (corrupt < 0)
    {
        fs_log_error("exFAT rename corrupted!\n");
    }
    goto out;
}
#endif  /* FSYS_EXFAT_RW */

const struct inode_operations exfat_dir_inode_ops =
{
    .lookup     = exfat_lookup,
#if defined CONFIG_FSYS_EXFAT_RW
    .create     = exfat_create,
    .unlink     = exfat_unlink,
    .mkdir      = exfat_mkdir,
    .rmdir      = exfat_rmdir,
    .rename     = exfat_rename,
#endif
};