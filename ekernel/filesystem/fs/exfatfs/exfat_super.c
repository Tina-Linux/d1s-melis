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
#include "exfat_bitmap.h"
#include "exfat_upcase.h"
#include "slab.h"
#include "buffer_head.h"
#include "page_pool.h"
#include "errno.h"
#include "endians.h"
#include "nls.h"
#include "fsys_debug.h"
#include "fsys_libs.h"
#include "blk_dev.h"

static kmem_cache_t *exfat_inode_cachep;

static void exfat_inode_init_once(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
    struct exfat_inode_info *exi = (struct exfat_inode_info *)obj;

    INIT_HLIST_NODE(&exi->i_hash);
    exfat_cache_inode_init_once(exi);
    inode_init_once(&exi->vfs_inode);
}

int exfat_init_inodecache(void)
{
    exfat_inode_cachep = kmem_cache_create("exfat_inode_cache",
                                           sizeof(struct exfat_inode_info),
                                           0, 0,
                                           /*(SLAB_RECLAIM_ACCOUNT | SLAB_MEM_SPREAD), */
                                           exfat_inode_init_once, NULL);
    if (!exfat_inode_cachep)
    {
        return -ENOMEM;
    }
    return 0;
}

void exfat_destroy_inodecache(void)
{
    kmem_cache_destroy(exfat_inode_cachep);
}

static struct inode *exfat_alloc_inode(struct super_block *sb)
{
    struct exfat_inode_info *exi;
    exi = kmem_cache_alloc(exfat_inode_cachep, GFP_KERNEL);
    if (!exi)
    {
        return NULL;
    }
    return &exi->vfs_inode;
}

static void exfat_destroy_inode(struct inode *inode)
{
    kmem_cache_free(exfat_inode_cachep, EXFAT_I(inode));
}

static void exfat_clear_inode(struct inode *inode)
{
    exfat_cache_inval(inode);
    exfat_detach(inode);
}

//enum { Opt_uid, Opt_gid, Opt_fmode, Opt_dmode, Opt_nls, Opt_err, };
//
//static match_table_t exfat_tokens = {
//  { Opt_uid, "uid=%u" },
//  { Opt_gid, "gid=%u" },
//  { Opt_fmode, "fmode=%o" },
//  { Opt_dmode, "dmode=%o" },
//  /* FIXME: should use "iocharset"? */
//  { Opt_nls, "nls=%s" },
//  { Opt_err, NULL },
//};

//static int exfat_show_options(struct seq_file *m, struct vfsmount *mnt)
//{
//  struct exfat_sb_info *sbi = EXFAT_SB(mnt->mnt_sb);
//  struct exfat_mount_opts *opts = &sbi->opts;
//
//  if (opts->uid)
//      seq_printf(m, ",uid=%u", opts->uid);
//  if (opts->gid)
//      seq_printf(m, ",gid=%u", opts->gid);
//  seq_printf(m, ",fmode=%04o", opts->fmode);
//  seq_printf(m, ",dmode=%04o", opts->dmode);
//  seq_printf(m, ",nls=%s", opts->nls->charset);
//
//  return 0;
//}

static int exfat_parse_options(struct exfat_mount_opts *opts, char *data,
                               int silent, int is_remount)
{
    //  char *p, *nls_str;
    //  substring_t args[MAX_OPT_ARGS];
    //  int err, option;

    opts->uid   = 0;
    opts->gid   = 0;
    opts->fmode = S_IWUSR | S_IRUGO;
    opts->dmode = S_IWUSR | S_IRUGO | S_IXUGO;
    //opts->nls = load_nls("cp936");
    return 0;

    //  if (!data)
    //      goto out;
    //
    //  err = -EINVAL;
    //  while ((p = strsep(&data, ",")) != NULL) {
    //      int token;
    //      if (!*p)
    //          continue;
    //
    //      token = match_token(p, exfat_tokens, args);
    //      switch (token) {
    //      case Opt_uid:
    //          if (is_remount) {
    //              if (!silent)
    //                  __log(KERN_ERR "exFAT: cannot change"
    //                         " uid option on remount\n");
    //              goto error;
    //          }
    //          if (match_int(&args[0], &option))
    //              goto error;
    //          opts->uid = option;
    //          break;
    //      case Opt_gid:
    //          if (is_remount) {
    //              if (!silent)
    //                  __log(KERN_ERR "exFAT: cannot change"
    //                         " gid option on remount\n");
    //              goto error;
    //          }
    //          if (match_int(&args[0], &option))
    //              goto error;
    //          opts->gid = option;
    //          break;
    //      case Opt_fmode:
    //          if (is_remount) {
    //              if (!silent)
    //                  __log(KERN_ERR "exFAT: cannot change"
    //                         " fmode option on remount\n");
    //              goto error;
    //          }
    //          if (match_octal(&args[0], &option))
    //              goto error;
    //          opts->fmode = option & S_IRWXUGO;
    //          break;
    //      case Opt_dmode:
    //          if (is_remount) {
    //              if (!silent)
    //                  __log(KERN_ERR "exFAT: cannot change"
    //                         " dmode option on remount\n");
    //              goto error;
    //          }
    //          if (match_octal(&args[0], &option))
    //              goto error;
    //          opts->dmode = option & S_IRWXUGO;
    //          break;
    //      case Opt_nls:
    //          if (is_remount) {
    //              if (!silent)
    //                  __log(KERN_ERR "exFAT: cannot change"
    //                         " nls option on remount\n");
    //              goto error;
    //          }
    //          nls_str = match_strdup(&args[0]);
    //          if (!nls_str) {
    //              err = -ENOMEM;
    //              goto error;
    //          }
    //
    //          if (opts->nls)
    //              unload_nls(opts->nls);
    //          opts->nls = load_nls(nls_str);
    //          if (!opts->nls) {
    //              if (!silent)
    //                  __log(KERN_ERR "exFAT: couldn't load "
    //                         " nls \"%s\"\n", nls_str);
    //          }
    //          kfree(nls_str);
    //          if (!opts->nls)
    //              goto error;
    //          break;
    //      /* Unknown option */
    //      default:
    //          if (!silent) {
    //              __log(KERN_ERR "exFAT: Unrecognized mount"
    //                     " option \"%s\" or missing value\n", p);
    //          }
    //          goto error;
    //      }
    //  }
    //
    //out:
    //  return 0;
    //
    //error:
    //  if (opts->nls) {
    //      unload_nls(opts->nls);
    //      opts->nls = NULL;
    //  }
    //  return err;
}

//static int exfat_remount(struct super_block *sb, int *flags, char *data)
//{
//  struct exfat_sb_info *sbi = EXFAT_SB(sb);
//  struct exfat_mount_opts opts;
//  int err, remount_rw;
//
//  remount_rw = !(*flags & MS_RDONLY) && (sb->s_flags & MS_RDONLY);
//
//  if (!sbi->upcase && remount_rw) {
//      __log(KERN_ERR "exFAT: upper-case table isn't available,"
//             " so filesystem can't make writable\n");
//      return -EINVAL;
//  }
//
//  /*
//   * FIXME: currently, can't change all options on remount, must
//   * rethink about remount options.
//   */
//  err = exfat_parse_options(&opts, data, 0, 1);
//  if (err)
//      return err;
//
//  return 0;
//}

static void exfat_put_super(struct super_block *sb)
{
    struct exfat_sb_info *sbi = EXFAT_SB(sb);

    exfat_free_bitmap(sbi);
    exfat_free_upcase(sbi);
    //if (sbi->opts.nls)
    //  unload_nls(sbi->opts.nls);
    sb->s_fs_info = NULL;
    //kfree(sbi);
    free(sbi);
}

static int exfat_statfs(struct super_block *sb, struct kstatfs *buf, __u32 flags)
{
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct inode  *ino;

    /* If the count of free cluster is still unknown,
     * counts it here, it is slowly.
     */
    ino =  sbi->bitmap_inode;
    if (flags & FSYS_KSTATUS_BFREE)
    {
        if ((int)sbi->free_clusters == -1)
        {
            int err = exfat_count_free_clusters(ino);
            if (err)
            {
                return err;
            }
        }
    }
    buf->f_type = sb->s_magic;
    buf->f_bsize = sbi->clus_size;

    /* should be sub EXFAT_START_ENT or not ?? */
    buf->f_blocks = sbi->total_clusters; /*- EXFAT_START_ENT */

    if (flags & FSYS_KSTATUS_BFREE)
    {
        buf->f_bfree = sbi->free_clusters;
    }
    else
    {
        buf->f_bfree = 0;
    }
    buf->f_namelen = EXFAT_MAX_NAMELEN;
    strncpy(buf->f_fsname, sb->s_fsname, MAX_FS_NAME_LEN);
    strncpy(buf->f_volname, sb->s_volname, MAX_FS_NAME_LEN);

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

static int exfat_fast_get_inode(struct inode *inode,
                                struct exfat_parse_data *pd)
{
    struct super_block *sb = inode->i_sb;
    struct exfat_inode_info *exi = EXFAT_I(inode);
    struct buffer_head *bh;
    unsigned long left, offset, size;
    int index, err;

    /* sanity check : fast read mode */
    if (!exi->de_blocknr[0])
    {
        fs_log_error("exFAT: wrong fast read mode\n");
        return -EPERM;
    }
    /* invalid chunks number check */
    if ((exi->de_size >> EXFAT_CHUNK_BITS) < EXFAT_MIN_CHUNK_NUMBER)
    {
        fs_log_error("exFAT: too few chunk number %d\n",
                     exi->de_size >> EXFAT_CHUNK_BITS);
        return -EPERM;
    }
    index = 0;
    left = exi->de_size;
    offset = exi->de_offset;
    while (left)
    {
        bh = sb_bread(sb, exi->de_blocknr[index]);
        if (!bh)
        {
            err = -EIO;
            goto error;
        }
        pd->bhs[index] = bh;
        size = min(left, sb->s_blocksize - offset);
        left -= size;
        pd->nr_bhs++;
        index++;
        offset = 0;
    }
    pd->size = exi->de_size;
    pd->bh_offset = exi->de_offset;
    return 0;

error:
    pd_brelse(pd);
    return err;
}

static void exfat_mirror_super_block_flush(struct super_block *sb,
        struct exfat_super_block *exsb)
{
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct buffer_head *bh;
    struct exfat_super_block *mirror_exsb;

    bh = sb_bread(sb, EXFAT_SB_MIRROR);
    if (bh == NULL)
    {
        fs_log_error("exFAT: bread failed in exfat_mirror_super_block\n");
        return;
    }
    mirror_exsb = (struct exfat_super_block *)bh->b_data;

    mirror_exsb->allocated_percent = exsb->allocated_percent;
    mirror_exsb->state = exsb->state;

    mark_buffer_dirty(bh);
    brelse(bh);
}

static int exfat_exsb_validate(struct super_block *sb,
                               struct exfat_super_block *exsb,
                               int silent)
{
    struct exfat_sb_info *sbi = EXFAT_SB(sb);

#if 0
    struct inode *bd_inode = sb->s_bdev->bd_inode;
#endif

    const char *message;
    sector_t nr_sectors;
    int i, zero;

    /*
     * FIXME: should check the physical block aligment? (eraceblock too?)
     * If aligment is invalid, should be warn it (performance may be bad)
     */

    if (memcmp(exsb->oem_id, EXFAT_OEM_ID, sizeof(exsb->oem_id)))
    {
        message = "invalid OEM ID";
        goto error;
    }
    zero = 1;
    for (i = 0; i < ARRAY_SIZE(exsb->unused1); i++)
    {
        if (exsb->unused1[i])
        {
            zero = 0;
            break;
        }
    }
    if (!zero || exsb->unused0)
    {
        message = "invalid garbage found in boot sector";
        goto error;
    }
    if (exsb->blocksize_bits < EXFAT_MIN_BLKBITS ||
        EXFAT_MAX_BLKBITS < exsb->blocksize_bits)
    {
        message = "invalid blocksize";
        goto error;
    }
    if ((exsb->blocksize_bits + sbi->bpc_bits) > EXFAT_MAX_CLUS_BITS)
    {
        message = "cluster size is too large";
        goto error;
    }
#if 0
    if ((i_size_read(bd_inode) >> exsb->blocksize_bits) < EXFAT_SB_BLOCKS)
    {
        message = "too small device";
        goto error;
    }
#endif
    nr_sectors = le64_to_cpu(exsb->nr_sectors);
    if (!nr_sectors)
    {
        message = "invalid total sectors count";
        goto error;
    }
    if (sbi->fat_blocknr < EXFAT_SB_BLOCKS)
    {
        message = "invalid block number of FAT";
        goto error;
    }
    if (!sbi->fat_block_counts)
    {
        message = "invalid FAT blocks count";
        goto error;
    }
    if (sbi->clus_blocknr < EXFAT_SB_BLOCKS)
    {
        message = "invalid block number of start cluster";
        goto error;
    }
    if (!sbi->total_clusters ||
        (sbi->total_clusters << sbi->bpc_bits) > nr_sectors)
    {
        message = "invalid total clusters count";
        goto error;
    }
    if (sbi->rootdir_clusnr < EXFAT_START_ENT)
    {
        message = "invalid block number of root directory";
        goto error;
    }
#if 0
    if (exsb->xxxx02 != 0 && exsb->xxxx02 != 1)
    {
        message = "invalid xxxx02";
        goto error;
    }
    if (exsb->xxxx03 != 0 && exsb->xxxx03 != 1)
    {
        message = "invalid xxxx03";
        goto error;
    }
#endif

    if (exsb->signature != cpu_to_le16(EXFAT_BOOT_SIG))
    {
        message = "invalid boot block signature";
        goto error;
    }
    return 0;

error:
    if (!silent)
    {
        fs_log_info("exFAT: %s\n", message);
    }

    return -EINVAL;
}

#if defined CONFIG_FSYS_EXFAT_RW
static __s32 exfat_write_inode(struct inode *inode, __s32 wait)
{
    struct exfat_inode_info *exi = EXFAT_I(inode);
    struct exfat_parse_iter_data pd_iter;
    struct exfat_parse_data pd;
    struct exfat_chunk_dirent *dirent;
    struct exfat_chunk_data *data;
    struct exfat_chunk_name *name;
    int err;
    u16 cksum;

    /* protect reserved inodes, no need to deal detach inode */
    if (inode->i_ino <= EXFAT_RESERVED_INO || !exi->de_blocknr[0])
    {
        return 0;
    }
    pd_init(&pd);
    err = exfat_fast_get_inode(inode, &pd);
    if (err)
    {
        return err;
    }
    /* get dirent chunk */
    dirent = pd_iter_first_de(&pd, &pd_iter);

    /* updating dirent chunk */
    dirent->attrib = exi->attrib;
    exfat_time_unix2exfat(&(inode->i_mtime), &(dirent->mtime),
                          &(dirent->mdate), &(dirent->mtime_cs));
    exfat_time_unix2exfat(&(inode->i_atime), &(dirent->atime),
                          &(dirent->adate),  NULL);
    exfat_time_unix2exfat(&(inode->i_ctime), &(dirent->crtime),
                          &(dirent->crdate), &(dirent->crtime_cs));

    /* recalc checksum */
    cksum = exfat_checksum16(0, dirent, 2);
    cksum = exfat_checksum16(cksum, (u8 *)dirent + 4, EXFAT_CHUNK_SIZE - 4);

    /* delete data chunk */
    data = pd_iter_next_de(&pd, &pd_iter);

    /* updating data chunk */
    data->size = cpu_to_le64(inode->i_size);    /* update inode size */
    data->size2 = data->size;                   /* update size2 */
    data->flag = exi->data_flag;
    data->clusnr = exi->clusnr;

    /* recalc data chunk checksum */
    cksum = exfat_checksum16(cksum, data, EXFAT_CHUNK_SIZE);

    /* recalc name chunks checksum */
    while ((name = pd_iter_next_de(&pd, &pd_iter)) != NULL)
    {
        cksum = exfat_checksum16(cksum, name, EXFAT_CHUNK_SIZE);
    }
    /* set checksum of sub_chunks last */
    dirent->checksum = cksum;

    /* mark buffer dirty */
    pd_dirty(&pd);
    if (IS_DIRSYNC(inode))
    {
        err = exfat_sync_bhs(pd.bhs, pd.nr_bhs);
    }
    pd_brelse(&pd);
    return err;
}

int exfat_sync_inode(struct inode *inode)
{
    return exfat_write_inode(inode, 1);
}

static void exfat_delete_inode(struct inode *inode)
{
    //truncate_inode_pages(&inode->i_data, 0);
    if (!is_bad_inode(inode))
    {
        inode->i_size = 0;
        exfat_truncate(inode);
    }
    clear_inode(inode);
}

/*
 * write data and metadata corresponding to i1 and i2.  The io is
 * started but we do not wait for any of it to finish.
 *
 * filemap_flush is used for the block device, so if there is a dirty
 * page for a block already in flight, we will not wait and start the
 * io over again
 */
int exfat_flush_inodes(struct super_block *sb,
                       struct inode *i1, struct inode *i2)
{
    int ret = 0;
    if (i1)
    {
        ret = exfat_write_inode(i1, 1);
    }
    if (!ret && i2)
    {
        ret = exfat_write_inode(i2, 1);
    }
    return ret;
}

/*
 * set the state of one volume
 */
void exfat_set_volume_state(struct super_block *sb, u16 state)
{
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    sbi->state = state;

    /* set sb dirty */
    sb->s_dirt = 1;
}

/* Flushes super block of exfat
 * Currently only write volume state and allocated_percent.
 */
void exfat_super_block_flush(struct super_block *sb)
{
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct buffer_head *bh;
    struct exfat_super_block *exsb;

    bh = sb_bread(sb, 0);
    if (bh == NULL)
    {
        fs_log_error("exFAT: bread failed in exfat_clusters_flush\n");
        return;
    }
    exsb = (struct exfat_super_block *)bh->b_data;

    /* Sanity check */
    if (exfat_exsb_validate(sb, exsb, 1))
    {
        fs_log_error("exFAT: Did not find valid super block\n");
    }
    else
    {

        /* recalc allocate percent */
        exsb->allocated_percent = (u8)(sbi->free_clusters / exsb->total_clusters);
        exsb->state = sbi->state;

        /* maybe update others here */
        /* super block now only wirte state and allocated_percent,
         * checksum not include state and allocate_percent,
         * so no need to recalc checksum of super block.
         * NOTE : if will write other super block data,
         *        must recalc checksum. */
        mark_buffer_dirty(bh);

        /* update other copy of super block  */
        exfat_mirror_super_block_flush(sb, exsb);
    }
    brelse(bh);
}

static void exfat_write_super(struct super_block *sb)
{
    sb->s_dirt = 0;

    if (!(sb->s_flags & MS_RDONLY))
    {
        exfat_super_block_flush(sb);
    }
    return;
}
#endif  /* FSYS_EXFAT_RW */

static const struct super_operations exfat_sops =
{
    .alloc_inode    = exfat_alloc_inode,
    .destroy_inode  = exfat_destroy_inode,
    .clear_inode    = exfat_clear_inode,
    .put_super      = exfat_put_super,
    .statfs         = exfat_statfs,
#if defined CONFIG_FSYS_EXFAT_RW
    .write_super    = exfat_write_super,
    .write_inode    = exfat_write_inode,
    .delete_inode   = exfat_delete_inode,
#endif
};

/* Verify checksum for super block */
static int exfat_exsb_verify_checksum(struct super_block *sb, int silent)
{
    int i;
#if 0
    struct address_space *bd_mapping = sb->s_bdev->bd_inode->i_mapping;
    struct file_ra_state ra;
    int nr_pages;

    memset(&ra, 0, sizeof(ra));
    file_ra_state_init(&ra, bd_mapping);

    /* FIXME: sector 0 was already read, so 0 page will not be submitted */
    nr_pages = (EXFAT_SB_BLOCKS * 2)
               >> (PAGE_CACHE_SHIFT - sb->s_blocksize_bits);
    ra.ra_pages = nr_pages;
    page_cache_sync_readahead(bd_mapping, &ra, NULL, 0, nr_pages);
#endif
    for (i = 0; i < 2; i++)
    {
        sector_t start_blocknr = EXFAT_SB_BLOCKS * i;
        struct buffer_head *bh;
        __le32 *checksums;
        u32 s;
        unsigned int n, blocknr;

        bh = sb_bread(sb, start_blocknr);
        if (!bh)
        {
            goto error_io;
        }
        /* Skip exsb->state and exsb->allocated_percent */
        s = exfat_checksum32(0, bh->b_data, 0x6a);
        s = exfat_checksum32(s, bh->b_data + 0x6c, 4);
        s = exfat_checksum32(s, bh->b_data + 0x71, bh->b_size - 0x71);
        brelse(bh);
        for (blocknr = 1; blocknr < EXFAT_SB_DATA_BLOCKS; blocknr++)
        {
            bh = sb_bread(sb, start_blocknr + blocknr);
            if (!bh)
            {
                goto error_io;
            }
            s = exfat_checksum32(s, bh->b_data, bh->b_size);
            brelse(bh);
        }

        bh = sb_bread(sb, start_blocknr + EXFAT_SB_CKSUM_BLOCK);
        if (!bh)
        {
            goto error_io;
        }
        checksums = (__le32 *)bh->b_data;
        for (n = 0; n < (bh->b_size / sizeof(checksums[0])); n++)
        {
            if (checksums[n] != cpu_to_le32(s))
            {
                brelse(bh);
                goto error_checksum;
            }
        }
        brelse(bh);
    }

    return 0;

error_io:
    fs_log_error("exFAT: couldn't read super block to verify\n");
    return -EIO;

error_checksum:
    if (!silent)
    {
        fs_log_info("exFAT: checksum failed for super block\n");
    }
    return -EINVAL;
}

__s32 exfat_fill_super(struct super_block *sb, void *data, __s32 silent)
{
    struct exfat_super_block *exsb;
    struct exfat_sb_info *sbi;
    struct buffer_head *bh = NULL;
    struct inode *root_inode;
    unsigned long blocksize;
    int err;

    sbi = calloc(sizeof(struct exfat_sb_info), 1);
    if (!sbi)
    {
        return -ENOMEM;
    }
    sb->s_fs_info = sbi;

    sb->s_magic = EXFAT_SUPER_MAGIC;
    sb->s_maxbytes = MAX_LFS_FILESIZE; /* FIXME: ~0ULL? */
    sb->s_op = &exfat_sops;
    sb->s_time_gran = 10000000; /* 10ms: FIXME: but atime is 2sec */
    sbi->bitmap_inode = NULL;
    sbi->upcase = NULL;

    sb_min_blocksize(sb, 512);
    //    sb->s_blocksize_bits = 9;
    //    sb->s_blocksize = 512;

    bh = sb_bread(sb, 0);
    if (!bh)
    {
        fs_log_error("exFAT: unable to read sector 0\n");
        err = -EIO;
        goto error;
    }
    exsb = (struct exfat_super_block *)bh->b_data;

    sbi->fat_blocknr = le32_to_cpu(exsb->fat_blocknr);
    sbi->fat_block_counts = le32_to_cpu(exsb->fat_block_counts);
    sbi->clus_blocknr = le32_to_cpu(exsb->clus_blocknr);
    sbi->total_clusters = le32_to_cpu(exsb->total_clusters);
    sbi->rootdir_clusnr = le32_to_cpu(exsb->rootdir_clusnr);
    sbi->state = le16_to_cpu(exsb->state);
    sbi->bpc_bits = exsb->block_per_clus_bits;
    sbi->bpc = 1 << exsb->block_per_clus_bits;

    /*
     * calc bits of cluster number per bitmap block
     * 1 Bytes = 8 bits
     * add by sunny, 2009.7.8
     */
    sbi->cpbb_bits = sb->s_blocksize_bits + 3;
    sbi->cpbb = 1 << sbi->cpbb_bits;
    sbi->chunk_per_block = sb->s_blocksize >> EXFAT_CHUNK_BITS;
    sbi->serial_number = le32_to_cpu(exsb->serial_number);
    err = exfat_exsb_validate(sb, exsb, silent);
    if (err)
    {
        goto error;
    }

    blocksize = 1 << exsb->blocksize_bits;

    brelse(bh);
    bh = NULL;

    /* block size not is 512 bytes,
     * should reset block size again ????
     */
    if (sb->s_blocksize != blocksize)
    {
        err = -EINVAL;

        /* FIXME: Windows supports this? */
        if (!sb_set_blocksize(sb, blocksize))
        {
            fs_log_error("exFAT: unable to set block size\n");
            goto error;
        }
    }

    exfat_hash_init(sb);
    sbi->clus_size = sb->s_blocksize << sbi->bpc_bits;
    sbi->clus_bits = sb->s_blocksize_bits + sbi->bpc_bits;
    sbi->fpb = sb->s_blocksize >> EXFAT_ENT_BITS;
    sbi->fpb_bits = sb->s_blocksize_bits - EXFAT_ENT_BITS;

    err = exfat_exsb_verify_checksum(sb, silent);
    if (err)
    {
        goto error;
    }

    err = exfat_parse_options(&sbi->opts, data, silent, 0);
    if (err)
    {
        goto error;
    }

    root_inode = exfat_rootdir_iget(sb);
    if (IS_ERR(PTR_ERR(root_inode)))
    {
        err = PTR_ERR(root_inode);
        goto error;
    }
    err = exfat_read_rootdir(root_inode);
    if (err)
    {
        goto error_iput;
    }

    if (sbi->state & EXFAT_SB_DIRTY)
    {
        fs_log_warning("exFAT: filesystem is not clean\n");

        /* FIXME: we need to do something? */
        /* exFAT state dirty already,
         * Microsoft deal method :
         * exFAT can readonly, can't write any data.
         * 2009.8.9  By sunny
         */
        sb->s_flags = MS_RDONLY;
    }
    sb->s_root = d_alloc_root(root_inode);
    if (!sb->s_root)
    {
        fs_log_error("exFAT: couldn't get root dentry\n");
        goto error_iput;
    }
    sb->s_root->d_op = &exfat_dentry_ops;

    return 0;

error_iput:
    iput(root_inode);
error:
    brelse(bh);

    exfat_put_super(sb);

    return err;
}