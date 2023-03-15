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
#ifndef  __CDFS_H__
#define  __CDFS_H__

#include "fstypedef.h"
#include "endians.h"
#include "fsunaligned.h"
#include "buffer_head.h"
#include "fs.h"
#include "cdfs_i.h"


#define ISOFS_SUPER_MAGIC   0x9660

/* ISO9600 extensions support config */
//#define       CONFIG_ZISOFS   /* Compression support              */
#define     CONFIG_JOLIET   /* Microsoft extension joliet       */
//#define       CONFIG_ROCK     /* Unix_style extension rock ridge  */
//#define       CONFIG_EXPORT   /* Export ISOFS, used for network filesystem*/

enum isofs_file_format
{
    isofs_file_normal = 0,
    isofs_file_sparse = 1,
    isofs_file_compressed = 2,
};

/*
 * iso fs inode data in memory
 */
struct iso_inode_info
{
    unsigned long i_iget5_block;
    unsigned long i_iget5_offset;
    unsigned int  i_first_extent;
    unsigned char i_file_format;
    unsigned char i_format_parm[3];
    unsigned long i_next_section_block;
    unsigned long i_next_section_offset;
    off_t         i_section_size;
    struct inode  vfs_inode;
};

/*
 * iso9660 super-block data in memory
 */
struct isofs_sb_info
{
    unsigned long s_ninodes;
    unsigned long s_nzones;
    unsigned long s_firstdatazone;
    unsigned long s_log_zone_size;
    unsigned long s_max_size;

    unsigned char s_high_sierra; /* A simple flag */
    unsigned char s_mapping;
    int           s_rock_offset; /* offset of SUSP fields within SU area */
    unsigned char s_rock;
    unsigned char s_joliet_level;
    unsigned char s_utf8;
    unsigned char s_cruft; /* Broken disks with high
                              byte of length containing
                              junk */
    unsigned char s_unhide;
    unsigned char s_nosuid;
    unsigned char s_nodev;
    unsigned char s_nocompress;
    unsigned char s_hide;
    unsigned char s_showassoc;

    mode_t s_mode;
    gid_t s_gid;
    uid_t s_uid;
    struct nls_table *s_nls_iocharset; /* Native language support table */
};

static inline struct isofs_sb_info *ISOFS_SB(struct super_block *sb)
{
    return sb->s_fs_info;
}

static inline struct iso_inode_info *ISOFS_I(struct inode *inode)
{
    return container_of(inode, struct iso_inode_info, vfs_inode);
}

static inline int isonum_711(char *p)
{
    return *(u8 *)p;
}
static inline int isonum_712(char *p)
{
    return *(s8 *)p;
}
static inline unsigned int isonum_721(char *p)
{
    return le16_to_cpu(get_unaligned((__le16 *)p));
}
static inline unsigned int isonum_722(char *p)
{
    return be16_to_cpu(get_unaligned((__le16 *)p));
}
static inline unsigned int isonum_723(char *p)
{
    /* Ignore bigendian datum due to broken mastering programs */
    return le16_to_cpu(get_unaligned((__le16 *)p));
}
static inline unsigned int isonum_731(char *p)
{
    return le32_to_cpu(get_unaligned((__le32 *)p));
}
static inline unsigned int isonum_732(char *p)
{
    return be32_to_cpu(get_unaligned((__le32 *)p));
}

static /*inline*/ unsigned int isonum_733(char *p)
{
    /* Ignore bigendian datum due to broken mastering programs */

    __le32 temp = 0;
    __u32  value = 0;
    temp = get_unaligned((__le32 *)p);
    value = le32_to_cpu(temp);
    return value;
}
extern int iso_date(char *, int);

struct inode;       /* To make gcc happy */

extern int parse_rock_ridge_inode(struct iso_directory_record *, struct inode *);
extern int get_rock_ridge_filename(struct iso_directory_record *, char *, struct inode *);
extern int isofs_name_translate(struct iso_directory_record *, char *, struct inode *);

int get_joliet_filename(struct iso_directory_record *, unsigned char *, struct inode *);
int get_acorn_filename(struct iso_directory_record *, char *, struct inode *);

extern struct dentry *isofs_lookup(struct inode *, struct dentry *, struct nameidata *);
extern struct buffer_head *isofs_bread(struct inode *, sector_t);
extern int isofs_get_blocks(struct inode *inode,
                            sector_t iblock,
                            sector_t *phys,
                            unsigned long *max_blocks);
extern int isofs_get_block(struct inode *inode,
                           int iblock,
                           unsigned long *max_blocks,
                           sector_t *phys,
                           int create);

extern struct inode *isofs_iget(struct super_block *sb,
                                unsigned long block,
                                unsigned long offset);

/* Because the inode number is no longer relevant to finding the
 * underlying meta-data for an inode, we are free to choose a more
 * convenient 32-bit number as the inode number.  The inode numbering
 * scheme was recommended by Sergey Vlasov and Eric Lammerts. */
static inline unsigned long isofs_get_ino(unsigned long block,
        unsigned long offset,
        unsigned long bufbits)
{
    return (block << (bufbits - 5)) | (offset >> 5);
}

/* Every directory can have many redundant directory entries scattered
 * throughout the directory tree.  First there is the directory entry
 * with the name of the directory stored in the parent directory.
 * Then, there is the "." directory entry stored in the directory
 * itself.  Finally, there are possibly many ".." directory entries
 * stored in all the subdirectories.
 *
 * In order for the NFS get_parent() method to work and for the
 * general consistency of the dcache, we need to make sure the
 * "i_iget5_block" and "i_iget5_offset" all point to exactly one of
 * the many redundant entries for each directory.  We normalize the
 * block and offset by always making them point to the "."  directory.
 *
 * Notice that we do not use the entry for the directory with the name
 * that is located in the parent directory.  Even though choosing this
 * first directory is more natural, it is much easier to find the "."
 * entry in the NFS get_parent() method because it is implicitly
 * encoded in the "extent + ext_attr_length" fields of _all_ the
 * redundant entries for the directory.  Thus, it can always be
 * reached regardless of which directory entry you have in hand.
 *
 * This works because the "." entry is simply the first directory
 * record when you start reading the file that holds all the directory
 * records, and this file starts at "extent + ext_attr_length" blocks.
 * Because the "." entry is always the first entry listed in the
 * directories file, the normalized "offset" value is always 0.
 *
 * You should pass the directory entry in "de".  On return, "block"
 * and "offset" will hold normalized values.  Only directories are
 * affected making it safe to call even for non-directory file
 * types. */
static inline void isofs_normalize_block_and_offset(struct iso_directory_record *de,
        unsigned long *block,
        unsigned long *offset)
{
    /* Only directories are normalized. */
    if (de->flags[0] & 2)
    {
        *offset = 0;
        *block = (unsigned long)isonum_733(de->extent)
                 + (unsigned long)isonum_711(de->ext_attr_length);
    }
}

extern const struct inode_operations isofs_dir_inode_operations;
extern const struct file_operations isofs_dir_operations;
extern const struct file_operations isofs_file_operations;

#ifdef  CONFIG_ROCK
extern const struct address_space_operations isofs_symlink_aops;
#endif

#ifdef  CONFIG_EXPORT
extern const struct export_operations isofs_export_ops;
#endif

/* cdfs_inode.c */
extern struct inode *isofs_alloc_inode(struct super_block *sb);
extern void isofs_destroy_inode(struct inode *inode);
extern int  isofs_init_inodecache(void);
extern void isofs_destroy_inodecache(void);
extern void isofs_read_inode(struct inode *inode);

/* cdfs_super.c */
extern __s32 isofs_fill_super(struct super_block *s, void *data, __s32 silent);

/* cdfs_lowlevel.c */
extern unsigned int isofs_get_last_session(struct super_block *sb);

#endif  /* __CDFS_H__ */