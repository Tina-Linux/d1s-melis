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
#include "fatfs.h"
#include "dcache.h"
#include "buffer_head.h"
#include "namei.h"
#include "nls.h"
#include "endians.h"
#include "errno.h"
#include "page_pool.h"
#include "slab.h"
#include "fsys_debug.h"

static int vfat_revalidate(struct dentry *dentry, struct nameidata *nd)
{
    int ret = 1;

    if (!dentry->d_inode &&
        nd && !(nd->flags & LOOKUP_CONTINUE) && (nd->flags & LOOKUP_CREATE))
        /*
         * negative dentry is dropped, in order to make sure
         * to use the name which a user desires if this is
         * create path.
         */
    {
        ret = 0;
    }
    else
    {
        //      if (dentry->d_time != dentry->d_parent->d_inode->i_version)
        //          ret = 0;
    }
    return ret;
}

/* returns the length of a struct qstr, ignoring trailing dots */
static unsigned int vfat_striptail_len(struct qstr *qstr)
{
    unsigned int len = qstr->len;

    while (len && qstr->name[len - 1] == '.')
    {
        len--;
    }
    return len;
}

/*
 * Compute the hash for the vfat name corresponding to the dentry.
 * Note: if the name is invalid, we leave the hash code unchanged so
 * that the existing dentry can be used. The vfat fs routines will
 * return ENOENT or EINVAL as appropriate.
 */
static int vfat_hash(struct dentry *dentry, struct qstr *qstr)
{
    qstr->hash = full_name_hash(qstr->name, vfat_striptail_len(qstr));
    return 0;
}

/*
 * Compute the hash for the vfat name corresponding to the dentry.
 * Note: if the name is invalid, we leave the hash code unchanged so
 * that the existing dentry can be used. The vfat fs routines will
 * return ENOENT or EINVAL as appropriate.
 */
static int vfat_hashi(struct dentry *dentry, struct qstr *qstr)
{
    const unsigned char *name;
    unsigned int len;
    unsigned long hash;

    name = qstr->name;
    len = vfat_striptail_len(qstr);

    hash = init_name_hash();
    while (len--)
    {
        hash = partial_name_hash(nls_tolower(nls, *name++), hash);
    }
    qstr->hash = end_name_hash(hash);

    return 0;
}

/*
 * Case insensitive compare of two vfat names.
 */
static int vfat_cmpi(struct dentry *dentry, struct qstr *a, struct qstr *b)
{
    unsigned int alen, blen;

    /* A filename cannot end in '.' or we treat it like it has none */
    alen = vfat_striptail_len(a);
    blen = vfat_striptail_len(b);
    if (alen == blen)
    {
        if (nls_strnicmp(nls, a->name, b->name, alen) == 0)
        {
            return 0;
        }
    }
    return 1;
}

/*
 * Case sensitive compare of two vfat names.
 */
static int vfat_cmp(struct dentry *dentry, struct qstr *a, struct qstr *b)
{
    unsigned int alen, blen;

    /* A filename cannot end in '.' or we treat it like it has none */
    alen = vfat_striptail_len(a);
    blen = vfat_striptail_len(b);
    if (alen == blen)
    {
        if (strncmp(a->name, b->name, alen) == 0)
        {
            return 0;
        }
    }
    return 1;
}

const struct dentry_operations vfat_dentry_ops[4] =
{
    {
        .d_hash     = vfat_hashi,
        .d_compare  = vfat_cmpi,
    },
    {
        .d_revalidate   = vfat_revalidate,
        .d_hash     = vfat_hashi,
        .d_compare  = vfat_cmpi,
    },
    {
        .d_hash     = vfat_hash,
        .d_compare  = vfat_cmp,
    },
    {
        .d_revalidate   = vfat_revalidate,
        .d_hash     = vfat_hash,
        .d_compare  = vfat_cmp,
    }
};

static int vfat_find(struct inode *dir, struct qstr *qname,
                     struct fat_slot_info *sinfo)
{
    unsigned int len = vfat_striptail_len(qname);
    if (len == 0)
    {
        return -ENOENT;
    }
    return fat_search_long(dir, qname->name, len, sinfo);
}

static struct dentry *vfat_lookup(struct inode *dir, struct dentry *dentry,
                                  struct nameidata *nd)
{
    struct super_block *sb = dir->i_sb;
    struct fat_slot_info sinfo;
    struct inode *inode = NULL;
    struct dentry *alias;
    int err, table;

    table = (MSDOS_SB(sb)->options.name_check == 's') ? 2 : 0;
    dentry->d_op = &vfat_dentry_ops[table];

    err = vfat_find(dir, &dentry->d_name, &sinfo);
    if (err)
    {
        table++;
        goto error;
    }
    inode = fat_build_inode(sb, sinfo.de, sinfo.i_pos);
    brelse(sinfo.bh);
    if (IS_ERR(PTR_ERR(inode)))
    {
        return ERR_PTR(PTR_ERR(inode));
    }

error:
    dentry->d_op = &vfat_dentry_ops[table];
    //  dentry->d_time = dentry->d_parent->d_inode->i_version;
    d_splice_alias(inode, dentry);

    //~PeterChen~
    //dentry is NULL means it did not find any alias from d_cache.which can be
    //build by other process run.
    //If you want to enable multi-process fs, you need to get the d_find_alias()
    // functions back from kernel 2.6.22.
    dentry = NULL;
    return dentry;
}


#if defined CONFIG_FSYS_FAT_RW

/* Characters that are undesirable in an MS-DOS file name */
static   __wchar_t vfat_bad_char(__wchar_t w)
{
    return (w < 0x0020)
           || (w == '*') || (w == '?') || (w == '<') || (w == '>')
           || (w == '|') || (w == '"') || (w == ':') || (w == '/')
           || (w == '\\');
}

static   __wchar_t vfat_replace_char(__wchar_t w)
{
    return (w == '[') || (w == ']') || (w == ';') || (w == ',')
           || (w == '+') || (w == '=');
}

static __wchar_t vfat_skip_char(__wchar_t w)
{
    return (w == '.') || (w == ' ');
}

static   int vfat_is_used_badchars(const __wchar_t *s, int len)
{
    int i;

    for (i = 0; i < len; i++)
        if (vfat_bad_char(s[i]))
        {
            return -EINVAL;
        }
    return 0;
}

static int vfat_valid_longname(const unsigned char *name, unsigned int len)
{
    if (name[len - 1] == ' ')
    {
        return -EINVAL;
    }
    if (len >= 256)
    {
        return -ENAMETOOLONG;
    }
    return 0;
}

static int vfat_find_form(struct inode *dir, unsigned char *name)
{
    struct fat_slot_info sinfo;
    int err = fat_scan(dir, name, &sinfo);
    if (err)
    {
        return -ENOENT;
    }
    brelse(sinfo.bh);
    return 0;
}

/*
 *
 *  isalphabeta ()
 *  cChar is a letter ?
 */
int isalphabeta(int cChar)
{
    if (((cChar >= 'A') && (cChar <= 'Z')) ||
        ((cChar >= 'a') && (cChar <= 'z')))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
 * 1) Valid characters for the 8.3 format alias are any combination of
 * letters, uppercase alphabets, digits, any of the
 * following special characters:
 *     $ % ' ` - @ { } ~ ! # ( ) & _ ^
 * In this case Longfilename is not stored in disk.
 *
 * WinNT's Extension:
 * File name and extension name is contain uppercase/lowercase
 * only. And it is expressed by CASE_LOWER_BASE and CASE_LOWER_EXT.
 *
 * 2) File name is 8.3 format, but it contain the uppercase and
 * lowercase char, muliti bytes char, etc. In this case numtail is not
 * added, but Longfilename is stored.
 *
 * 3) When the one except for the above, or the following special
 * character are contained:
 *        .   [ ] ; , + =
 * numtail is added, and Longfilename must be stored in disk .
 */
struct shortname_info
{
    unsigned char lower: 1,
             upper: 1,
             valid: 1;
};
#define INIT_SHORTNAME_INFO(x)  do {        \
        (x)->lower = 1;             \
        (x)->upper = 1;             \
        (x)->valid = 1;             \
    } while (0)

static   int to_shortname_char(struct nls_table *nls,
                               unsigned char *buf, int buf_size,
                               __wchar_t *src, struct shortname_info *info)
{
    int len;

    if (vfat_skip_char(*src))
    {
        info->valid = 0;
        return 0;
    }
    if (vfat_replace_char(*src))
    {
        info->valid = 0;
        buf[0] = '_';
        return 1;
    }

    len = nls->uni2char(*src, buf, buf_size);
    if (len <= 0)
    {
        info->valid = 0;
        buf[0] = '_';
        len = 1;
    }
    else if (len == 1)
    {
        unsigned char prev = buf[0];

        if (buf[0] >= 0x7F)
        {
            info->lower = 0;
            info->upper = 0;
        }

        buf[0] = nls_toupper(nls, buf[0]);
        if (isalphabeta(buf[0]))
        {
            if (buf[0] == prev)
            {
                info->lower = 0;
            }
            else
            {
                info->upper = 0;
            }
        }
    }
    else
    {
        info->lower = 0;
        info->upper = 0;
    }

    return len;
}

/*
 * Given a valid longname, create a unique shortname.  Make sure the
 * shortname does not exist
 * Returns negative number on error, 0 for a normal
 * return, and 1 for valid shortname
 */
static int vfat_create_shortname(struct inode *dir, struct nls_table *nls,
                                 __wchar_t *uname, int ulen,
                                 unsigned char *name_res, unsigned char *lcase)
{
    struct fat_mount_options *opts = &MSDOS_SB(dir->i_sb)->options;
    __wchar_t *ip, *ext_start, *end, *name_start;
    unsigned char base[9], ext[4], buf[5], *p;
    unsigned char charbuf[NLS_MAX_CHARSET_SIZE];
    int chl, chi;
    int sz = 0, extlen, baselen, i, numtail_baselen, numtail2_baselen;
    int is_shortname;
    struct shortname_info base_info, ext_info;

    is_shortname = 1;
    INIT_SHORTNAME_INFO(&base_info);
    INIT_SHORTNAME_INFO(&ext_info);

    /* Now, we need to create a shortname from the long name */
    ext_start = end = &uname[ulen];
    while (--ext_start >= uname)
    {
        if (*ext_start == 0x002E)   /* is `.' */
        {
            if (ext_start == end - 1)
            {
                sz = ulen;
                ext_start = NULL;
            }
            break;
        }
    }

    if (ext_start == uname - 1)
    {
        sz = ulen;
        ext_start = NULL;
    }
    else if (ext_start)
    {
        /*
         * Names which start with a dot could be just
         * an extension eg. "...test".  In this case Win95
         * uses the extension as the name and sets no extension.
         */
        name_start = &uname[0];
        while (name_start < ext_start)
        {
            if (!vfat_skip_char(*name_start))
            {
                break;
            }
            name_start++;
        }
        if (name_start != ext_start)
        {
            sz = ext_start - uname;
            ext_start++;
        }
        else
        {
            sz = ulen;
            ext_start = NULL;
        }
    }

    numtail_baselen = 6;
    numtail2_baselen = 2;
    for (baselen = i = 0, p = base, ip = uname; i < sz; i++, ip++)
    {
        chl = to_shortname_char(nls, charbuf, sizeof(charbuf),
                                ip, &base_info);
        if (chl == 0)
        {
            continue;
        }

        if (baselen < 2 && (baselen + chl) > 2)
        {
            numtail2_baselen = baselen;
        }
        if (baselen < 6 && (baselen + chl) > 6)
        {
            numtail_baselen = baselen;
        }
        for (chi = 0; chi < chl; chi++)
        {
            *p++ = charbuf[chi];
            baselen++;
            if (baselen >= 8)
            {
                break;
            }
        }
        if (baselen >= 8)
        {
            if ((chi < chl - 1) || (ip + 1) - uname < sz)
            {
                is_shortname = 0;
            }
            break;
        }
    }
    if (baselen == 0)
    {
        return -EINVAL;
    }

    extlen = 0;
    if (ext_start)
    {
        for (p = ext, ip = ext_start; extlen < 3 && ip < end; ip++)
        {
            chl = to_shortname_char(nls, charbuf, sizeof(charbuf),
                                    ip, &ext_info);
            if (chl == 0)
            {
                continue;
            }

            if ((extlen + chl) > 3)
            {
                is_shortname = 0;
                break;
            }
            for (chi = 0; chi < chl; chi++)
            {
                *p++ = charbuf[chi];
                extlen++;
            }
            if (extlen >= 3)
            {
                if (ip + 1 != end)
                {
                    is_shortname = 0;
                }
                break;
            }
        }
    }
    ext[extlen] = '\0';
    base[baselen] = '\0';

    /* Yes, it can happen. ".\xe5" would do it. */
    if (base[0] == DELETED_FLAG)
    {
        base[0] = 0x05;
    }

    /* OK, at this point we know that base is not longer than 8 symbols,
     * ext is not longer than 3, base is nonempty, both don't contain
     * any bad symbols (lowercase transformed to uppercase).
     */

    memset(name_res, ' ', MSDOS_NAME);
    memcpy(name_res, base, baselen);
    memcpy(name_res + 8, ext, extlen);
    *lcase = 0;
    if (is_shortname && base_info.valid && ext_info.valid)
    {
        if (vfat_find_form(dir, name_res) == 0)
        {
            return -EEXIST;
        }

        if (opts->shortname & VFAT_SFN_CREATE_WIN95)
        {
            return (base_info.upper && ext_info.upper);
        }
        else if (opts->shortname & VFAT_SFN_CREATE_WINNT)
        {
            if ((base_info.upper || base_info.lower) &&
                (ext_info.upper || ext_info.lower))
            {
                if (!base_info.upper && base_info.lower)
                {
                    *lcase |= CASE_LOWER_BASE;
                }
                if (!ext_info.upper && ext_info.lower)
                {
                    *lcase |= CASE_LOWER_EXT;
                }
                return 1;
            }
            return 0;
        }
        else
        {
            BUG();
        }
    }

    if (opts->numtail == 0)
        if (vfat_find_form(dir, name_res) < 0)
        {
            return 0;
        }

    /*
     * Try to find a unique extension.  This used to
     * iterate through all possibilities sequentially,
     * but that gave extremely bad performance.  Windows
     * only tries a few cases before using random
     * values for part of the base.
     */

    if (baselen > 6)
    {
        baselen = numtail_baselen;
        name_res[7] = ' ';
    }
    name_res[baselen] = '~';
    for (i = 1; i < 10; i++)
    {
        name_res[baselen + 1] = i + '0';
        if (vfat_find_form(dir, name_res) < 0)
        {
            return 0;
        }
    }

    i = jiffies & 0xffff;
    sz = (jiffies >> 16) & 0x7;
    if (baselen > 2)
    {
        baselen = numtail2_baselen;
        name_res[7] = ' ';
    }
    name_res[baselen + 4] = '~';
    name_res[baselen + 5] = '1' + sz;
    while (1)
    {
        snprintf(buf, sizeof(buf), "%04X", i & 0xffff);
        memcpy(&name_res[baselen], buf, 4);
        if (vfat_find_form(dir, name_res) < 0)
        {
            break;
        }
        i -= 11;
    }
    return 0;
}

/* Translate a string, including coded sequences into Unicode */
static int xlate_to_uni(const unsigned char *name, int len, unsigned char *outname,
                        int *longlen, int *outlen, int escape, int utf8,
                        struct nls_table *nls)
{
    const unsigned char *ip;
    unsigned char nc;
    unsigned char *op;
    unsigned int ec;
    int i, k, fill;
    int charlen;

    if (utf8)
    {
        int name_len = strlen(name);

        *outlen = utf8_mbstowcs((__wchar_t *)outname, name, PAGE_SIZE);

        /*
         * We stripped '.'s before and set len appropriately,
         * but utf8_mbstowcs doesn't care about len
         */
        *outlen -= (name_len - len);

        op = &outname[*outlen * sizeof(__wchar_t)];
    }
    else
    {
        if (nls)
        {
            for (i = 0, ip = name, op = outname, *outlen = 0;
                 i < len && *outlen <= 260;
                 *outlen += 1)
            {
                if (escape && (*ip == ':'))
                {
                    if (i > len - 5)
                    {
                        return -EINVAL;
                    }
                    ec = 0;
                    for (k = 1; k < 5; k++)
                    {
                        nc = ip[k];
                        ec <<= 4;
                        if (nc >= '0' && nc <= '9')
                        {
                            ec |= nc - '0';
                            continue;
                        }
                        if (nc >= 'a' && nc <= 'f')
                        {
                            ec |= nc - ('a' - 10);
                            continue;
                        }
                        if (nc >= 'A' && nc <= 'F')
                        {
                            ec |= nc - ('A' - 10);
                            continue;
                        }
                        return -EINVAL;
                    }
                    *op++ = ec & 0xFF;
                    *op++ = ec >> 8;
                    ip += 5;
                    i += 5;
                }
                else
                {
                    if ((charlen = nls->char2uni(ip, len - i, (__wchar_t *)op)) < 0)
                    {
                        return -EINVAL;
                    }
                    ip += charlen;
                    i += charlen;
                    op += 2;
                }
            }
        }
        else
        {
            for (i = 0, ip = name, op = outname, *outlen = 0;
                 i < len && *outlen <= 260;
                 i++, *outlen += 1)
            {
                *op++ = *ip++;
                *op++ = 0;
            }
        }
    }
    if (*outlen > 260)
    {
        return -ENAMETOOLONG;
    }

    *longlen = *outlen;
    if (*outlen % 13)
    {
        *op++ = 0;
        *op++ = 0;
        *outlen += 1;
        if (*outlen % 13)
        {
            fill = 13 - (*outlen % 13);
            for (i = 0; i < fill; i++)
            {
                *op++ = 0xff;
                *op++ = 0xff;
            }
            *outlen += fill;
        }
    }

    return 0;
}

static int vfat_build_slots(struct inode *dir, const unsigned char *name,
                            int len, int is_dir, int cluster,
                            struct timespec *ts,
                            struct msdos_dir_slot *slots, int *nr_slots)
{
    struct msdos_sb_info *sbi = MSDOS_SB(dir->i_sb);
    struct fat_mount_options *opts = &sbi->options;
    struct msdos_dir_slot *ps;
    struct msdos_dir_entry *de;
    unsigned long page;
    unsigned char cksum, lcase;
    unsigned char msdos_name[MSDOS_NAME];
    __wchar_t *uname;
    __s16 time, date;
    int err, ulen, usize, i;
    __s64 offset;
    __s32 is_hid = 0;

    *nr_slots = 0;
    err = vfat_valid_longname(name, len);
    if (err)
    {
        return err;
    }

    page = __get_free_page(GFP_KERNEL);
    if (!page)
    {
        return -ENOMEM;
    }

    if (name[0] == '.')
    {

        /* create hidden entry */
        is_hid = 1;
    }

    uname = (__wchar_t *)page;
    err = xlate_to_uni(name, len, (unsigned char *)uname, &ulen, &usize,
                       opts->unicode_xlate, opts->utf8, nls/*sbi->nls_io*/);
    if (err)
    {
        goto out_free;
    }

    err = vfat_is_used_badchars(uname, ulen);
    if (err)
    {
        goto out_free;
    }

    err = vfat_create_shortname(dir, nls/*sbi->nls_disk*/, uname, ulen,
                                msdos_name, &lcase);
    if (err < 0)
    {
        goto out_free;
    }
    else if (err == 1)
    {
        de = (struct msdos_dir_entry *)slots;
        err = 0;
        goto shortname;
    }

    /* build the entry of long file name */
    cksum = fat_checksum(msdos_name);

    *nr_slots = usize / 13;
    for (ps = slots, i = *nr_slots; i > 0; i--, ps++)
    {
        ps->id = i;
        ps->attr = ATTR_EXT;
        ps->reserved = 0;
        ps->alias_checksum = cksum;
        ps->start = 0;
        offset = (i - 1) * 13;
        fatwchar_to16(ps->name0_4, uname + offset, 5);
        fatwchar_to16(ps->name5_10, uname + offset + 5, 6);
        fatwchar_to16(ps->name11_12, uname + offset + 11, 2);
    }
    slots[0].id |= 0x40;
    de = (struct msdos_dir_entry *)ps;

shortname:
    /* build the entry of 8.3 alias name */
    (*nr_slots)++;
    memcpy(de->name, msdos_name, MSDOS_NAME);
    de->attr = is_dir ? ATTR_DIR : ATTR_ARCH;
    if (is_hid)
    {
        de->attr |= ATTR_HIDDEN;
    }
    de->lcase = lcase;
    fat_date_unix2dos(ts->tv_sec, &time, &date);
    de->time = de->ctime = time;
    de->date = de->cdate = de->adate = date;
    de->ctime_cs = 0;
    de->start = cpu_to_le16(cluster);
    de->starthi = cpu_to_le16(cluster >> 16);
    de->size = 0;
out_free:
    free_page(page);
    return err;
}

static int vfat_add_entry(struct inode *dir, struct qstr *qname, int is_dir,
                          int cluster, struct timespec *ts,
                          struct fat_slot_info *sinfo)
{
    struct msdos_dir_slot *slots;
    unsigned int len;
    int err, nr_slots;

    len = vfat_striptail_len(qname);
    if (len == 0)
    {
        return -ENOENT;
    }

    slots = kmalloc(sizeof(*slots) * MSDOS_SLOTS, GFP_KERNEL);
    if (slots == NULL)
    {
        return -ENOMEM;
    }

    err = vfat_build_slots(dir, qname->name, len, is_dir, cluster, ts,
                           slots, &nr_slots);
    if (err)
    {
        goto cleanup;
    }

    err = fat_add_entries(dir, slots, nr_slots, sinfo);
    if (err)
    {
        goto cleanup;
    }

    /* update timestamp */
    dir->i_ctime = dir->i_mtime = dir->i_atime = *ts;
    if (IS_DIRSYNC(dir))
    {
        (void)fat_sync_inode(dir);
    }
    else
    {
        mark_inode_dirty(dir);
    }
cleanup:
    kfree(slots);
    return err;
}

static __s32 vfat_create(struct inode *dir, struct dentry *dentry, __s32 mode,
                         struct nameidata *nd)
{
    struct super_block *sb = dir->i_sb;
    struct inode *inode;
    struct fat_slot_info sinfo;
    struct timespec ts;
    int err;

    ts = CURRENT_TIME_SEC;
    err = vfat_add_entry(dir, &dentry->d_name, 0, 0, &ts, &sinfo);
    if (err)
    {
        goto out;
    }
    dir->i_version++;

    inode = fat_build_inode(sb, sinfo.de, sinfo.i_pos);
    brelse(sinfo.bh);
    if (IS_ERR(PTR_ERR(inode)))
    {
        err = PTR_ERR(inode);
        goto out;
    }
    inode->i_version++;
    inode->i_mtime = inode->i_atime = inode->i_ctime = ts;
    /* timestamp is already written, so mark_inode_dirty() is unneeded. */

    //  dentry->d_time = dentry->d_parent->d_inode->i_version;
    d_instantiate(dentry, inode);
out:
    return err;
}

static __s32 vfat_rmdir(struct inode *dir, struct dentry *dentry)
{
    struct inode *inode = dentry->d_inode;
    struct fat_slot_info sinfo;
    int err;

    err = fat_dir_empty(inode);
    if (err)
    {
        goto out;
    }
    err = vfat_find(dir, &dentry->d_name, &sinfo);
    if (err)
    {
        goto out;
    }

    err = fat_remove_entries(dir, &sinfo);  /* and releases bh */
    if (err)
    {
        goto out;
    }
    drop_nlink(dir);

    clear_nlink(inode);
    inode->i_mtime = inode->i_atime = CURRENT_TIME_SEC;
    fat_detach(inode);
out:

    return err;
}

static __s32 vfat_unlink(struct inode *dir, struct dentry *dentry)
{
    struct inode *inode = dentry->d_inode;
    struct fat_slot_info sinfo;
    int err;

    err = vfat_find(dir, &dentry->d_name, &sinfo);
    if (err)
    {
        goto out;
    }

    err = fat_remove_entries(dir, &sinfo);  /* and releases bh */
    if (err)
    {
        goto out;
    }

    clear_nlink(inode);
    inode->i_mtime = inode->i_atime = CURRENT_TIME_SEC;
    fat_detach(inode);
out:
    return err;
}

static __s32 vfat_mkdir(struct inode *dir, struct dentry *dentry, __s32 mode)
{
    struct super_block *sb = dir->i_sb;
    struct inode *inode;
    struct fat_slot_info sinfo;
    struct timespec ts;
    int err, cluster;

    ts = CURRENT_TIME_SEC;
    cluster = fat_alloc_new_dir(dir, &ts);
    if (cluster < 0)
    {
        err = cluster;
        goto out;
    }
    err = vfat_add_entry(dir, &dentry->d_name, 1, cluster, &ts, &sinfo);
    if (err)
    {
        goto out_free;
    }
    dir->i_version++;
    inc_nlink(dir);

    inode = fat_build_inode(sb, sinfo.de, sinfo.i_pos);
    brelse(sinfo.bh);
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

    //  dentry->d_time = dentry->d_parent->d_inode->i_version;
    d_instantiate(dentry, inode);

    return 0;

out_free:
    fat_free_clusters(dir, cluster);
out:
    return err;
}

static __s32 vfat_rename(struct inode *old_dir, struct dentry *old_dentry,
                         struct inode *new_dir, struct dentry *new_dentry)
{
    struct buffer_head *dotdot_bh;
    struct msdos_dir_entry *dotdot_de;
    struct inode *old_inode, *new_inode;
    struct fat_slot_info old_sinfo, sinfo;
    struct timespec ts;
    __s64 dotdot_i_pos, new_i_pos;
    int err, is_dir, update_dotdot, corrupt = 0;

    old_sinfo.bh = sinfo.bh = dotdot_bh = NULL;
    old_inode = old_dentry->d_inode;
    new_inode = new_dentry->d_inode;
    err = vfat_find(old_dir, &old_dentry->d_name, &old_sinfo);
    if (err)
    {
        goto out;
    }

    is_dir = S_ISDIR(old_inode->i_mode);
    update_dotdot = (is_dir && old_dir != new_dir);
    if (update_dotdot)
    {
        if (fat_get_dotdot_entry(old_inode, &dotdot_bh, &dotdot_de,
                                 &dotdot_i_pos) < 0)
        {
            err = -EIO;
            goto out;
        }
    }

    ts = CURRENT_TIME_SEC;
    if (new_inode)
    {
        if (is_dir)
        {
            err = fat_dir_empty(new_inode);
            if (err)
            {
                goto out;
            }
        }
        new_i_pos = MSDOS_I(new_inode)->i_pos;
        fat_detach(new_inode);
    }
    else
    {
        err = vfat_add_entry(new_dir, &new_dentry->d_name, is_dir, 0,
                             &ts, &sinfo);
        if (err)
        {
            goto out;
        }
        new_i_pos = sinfo.i_pos;
    }
    new_dir->i_version++;

    fat_detach(old_inode);
    fat_attach(old_inode, new_i_pos);
    if (IS_DIRSYNC(new_dir))
    {
        err = fat_sync_inode(old_inode);
        if (err)
        {
            goto error_inode;
        }
    }
    else
    {
        mark_inode_dirty(old_inode);
    }

    if (update_dotdot)
    {
        int start = MSDOS_I(new_dir)->i_logstart;
        dotdot_de->start = cpu_to_le16(start);
        dotdot_de->starthi = cpu_to_le16(start >> 16);
        mark_buffer_dirty(dotdot_bh);
        if (IS_DIRSYNC(new_dir))
        {
            err = sync_dirty_buffer(dotdot_bh);
            if (err)
            {
                goto error_dotdot;
            }
        }
        drop_nlink(old_dir);
        if (!new_inode)
        {
            inc_nlink(new_dir);
        }
    }

    err = fat_remove_entries(old_dir, &old_sinfo);  /* and releases bh */
    old_sinfo.bh = NULL;
    if (err)
    {
        goto error_dotdot;
    }
    old_dir->i_version++;
    old_dir->i_ctime = old_dir->i_mtime = ts;
    if (IS_DIRSYNC(old_dir))
    {
        (void)fat_sync_inode(old_dir);
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
    brelse(sinfo.bh);
    brelse(dotdot_bh);
    brelse(old_sinfo.bh);

    return err;

error_dotdot:
    /* data cluster is shared, serious corruption */
    corrupt = 1;

    if (update_dotdot)
    {
        int start = MSDOS_I(old_dir)->i_logstart;
        dotdot_de->start = cpu_to_le16(start);
        dotdot_de->starthi = cpu_to_le16(start >> 16);
        mark_buffer_dirty(dotdot_bh);
        corrupt |= sync_dirty_buffer(dotdot_bh);
    }
error_inode:
    fat_detach(old_inode);
    fat_attach(old_inode, old_sinfo.i_pos);
    if (new_inode)
    {
        fat_attach(new_inode, new_i_pos);
        if (corrupt)
        {
            corrupt |= fat_sync_inode(new_inode);
        }
    }
    else
    {
        /*
         * If new entry was not sharing the data cluster, it
         * shouldn't be serious corruption.
         */
        int err2 = fat_remove_entries(new_dir, &sinfo);
        if (corrupt)
        {
            corrupt |= err2;
        }
        sinfo.bh = NULL;
    }
    if (corrupt < 0)
    {
        fat_fs_panic(new_dir->i_sb,
                     "%s: Filesystem corrupted (i_pos %d)",
                     __FUNCTION__, (int)sinfo.i_pos);
    }
    goto out;
}
#endif

struct inode_operations vfat_dir_inode_operations =
{
    .lookup     = vfat_lookup,
#if defined CONFIG_FSYS_FAT_RW
    .create     = vfat_create,
    .unlink     = vfat_unlink,
    .mkdir      = vfat_mkdir,
    .rmdir      = vfat_rmdir,
    .rename     = vfat_rename,
#endif
};