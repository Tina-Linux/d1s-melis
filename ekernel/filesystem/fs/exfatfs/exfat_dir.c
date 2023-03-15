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
#include "slab.h"
#include "buffer_head.h"
#include "errno.h"
#include "endians.h"
#include "nls.h"
#include "page_pool.h"
#include "fsys_libs.h"
#include "fsys_debug.h"

void pd_init(struct exfat_parse_data *pd)
{
    pd->nr_bhs = 0;
}

void pd_brelse(struct exfat_parse_data *pd)
{
    struct buffer_head *bh;
    while (pd->nr_bhs)
    {
        pd->nr_bhs--;
        bh = pd->bhs[pd->nr_bhs];
        brelse(bh);
    }
    pd_init(pd);
}

void pd_dirty(struct exfat_parse_data *pd)
{
    int i;
    for (i = 0; i < pd->nr_bhs; i++)
    {
        mark_buffer_dirty(pd->bhs[i]);
    }
}

static void *pd_get_chunk(struct exfat_parse_data *pd)
{
    return pd->bhs[0]->b_data + pd->bh_offset;
}

void *pd_iter_first_de(struct exfat_parse_data *pd,
                       struct exfat_parse_iter_data *pd_iter)
{
    EXFAT_DEBUG_ON(pd->size < EXFAT_CHUNK_SIZE, "pd->size %lu\n", pd->size);
    pd_iter->left = pd->size - EXFAT_CHUNK_SIZE;
    pd_iter->bh_offset = pd->bh_offset;
    pd_iter->bh_index = 0;
    return pd_get_chunk(pd);
}

void *pd_iter_next_de(struct exfat_parse_data *pd,
                      struct exfat_parse_iter_data *pd_iter)
{
    if (pd_iter->left < EXFAT_CHUNK_SIZE)
    {
        return NULL;
    }
    pd_iter->left -= EXFAT_CHUNK_SIZE;
    pd_iter->bh_offset += EXFAT_CHUNK_SIZE;
    if (pd_iter->bh_offset >= pd->bhs[pd_iter->bh_index]->b_size)
    {
        pd_iter->bh_index++;
        pd_iter->bh_offset = 0;
    }
    return pd->bhs[pd_iter->bh_index]->b_data + pd_iter->bh_offset;
}

enum { PARSE_NEXT, PARSE_STOP, };

struct exfat_parse_callback
{
    int (*parse)(struct inode *, loff_t,
                 struct exfat_parse_data *,
                 struct exfat_parse_callback *);
    void         *data;
};

static int exfat_parse_dir(struct file *filp,
                           struct inode *dir, loff_t *ppos,
                           struct exfat_parse_callback *pcb)
{
    struct super_block *sb = dir->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct exfat_parse_data pd;
    struct buffer_head *bh;
    sector_t blocknr, last_iblock;
    unsigned long blocksize, blocks, offset, de_left;
    loff_t pos;
    int ret, err;

    blocksize = 1 << dir->i_blkbits;
    last_iblock = dir->i_size >> dir->i_blkbits;

    /* limitation : directory size must align by cluster size */
    if (dir->i_size & (sbi->clus_size - 1))
    {
        fs_log_error("invalid directory size (size %lld)", dir->i_size);
        return -EIO;
    }
    pos = *ppos;
    /* FIXME: can cache the free space for next create()/mkdir()/etc? */
    pd_init(&pd);
    //err = -ENOENT;
    err = ret = 0;      /* ????? */
    blocks = de_left = 0;
    while (!ret && pos < dir->i_size)
    {
        if (!blocks)
        {
            /* FIXME: readahead, probably last_iblock is too big */
            sector_t iblock;
            iblock = pos >> dir->i_blkbits;
            blocks = (last_iblock - iblock);
            ret = exfat_get_block(dir, iblock, &blocks, &blocknr, 0);
            if (ret)
            {
                fs_log_error("%s : get block number have error\n", __func__);
                err = ret;
                break;
            }
            if (!blocknr)
            {
                fs_log_error("%s : get invalid block number\n", __func__);
                err = -EINVAL;
                break;
            }
        }
        offset = pos & (blocksize - 1);
        bh = sb_bread(sb, blocknr);
        if (!bh)
        {
            err = -EIO;
            break;
        }
        if (de_left)
        {
            unsigned long len;
            get_bh(bh);
            pd.bhs[pd.nr_bhs] = bh;
            pd.nr_bhs++;
            len = min(de_left, blocksize);
            de_left -= len;
            offset += len;
            pos += len;
            if (!de_left)
            {
                goto dirent_complete;
            }
            goto next_block;
        }
        while (offset < bh->b_size)
        {
            struct exfat_chunk_dirent *dirent;
            unsigned long len, chunks;
            dirent = (void *)(bh->b_data + offset);
            if (dirent->type == EXFAT_TYPE_EOD)
            {
                /* encount end entry, end parse */
                pos = dir->i_size;
                break;
            }
            else if (!(dirent->type & EXFAT_TYPE_VALID))
            {
                /* encount invalid entry,
                 * maybe delete or freed chunk, just skip it */
                offset += EXFAT_CHUNK_SIZE;
                pos += EXFAT_CHUNK_SIZE;
                continue;
            }
            if (dirent->type == EXFAT_TYPE_DIRENT)
            {
                /* normal directory chunk */
                chunks = 1 + dirent->sub_chunks;
            }
            else
            {
                /* special chunk : volume name, upcase, bitmap... */
                chunks = 1;
            }
            de_left = chunks << EXFAT_CHUNK_BITS;
            pd.size = de_left;
            get_bh(bh);             /* sb_bread() have inc bh counter once */
            pd.bhs[0] = bh;
            pd.nr_bhs = 1;
            pd.bh_offset = offset;
            len = min(de_left, blocksize - offset);
            de_left -= len;
            offset += len;
            pos += len;
            if (!de_left)
            {
dirent_complete:
                ret = pcb->parse(dir, pos - pd.size, &pd, pcb);
                pd_brelse(&pd);
                if (ret != PARSE_NEXT)
                {
                    if (ret < 0)
                    {
                        err = ret;
                    }
                    else
                    {
                        err = 0;
                    }
                    pos -= pd.size;
                    brelse(bh);
                    goto parse_end;
                }
            }
        }
next_block:
        brelse(bh);
        blocknr++;
        blocks--;
    }
parse_end:
    *ppos = pos;
    pd_brelse(&pd);
    return err;
}

/* FIXME: unicode string should be cached? */
static int exfat_d_revalidate(struct dentry *dentry, struct nameidata *nd)
{
    return 1;
}

static int exfat_d_hash(struct dentry *dentry, struct qstr *name)
{
    /* FIXME: can't cache converted name/hash? */
    return 0;
}

static int exfat_d_compare(struct dentry *dentry, struct qstr *a,
                           struct qstr *b)
{
    /* FIXME: can't use converted name/hash? */
    if (a->len != b->len)
    {
        return 1;
    }
    return memcmp(a->name, b->name, a->len);
}

static int exfat_d_delete(struct dentry *dentry)
{
    return 0;
}

static void exfat_d_release(struct dentry *dentry)
{
}

static void exfat_d_iput(struct dentry *dentry, struct inode *inode)
{
    iput(inode);
}

struct dentry_operations exfat_dentry_ops =
{
    .d_revalidate   = exfat_d_revalidate,
    .d_hash         = exfat_d_hash,
    .d_compare      = exfat_d_compare,
    .d_delete       = exfat_d_delete,
    .d_release      = exfat_d_release,
    .d_iput         = exfat_d_iput,
};

static int exfat_match(struct exfat_sb_info *sbi, __wchar_t *a, __le16 *b, u8 len)
{
    u8 i;
    for (i = 0; i < len; i++)
    {
        /* FIXME: assume a[i] is upper-case */
        if (a[i] != exfat_towupper(sbi, le16_to_cpu(b[i])))
        {
            return 0;
        }
    }
    return -1;
}

struct lookup_parse_data
{
    struct inode *inode;
    __wchar_t *wstr;
    u8 wlen;
    u16 hash;
};

static int lookup_parse(struct inode *dir, loff_t pos,
                        struct exfat_parse_data *pd,
                        struct exfat_parse_callback *pcb)
{
    struct super_block *sb = dir->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct lookup_parse_data *pcb_data = pcb->data;
    struct exfat_parse_iter_data pd_iter;
    struct exfat_chunk_dirent *dirent;
    struct exfat_chunk_data *data;
    struct exfat_chunk_name *name;
    __wchar_t *wstr;
    u16 csum;
    u8 len, de_namelen;

    dirent = pd_iter_first_de(pd, &pd_iter);
    if (dirent->type != EXFAT_TYPE_DIRENT)
    {
        return PARSE_NEXT;
    }
    /* ->sub_chunk was already checked by caller. */
    EXFAT_DEBUG_ON((1 + dirent->sub_chunks) << EXFAT_CHUNK_BITS != pd->size,
                   "sub_chunks %u, pd->size %lu\n", dirent->sub_chunks, pd->size);
    fs_log_info("exFAT: 0x%02x found\n", dirent->type);
    /* Skip dirent->checksum field */
    csum = exfat_checksum16(0, dirent, 2);
    csum = exfat_checksum16(csum, (u8 *)dirent + 4, EXFAT_CHUNK_SIZE - 4);
    data = pd_iter_next_de(pd, &pd_iter);
    if (!data)
    {
        return PARSE_NEXT;
    }
    if (data->type != EXFAT_TYPE_DATA)
    {
        return PARSE_NEXT;
    }
    fs_log_info("%s: wlen %u, name_len %u\n", __func__,
                pcb_data->wlen, data->name_len);
    if (pcb_data->wlen != data->name_len)
    {
        return PARSE_NEXT;
    }
    fs_log_info("%s: hash 0x%04x, checksum 0x%04x\n", __func__,
                pcb_data->hash, le16_to_cpu(data->hash));
    if (pcb_data->hash != le16_to_cpu(data->hash))
    {
        return PARSE_NEXT;
    }
    de_namelen = data->name_len;
    csum = exfat_checksum16(csum, data, EXFAT_CHUNK_SIZE);
    wstr = pcb_data->wstr;
    while (de_namelen)
    {
        name = pd_iter_next_de(pd, &pd_iter);
        if (!name)
        {
            return PARSE_NEXT;
        }
        if (name->type != EXFAT_TYPE_NAME)
        {
            return PARSE_NEXT;
        }
        len = min_t(u8, de_namelen, EXFAT_CHUNK_NAME_SIZE);
        if (!exfat_match(sbi, wstr, name->name, len))
        {
            return PARSE_NEXT;
        }
        wstr += len;
        de_namelen -= len;
        csum = exfat_checksum16(csum, name, EXFAT_CHUNK_SIZE);
    }
    /* Checksum of the remaining entries (may not be exfat_chunk_name) */
    while ((name = pd_iter_next_de(pd, &pd_iter)) != NULL)
    {
        if (!(name->type & EXFAT_TYPE_SUBCHUNK))
        {
            return PARSE_NEXT;
        }
        csum = exfat_checksum16(csum, name, EXFAT_CHUNK_SIZE);
    }
    if (le16_to_cpu(dirent->checksum) != csum)
    {
        fs_log_info("checksum failed for directory entry"
                    "in lookup (0x%04x != 0x%04x)",
                    le16_to_cpu(dirent->checksum), csum);
        return -EIO;
    }
    fs_log_info("%s: found\n", __func__);
    pcb_data->inode = exfat_build_inode(sb, pd, dirent, data);
    if (IS_ERR(PTR_ERR(pcb_data->inode)))
    {
        return PTR_ERR(pcb_data->inode);
    }

    /* find special directory entry,
     * finish lookup work */
    return PARSE_STOP;
}

struct dentry *exfat_lookup(struct inode *dir, struct dentry *dentry,
                            struct nameidata *nd)
{
    struct exfat_sb_info *sbi = EXFAT_SB(dir->i_sb);
    //struct nls_table *nls = sbi->opts.nls;
    struct exfat_parse_callback pcb;
    struct lookup_parse_data pcb_data;
    const unsigned char *name;
    __wchar_t *wstr;
    unsigned int i, name_left, wlen, wleft;
    loff_t pos;
    int err;

    /* FIXME: can't use converted name/hash? */
    wlen = wleft = min(dentry->d_name.len, EXFAT_MAX_NAMELEN);
    pcb_data.wstr = wstr = kmalloc(wlen * sizeof(*wstr), GFP_KERNEL);
    if (!wstr)
    {
        return ERR_PTR(-ENOMEM);
    }
    name = dentry->d_name.name;
    name_left = dentry->d_name.len;
    /* FIXME: maybe toupper and hash should be computed at same time */
    err = exfat_conv_c2u(nls, &name, &name_left, &wstr, &wleft);
    /* FIXME: shouldn't return -ENAMETOOLONG? */
    if (err)
    {
        kfree(pcb_data.wstr);
        return ERR_PTR(err);
    }
    pcb_data.wlen = wlen - wleft;
    pcb_data.hash = 0;
    for (i = 0; i < pcb_data.wlen; i++)
    {
        __le16 uc;
        pcb_data.wstr[i] = exfat_towupper(sbi, pcb_data.wstr[i]);
        uc = cpu_to_le16(pcb_data.wstr[i]);
        pcb_data.hash = exfat_checksum16(pcb_data.hash, &uc, sizeof(uc));
    }
    pcb_data.inode = NULL;
    pcb.parse = lookup_parse;
    pcb.data = &pcb_data;

    /* FIXME: pos can be previous lookuped position.  ???, must rethink this */
    pos = 0;
    err = exfat_parse_dir(NULL, dir, &pos, &pcb);
    kfree(pcb_data.wstr);

    /* FIXME: can cache the position for next lookup()?
     * Assume sequential locality (stat() for all files after readdir())
     */
    dentry->d_op = &exfat_dentry_ops;
    dentry = d_splice_alias(pcb_data.inode, dentry);
    if (dentry)
    {
        dentry->d_op = &exfat_dentry_ops;
    }
    /* set dentry to NULL
     * dentry been allocated by extern,
     * so no need return new dentry out */
    dentry = NULL;
    return dentry;
}

struct subdirs_parse_data
{
    unsigned int  subdirs_number;
};

static int subdirs_parse(struct inode *dir, loff_t pos,
                         struct exfat_parse_data *pd,
                         struct exfat_parse_callback *pcb)
{
    struct subdirs_parse_data *pcb_data = pcb->data;
    struct exfat_parse_iter_data pd_iter;
    struct exfat_chunk_dirent *dirent;

    dirent = pd_iter_first_de(pd, &pd_iter);
    if (dirent->type == EXFAT_TYPE_EOD)
    {
        return PARSE_STOP;
    }
    if (dirent->type == EXFAT_TYPE_DIRENT)
    {
        pcb_data->subdirs_number++;
    }
    return PARSE_NEXT;
}

unsigned int exfat_subdirs(struct inode *dir)
{
    struct exfat_parse_callback pcb;
    struct subdirs_parse_data   pcb_data;
    loff_t pos = 0;

    pcb_data.subdirs_number = 0;
    pcb.parse = subdirs_parse;
    pcb.data = &pcb_data;

    exfat_parse_dir(NULL, dir, &pos, &pcb);

    return pcb_data.subdirs_number;
}

struct find_parse_data
{
    struct exfat_slot_info *sinfo;
    __wchar_t              *wstr;
    u8                      wlen;
    u16                     hash;
};

static int find_parse(struct inode *dir, loff_t pos,
                      struct exfat_parse_data *pd,
                      struct exfat_parse_callback *pcb)
{
    struct super_block *sb = dir->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct find_parse_data *pcb_data = pcb->data;
    struct exfat_parse_iter_data pd_iter;
    struct exfat_chunk_dirent *dirent;
    struct exfat_chunk_data *data;
    struct exfat_chunk_name *name;
    __wchar_t *wstr;
    u16 csum;
    u8 len, de_namelen;
    int i;

    dirent = pd_iter_first_de(pd, &pd_iter);
    if (dirent->type != EXFAT_TYPE_DIRENT)
    {
        return PARSE_NEXT;
    }

    /* ->sub_chunk was already checked by caller. */
    EXFAT_DEBUG_ON((1 + dirent->sub_chunks) << EXFAT_CHUNK_BITS != pd->size,
                   "sub_chunks %u, pd->size %lu\n", dirent->sub_chunks, pd->size);
    fs_log_info("exFAT: 0x%02x found\n", dirent->type);

    /* Skip dirent->checksum field */
    csum = exfat_checksum16(0, dirent, 2);
    csum = exfat_checksum16(csum, (u8 *)dirent + 4, EXFAT_CHUNK_SIZE - 4);
    data = pd_iter_next_de(pd, &pd_iter);
    if (!data)
    {
        return PARSE_NEXT;
    }
    if (data->type != EXFAT_TYPE_DATA)
    {
        return PARSE_NEXT;
    }
    fs_log_info("%s: wlen %u, name_len %u\n", __func__,
                pcb_data->wlen, data->name_len);
    if (pcb_data->wlen != data->name_len)
    {
        return PARSE_NEXT;
    }
    fs_log_info("%s: hash 0x%04x, checksum 0x%04x\n", __func__,
                pcb_data->hash, le16_to_cpu(data->hash));
    if (pcb_data->hash != le16_to_cpu(data->hash))
    {
        return PARSE_NEXT;
    }
    de_namelen = data->name_len;
    csum = exfat_checksum16(csum, data, EXFAT_CHUNK_SIZE);
    wstr = pcb_data->wstr;
    while (de_namelen)
    {
        name = pd_iter_next_de(pd, &pd_iter);
        if (!name)
        {
            return PARSE_NEXT;
        }
        if (name->type != EXFAT_TYPE_NAME)
        {
            return PARSE_NEXT;
        }
        len = min_t(u8, de_namelen, EXFAT_CHUNK_NAME_SIZE);
        if (!exfat_match(sbi, wstr, name->name, len))
        {
            return PARSE_NEXT;
        }
        wstr += len;
        de_namelen -= len;
        csum = exfat_checksum16(csum, name, EXFAT_CHUNK_SIZE);
    }
    /* Checksum of the remaining entries (may not be exfat_chunk_name) */
    while ((name = pd_iter_next_de(pd, &pd_iter)) != NULL)
    {
        if (!(name->type & EXFAT_TYPE_SUBCHUNK))
        {
            return PARSE_NEXT;
        }
        csum = exfat_checksum16(csum, name, EXFAT_CHUNK_SIZE);
    }
    if (le16_to_cpu(dirent->checksum) != csum)
    {
        fs_log_info("checksum failed for directory entry"
                    " in lookup (0x%04x != 0x%04x)",
                    le16_to_cpu(dirent->checksum), csum);
        return -EIO;
    }
    fs_log_info("%s: found\n", __func__);

    /* set parse data information */
    pcb_data->sinfo->nr_slots = dirent->sub_chunks + 1;
    pcb_data->sinfo->clusnr = data->clusnr;
    /* ????? maybe will get inode size, for free inode allocated space */
    pcb_data->sinfo->pd.size = pd->size;
    for (i = 0; i < pd->nr_bhs; i++)
    {
        get_bh(pd->bhs[i]);
        pcb_data->sinfo->pd.bhs[i] = pd->bhs[i];
    }
    pcb_data->sinfo->pd.nr_bhs = pd->nr_bhs;
    pcb_data->sinfo->pd.bh_offset = pd->bh_offset;
    /* find special directory entry,
     * finish lookup work */
    return PARSE_STOP;
}

#if defined CONFIG_FSYS_EXFAT_RW
int  exfat_find(struct inode *dir, struct qstr *qname,
                struct exfat_slot_info *sinfo)
{
    struct exfat_sb_info *sbi = EXFAT_SB(dir->i_sb);
    //struct nls_table *nls = sbi->opts.nls;
    struct exfat_parse_callback pcb;
    struct find_parse_data pcb_data;
    const unsigned char *name;
    __wchar_t *wstr;
    unsigned int i, name_left, wlen, wleft;
    loff_t pos;
    int err;

    /* FIXME: can't use converted name/hash? */
    wlen = wleft = min(qname->len, EXFAT_MAX_NAMELEN);
    pcb_data.wstr = wstr = kmalloc(wlen * sizeof(*wstr), GFP_KERNEL);
    if (!wstr)
    {
        return -ENOMEM;
    }
    name = qname->name;
    name_left = qname->len;
    /* FIXME: maybe toupper and hash should be computed at same time */
    err = exfat_conv_c2u(nls, &name, &name_left, &wstr, &wleft);
    /* FIXME: shouldn't return -ENAMETOOLONG? */
    if (err)
    {
        kfree(pcb_data.wstr);
        return err;
    }
    pcb_data.wlen = wlen - wleft;
    pcb_data.hash = 0;
    for (i = 0; i < pcb_data.wlen; i++)
    {
        __le16 uc;
        pcb_data.wstr[i] = exfat_towupper(sbi, pcb_data.wstr[i]);
        uc = cpu_to_le16(pcb_data.wstr[i]);
        pcb_data.hash = exfat_checksum16(pcb_data.hash, &uc, sizeof(uc));
    }
    pcb_data.sinfo = sinfo;
    pcb.parse = find_parse;
    pcb.data = &pcb_data;
    /* FIXME: pos can be previous lookuped position. */
    pos = 0;
    err = exfat_parse_dir(NULL, dir, &pos, &pcb);
    kfree(pcb_data.wstr);
    /* FIXME: can cache the position for next lookup()?
     * Assume sequential locality (stat() for all files after readdir())
     */
    return err;
}
#endif  /* FSYS_EXFAT_RW */

struct readdir_parse_data
{
    void        *dirbuf;
    filldir_t    filldir;
    void        *str;
};

#define EXFAT_MAX_NAME_BUFSIZE  (EXFAT_MAX_NAMELEN * NLS_MAX_CHARSET_SIZE)
#define DO_READDIR_CHECKSUM

static int readdir_parse(struct inode *dir, loff_t pos,
                         struct exfat_parse_data *pd,
                         struct exfat_parse_callback *pcb)
{
    struct super_block *sb = dir->i_sb;
    //struct nls_table *nls = EXFAT_SB(sb)->opts.nls;
    struct readdir_parse_data *pcb_data = pcb->data;
    struct exfat_parse_iter_data pd_iter;
    struct exfat_chunk_dirent *dirent;
    struct exfat_chunk_data *data;
    struct exfat_chunk_name *name;
    struct inode *inode;
    unsigned long ino;
    unsigned int d_type;
#ifdef DO_READDIR_CHECKSUM
    u16 csum;
#endif
    u8 de_namelen;
    const __le16 *ustr;
    unsigned char *str;
    unsigned int uleft, str_left, str_len;
    int err;

    /* FIXME: should cache whether checksum was done */

    dirent = pd_iter_first_de(pd, &pd_iter);
    if (dirent->type != EXFAT_TYPE_DIRENT)
    {
        return PARSE_NEXT;
    }
    /* ->sub_chunk was already checked by caller. */
    EXFAT_DEBUG_ON((1 + dirent->sub_chunks) << EXFAT_CHUNK_BITS != pd->size,
                   "sub_chunks %u, pd->size %lu\n", dirent->sub_chunks, pd->size);
    fs_log_info("exFAT: 0x%02x found\n", dirent->type);
#ifdef DO_READDIR_CHECKSUM
    /* Skip dirent->checksum field */
    csum = exfat_checksum16(0, dirent, 2);
    csum = exfat_checksum16(csum, (u8 *)dirent + 4, EXFAT_CHUNK_SIZE - 4);
#endif

    data = pd_iter_next_de(pd, &pd_iter);
    if (!data)
    {
        return PARSE_NEXT;
    }
    if (data->type != EXFAT_TYPE_DATA)
    {
        return PARSE_NEXT;
    }
    de_namelen = data->name_len;
#ifdef DO_READDIR_CHECKSUM
    csum = exfat_checksum16(csum, data, EXFAT_CHUNK_SIZE);
#endif

    if (!pcb_data->str)
    {
        /* FIXME: too big, should be own kmem_cachep */
        pcb_data->str = kmalloc(EXFAT_MAX_NAME_BUFSIZE, GFP_KERNEL);
        if (!pcb_data->str)
        {
            return -ENOMEM;
        }
    }

    str = pcb_data->str;
    str_left = EXFAT_MAX_NAME_BUFSIZE;
    while (de_namelen)
    {
        name = pd_iter_next_de(pd, &pd_iter);
        if (!name)
        {
            return PARSE_NEXT;
        }
        if (name->type != EXFAT_TYPE_NAME)
        {
            return PARSE_NEXT;
        }

        ustr = name->name;
        uleft = min_t(u8, de_namelen, EXFAT_CHUNK_NAME_SIZE);
        de_namelen -= uleft;

        /*err = */exfat_conv_u2c(nls, &ustr, &uleft, &str, &str_left);
        //if (err) {
        //  /* FIXME: if char was invalid, what to do */
        //  if (err == -EINVAL) {
        //      fs_log_error("exFAT: invalid char 0x%04x in file name\n",
        //                   le16_to_cpu(*ustr));
        //  }
        //  /* FIXME: shouldn't return -ENAMETOOLONG */
        //  return err;
        //}
#ifdef DO_READDIR_CHECKSUM
        csum = exfat_checksum16(csum, name, EXFAT_CHUNK_SIZE);
#endif
    }

    /* Checksum of the remaining entries (may not be exfat_chunk_name) */
    while ((name = pd_iter_next_de(pd, &pd_iter)) != NULL)
    {
        if (!(name->type & EXFAT_TYPE_SUBCHUNK))
        {
            return PARSE_NEXT;
        }
#ifdef DO_READDIR_CHECKSUM
        csum = exfat_checksum16(csum, name, EXFAT_CHUNK_SIZE);
#endif
    }
#ifdef DO_READDIR_CHECKSUM
    if (le16_to_cpu(dirent->checksum) != csum)
    {
        fs_log_error("checksum failed for directory entry"
                     "in readdir (0x%04x != 0x%04x)",
                     le16_to_cpu(dirent->checksum), csum);
        return -EIO;
    }
#endif

    inode = exfat_ilookup(sb, pd->bhs[0]->b_blocknr, pd->bh_offset);
    if (inode)
    {
        ino = inode->i_ino;
        iput(inode);
    }
    else
    {
        ino = iunique(sb, EXFAT_RESERVED_INO);
    }
    /* convert exFAT dirent attributes to VFS dirent attributes,
     * And exFAT dirent attributes is identical with VFS dirent attributes,
     * so no need to convert atrributes
     */
    d_type = dirent->attrib;

    str_len = EXFAT_MAX_NAME_BUFSIZE - str_left;

    str[0] = '\0';
    fs_log_info("%s: ent %p, str %s, len %u, pos %lld, ino %lu, type 0x%x\n",
                __func__, pcb_data->dirbuf, (char *)pcb_data->str, str_len,
                pos, ino, d_type);
    if (pcb_data->filldir(pcb_data->dirbuf, pcb_data->str, str_len, 0,
                          pos, ino, d_type, (__u64)(data->size)))
    {

        /* fill directory always, until directory buffer full */
        fs_log_trace01("exFAT:fill directory buffer full\n");
        return PARSE_STOP;
    }
    /* read one directory already, should be continue or stop ??? */
    return PARSE_NEXT;
}

static __s32 exfat_readdir(struct file *filp, void *dirbuf, filldir_t filldir)
{
    struct inode *dir = filp->f_dentry->d_inode;
    struct exfat_parse_callback pcb;
    struct readdir_parse_data pcb_data;
    loff_t pos;
    int err;

    /* exFAT doesn't have "." and "..", so returns fake entries
     * FIXME: epos not support "." and ".." subdirectory
     */
    //  while (filp->f_pos < 2) {
    //      unsigned long ino;
    //      int len = filp->f_pos + 1;
    //
    //      if (filp->f_pos == 0)
    //          ino = dir->i_ino;
    //      else
    //          ino = parent_ino(filp->f_path.dentry);
    //
    //      __log("%s: ent %p, .., len %u, pos %lld, ino %lu, type 0x%x",
    //             __func__, dirbuf, len, filp->f_pos, ino, DT_DIR);
    //      if (filldir(dirbuf, "..", len, /* filp->f_pos, ino */, DT_DIR))
    //          return 0;
    //
    //      filp->f_pos++;
    //  }
    /* direntory position offset must align by EXFAT_CHUNK_SIZE */
    if (filp->f_pos > 2 && (filp->f_pos & (EXFAT_CHUNK_SIZE - 1)))
    {
        return -ENOENT;
    }

    /* position offset must small then directory size */
    if (filp->f_pos >= dir->i_size)
    {
        return -ENOENT;
    }

    pcb_data.dirbuf = dirbuf;
    pcb_data.filldir = filldir;
    pcb_data.str = NULL;
    pcb.parse = readdir_parse;
    pcb.data = &pcb_data;

    pos = filp->f_pos /* & ~(loff_t)(EXFAT_CHUNK_SIZE - 1) */;
    err = exfat_parse_dir(filp, dir, &pos, &pcb);
    filp->f_pos = pos;
    //  if (pos < 2)
    //      filp->f_pos = 2;
    //  else
    //      filp->f_pos = pos;

    if (pcb_data.str)
    {
        kfree(pcb_data.str);
    }

    return err;
}

struct rootdir_parse_data
{
    u32 bitmap_clusnr;
    u64 bitmap_size;
    u32 upcase_checksum;
    u32 upcase_clusnr;
    u64 upcase_size;
};

/*
 * check root directory parse finished or not
 *
 */
int rootdir_parse_finish(struct rootdir_parse_data *pcb_data)
{
    if (pcb_data->bitmap_clusnr && pcb_data->upcase_clusnr)
    {
        return 1;
    }
    /* root directory work not finished now */
    return 0;
}

static int rootdir_parse(struct inode *dir, loff_t pos,
                         struct exfat_parse_data *pd,
                         struct exfat_parse_callback *pcb)
{
    struct exfat_chunk_bitmap *bitmap;
    struct exfat_chunk_upcase *upcase;
    struct rootdir_parse_data *pcb_data = pcb->data;

    bitmap = pd_get_chunk(pd);
    switch (bitmap->type)
    {
        case EXFAT_TYPE_BITMAP:
            if (pcb_data->bitmap_clusnr)
            {
                /* FIXME: for this, what do we do */
                fs_log_warning("exFAT: found another"
                               "free space bitmap, ignored\n");
                break;
            }
            pcb_data->bitmap_clusnr = le32_to_cpu(bitmap->clusnr);
            pcb_data->bitmap_size = le64_to_cpu(bitmap->size);
            break;
        case EXFAT_TYPE_UPCASE:
            if (pcb_data->upcase_clusnr)
            {
                /* FIXME: for this, what do we do */
                fs_log_warning("exFAT: found another"
                               "upper-case table, ignored\n");
                break;
            }
            upcase = pd_get_chunk(pd);
            pcb_data->upcase_checksum = le32_to_cpu(upcase->checksum);
            pcb_data->upcase_clusnr = le32_to_cpu(upcase->clusnr);
            pcb_data->upcase_size = le64_to_cpu(upcase->size);
            break;
    }
    /* parse root directory work finished or not */
    if (rootdir_parse_finish(pcb_data))
    {
        return PARSE_STOP;
    }
    return PARSE_NEXT;
}

int exfat_read_rootdir(struct inode *dir)
{
    struct super_block *sb = dir->i_sb;
    struct exfat_parse_callback pcb;
    struct rootdir_parse_data pcb_data;
    loff_t pos;
    int err;

    pcb_data.bitmap_clusnr = 0;
    pcb_data.upcase_clusnr = 0;
    pcb.parse = rootdir_parse;
    pcb.data = &pcb_data;

    pos = 0;
    err = exfat_parse_dir(NULL, dir, &pos, &pcb);
    if (err)
    {
        return err;
    }
    err = exfat_setup_bitmap(sb, pcb_data.bitmap_clusnr,
                             pcb_data.bitmap_size);
    if (err)
    {
        return err;
    }
    /* add bitmap inode under root directory */
    inc_nlink(dir);
    err = exfat_setup_upcase(sb, pcb_data.upcase_checksum,
                             pcb_data.upcase_clusnr,
                             pcb_data.upcase_size);
    if (err)
    {
        exfat_free_bitmap(EXFAT_SB(sb));
        return err;
    }
    /* add upcase inode under root directory */
    inc_nlink(dir);
    return 0;
}

const struct file_operations exfat_dir_ops =
{
    .llseek     = generic_file_llseek,
    .readdir    = exfat_readdir,
    //  .fsync      = file_fsync,           /* ?????? */
    //  .release    = exfat_release_dir,
};