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
#include <string.h>
#include "fs.h"
#include "fatfs.h"
#include "buffer_head.h"
#include "nls.h"
#include "endians.h"
#include "supports.h"
#include "page_pool.h"
#include "errno.h"
#include "fsys_debug.h"

//Modify by Bayden
//是否支持宏杰加密工具的隐藏目录(2078\2028版本)
#define HIDDEN_DIR_SUPPORT_FLAG_1504_5120_7642_2078 1

//Modify by Bayden
//是否支持恒波加密工具的隐藏目录
#define HIDDEN_DIR_SUPPORT_FLAG_5746_1845_5532_0794 1


//Modify by Bayden
//是否支持超级特工U盘加密大师V9.0工具的隐藏目录
#define HIDDEN_DIR_SUPPORT_FLAG_5647_1824_3946_8852    1

//Modify by Bayden
//是否支持加密工具的隐藏目录(目录最后一个字符为.)
#define DOT_DIR_SUPPORT_FLAG_1504_5120_7642_1640    1


inline void fat16_towchar(__wchar_t *dst, const __u8 *src, unsigned int len)
{
#ifdef __BIG_ENDIAN
    while (len--)
    {
        *dst++ = src[0] | (src[1] << 8);
        src += 2;
    }
#else
    memcpy(dst, src, len * 2);
#endif
}

static   __s64 fat_make_i_pos(struct super_block *sb,
                              struct buffer_head *bh,
                              struct msdos_dir_entry *de)
{
    return ((__s64)bh->b_blocknr << MSDOS_SB(sb)->dir_per_block_bits)
           | (de - (struct msdos_dir_entry *)bh->b_data);
}

//static   void fat_dir_readahead(struct inode *dir, int iblock,
//                   int phys)
//{
//  struct super_block *sb = dir->i_sb;
//  struct msdos_sb_info *sbi = MSDOS_SB(sb);
//  struct buffer_head *bh;
//  int sec;
//
//  /* This is not a first sector of cluster, or sec_per_clus == 1 */
//  if ((iblock & (sbi->sec_per_clus - 1)) || sbi->sec_per_clus == 1)
//      return;
//  /* root dir of FAT12/FAT16 */
//  if ((sbi->fat_bits != 32) && (dir->i_ino == MSDOS_ROOT_INO))
//      return;
//
//  bh = sb_find_get_block(sb, phys);
//  if (bh == NULL || !buffer_uptodate(bh)) {
//      for (sec = 0; sec < sbi->sec_per_clus; sec++)
//          sb_breadahead(sb, phys + sec);
//  }
//  brelse(bh);
//}

/* Returns the inode number of the directory entry at offset pos. If bh is
   non-NULL, it is brelse'd before. Pos is incremented. The buffer header is
   returned in bh.
   AV. Most often we do it item-by-item. Makes sense to optimize.
   AV. OK, there we go: if both bh and de are non-NULL we assume that we just
   AV. want the next entry (took one explicit de=NULL in vfat/namei.c).
   AV. It's done in fat_get_entry() (inlined), here the slow case lives.
   AV. Additionally, when we return -1 (i.e. reached the end of directory)
   AV. we make bh NULL.
 */
static int fat__get_entry(struct inode *dir, __s64 *pos,
                          struct buffer_head **bh, struct msdos_dir_entry **de)
{
    struct super_block *sb = dir->i_sb;
    int phys, iblock;
    unsigned long mapped_blocks;
    int err, offset;

next:
    if (*bh)
    {
        brelse(*bh);
    }

    *bh = NULL;
    iblock = *pos >> sb->s_blocksize_bits;
    err = fat_bmap(dir, iblock, &phys, &mapped_blocks);
    if (err || !phys)
    {
        return -1;    /* beyond EOF or error */
    }

    //  fat_dir_readahead(dir, iblock, phys);

    *bh = sb_bread(sb, phys);
    if (*bh == NULL)
    {
        fs_log_error("FAT: Directory bread(block %u) failed\n",
                     (unsigned long)phys);
        /* skip this block */
        *pos = (iblock + 1) << sb->s_blocksize_bits;
        goto next;
    }

    offset = *pos & (sb->s_blocksize - 1);
    *pos += sizeof(struct msdos_dir_entry);
    *de = (struct msdos_dir_entry *)((*bh)->b_data + offset);

    return 0;
}

static   int fat_get_entry(struct inode *dir, __s64 *pos,
                           struct buffer_head **bh,
                           struct msdos_dir_entry **de)
{
    /* Fast stuff first */
    if (*bh && *de &&
        (*de - (struct msdos_dir_entry *)(*bh)->b_data) < MSDOS_SB(dir->i_sb)->dir_per_block - 1)
    {
        *pos += sizeof(struct msdos_dir_entry);
        (*de)++;
        return 0;
    }
    return fat__get_entry(dir, pos, bh, de);
}

/*
 * Convert Unicode 16 to UTF-8, translated Unicode, or ASCII.
 * If uni_xlate is enabled and we can't get a 1:1 conversion, use a
 * colon as an escape character since it is normally invalid on the vfat
 * filesystem. The following four characters are the hexadecimal digits
 * of Unicode value. This lets us do a full dump and restore of Unicode
 * filenames. We could get into some trouble with long Unicode names,
 * but ignore that right now.
 * Ahem... Stack smashing in ring 0 isn't fun. Fixed.
 */
static int uni16_to_x8(unsigned char *ascii, __wchar_t *uni, int uni_xlate,
                       struct nls_table *nls)
{
    __wchar_t *ip, ec;
    unsigned char *op, nc;
    int charlen;
    int k;

    ip = uni;
    op = ascii;

    while (*ip)
    {
        ec = *ip++;
        if ((charlen = nls->uni2char(ec, op, NLS_MAX_CHARSET_SIZE)) > 0)
        {
            op += charlen;
        }
        else
        {
            if (uni_xlate == 1)
            {
                *op = ':';
                for (k = 4; k > 0; k--)
                {
                    nc = ec & 0xF;
                    op[k] = nc > 9  ? nc + ('a' - 10)
                            : nc + '0';
                    ec >>= 4;
                }
                op += 5;
            }
            else
            {
                *op++ = '?';
            }
        }
        /* We have some slack there, so it's OK */
        if (op > ascii + 1024)
        {
            op = ascii + 1024;
            break;
        }
    }
    *op = 0;
    return (op - ascii);
}

static   int fat_short2uni(struct nls_table *t, unsigned char *c, int clen, __wchar_t *uni)
{
    int charlen;

    charlen = t->char2uni(c, clen, uni);
    if (charlen < 0)
    {
        *uni = 0x003f;  /* a question mark */
        charlen = 1;
    }
    return charlen;
}

static   int fat_short2lower_uni(struct nls_table *t, unsigned char *c, int clen, __wchar_t *uni)
{
    int charlen;
    __wchar_t wc;

    charlen = t->char2uni(c, clen, &wc);
    if (charlen < 0)
    {
        *uni = 0x003f;  /* a question mark */
        charlen = 1;
    }
    else if (charlen <= 1)
    {
        unsigned char nc = t->charset2lower[*c];

        if (!nc)
        {
            nc = *c;
        }

        if ((charlen = t->char2uni(&nc, 1, uni)) < 0)
        {
            *uni = 0x003f;  /* a question mark */
            charlen = 1;
        }
    }
    else
    {
        *uni = wc;
    }

    return charlen;
}

static   int fat_shortname2uni(struct nls_table *nls, unsigned char *buf, int buf_size,
                               __wchar_t *uni_buf, unsigned short opt, int lower)
{
    int len = 0;

    if (opt & VFAT_SFN_DISPLAY_LOWER)
    {
        len =  fat_short2lower_uni(nls, buf, buf_size, uni_buf);
    }
    else if (opt & VFAT_SFN_DISPLAY_WIN95)
    {
        len = fat_short2uni(nls, buf, buf_size, uni_buf);
    }
    else if (opt & VFAT_SFN_DISPLAY_WINNT)
    {
        if (lower)
        {
            len = fat_short2lower_uni(nls, buf, buf_size, uni_buf);
        }
        else
        {
            len = fat_short2uni(nls, buf, buf_size, uni_buf);
        }
    }
    else
    {
        len = fat_short2uni(nls, buf, buf_size, uni_buf);
    }

    return len;
}

enum { PARSE_INVALID = 1, PARSE_NOT_LONGNAME, PARSE_EOF, };

/**
 * fat_parse_long - Parse extended directory entry.
 *
 * This function returns zero on success, negative value on error, or one of
 * the following:
 *
 * %PARSE_INVALID - Directory entry is invalid.
 * %PARSE_NOT_LONGNAME - Directory entry does not contain longname.
 * %PARSE_EOF - Directory has no more entries.
 */
static int fat_parse_long(struct inode *dir, __s64 *pos,
                          struct buffer_head **bh, struct msdos_dir_entry **de,
                          __wchar_t **unicode, unsigned char *nr_slots)
{
    struct msdos_dir_slot *ds;
    unsigned char id, slot, slots, alias_checksum;

    if (!*unicode)
    {
        *unicode = (__wchar_t *)__get_free_page(GFP_KERNEL);
        if (!*unicode)
        {
            brelse(*bh);
            return -ENOMEM;
        }
    }
parse_long:
    slots = 0;
    ds = (struct msdos_dir_slot *)*de;
    id = ds->id;
    if (!(id & 0x40))
    {
        return PARSE_INVALID;
    }
    slots = id & ~0x40;
    if (slots > 20 || !slots)   /* ceil(256 * 2 / 26) */
    {
        return PARSE_INVALID;
    }
    *nr_slots = slots;
    alias_checksum = ds->alias_checksum;

    slot = slots;
    while (1)
    {
        int offset;

        slot--;
        offset = slot * 13;
        fat16_towchar(*unicode + offset, ds->name0_4, 5);
        fat16_towchar(*unicode + offset + 5, ds->name5_10, 6);
        fat16_towchar(*unicode + offset + 11, ds->name11_12, 2);

        if (ds->id & 0x40)
        {
            (*unicode)[offset + 13] = 0;
        }
        if (fat_get_entry(dir, pos, bh, de) < 0)
        {
            return PARSE_EOF;
        }
        if (slot == 0)
        {
            break;
        }
        ds = (struct msdos_dir_slot *)*de;
        if (ds->attr != ATTR_EXT)
        {
            return PARSE_NOT_LONGNAME;
        }
        if ((ds->id & ~0x40) != slot)
        {
            goto parse_long;
        }
        if (ds->alias_checksum != alias_checksum)
        {
            goto parse_long;
        }
    }


#if HIDDEN_DIR_SUPPORT_FLAG_1504_5120_7642_2078
    if (unicode && *unicode)
    {
#define HIDDEN_DIR_STR_1504_5120_7642_1640   L"uhjjm,NotDelete..\x0"

        __s32 len;
        __s32 i;
        const wchar_t pw[] = HIDDEN_DIR_STR_1504_5120_7642_1640;

        len = sizeof(HIDDEN_DIR_STR_1504_5120_7642_1640);
#if 0
        for (i = 0 ; i < len / 2 - 2 ; i++)
        {
            if (pw[i] != (*unicode)[i])
            {
                break;
            }
        }
#endif

        //if(i >= len/2-2)
        if (0 == memcmp(*unicode, pw, len - 2))
        {
            for (i = 0 ; i < len / 2 ; i++)
            {
                if (0x002e == (*unicode)[i])
                {
                    memcpy((*unicode) + i, "\x0\x0\xff", sizeof("\x0\x0\xff") - 1);
                    break;
                }
            }
        }
        else
        {
            char *p;
            __s32 len;

#define NULL_STR_NAME_1452_9875_1647_6643 "\x4D\x00\x49\x00\x43\x00\x52\x00\x4F\x00\x53\x00\x4F\x00\x46\x00\
\x54\x00\x2D\x00\x37\x00\x37\x00\x30\x00\x31\x00\x30\x00\x39\x00\
\x31\x00\x2D\x00\x36\x00\x36\x00\x36\x00\x36\x00\x38\x00\x38\x00\
\x38\x00\x2D\x00\x31\x00\x36\x00\x38\x00\x32\x00\x36\x00\x36\x00\
\x38\x00\x38\x00\x2E\x00\x2E\x00\x00\x00\xFF"

#define NULL_STR_NAME_1452_9875_1647_6646 "\x4D\x00\x49\x00\x43\x00\x52\x00\x4F\x00\x53\x00\x4F\x00\x46\x00\
\x54\x00\x2D\x00\x37\x00\x37\x00\x30\x00\x31\x00\x30\x00\x39\x00\
\x31\x00\x2D\x00\x36\x00\x36\x00\x36\x00\x36\x00\x38\x00\x38\x00\
\x38\x00\x2D\x00\x31\x00\x36\x00\x38\x00\x32\x00\x36\x00\x36\x00\
\x38\x00\x38\x00\x00\x00\xff\x00\x00\x00\xff"


#define NULL_STR_NAME_1452_9875_1647_6644 "\x20\x00\x20\x00\x20\x00\x00\x00\xff"
#define NULL_STR_NAME_1452_9875_1647_6645 "\x41\x00\x41\x00\x41\x00\x00\x00\xff"

            p = NULL_STR_NAME_1452_9875_1647_6644;
            len = sizeof(NULL_STR_NAME_1452_9875_1647_6644);

            if (0 == memcmp(*unicode, p, len - 1))
            {
                memcpy(*unicode, NULL_STR_NAME_1452_9875_1647_6645, len - 1);
            }
            else
            {
                p = NULL_STR_NAME_1452_9875_1647_6643;
                len = sizeof(NULL_STR_NAME_1452_9875_1647_6643);
                if (0 == memcmp(*unicode, p, len - 1))
                {
                    memcpy(*unicode, NULL_STR_NAME_1452_9875_1647_6646, len - 1);
                }
            }

        }
    }
#endif


#if HIDDEN_DIR_SUPPORT_FLAG_5746_1845_5532_0794
    if (unicode && *unicode)
    {
        char *p;
        __s32 len;

#define NULL_STR_NAME_5746_1845_5532_0794 "\x47\xe3\x20\x00\x2e\x00\x00\x00\xff"
#define NULL_STR_NAME_5746_1845_5532_0795 "\x61\x00\x61\x00\x61\x00\x00\x00\xff"

        p = NULL_STR_NAME_5746_1845_5532_0794;
        len = sizeof(NULL_STR_NAME_5746_1845_5532_0794);

        if (0 == memcmp(*unicode, p, len - 1))
        {
            memcpy(*unicode, NULL_STR_NAME_5746_1845_5532_0795, len - 1);
        }
    }
#endif


#if    HIDDEN_DIR_SUPPORT_FLAG_5647_1824_3946_8852
    if (unicode && *unicode)
    {
        __wchar_t *pw;

        pw = *unicode;

        //if(0x002e != *pw)//正常的当前文件夹和当前目录
        {
            while (0 != *pw)
            {
                pw++;
            }

            pw -= 2;

            if (pw == *unicode)
            {
                if (0x002e == *pw
                    && 0x002e == *(pw + 1))
                {
                    *pw = 0x41;
                    pw++;
                    *pw = 0x41;
                }
            }
        }
    }

    if (unicode && *unicode)
    {
        char *p;
        __s32 len;

#define NULL_STR_NAME_5746_1845_5532_1747 "\x45\x00\x54\x00\x7f\x00\x7f\x00\x2e\x00\x00\x00\xff"
#define NULL_STR_NAME_5746_1845_5532_4455 "\x45\x00\x54\x00\x7f\x00\x7f\x00\x41\x00\x00\x00\xff"

        p = NULL_STR_NAME_5746_1845_5532_1747;
        len = sizeof(NULL_STR_NAME_5746_1845_5532_1747);

        if (0 == memcmp(*unicode, p, len - 1))
        {
            memcpy(*unicode, NULL_STR_NAME_5746_1845_5532_4455, len - 1);
            goto end;
        }
    }

#endif

#if DOT_DIR_SUPPORT_FLAG_1504_5120_7642_1640
    if (unicode && *unicode)
    {
        __wchar_t *pw;

        pw = *unicode;

        //if(0x002e != *pw)//正常的当前文件夹和当前目录
        {
            while (0 != *pw)
            {
                pw++;
            }

            *(char *)(pw + 1) = 0xff;
            pw--;

            while (pw > *unicode)
            {
                if (0x002e == *pw)
                {
                    *pw = 0x0;
                    *(pw + 1) = 0xff;
                }
                else
                {
                    break;
                }

                pw--;
            }
        }
    }
#endif

end:

    if ((*de)->name[0] == DELETED_FLAG)
    {
        return PARSE_INVALID;
    }
    if ((*de)->attr == ATTR_EXT)
    {
        goto parse_long;
    }
    if (IS_FREE((*de)->name) || ((*de)->attr & ATTR_VOLUME))
    {
        return PARSE_INVALID;
    }
    if (fat_checksum((*de)->name) != alias_checksum)
    {
        *nr_slots = 0;
    }

    return 0;
}

/*
 * Return values: negative -> error, 0 -> not found, positive -> found,
 * value is the total amount of slots, including the shortname entry.
 */
int fat_search_long(struct inode *inode, const unsigned char *name,
                    int name_len, struct fat_slot_info *sinfo)
{
    struct super_block *sb = inode->i_sb;
    struct msdos_sb_info *sbi = MSDOS_SB(sb);
    struct buffer_head *bh = NULL;
    struct msdos_dir_entry *de;
    struct nls_table *nls_io = nls; //sbi->nls_io;
    struct nls_table *nls_disk = nls; //sbi->nls_disk;
    static __wchar_t bufuname[14];
    unsigned int  xlate_len;
    unsigned char nr_slots;
    __wchar_t *unicode = NULL;
    static unsigned char work[8], bufname[260]; /* 256 + 4 */
    int uni_xlate = sbi->options.unicode_xlate;
    int utf8 = sbi->options.utf8;
    int anycase = (sbi->options.name_check != 's');
    unsigned short opt_shortname = sbi->options.shortname;
    __s64 cpos = 0;
    int chl, i, j, last_u, err;

    err = -ENOENT;
    while (1)
    {
        if (fat_get_entry(inode, &cpos, &bh, &de) == -1)
        {
            goto EODir;
        }
parse_record:
        nr_slots = 0;
        if (de->name[0] == DELETED_FLAG)
        {
            continue;
        }
        if (de->attr != ATTR_EXT && (de->attr & ATTR_VOLUME))
        {
            continue;
        }
        if (de->attr != ATTR_EXT && IS_FREE(de->name))
        {
            continue;
        }
        if (de->attr == ATTR_EXT)
        {
            int status = fat_parse_long(inode, &cpos, &bh, &de,
                                        &unicode, &nr_slots);
            if (status < 0)
            {
                return status;
            }
            else if (status == PARSE_INVALID)
            {
                continue;
            }
            else if (status == PARSE_NOT_LONGNAME)
            {
                goto parse_record;
            }
            else if (status == PARSE_EOF)
            {
                goto EODir;
            }
        }

        memcpy(work, de->name, sizeof(de->name));
        /* see namei.c, msdos_format_name */
        if (work[0] == 0x05)
        {
            work[0] = 0xE5;
        }
        for (i = 0, j = 0, last_u = 0; i < 8;)
        {
            if (!work[i])
            {
                break;
            }
            chl = fat_shortname2uni(nls_disk, &work[i], 8 - i,
                                    &bufuname[j++], opt_shortname,
                                    de->lcase & CASE_LOWER_BASE);
            if (chl <= 1)
            {
                if (work[i] != ' ')
                {
                    last_u = j;
                }
            }
            else
            {
                last_u = j;
            }
            i += chl;
        }
        j = last_u;
        fat_short2uni(nls_disk, ".", 1, &bufuname[j++]);
        for (i = 0; i < 3;)
        {
            if (!de->ext[i])
            {
                break;
            }
            chl = fat_shortname2uni(nls_disk, &de->ext[i], 3 - i,
                                    &bufuname[j++], opt_shortname,
                                    de->lcase & CASE_LOWER_EXT);
            if (chl <= 1)
            {
                if (de->ext[i] != ' ')
                {
                    last_u = j;
                }
            }
            else
            {
                last_u = j;
            }
            i += chl;
        }
        if (!last_u)
        {
            continue;
        }

        bufuname[last_u] = 0x0000;
        xlate_len = utf8
                    ? utf8_wcstombs(bufname, bufuname, sizeof(bufname))
                    : uni16_to_x8(bufname, bufuname, uni_xlate, nls_io);
        if (xlate_len == name_len)
            if ((!anycase && !memcmp(name, bufname, xlate_len)) ||
                (anycase && !nls_strnicmp(nls_io, name, bufname,
                                          xlate_len)))
            {
                goto Found;
            }

        if (nr_slots)
        {

            /* unicode buffer allocate from page pool */
            void *long_name = unicode + 261;
            int buffer_size = PAGE_SIZE - (261 * sizeof(unicode[0]));

            xlate_len = utf8
                        ? utf8_wcstombs(long_name, unicode, buffer_size)
                        : uni16_to_x8(long_name, unicode, uni_xlate, nls_io);
            if (xlate_len != name_len)
            {
                continue;
            }
            if ((!anycase && !memcmp(name, long_name, xlate_len)) ||
                (anycase && !nls_strnicmp(nls_io, name, long_name,
                                          xlate_len)))
            {
                goto Found;
            }
        }
    }

Found:
    nr_slots++; /* include the de */
    sinfo->slot_off = cpos - nr_slots * sizeof(*de);
    sinfo->nr_slots = nr_slots;
    sinfo->de = de;
    sinfo->bh = bh;
    sinfo->i_pos = fat_make_i_pos(sb, sinfo->bh, sinfo->de);
    err = 0;
EODir:
    if (unicode)
    {
        free_page((unsigned long)unicode);
    }

    return err;
}

struct fat_ioctl_filldir_callback
{
    void *dirent;
    int result;
    /* for dir ioctl */
    const char *longname;
    int long_len;
    const char *shortname;
    int short_len;
};

static int __fat_readdir(struct inode *inode, struct file *filp, void *dirent,
                         filldir_t filldir, int short_only, int both)
{
    struct super_block *sb = inode->i_sb;
    struct msdos_sb_info *sbi = MSDOS_SB(sb);
    struct buffer_head *bh;
    struct msdos_dir_entry *de;
    struct nls_table *nls_io = nls; //sbi->nls_io;
    struct nls_table *nls_disk = nls; //sbi->nls_disk;
    unsigned char long_slots;
    const char *fill_name;
    int fill_len;
    static __wchar_t bufuname[14];
    __wchar_t *unicode = NULL;
    static unsigned char work[8], bufname[56];
    unsigned char c, *ptname = bufname;
    int uni_xlate = sbi->options.unicode_xlate;
    int isvfat = sbi->options.isvfat;
    int utf8 = sbi->options.utf8;
    int nocase = sbi->options.nocase;
    unsigned short opt_shortname = sbi->options.shortname;
    unsigned long inum = 0;
    int chi, chl, i, i2, j, last, last_u, dotoffset = 0;
    __s64 cpos/* current pos */, lpos /* last pos */;
    int ret = 0;

    cpos = filp->f_pos;

    bh = NULL;
GetNew:
    if (fat_get_entry(inode, &cpos, &bh, &de) == -1)
    {
        goto EODir;
    }
    /* check encount end of entry or not first,
     * Microsoft FAT spec means : there are no allocated directory
     * entries after this one, but linux kernal not follow by this.
     * By Sunny. 2010.2.1
     */
    if (de->name[0] == EOD_FLAG)
    {
        cpos = inode->i_size;   /* end of directory */
        goto EODir;
    }
parse_record:
    long_slots = 0;
    /* Check for long filename entry */
    if (isvfat)
    {
        if (de->name[0] == DELETED_FLAG)
        {
            goto RecEnd;
        }
        if (de->attr != ATTR_EXT && (de->attr & ATTR_VOLUME))
        {
            goto RecEnd;
        }
        if (de->attr != ATTR_EXT && IS_FREE(de->name))
        {
            goto RecEnd;
        }
    }
    else
    {
        if ((de->attr & ATTR_VOLUME) || IS_FREE(de->name))
        {
            goto RecEnd;
        }
    }

    if (isvfat && de->attr == ATTR_EXT)
    {
        int status = fat_parse_long(inode, &cpos, &bh, &de,
                                    &unicode, &long_slots);
        if (status < 0)
        {
            filp->f_pos = cpos;
            ret = status;
            goto out;
        }
        else if (status == PARSE_INVALID)
        {
            goto RecEnd;
        }
        else if (status == PARSE_NOT_LONGNAME)
        {
            goto parse_record;
        }
        else if (status == PARSE_EOF)
        {
            goto EODir;
        }
    }

    if (sbi->options.dotsOK)
    {
        ptname = bufname;
        dotoffset = 0;
        if (de->attr & ATTR_HIDDEN)
        {
            *ptname++ = '.';
            dotoffset = 1;
        }
    }

    memcpy(work, de->name, sizeof(de->name));
    /* see namei.c, msdos_format_name */
    if (work[0] == 0x05)
    {
        work[0] = 0xE5;
    }
    for (i = 0, j = 0, last = 0, last_u = 0; i < 8;)
    {
        if (!(c = work[i]))
        {
            break;
        }
        chl = fat_shortname2uni(nls_disk, &work[i], 8 - i,
                                &bufuname[j++], opt_shortname,
                                de->lcase & CASE_LOWER_BASE);
        if (chl <= 1)
        {
            ptname[i++] = (!nocase && c >= 'A' && c <= 'Z') ? c + 32 : c;
            if (c != ' ')
            {
                last = i;
                last_u = j;
            }
        }
        else
        {
            last_u = j;
            for (chi = 0; chi < chl && i < 8; chi++)
            {
                ptname[i] = work[i];
                i++;
                last = i;
            }
        }
    }
    i = last;
    j = last_u;
    fat_short2uni(nls_disk, ".", 1, &bufuname[j++]);
    ptname[i++] = '.';
    for (i2 = 0; i2 < 3;)
    {
        if (!(c = de->ext[i2]))
        {
            break;
        }
        chl = fat_shortname2uni(nls_disk, &de->ext[i2], 3 - i2,
                                &bufuname[j++], opt_shortname,
                                de->lcase & CASE_LOWER_EXT);
        if (chl <= 1)
        {
            i2++;
            ptname[i++] = (!nocase && c >= 'A' && c <= 'Z') ? c + 32 : c;
            if (c != ' ')
            {
                last = i;
                last_u = j;
            }
        }
        else
        {
            last_u = j;
            for (chi = 0; chi < chl && i2 < 3; chi++)
            {
                ptname[i++] = de->ext[i2++];
                last = i;
            }
        }
    }
    if (!last)
    {
        goto RecEnd;
    }

    i = last + dotoffset;
    j = last_u;

    lpos = cpos - (long_slots + 1) * sizeof(struct msdos_dir_entry);
    if (!memcmp(de->name, MSDOS_DOT, MSDOS_NAME))
    {
        inum = inode->i_ino;
    }
    else if (!memcmp(de->name, MSDOS_DOTDOT, MSDOS_NAME))
    {
        ;//inum = parent_ino(filp->f_path.dentry);
    }
    else
    {
        __s64 i_pos = fat_make_i_pos(sb, bh, de);
        struct inode *tmp = fat_iget(sb, i_pos);
        if (tmp)
        {
            inum = tmp->i_ino;
            iput(tmp);
        }
        else
        {
            inum = iunique(sb, MSDOS_ROOT_INO);
        }
    }

    if (isvfat)
    {
        bufuname[j] = 0x0000;
        i = utf8 ? utf8_wcstombs(bufname, bufuname, sizeof(bufname))
            : uni16_to_x8(bufname, bufuname, uni_xlate, nls_io);
    }

    fill_name = bufname;
    fill_len = i;
    if (!short_only && long_slots)
    {
        /* convert the unicode long name. 261 is maximum size
         * of unicode buffer. (13 * slots + nul) */
        void *longname = unicode + 261;
        int buf_size = PAGE_SIZE - (261 * sizeof(unicode[0]));
        int long_len = utf8
                       ? utf8_wcstombs(longname, unicode, buf_size)
                       : uni16_to_x8(longname, unicode, uni_xlate, nls_io);
#if 0
        fs_log_info("name string:%s. ansi && uni code below:\n", longname);
        __dump_mem(longname, 0x0, 16);
        __dump_mem(unicode, 0x0, 16);
#endif
        if (!both)
        {
            fill_name = longname;
            fill_len = long_len;
        }
        else
        {
            /* hack for fat_ioctl_filldir() */
            struct fat_ioctl_filldir_callback *p = dirent;

            p->longname = longname;
            p->long_len = long_len;
            p->shortname = bufname;
            p->short_len = i;
            fill_name = NULL;
            fill_len = 0;
        }
    }

    if (filldir(dirent, fill_name, fill_len + 1, 0, lpos, inum,
                de->attr, (__u64)de->size))
    {
        goto NoEnoughFillBuff;
    }

RecEnd:
    filp->f_pos = cpos;
    goto GetNew;

EODir:
    filp->f_pos = cpos;
NoEnoughFillBuff:
    brelse(bh);
    if (unicode)
    {
        free_page((unsigned long)unicode);
    }
out:
    return ret;
}

/**
  *return value:
  *     1: fill buffer overflow
  *     0: end of dir
  *    <0: ENOMEM error or other fatal error, need stop to go on scaning the directory
  */
static __s32 fat_readdir(struct file *filp, void *dirent, filldir_t filldir)
{
    struct inode *inode = filp->f_dentry->d_inode;
    return __fat_readdir(inode, filp, dirent, filldir, 0, 0);
}

static int fat_ioctl_filldir(void *__buf, const char *name, int name_len,
                             __s64 offset, __u64 ino, unsigned int d_type)
{
    //  struct fat_ioctl_filldir_callback *buf = __buf;
    //  struct dirent *d1 = buf->dirent;
    //  struct dirent *d2 = d1 + 1;
    //
    //  if (buf->result)
    //      return -EINVAL;
    //  buf->result++;
    //
    //  if (name != NULL) {
    //      /* dirent has only short name */
    //      if (name_len >= sizeof(d1->d_name))
    //          name_len = sizeof(d1->d_name) - 1;
    //
    //      if (put_user(0, d2->d_name)         ||
    //          put_user(0, &d2->d_reclen)          ||
    //          copy_to_user(d1->d_name, name, name_len)    ||
    //          put_user(0, d1->d_name + name_len)      ||
    //          put_user(name_len, &d1->d_reclen))
    //          goto efault;
    //  } else {
    //      /* dirent has short and long name */
    //      const char *longname = buf->longname;
    //      int long_len = buf->long_len;
    //      const char *shortname = buf->shortname;
    //      int short_len = buf->short_len;
    //
    //      if (long_len >= sizeof(d1->d_name))
    //          long_len = sizeof(d1->d_name) - 1;
    //      if (short_len >= sizeof(d1->d_name))
    //          short_len = sizeof(d1->d_name) - 1;
    //
    //      if (copy_to_user(d2->d_name, longname, long_len)    ||
    //          put_user(0, d2->d_name + long_len)          ||
    //          put_user(long_len, &d2->d_reclen)           ||
    //          put_user(ino, &d2->d_ino)               ||
    //          put_user(offset, &d2->d_off)            ||
    //          copy_to_user(d1->d_name, shortname, short_len)  ||
    //          put_user(0, d1->d_name + short_len)         ||
    //          put_user(short_len, &d1->d_reclen))
    //          goto efault;
    //  }
    //  return 0;
    //efault:
    //  buf->result = -EFAULT;
    return -EFAULT;
}

static int fat_ioctl_readdir(struct inode *inode, struct file *filp,
                             void *dirent, filldir_t filldir,
                             int short_only, int both)
{
    struct fat_ioctl_filldir_callback buf;
    int ret;

    buf.dirent = dirent;
    buf.result = 0;
    ret = -ENOENT;
    if (!IS_DEADDIR(inode))
    {
        ret = __fat_readdir(inode, filp, &buf, filldir,
                            short_only, both);
    }
    if (ret >= 0)
    {
        ret = buf.result;
    }
    return ret;
}

static __s32 fat_dir_ioctl(struct inode *inode, struct file *filp,
                           __u32 cmd, __u32 aux, void *buffer)
{
    struct dirent *d1 = (struct dirent *)buffer;
    int short_only, both;

    switch (cmd)
    {
        case VFAT_IOCTL_READDIR_SHORT:
            short_only = 1;
            both = 0;
            break;
        case VFAT_IOCTL_READDIR_BOTH:
            short_only = 0;
            both = 1;
            break;
        default:
            return fat_generic_ioctl(inode, filp, cmd, aux, buffer);
    }

    //  if (!access_ok(VERIFY_WRITE, d1, sizeof(struct dirent[2])))
    //      return -EFAULT;
    /*
     * Yes, we don't need this put_user() absolutely. However old
     * code didn't return the right value. So, app use this value,
     * in order to check whether it is EOF.
     */
    //  if (put_user(0, &d1->d_reclen))
    return -EFAULT;

    //  return fat_ioctl_readdir(inode, filp, d1, fat_ioctl_filldir,
    //               short_only, both);
}

//#ifdef CONFIG_COMPAT
//#define   VFAT_IOCTL_READDIR_BOTH32   _IOR('r', 1, struct compat_dirent[2])
//#define   VFAT_IOCTL_READDIR_SHORT32  _IOR('r', 2, struct compat_dirent[2])
//
//FAT_IOCTL_FILLDIR_FUNC(fat_compat_ioctl_filldir, compat_dirent);
//
//static long fat_compat_dir_ioctl(struct file *filp, unsigned cmd,
//               unsigned long arg)
//{
//  struct inode *inode = filp->f_path.dentry->d_inode;
//  struct compat_dirent *d1 = compat_ptr(arg);
//  int short_only, both;
//
//  switch (cmd) {
//  case VFAT_IOCTL_READDIR_SHORT32:
//      short_only = 1;
//      both = 0;
//      break;
//  case VFAT_IOCTL_READDIR_BOTH32:
//      short_only = 0;
//      both = 1;
//      break;
//  default:
//      return -ENOIOCTLCMD;
//  }
//
//  if (!access_ok(VERIFY_WRITE, d1, sizeof(struct compat_dirent[2])))
//      return -EFAULT;
//  /*
//   * Yes, we don't need this put_user() absolutely. However old
//   * code didn't return the right value. So, app use this value,
//   * in order to check whether it is EOF.
//   */
//  if (put_user(0, &d1->d_reclen))
//      return -EFAULT;
//
//  return fat_ioctl_readdir(inode, filp, d1, fat_compat_ioctl_filldir,
//               short_only, both);
//}
//#endif /* CONFIG_COMPAT */

const struct file_operations fat_dir_operations =
{
    //  .read       = generic_read_dir,
    .readdir    = fat_readdir,
    .ioctl      = fat_dir_ioctl,
#if defined CONFIG_FSYS_FAT_RW
    .fsync      = file_fsync,
#endif
};

static int fat_get_short_entry(struct inode *dir, __s64 *pos,
                               struct buffer_head **bh,
                               struct msdos_dir_entry **de)
{
    while (fat_get_entry(dir, pos, bh, de) >= 0)
    {
        /* free entry or long name entry or volume label */
        if (!IS_FREE((*de)->name) && !((*de)->attr & ATTR_VOLUME))
        {
            return 0;
        }
    }
    return -ENOENT;
}

/*
 * fat_subdirs counts the number of sub-directories of dir. It can be run
 * on directories being created.
 */
int fat_subdirs(struct inode *dir)
{
    struct buffer_head *bh;
    struct msdos_dir_entry *de;
    __s64 cpos;
    int count = 0;

    bh = NULL;
    cpos = 0;
    while (fat_get_short_entry(dir, &cpos, &bh, &de) >= 0)
    {
        if (de->attr & ATTR_DIR)
        {
            count++;
        }
    }
    brelse(bh);
    return count;
}



//没找到，分配一个

int fat_get_label(struct super_block *sb)
{
    struct inode *dir = sb->s_root->d_inode;
    struct buffer_head *bh;
    struct msdos_dir_entry *de;
    __s64  cpos;

    bh = NULL;
    cpos = 0;

    while (fat_get_entry(dir, &cpos, &bh, &de) >= 0)
    {
        if (!IS_FREE(de->name) && de->attr == ATTR_VOLUME)
        {
            strncpy(sb->s_volname, de->name, 11);
            brelse(bh);
            return 1;
        }
    }

    brelse(bh);
    return 0;
}

#if defined CONFIG_FSYS_FAT_RW
inline void fatwchar_to16(__u8 *dst, const __wchar_t *src, unsigned int len)
{
#ifdef __BIG_ENDIAN
    while (len--)
    {
        dst[0] = *src & 0x00FF;
        dst[1] = (*src & 0xFF00) >> 8;
        dst += 2;
        src++;
    }
#else
    memcpy(dst, src, len * 2);
#endif
}

/*
 * Scans a directory for a given file (name points to its formatted name).
 * Returns an error code or zero.
 */
int fat_scan(struct inode *dir, const unsigned char *name,
             struct fat_slot_info *sinfo)
{
    struct super_block *sb = dir->i_sb;

    sinfo->slot_off = 0;
    sinfo->bh = NULL;
    while (fat_get_short_entry(dir, &sinfo->slot_off, &sinfo->bh,
                               &sinfo->de) >= 0)
    {
        if (!strncmp(sinfo->de->name, name, MSDOS_NAME))
        {
            sinfo->slot_off -= sizeof(*sinfo->de);
            sinfo->nr_slots = 1;
            sinfo->i_pos = fat_make_i_pos(sb, sinfo->bh, sinfo->de);
            return 0;
        }
    }
    return -ENOENT;
}

/* See if directory is empty */
int fat_dir_empty(struct inode *dir)
{
    struct buffer_head *bh;
    struct msdos_dir_entry *de;
    __s64 cpos;
    int result = 0;

    bh = NULL;
    cpos = 0;
    while (fat_get_short_entry(dir, &cpos, &bh, &de) >= 0)
    {
        if (strncmp(de->name, MSDOS_DOT, MSDOS_NAME) &&
            strncmp(de->name, MSDOS_DOTDOT, MSDOS_NAME))
        {
            result = -ENOTEMPTY;
            break;
        }
    }
    brelse(bh);
    return result;
}

/*
 * The ".." entry can not provide the "struct fat_slot_info" informations
 * for inode. So, this function provide the some informations only.
 */
int fat_get_dotdot_entry(struct inode *dir, struct buffer_head **bh,
                         struct msdos_dir_entry **de, __s64 *i_pos)
{
    __s64 offset;

    offset = 0;
    *bh = NULL;
    while (fat_get_short_entry(dir, &offset, bh, de) >= 0)
    {
        if (!strncmp((*de)->name, MSDOS_DOTDOT, MSDOS_NAME))
        {
            *i_pos = fat_make_i_pos(dir->i_sb, *bh, *de);
            return 0;
        }
    }
    return -ENOENT;
}

static int __fat_remove_entries(struct inode *dir, __s64 pos, int nr_slots)
{
    struct super_block *sb = dir->i_sb;
    struct buffer_head *bh;
    struct msdos_dir_entry *de, *endp;
    int err = 0, orig_slots;

    while (nr_slots)
    {
        bh = NULL;
        if (fat_get_entry(dir, &pos, &bh, &de) < 0)
        {
            err = -EIO;
            break;
        }

        orig_slots = nr_slots;
        endp = (struct msdos_dir_entry *)(bh->b_data + sb->s_blocksize);
        while (nr_slots && de < endp)
        {
            de->name[0] = DELETED_FLAG;
            de++;
            nr_slots--;
        }
        mark_buffer_dirty(bh);
        if (IS_DIRSYNC(dir))
        {
            err = sync_dirty_buffer(bh);
        }
        brelse(bh);
        if (err)
        {
            break;
        }

        /* pos is *next* de's position, so this does `- sizeof(de)' */
        pos += ((orig_slots - nr_slots) * sizeof(*de)) - sizeof(*de);
    }

    return err;
}

int fat_remove_entries(struct inode *dir, struct fat_slot_info *sinfo)
{
    struct msdos_inode_info *mi = MSDOS_I(dir);
    struct msdos_dir_entry *de;
    struct buffer_head *bh;
    int err = 0, nr_slots;

    if (mi->dirent_search_start > sinfo->slot_off)
    {
        mi->dirent_search_start = sinfo->slot_off;
    }

    /*
     * First stage: Remove the shortname. By this, the directory
     * entry is removed.
     */
    nr_slots = sinfo->nr_slots;
    de = sinfo->de;
    sinfo->de = NULL;
    bh = sinfo->bh;
    sinfo->bh = NULL;
    while (nr_slots && de >= (struct msdos_dir_entry *)bh->b_data)
    {
        de->name[0] = DELETED_FLAG;
        de--;
        nr_slots--;
    }
    mark_buffer_dirty(bh);
    if (IS_DIRSYNC(dir))
    {
        err = sync_dirty_buffer(bh);
    }
    brelse(bh);
    if (err)
    {
        return err;
    }
    dir->i_version++;

    if (nr_slots)
    {
        /*
         * Second stage: remove the remaining longname slots.
         * (This directory entry is already removed, and so return
         * the success)
         */
        err = __fat_remove_entries(dir, sinfo->slot_off, nr_slots);
        if (err)
        {
            fs_log_warning("FAT: Couldn't remove the long name slots\n");
        }
    }

    dir->i_mtime = dir->i_atime = CURRENT_TIME_SEC;
    if (IS_DIRSYNC(dir))
    {
        (void)fat_sync_inode(dir);
    }
    else
    {
        mark_inode_dirty(dir);
    }

    return 0;
}

static int fat_zeroed_cluster(struct inode *dir, int blknr, int nr_used,
                              struct buffer_head **bhs, int nr_bhs)
{
    struct super_block *sb = dir->i_sb;
    int last_blknr = blknr + MSDOS_SB(sb)->sec_per_clus;
    int err, i, n;

    /* Zeroing the unused blocks on this cluster */
    blknr += nr_used;
    n = nr_used;
    while (blknr < last_blknr)
    {
        bhs[n] = sb_getblk(sb, blknr);
        if (!bhs[n])
        {
            err = -ENOMEM;
            goto error;
        }
        memset(bhs[n]->b_data, 0, sb->s_blocksize);
        set_buffer_uptodate(bhs[n]);
        mark_buffer_dirty(bhs[n]);

        n++;
        blknr++;
        if (n == nr_bhs)
        {
            if (IS_DIRSYNC(dir))
            {
                err = fat_sync_bhs(bhs, n);
                if (err)
                {
                    goto error;
                }
            }
            for (i = 0; i < n; i++)
            {
                brelse(bhs[i]);
            }
            n = 0;
        }
    }
    if (IS_DIRSYNC(dir))
    {
        err = fat_sync_bhs(bhs, n);
        if (err)
        {
            goto error;
        }
    }
    for (i = 0; i < n; i++)
    {
        brelse(bhs[i]);
    }

    return 0;

error:
    for (i = 0; i < n; i++)
    {
        bforget(bhs[i]);
    }
    return err;
}

int fat_alloc_new_dir(struct inode *dir, struct timespec *ts)
{
    struct super_block *sb = dir->i_sb;
    struct msdos_sb_info *sbi = MSDOS_SB(sb);
    struct buffer_head *bhs[MAX_BUF_PER_PAGE];
    struct msdos_dir_entry *de;
    int blknr;
    __s16 date, time;
    int err, cluster;

    err = fat_alloc_clusters(dir, &cluster, 1);
    if (err)
    {
        goto error;
    }

    blknr = fat_clus_to_blknr(sbi, cluster);
    bhs[0] = sb_getblk(sb, blknr);
    if (!bhs[0])
    {
        err = -ENOMEM;
        goto error_free;
    }

    fat_date_unix2dos(ts->tv_sec, &time, &date);

    de = (struct msdos_dir_entry *)bhs[0]->b_data;
    /* filling the new directory slots ("." and ".." entries) */
    memcpy(de[0].name, MSDOS_DOT, MSDOS_NAME);
    memcpy(de[1].name, MSDOS_DOTDOT, MSDOS_NAME);
    de->attr = de[1].attr = ATTR_DIR;
    de[0].lcase = de[1].lcase = 0;
    de[0].time = de[1].time = time;
    de[0].date = de[1].date = date;
    de[0].ctime_cs = de[1].ctime_cs = 0;
    if (sbi->options.isvfat)
    {
        /* extra timestamps */
        de[0].ctime = de[1].ctime = time;
        de[0].adate = de[0].cdate = de[1].adate = de[1].cdate = date;
    }
    else
    {
        de[0].ctime = de[1].ctime = 0;
        de[0].adate = de[0].cdate = de[1].adate = de[1].cdate = 0;
    }
    de[0].start = cpu_to_le16(cluster);
    de[0].starthi = cpu_to_le16(cluster >> 16);
    de[1].start = cpu_to_le16(MSDOS_I(dir)->i_logstart);
    de[1].starthi = cpu_to_le16(MSDOS_I(dir)->i_logstart >> 16);
    de[0].size = de[1].size = 0;
    memset(de + 2, 0, sb->s_blocksize - 2 * sizeof(*de));
    set_buffer_uptodate(bhs[0]);
    mark_buffer_dirty(bhs[0]);

    err = fat_zeroed_cluster(dir, blknr, 1, bhs, MAX_BUF_PER_PAGE);
    if (err)
    {
        goto error_free;
    }

    return cluster;

error_free:
    fat_free_clusters(dir, cluster);
error:
    return err;
}

static int fat_add_new_entries(struct inode *dir, void *slots, int nr_slots,
                               int *nr_cluster, struct msdos_dir_entry **de,
                               struct buffer_head **bh, __s64 *i_pos)
{
    struct super_block *sb = dir->i_sb;
    struct msdos_sb_info *sbi = MSDOS_SB(sb);
    struct buffer_head *bhs[MAX_BUF_PER_PAGE];
    int blknr, start_blknr, last_blknr;
    unsigned long size, copy;
    int err, i, n, offset, cluster[2];

    /*
     * The minimum cluster size is 512bytes, and maximum entry
     * size is 32*slots (672bytes).  So, iff the cluster size is
     * 512bytes, we may need two clusters.
     */
    size = nr_slots * sizeof(struct msdos_dir_entry);
    *nr_cluster = (size + (sbi->cluster_size - 1)) >> sbi->cluster_bits;
    BUG_ON(*nr_cluster > 2);

    err = fat_alloc_clusters(dir, cluster, *nr_cluster);
    if (err)
    {
        goto error;
    }

    /*
     * First stage: Fill the directory entry.  NOTE: This cluster
     * is not referenced from any inode yet, so updates order is
     * not important.
     */
    i = n = copy = 0;
    do
    {
        start_blknr = blknr = fat_clus_to_blknr(sbi, cluster[i]);
        last_blknr = start_blknr + sbi->sec_per_clus;
        while (blknr < last_blknr)
        {
            bhs[n] = sb_getblk(sb, blknr);
            if (!bhs[n])
            {
                err = -ENOMEM;
                goto error_nomem;
            }

            /* fill the directory entry */
            copy = min(size, sb->s_blocksize);
            memcpy(bhs[n]->b_data, slots, copy);
            slots = (void *)((char *)slots + copy);
            size -= copy;
            set_buffer_uptodate(bhs[n]);
            mark_buffer_dirty(bhs[n]);
            if (!size)
            {
                break;
            }
            n++;
            blknr++;
        }
    } while (++i < *nr_cluster);

    memset(bhs[n]->b_data + copy, 0, sb->s_blocksize - copy);
    offset = copy - sizeof(struct msdos_dir_entry);
    get_bh(bhs[n]);
    *bh = bhs[n];
    *de = (struct msdos_dir_entry *)((*bh)->b_data + offset);
    *i_pos = fat_make_i_pos(sb, *bh, *de);

    /* Second stage: clear the rest of cluster, and write outs */
    err = fat_zeroed_cluster(dir, start_blknr, ++n, bhs, MAX_BUF_PER_PAGE);
    if (err)
    {
        goto error_free;
    }

    return cluster[0];

error_free:
    brelse(*bh);
    *bh = NULL;
    n = 0;
error_nomem:
    for (i = 0; i < n; i++)
    {
        bforget(bhs[i]);
    }
    fat_free_clusters(dir, cluster[0]);
error:
    return err;
}

int fat_add_entries(struct inode *dir, void *slots, int nr_slots,
                    struct fat_slot_info *sinfo)
{
    struct msdos_inode_info *mi = MSDOS_I(dir);
    struct super_block *sb = dir->i_sb;
    struct msdos_sb_info *sbi = MSDOS_SB(sb);
    struct buffer_head *bh, *prev, *bhs[3]; /* 32*slots (672bytes) */
    struct msdos_dir_entry *de;
    int err, free_slots, i, nr_bhs;
    __s64 pos, i_pos;
    int first_search = 1;

    sinfo->nr_slots = nr_slots;

    /* First stage: search free direcotry entries */
    free_slots = nr_bhs = 0;
    bh = prev = NULL;
    //  pos = 0;
    pos = mi->dirent_search_start;
    err = -ENOSPC;
    while (fat_get_entry(dir, &pos, &bh, &de) > -1)
    {
        /* check the maximum size of directory */
        if (pos >= FAT_MAX_DIR_SIZE)
        {
            goto error;
        }

        if (IS_FREE(de->name))
        {
            if (first_search)
            {
                first_search = 0;
                mi->dirent_search_start = pos - sizeof(struct msdos_dir_entry);
            }
            if (prev != bh)
            {
                get_bh(bh);
                bhs[nr_bhs] = prev = bh;
                nr_bhs++;
            }
            free_slots++;
            if (free_slots == nr_slots)
            {
                goto found;
            }
        }
        else
        {
            for (i = 0; i < nr_bhs; i++)
            {
                brelse(bhs[i]);
            }
            prev = NULL;
            free_slots = nr_bhs = 0;
        }
    }
    if (dir->i_ino == MSDOS_ROOT_INO)
    {
        if (sbi->fat_bits != 32)
        {
            goto error;
        }
    }
    else if (MSDOS_I(dir)->i_start == 0)
    {
        fs_log_error("FAT: Corrupted directory (i_pos %d)\n",
                     (int)MSDOS_I(dir)->i_pos);
        err = -EIO;
        goto error;
    }

found:
    err = 0;
    pos -= free_slots * sizeof(*de);
    nr_slots -= free_slots;
    if (free_slots)
    {
        /*
         * Second stage: filling the free entries with new entries.
         * NOTE: If this slots has shortname, first, we write
         * the long name slots, then write the short name.
         */
        int size = free_slots * sizeof(*de);
        int offset = pos & (sb->s_blocksize - 1);
        int long_bhs = nr_bhs - (nr_slots == 0);

        /* Fill the long name slots. */
        for (i = 0; i < long_bhs; i++)
        {
            int copy = min_t(int, sb->s_blocksize - offset, size);
            memcpy(bhs[i]->b_data + offset, slots, copy);
            mark_buffer_dirty(bhs[i]);
            offset = 0;
            slots = (void *)((char *)slots + copy);
            size -= copy;
        }
        if (long_bhs && IS_DIRSYNC(dir))
        {
            err = fat_sync_bhs(bhs, long_bhs);
        }
        if (!err && i < nr_bhs)
        {
            /* Fill the short name slot. */
            int copy = min_t(int, sb->s_blocksize - offset, size);
            memcpy(bhs[i]->b_data + offset, slots, copy);
            mark_buffer_dirty(bhs[i]);
            if (IS_DIRSYNC(dir))
            {
                err = sync_dirty_buffer(bhs[i]);
            }
        }
        for (i = 0; i < nr_bhs; i++)
        {
            brelse(bhs[i]);
        }
        if (err)
        {
            goto error_remove;
        }
    }

    if (nr_slots)
    {
        int cluster, nr_cluster;

        /*
         * Third stage: allocate the cluster for new entries.
         * And initialize the cluster with new entries, then
         * add the cluster to dir.
         */
        cluster = fat_add_new_entries(dir, slots, nr_slots, &nr_cluster,
                                      &de, &bh, &i_pos);
        if (cluster < 0)
        {
            err = cluster;
            goto error_remove;
        }
        err = fat_chain_add(dir, cluster, nr_cluster);
        if (err)
        {
            fat_free_clusters(dir, cluster);
            goto error_remove;
        }
        if (dir->i_size & (sbi->cluster_size - 1))
        {
            fat_fs_panic(sb, "Odd directory size");
            dir->i_size = (dir->i_size + sbi->cluster_size - 1)
                          & ~((__s64)sbi->cluster_size - 1);
        }
        dir->i_size += nr_cluster << sbi->cluster_bits;
        MSDOS_I(dir)->mmu_private += nr_cluster << sbi->cluster_bits;
    }
    sinfo->slot_off = pos;
    sinfo->de = de;
    sinfo->bh = bh;
    sinfo->i_pos = fat_make_i_pos(sb, sinfo->bh, sinfo->de);

    return 0;

error:
    brelse(bh);
    for (i = 0; i < nr_bhs; i++)
    {
        brelse(bhs[i]);
    }
    return err;

error_remove:
    brelse(bh);
    if (free_slots)
    {
        __fat_remove_entries(dir, pos, free_slots);
    }
    return err;
}
#endif