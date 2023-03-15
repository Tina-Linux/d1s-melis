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
#ifndef __UDF_SB_H__
#define __UDF_SB_H__

/* Since UDF 2.01 is ISO 13346 based... */
#define UDF_SUPER_MAGIC         0x15013346

#define UDF_MAX_READ_VERSION        0x0250
#define UDF_MAX_WRITE_VERSION       0x0201

#define UDF_FLAG_USE_EXTENDED_FE    0
#define UDF_VERS_USE_EXTENDED_FE    0x0200
#define UDF_FLAG_USE_STREAMS        1
#define UDF_VERS_USE_STREAMS        0x0200
#define UDF_FLAG_USE_SHORT_AD       2
#define UDF_FLAG_USE_AD_IN_ICB      3
#define UDF_FLAG_USE_FILE_CTIME_EA  4
#define UDF_FLAG_STRICT         5
#define UDF_FLAG_UNDELETE       6
#define UDF_FLAG_UNHIDE         7
#define UDF_FLAG_VARCONV        8
#define UDF_FLAG_NLS_MAP        9
#define UDF_FLAG_UTF8           10
#define UDF_FLAG_UID_FORGET     11    /* save -1 for uid to disk */
#define UDF_FLAG_UID_IGNORE     12    /* use sb uid instead of on disk uid */
#define UDF_FLAG_GID_FORGET     13
#define UDF_FLAG_GID_IGNORE     14
#define UDF_FLAG_UID_SET    15
#define UDF_FLAG_GID_SET    16
#define UDF_FLAG_SESSION_SET    17
#define UDF_FLAG_LASTBLOCK_SET  18
#define UDF_FLAG_BLOCKSIZE_SET  19

#define UDF_PART_FLAG_UNALLOC_BITMAP    0x0001
#define UDF_PART_FLAG_UNALLOC_TABLE 0x0002
#define UDF_PART_FLAG_FREED_BITMAP  0x0004
#define UDF_PART_FLAG_FREED_TABLE   0x0008
#define UDF_PART_FLAG_READ_ONLY     0x0010
#define UDF_PART_FLAG_WRITE_ONCE    0x0020
#define UDF_PART_FLAG_REWRITABLE    0x0040
#define UDF_PART_FLAG_OVERWRITABLE  0x0080

#define UDF_MAX_BLOCK_LOADED    8

#define UDF_TYPE1_MAP15         0x1511U
#define UDF_VIRTUAL_MAP15       0x1512U
#define UDF_VIRTUAL_MAP20       0x2012U
#define UDF_SPARABLE_MAP15      0x1522U
#define UDF_METADATA_MAP25      0x2511U

#define UDF_INVALID_MODE        ((mode_t)-1)

//#pragma pack(1) /* XXX(hch): Why?  This file just defines in-core structures */

struct udf_meta_data
{
    __u32   s_meta_file_loc;
    __u32   s_mirror_file_loc;
    __u32   s_bitmap_file_loc;
    __u32   s_alloc_unit_size;
    __u16   s_align_unit_size;
    __u8    s_dup_md_flag;
    struct inode *s_metadata_fe;
    struct inode *s_mirror_fe;
    struct inode *s_bitmap_fe;
};

struct udf_sparing_data
{
    __u16   s_packet_len;
    struct buffer_head *s_spar_map[4];
};

struct udf_virtual_data
{
    __u32   s_num_entries;
    __u16   s_start_offset;
};

struct udf_bitmap
{
    __u32           s_extLength;
    __u32           s_extPosition;
    __u16           s_nr_groups;
    struct buffer_head  **s_block_bitmap;
};

struct udf_part_map
{
    union
    {
        struct udf_bitmap   *s_bitmap;
        struct inode        *s_table;
    } s_uspace;
    union
    {
        struct udf_bitmap   *s_bitmap;
        struct inode        *s_table;
    } s_fspace;
    __u32   s_partition_root;
    __u32   s_partition_len;
    __u16   s_partition_type;
    __u16   s_partition_num;
    union
    {
        struct udf_sparing_data s_sparing;
        struct udf_virtual_data s_virtual;
        struct udf_meta_data s_metadata;
    } s_type_specific;
    __u32(*s_partition_func)(struct super_block *, __u32, __u16, __u32);
    __u16   s_volumeseqnum;
    __u16   s_partition_flags;
};

//#pragma pack()

struct udf_sb_info
{
    struct udf_part_map *s_partmaps;
    __u8            s_volume_ident[32];

    /* Overall info */
    __u16           s_partitions;
    __u16           s_partition;

    /* Sector headers */
    __s32           s_session;
    __u32           s_anchor;
    __u32           s_last_block;

    struct buffer_head  *s_lvid_bh;

    /* Default permissions */
    mode_t          s_umask;
    gid_t           s_gid;
    uid_t           s_uid;
    mode_t          s_fmode;
    mode_t          s_dmode;

    /* Root Info */
    struct timespec     s_record_time;

    /* Fileset Info */
    __u16           s_serial_number;

    /* highest UDF revision we have recorded to this media */
    __u16           s_udfrev;

    /* Miscellaneous flags */
    __u32           s_flags;

    /* Encoding info */
    struct nls_table    *s_nls_map;

    /* VAT inode */
    struct inode        *s_vat_inode;

    //  struct mutex        s_alloc_mutex;
    /* Protected by s_alloc_mutex */
    unsigned int        s_lvid_dirty;
};

static inline struct udf_sb_info *UDF_SB(struct super_block *sb)
{
    return (struct udf_sb_info *)(sb->s_fs_info);
}

struct logicalVolIntegrityDescImpUse *udf_sb_lvidiu(struct udf_sb_info *sbi);

int udf_compute_nr_groups(struct super_block *sb, u32 partition);

#define UDF_QUERY_FLAG(X,Y)         ( UDF_SB(X)->s_flags & ( 1 << (Y) ) )
#define UDF_SET_FLAG(X,Y)           ( UDF_SB(X)->s_flags |= ( 1 << (Y) ) )
#define UDF_CLEAR_FLAG(X,Y)         ( UDF_SB(X)->s_flags &= ~( 1 << (Y) ) )

#define UDF_UPDATE_UDFREV(X,Y)      ( ((Y) > UDF_SB_UDFREV(X)) ? UDF_SB_UDFREV(X) = (Y) : UDF_SB_UDFREV(X) )

#define UDF_SB_PARTMAPS(X)          ( UDF_SB(X)->s_partmaps )
#define UDF_SB_PARTTYPE(X,Y)        ( UDF_SB_PARTMAPS(X)[(Y)].s_partition_type )
#define UDF_SB_PARTROOT(X,Y)        ( UDF_SB_PARTMAPS(X)[(Y)].s_partition_root )
#define UDF_SB_PARTLEN(X,Y)         ( UDF_SB_PARTMAPS(X)[(Y)].s_partition_len )
#define UDF_SB_PARTVSN(X,Y)         ( UDF_SB_PARTMAPS(X)[(Y)].s_volumeseqnum )
#define UDF_SB_PARTNUM(X,Y)         ( UDF_SB_PARTMAPS(X)[(Y)].s_partition_num )
#define UDF_SB_TYPESPAR(X,Y)        ( UDF_SB_PARTMAPS(X)[(Y)].s_type_specific.s_sparing )
#define UDF_SB_TYPEVIRT(X,Y)        ( UDF_SB_PARTMAPS(X)[(Y)].s_type_specific.s_virtual )
#define UDF_SB_PARTFUNC(X,Y)        ( UDF_SB_PARTMAPS(X)[(Y)].s_partition_func )
#define UDF_SB_PARTFLAGS(X,Y)       ( UDF_SB_PARTMAPS(X)[(Y)].s_partition_flags )
#define UDF_SB_BITMAP(X,Y,Z,I)      ( UDF_SB_PARTMAPS(X)[(Y)].Z.s_bitmap->s_block_bitmap[I] )
#define UDF_SB_BITMAP_NR_GROUPS(X,Y,Z)      ( UDF_SB_PARTMAPS(X)[(Y)].Z.s_bitmap->s_nr_groups )

#define UDF_SB_VOLIDENT(X)          ( UDF_SB(X)->s_volident )
#define UDF_SB_NUMPARTS(X)          ( UDF_SB(X)->s_partitions )
#define UDF_SB_PARTITION(X)         ( UDF_SB(X)->s_partition )
#define UDF_SB_SESSION(X)           ( UDF_SB(X)->s_session )
#define UDF_SB_ANCHOR(X)            ( UDF_SB(X)->s_anchor )
#define UDF_SB_LASTBLOCK(X)         ( UDF_SB(X)->s_lastblock )
#define UDF_SB_LVIDBH(X)            ( UDF_SB(X)->s_lvidbh )
#define UDF_SB_LVID(X)              ( (struct logicalVolIntegrityDesc *)UDF_SB_LVIDBH(X)->b_data )
#define UDF_SB_LVIDIU(X)            ( (struct logicalVolIntegrityDescImpUse *)&(UDF_SB_LVID(X)->impUse[le32_to_cpu(UDF_SB_LVID(X)->numOfPartitions) * 2 * sizeof(uint32_t)/sizeof(uint8_t)]) )

#define UDF_SB_UMASK(X)             ( UDF_SB(X)->s_umask )
#define UDF_SB_GID(X)               ( UDF_SB(X)->s_gid )
#define UDF_SB_UID(X)               ( UDF_SB(X)->s_uid )
#define UDF_SB_RECORDTIME(X)        ( UDF_SB(X)->s_recordtime )
#define UDF_SB_SERIALNUM(X)         ( UDF_SB(X)->s_serialnum )
#define UDF_SB_UDFREV(X)            ( UDF_SB(X)->s_udfrev )
#define UDF_SB_FLAGS(X)             ( UDF_SB(X)->s_flags )
#define UDF_SB_VAT(X)               ( UDF_SB(X)->s_vat )


#endif /* __LINUX_UDF_SB_H */