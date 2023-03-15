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
#ifndef __UDF_FS_SB_H__
#define __UDF_FS_SB_H__

#pragma pack(1)

#define UDF_MAX_BLOCK_LOADED    8

#define UDF_TYPE1_MAP15         0x1511U
#define UDF_VIRTUAL_MAP15       0x1512U
#define UDF_VIRTUAL_MAP20       0x2012U
#define UDF_SPARABLE_MAP15      0x1522U

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
    } s_type_specific;
    __u32(*s_partition_func)(struct super_block *, __u32, __u16, __u32);
    __u16   s_volumeseqnum;
    __u16   s_partition_flags;
};

#pragma pack()

struct udf_sb_info
{
    struct udf_part_map *s_partmaps;
    __u8            s_volident[32];

    /* Overall info */
    __u16           s_partitions;
    __u16           s_partition;

    /* Sector headers */
    __s32           s_session;
    __u32           s_anchor[4];
    __u32           s_lastblock;

    struct buffer_head  *s_lvidbh;

    /* Default permissions */
    mode_t          s_umask;
    gid_t           s_gid;
    uid_t           s_uid;

    /* Root Info */
    struct timespec     s_recordtime;

    /* Fileset Info */
    __u16           s_serialnum;

    /* highest UDF revision we have recorded to this media */
    __u16           s_udfrev;

    /* Miscellaneous flags */
    __u32           s_flags;

    /* Encoding info */
    struct nls_table    *s_nls_map;

    /* VAT inode */
    struct inode        *s_vat;

    //struct mutex      s_alloc_mutex;
};

#endif /* __UDF_FS_SB_H__ */