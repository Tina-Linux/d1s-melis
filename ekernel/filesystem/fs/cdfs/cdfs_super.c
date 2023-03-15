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
#include "fs.h"
#include "nls.h"
#include "slab.h"
#include "page_pool.h"
#include "fsys_debug.h"
#include "err.h"
#include "blk_dev.h"
#include "fsys_libs.h"
#include "ctype.h"
#include "cdfs.h"

static int isofs_statfs(struct super_block *sb, struct kstatfs *buf, __u32 flags)
{
    /* flags nouse under cdfs */

    buf->f_type = sb->s_magic;
    buf->f_bsize = sb->s_blocksize;
    buf->f_blocks = (ISOFS_SB(sb)->s_nzones
                     << (ISOFS_SB(sb)->s_log_zone_size - sb->s_blocksize_bits));
    buf->f_bfree = 0;
    //  buf->f_bavail = 0;
    buf->f_files = ISOFS_SB(sb)->s_ninodes;
    //  buf->f_ffree = 0;
    //  buf->f_namelen = NAME_MAX;
    /* set fs attribute here */
    if (sb->s_flags & MS_RDONLY)
    {
        buf->f_attribute =  FS_ATTR_READONLY; /* readonly fs */
    }
    else
    {
        buf->f_attribute = 0;
    }
    return 0;
}

static void isofs_put_super(struct super_block *sb)
{
    struct isofs_sb_info *sbi = ISOFS_SB(sb);

#ifdef CONFIG_JOLIET
    if (sbi->s_nls_iocharset)
    {
        unload_nls(sbi->s_nls_iocharset);
        sbi->s_nls_iocharset = NULL;
    }
#endif

    kfree(sbi);
    sb->s_fs_info = NULL;
    return;
}

/*
 * Compute the hash for the isofs name corresponding to the dentry.
 */
static int isofs_hash_common(struct dentry *dentry, struct qstr *qstr, int ms)
{
    const char *name;
    int len;

    len = qstr->len;
    name = qstr->name;
    if (ms)
    {
        while (len && name[len - 1] == '.')
        {
            len--;
        }
    }

    qstr->hash = full_name_hash(name, len);

    return 0;
}

/*
 * Compute the hash for the isofs name corresponding to the dentry.
 */
static int isofs_hashi_common(struct dentry *dentry, struct qstr *qstr, int ms)
{
    const char *name;
    int len;
    char c;
    unsigned long hash;

    len = qstr->len;
    name = qstr->name;
    if (ms)
    {
        while (len && name[len - 1] == '.')
        {
            len--;
        }
    }

    hash = init_name_hash();
    while (len--)
    {
        c = tolower(*name++);
        hash = partial_name_hash(c, hash);
    }
    qstr->hash = end_name_hash(hash);

    return 0;
}

/*
 * Case insensitive compare of two isofs names.
 */
static int isofs_dentry_cmpi_common(struct dentry *dentry, struct qstr *a,
                                    struct qstr *b, int ms)
{
    int alen, blen;

    /* A filename cannot end in '.' or we treat it like it has none */
    alen = a->len;
    blen = b->len;
    if (ms)
    {
        while (alen && a->name[alen - 1] == '.')
        {
            alen--;
        }
        while (blen && b->name[blen - 1] == '.')
        {
            blen--;
        }
    }
    if (alen == blen)
    {
        if (strncasecmp(a->name, b->name, alen) == 0)
        {
            return 0;
        }
    }
    return 1;
}

/*
 * Case sensitive compare of two isofs names.
 */
static int isofs_dentry_cmp_common(struct dentry *dentry, struct qstr *a,
                                   struct qstr *b, int ms)
{
    int alen, blen;

    /* A filename cannot end in '.' or we treat it like it has none */
    alen = a->len;
    blen = b->len;
    if (ms)
    {
        while (alen && a->name[alen - 1] == '.')
        {
            alen--;
        }
        while (blen && b->name[blen - 1] == '.')
        {
            blen--;
        }
    }
    if (alen == blen)
    {
        if (strncmp(a->name, b->name, alen) == 0)
        {
            return 0;
        }
    }
    return 1;
}

static int isofs_hash(struct dentry *dentry, struct qstr *qstr)
{
    return isofs_hash_common(dentry, qstr, 0);
}

static int isofs_hashi(struct dentry *dentry, struct qstr *qstr)
{
    return isofs_hashi_common(dentry, qstr, 0);
}

static int isofs_dentry_cmp(struct dentry *dentry, struct qstr *a, struct qstr *b)
{
    return isofs_dentry_cmp_common(dentry, a, b, 0);
}

static int isofs_dentry_cmpi(struct dentry *dentry, struct qstr *a, struct qstr *b)
{
    return isofs_dentry_cmpi_common(dentry, a, b, 0);
}

#ifdef CONFIG_JOLIET
static int isofs_hash_ms(struct dentry *dentry, struct qstr *qstr)
{
    return isofs_hash_common(dentry, qstr, 1);
}

static int isofs_hashi_ms(struct dentry *dentry, struct qstr *qstr)
{
    return isofs_hashi_common(dentry, qstr, 1);
}

static int isofs_dentry_cmp_ms(struct dentry *dentry, struct qstr *a, struct qstr *b)
{
    return isofs_dentry_cmp_common(dentry, a, b, 1);
}

static int isofs_dentry_cmpi_ms(struct dentry *dentry, struct qstr *a, struct qstr *b)
{
    return isofs_dentry_cmpi_common(dentry, a, b, 1);
}
#endif

static struct dentry_operations isofs_dentry_ops[] =
{
    {
        .d_hash     = isofs_hash,
        .d_compare  = isofs_dentry_cmp,
    },
    {
        .d_hash     = isofs_hashi,
        .d_compare  = isofs_dentry_cmpi,
    },
#ifdef CONFIG_JOLIET
    {
        .d_hash     = isofs_hash_ms,
        .d_compare  = isofs_dentry_cmp_ms,
    },
    {
        .d_hash     = isofs_hashi_ms,
        .d_compare  = isofs_dentry_cmpi_ms,
    },
#endif
};

struct iso9660_options
{
    char map;
    char rock;
    char joliet;
    char cruft;
    char hide;
    char showassoc;
    char nocompress;
    unsigned char check;
    unsigned int blocksize;
    mode_t mode;
    gid_t gid;
    uid_t uid;
    char *iocharset;
    unsigned char utf8;
    /* LVE */
    s32 session;
    s32 sbsector;
};

//enum {
//  Opt_block, Opt_check_r, Opt_check_s, Opt_cruft, Opt_gid, Opt_ignore,
//  Opt_iocharset, Opt_map_a, Opt_map_n, Opt_map_o, Opt_mode, Opt_nojoliet,
//  Opt_norock, Opt_sb, Opt_session, Opt_uid, Opt_unhide, Opt_utf8, Opt_err,
//  Opt_nocompress, Opt_hide, Opt_showassoc,
//};
//
//static match_table_t tokens = {
//  {Opt_norock, "norock"},
//  {Opt_nojoliet, "nojoliet"},
//  {Opt_unhide, "unhide"},
//  {Opt_hide, "hide"},
//  {Opt_showassoc, "showassoc"},
//  {Opt_cruft, "cruft"},
//  {Opt_utf8, "utf8"},
//  {Opt_iocharset, "iocharset=%s"},
//  {Opt_map_a, "map=acorn"},
//  {Opt_map_a, "map=a"},
//  {Opt_map_n, "map=normal"},
//  {Opt_map_n, "map=n"},
//  {Opt_map_o, "map=off"},
//  {Opt_map_o, "map=o"},
//  {Opt_session, "session=%u"},
//  {Opt_sb, "sbsector=%u"},
//  {Opt_check_r, "check=relaxed"},
//  {Opt_check_r, "check=r"},
//  {Opt_check_s, "check=strict"},
//  {Opt_check_s, "check=s"},
//  {Opt_uid, "uid=%u"},
//  {Opt_gid, "gid=%u"},
//  {Opt_mode, "mode=%u"},
//  {Opt_block, "block=%u"},
//  {Opt_ignore, "conv=binary"},
//  {Opt_ignore, "conv=b"},
//  {Opt_ignore, "conv=text"},
//  {Opt_ignore, "conv=t"},
//  {Opt_ignore, "conv=mtext"},
//  {Opt_ignore, "conv=m"},
//  {Opt_ignore, "conv=auto"},
//  {Opt_ignore, "conv=a"},
//  {Opt_nocompress, "nocompress"},
//  {Opt_err, NULL}
//};

static int iso_parse_options(char *options, struct iso9660_options *popt)
{
    char *p;
    int option;

    popt->map       = 'n';      /* acron not support    */
    popt->rock      = 'n';      /* rock not support     */
    popt->joliet    = 'y';      /* joliet support       */
    popt->cruft     = 'n';      /* file size limit      */
    popt->hide      = 'n';      /* not show hide file   */
    popt->showassoc = 'n';      /* not show assoc file  */
    popt->check     = 'u';      /* unset                */
    popt->nocompress = 1;       /* no decompresstion    */
    popt->blocksize = 1024;

    /*
     * r-x for all.  The disc could
     * be shared with DOS machines so
     * virtually anything could be
     * a valid executable.
     */
    popt->mode      = S_IRUGO | S_IXUGO;
    popt->gid = 0;
    popt->uid = 0;
    popt->iocharset = NULL;
    popt->utf8 = 0;
    popt->session = -1;
    popt->sbsector = -1;
    if (!options)
    {
        return 1;
    }

    return 1;
    //  while ((p = strsep(&options, ",")) != NULL) {
    //      int token;
    //      substring_t args[MAX_OPT_ARGS];
    //      unsigned n;
    //
    //      if (!*p)
    //          continue;
    //
    //      token = match_token(p, tokens, args);
    //      switch (token) {
    //      case Opt_norock:
    //          popt->rock = 'n';
    //          break;
    //      case Opt_nojoliet:
    //          popt->joliet = 'n';
    //          break;
    //      case Opt_hide:
    //          popt->hide = 'y';
    //          break;
    //      case Opt_unhide:
    //      case Opt_showassoc:
    //          popt->showassoc = 'y';
    //          break;
    //      case Opt_cruft:
    //          popt->cruft = 'y';
    //          break;
    //      case Opt_utf8:
    //          popt->utf8 = 1;
    //          break;
    //#ifdef CONFIG_JOLIET
    //      case Opt_iocharset:
    //          popt->iocharset = match_strdup(&args[0]);
    //          break;
    //#endif
    //      case Opt_map_a:
    //          popt->map = 'a';
    //          break;
    //      case Opt_map_o:
    //          popt->map = 'o';
    //          break;
    //      case Opt_map_n:
    //          popt->map = 'n';
    //          break;
    //      case Opt_session:
    //          if (match_int(&args[0], &option))
    //              return 0;
    //          n = option;
    //          if (n > 99)
    //              return 0;
    //          popt->session = n + 1;
    //          break;
    //      case Opt_sb:
    //          if (match_int(&args[0], &option))
    //              return 0;
    //          popt->sbsector = option;
    //          break;
    //      case Opt_check_r:
    //          popt->check = 'r';
    //          break;
    //      case Opt_check_s:
    //          popt->check = 's';
    //          break;
    //      case Opt_ignore:
    //          break;
    //      case Opt_uid:
    //          if (match_int(&args[0], &option))
    //              return 0;
    //          popt->uid = option;
    //          break;
    //      case Opt_gid:
    //          if (match_int(&args[0], &option))
    //              return 0;
    //          popt->gid = option;
    //          break;
    //      case Opt_mode:
    //          if (match_int(&args[0], &option))
    //              return 0;
    //          popt->mode = option;
    //          break;
    //      case Opt_block:
    //          if (match_int(&args[0], &option))
    //              return 0;
    //          n = option;
    //          if (n != 512 && n != 1024 && n != 2048)
    //              return 0;
    //          popt->blocksize = n;
    //          break;
    //      case Opt_nocompress:
    //          popt->nocompress = 1;
    //          break;
    //      default:
    //          return 0;
    //      }
    //  }
    //  return 1;
}

static const struct super_operations isofs_sops =
{
    .alloc_inode    = isofs_alloc_inode,
    .destroy_inode  = isofs_destroy_inode,
    .read_inode     = isofs_read_inode,
    .put_super      = isofs_put_super,
    .statfs         = isofs_statfs,
};

/*
 * Initialize the superblock and read the root inode.
 *
 * Note: a check_disk_change() has been done immediately prior
 * to this call, so we don't need to check again.
 */
__s32 isofs_fill_super(struct super_block *s, void *data, __s32 silent)
{
    struct buffer_head *bh = NULL, *pri_bh = NULL;
    struct hs_primary_descriptor *h_pri = NULL;
    struct iso_primary_descriptor *pri = NULL;
    struct iso_supplementary_descriptor *sec = NULL;
    struct iso_directory_record *rootp;
    struct inode *inode;
    struct iso9660_options opt;
    struct isofs_sb_info *sbi;
    unsigned long first_data_zone;
    int joliet_level = 0;
    int iso_blknum, block;
    int orig_zonesize;
    int table;
    unsigned int vol_desc_start;

    sbi = kmalloc(sizeof(*sbi), GFP_KERNEL);
    if (!sbi)
    {
        return -ENOMEM;
    }
    memset(sbi, 0, sizeof(*sbi));
    s->s_fs_info = sbi;

    if (!iso_parse_options((char *)data, &opt))
    {
        goto out_freesbi;
    }

    /*
     * First of all, get the hardware blocksize for this device.
     * If we don't know what it is, or the hardware blocksize is
     * larger than the blocksize the user specified, then use
     * that value.
     */
    /*
     * What if bugger tells us to go beyond page size?
     */
    opt.blocksize = sb_min_blocksize(s, opt.blocksize);

    sbi->s_high_sierra = 0; /* default is iso9660 */

    vol_desc_start = (opt.sbsector != -1) ?
                     opt.sbsector : isofs_get_last_session(s);

    for (iso_blknum = vol_desc_start + 16;
         iso_blknum < vol_desc_start + 100; iso_blknum++)
    {
        struct hs_volume_descriptor *hdp;
        struct iso_volume_descriptor  *vdp;

        block = iso_blknum << (ISOFS_BLOCK_BITS - s->s_blocksize_bits);
        if (!(bh = sb_bread(s, block)))
        {
            goto out_no_read;
        }

        vdp = (struct iso_volume_descriptor *)bh->b_data;
        hdp = (struct hs_volume_descriptor *)bh->b_data;

        /*
         * Due to the overlapping physical location of the descriptors,
         * ISO CDs can match hdp->id==HS_STANDARD_ID as well. To ensure
         * proper identification in this case, we first check for ISO.
         */
        if (strncmp(vdp->id, ISO_STANDARD_ID, sizeof vdp->id) == 0)
        {
            if (isonum_711(vdp->type) == ISO_VD_END)
            {
                break;
            }
            if (isonum_711(vdp->type) == ISO_VD_PRIMARY)
            {
                if (pri == NULL)
                {
                    pri = (struct iso_primary_descriptor *)vdp;
                    /* Save the buffer in case we need it ... */
                    pri_bh = bh;
                    bh = NULL;
                }
            }
#ifdef CONFIG_JOLIET
            else if (isonum_711(vdp->type) == ISO_VD_SUPPLEMENTARY)
            {
                sec = (struct iso_supplementary_descriptor *)vdp;
                if (sec->escape[0] == 0x25 && sec->escape[1] == 0x2f)
                {
                    if (opt.joliet == 'y')
                    {
                        if (sec->escape[2] == 0x40)
                        {
                            joliet_level = 1;
                        }
                        else if (sec->escape[2] == 0x43)
                        {
                            joliet_level = 2;
                        }
                        else if (sec->escape[2] == 0x45)
                        {
                            joliet_level = 3;
                        }

                        fs_log_debug("ISO 9660 Extensions: "
                                     "Microsoft Joliet Level %d\n",
                                     joliet_level);
                    }
                    goto root_found;
                }
                else
                {
                    /* Unknown supplementary volume descriptor */
                    sec = NULL;
                }
            }
#endif
        }
        else
        {
            if (strncmp(hdp->id, HS_STANDARD_ID, sizeof hdp->id) == 0)
            {
                if (isonum_711(hdp->type) != ISO_VD_PRIMARY)
                {
                    goto out_freebh;
                }

                sbi->s_high_sierra = 1;
                opt.rock = 'n';
                h_pri = (struct hs_primary_descriptor *)vdp;
                goto root_found;
            }
        }

        /* Just skip any volume descriptors we don't recognize */

        brelse(bh);
        bh = NULL;
    }
    /*
     * If we fall through, either no volume descriptor was found,
     * or else we passed a primary descriptor looking for others.
     */
    if (!pri)
    {
        goto out_unknown_format;
    }
    brelse(bh);
    bh = pri_bh;
    pri_bh = NULL;

root_found:

    if (joliet_level && (pri == NULL || opt.rock == 'n'))
    {
        /* This is the case of Joliet with the norock mount flag.
         * A disc with both Joliet and Rock Ridge is handled later
         */
        pri = (struct iso_primary_descriptor *) sec;
    }

    if (sbi->s_high_sierra)
    {
        rootp = (struct iso_directory_record *) h_pri->root_directory_record;
        sbi->s_nzones = isonum_733(h_pri->volume_space_size);
        sbi->s_log_zone_size = isonum_723(h_pri->logical_block_size);
        sbi->s_max_size = isonum_733(h_pri->volume_space_size);
    }
    else
    {
        if (!pri)
        {
            goto out_freebh;
        }
        rootp = (struct iso_directory_record *) pri->root_directory_record;
        sbi->s_nzones = isonum_733(pri->volume_space_size);
        sbi->s_log_zone_size = isonum_723(pri->logical_block_size);
        sbi->s_max_size = isonum_733(pri->volume_space_size);
    }

    sbi->s_ninodes = 0; /* No way to figure this out easily */

    orig_zonesize = sbi->s_log_zone_size;
    /*
     * If the zone size is smaller than the hardware sector size,
     * this is a fatal error.  This would occur if the disc drive
     * had sectors that were 2048 bytes, but the filesystem had
     * blocks that were 512 bytes (which should only very rarely
     * happen.)
     */
    if (orig_zonesize < opt.blocksize)
    {
        goto out_bad_size;
    }

    /* RDE: convert log zone size to bit shift */
    switch (sbi->s_log_zone_size)
    {
        case  512:
            sbi->s_log_zone_size =  9;
            break;
        case 1024:
            sbi->s_log_zone_size = 10;
            break;
        case 2048:
            sbi->s_log_zone_size = 11;
            break;

        default:
            goto out_bad_zone_size;
    }

    s->s_magic = ISOFS_SUPER_MAGIC;
    s->s_maxbytes = 0xffffffff; /* We can handle files up to 4 GB */

    /*
     * The CDROM is read-only, has no nodes (devices) on it, and since
     * all of the files appear to be owned by root, we really do not want
     * to allow suid.  (suid or devices will not show up unless we have
     * Rock Ridge extensions)
     */

    s->s_flags |= MS_RDONLY /* | MS_NODEV | MS_NOSUID */;

    /* Set this for reference. Its not currently used except on write
       which we don't have .. */

    first_data_zone = isonum_733(rootp->extent) +
                      isonum_711(rootp->ext_attr_length);
    sbi->s_firstdatazone = first_data_zone;

    fs_log_debug("ISOFS: Max size:%ld   Log zone size:%ld\n",
                 sbi->s_max_size, 1UL << sbi->s_log_zone_size);
    fs_log_debug("ISOFS: First datazone:%ld\n", sbi->s_firstdatazone);
    if (sbi->s_high_sierra)
    {
        fs_log_debug("ISOFS: Disc in High Sierra format.\n");
    }
    /*
     * If the Joliet level is set, we _may_ decide to use the
     * secondary descriptor, but can't be sure until after we
     * read the root inode. But before reading the root inode
     * we may need to change the device blocksize, and would
     * rather release the old buffer first. So, we cache the
     * first_data_zone value from the secondary descriptor.
     */
    if (joliet_level)
    {
        pri = (struct iso_primary_descriptor *) sec;
        rootp = (struct iso_directory_record *)
                pri->root_directory_record;
        first_data_zone = isonum_733(rootp->extent) +
                          isonum_711(rootp->ext_attr_length);
    }

    /*
     * We're all done using the volume descriptor, and may need
     * to change the device blocksize, so release the buffer now.
     */
    brelse(pri_bh);
    brelse(bh);

    /*
     * Force the blocksize to 512 for 512 byte sectors.  The file
     * read primitives really get it wrong in a bad way if we don't
     * do this.
     *
     * Note - we should never be setting the blocksize to something
     * less than the hardware sector size for the device.  If we
     * do, we would end up having to read larger buffers and split
     * out portions to satisfy requests.
     *
     * Note2- the idea here is that we want to deal with the optimal
     * zonesize in the filesystem.  If we have it set to something less,
     * then we have horrible problems with trying to piece together
     * bits of adjacent blocks in order to properly read directory
     * entries.  By forcing the blocksize in this way, we ensure
     * that we will never be required to do this.
     */
    sb_set_blocksize(s, orig_zonesize);

    sbi->s_nls_iocharset = NULL;

#ifdef CONFIG_JOLIET
    if (joliet_level && opt.utf8 == 0)
    {
        sbi->s_nls_iocharset = nls;
    }
#endif
    s->s_op = &isofs_sops;

#ifdef  CONFIG_EXPORT
    s->s_export_op = &isofs_export_ops;
#endif

    sbi->s_mapping = opt.map;
    sbi->s_rock = (opt.rock == 'y' ? 2 : 0);
    sbi->s_rock_offset = -1; /* initial offset, will guess until SP is found*/
    sbi->s_cruft = opt.cruft;
    sbi->s_hide = opt.hide;
    sbi->s_showassoc = opt.showassoc;
    sbi->s_uid = opt.uid;
    sbi->s_gid = opt.gid;
    sbi->s_utf8 = opt.utf8;
    sbi->s_nocompress = opt.nocompress;
    /*
     * It would be incredibly stupid to allow people to mark every file
     * on the disk as suid, so we merely allow them to set the default
     * permissions.
     */
    sbi->s_mode = opt.mode & 0777;

    /*
     * Read the root inode, which _may_ result in changing
     * the s_rock flag. Once we have the final s_rock value,
     * we then decide whether to use the Joliet descriptor.
     */
    inode = isofs_iget(s, sbi->s_firstdatazone, 0);

    /*
     * If this disk has both Rock Ridge and Joliet on it, then we
     * want to use Rock Ridge by default.  This can be overridden
     * by using the norock mount option.  There is still one other
     * possibility that is not taken into account: a Rock Ridge
     * CD with Unicode names.  Until someone sees such a beast, it
     * will not be supported.
     *
     * Sunny : If this disk has both Rock Ridge and Joliet on it,
     * then we want to use Joliet by default
     */
    if (sbi->s_rock == 1)
    {
        joliet_level = 0;
    }
    else if (joliet_level)
    {
        sbi->s_rock = 0;
        if (sbi->s_firstdatazone != first_data_zone)
        {
            sbi->s_firstdatazone = first_data_zone;
            fs_log_debug("ISOFS: changing to secondary root\n");
            iput(inode);
            inode = isofs_iget(s, sbi->s_firstdatazone, 0);
        }
    }

    if (opt.check == 'u')
    {
        /* Only Joliet is case insensitive by default */
        if (joliet_level)
        {
            opt.check = 'r';
        }
        else
        {
            opt.check = 's';
        }
    }
    sbi->s_joliet_level = joliet_level;

    /* check the root inode */
    if (!inode)
    {
        goto out_no_root;
    }
    if (!inode->i_op)
    {
        goto out_bad_root;
    }

    /* Make sure the root inode is a directory */
    if (!S_ISDIR(inode->i_mode))
    {
        fs_log_warning("isofs_fill_super: root inode is not a directory. "
                       "Corrupted media?\n");
        goto out_iput;
    }

    /* get the root dentry */
    s->s_root = d_alloc_root(inode);
    if (!(s->s_root))
    {
        goto out_no_root;
    }

    table = 0;
    if (joliet_level)
    {
        table += 2;
    }
    if (opt.check == 'r')
    {
        table++;
    }
    s->s_root->d_op = &isofs_dentry_ops[table];

    //  kfree(opt.iocharset);
    if (opt.iocharset)
    {
        kfree(opt.iocharset);
    }

    return 0;

    /*
     * Display error messages and free resources.
     */
out_bad_root:
    fs_log_warning("%s: root inode not initialized\n", __func__);
    goto out_iput;
out_no_root:
    fs_log_warning("%s: get root inode failed\n", __func__);
out_iput:
    iput(inode);
#ifdef CONFIG_JOLIET
    if (sbi->s_nls_iocharset)
    {
        unload_nls(sbi->s_nls_iocharset);
    }
#endif
    goto out_freesbi;
out_no_read:
    fs_log_warning("%s: bread failed, dev=, iso_blknum=%d, block=%d\n",
                   __func__, /*s->s_id,*/iso_blknum, block);
    goto out_freesbi;
out_bad_zone_size:
    fs_log_warning("ISOFS: Bad logical zone size %ld\n",
                   sbi->s_log_zone_size);
    goto out_freebh;
out_bad_size:
    fs_log_warning("ISOFS: Logical zone size(%d) < hardware blocksize(%u)\n",
                   orig_zonesize, opt.blocksize);
    goto out_freebh;
out_unknown_format:
    if (!silent)
    {
        fs_log_warning("ISOFS: Unable to identify CD-ROM format.\n");
    }

out_freebh:
    brelse(bh);
out_freesbi:
    //  kfree(opt.iocharset);
    if (opt.iocharset)
    {
        kfree(opt.iocharset);
    }

    kfree(sbi);
    s->s_fs_info = NULL;
    return -EINVAL;
}