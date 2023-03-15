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
#ifndef __EXFAT_FS_H__
#define __EXFAT_FS_H__

#include "fstypedef.h"

#define EXFAT_SUPER_MAGIC       0x54467865      /* exFAT magic */

#define EXFAT_OEM_ID            "EXFAT   "
#define EXFAT_BOOT_SIG          0xaa55

#define EXFAT_SB_BLOCKS         12
#define EXFAT_SB_DATA_BLOCKS    11
#define EXFAT_SB_CKSUM_BLOCK    11

/* the block number of mirror super block  */
#define EXFAT_SB_MIRROR         12

/* State flags. */
#define EXFAT_SB_DIRTY          0x0002
#define EXFAT_SB_CLEAR          0x0000

struct exfat_super_block
{
    __u8    jmp_boot[3];        /* boot strap short or near jump */
    __u8    oem_id[8];          /* oem-id */
    __u8    unused0;            /* 0x00... */
    __u32   unused1[13];

    /*0x40*/__le64  start_sector;   /* start sector of partition */
    __le64  nr_sectors;         /* number of sectors of partition */
    /*0x50*/__le32  fat_blocknr;    /* start blocknr of FAT */
    __le32  fat_block_counts;   /* number of FAT blocks */
    __le32  clus_blocknr;       /* start blocknr of cluster */
    __le32  total_clusters;     /* number of total clusters */
    /*0x60*/__le32  rootdir_clusnr; /* start clusnr of rootdir */
    __le32  serial_number;      /* volume serial number */
    __u8    xxxx01;             /* ??? (0x00 or any value (?)) */
    __u8    xxxx02;             /* ??? (0x01 or 0x00 (?)) */
    __le16  state;              /* state of this volume */
    __u8    blocksize_bits;     /* bits of block size */
    __u8    block_per_clus_bits;/* bits of blocks per cluster */
    __u8    xxxx03;             /* ??? (0x01 or 0x00 (?)) */
    __u8    xxxx04;             /* ??? (0x80 or any value (?)) */
    /*0x70*/__u8    allocated_percent;  /* percentage of allocated space (?) */
    __u8    xxxx05[397];        /* ??? (0x00...) */
    __u16   signature;          /* 0xaa55 */    /* must rethink le and be different */
} __attribute__((__packed__));

/* FAT entry size */
#define EXFAT_ENT_BITS      2
#define EXFAT_ENT_SIZE      (1 << EXFAT_ENT_BITS)

/* Start of data cluster's entry (number of reserved clusters) */
#define EXFAT_START_ENT     2

/* Values for FAT entry. */
#define EXFAT_ENT_FREE      0x00000000  /* free entry */
#define EXFAT_ENT_BAD       0xfffffff7  /* bad cluster */
#define EXFAT_ENT_EOF       0xffffffff  /* end of entry */

/* Value for bit allocated state */
#define EXFAT_BIT_ALLOCATED 1
#define EXFAT_BIT_FREE      0


/* Chunk size in directory. */
#define EXFAT_CHUNK_BITS        5
#define EXFAT_CHUNK_SIZE        (1 << EXFAT_CHUNK_BITS)
#define EXFAT_MIN_CHUNK_NUMBER  3

/* Type of chunk in directory. */
#define EXFAT_TYPE_VALID    0x80
#define EXFAT_TYPE_SUBCHUNK 0x40

/* some special chunk type */
#define EXFAT_TYPE_EOD      0x00
#define __EXFAT_TYPE_BITMAP 0x01
#define EXFAT_TYPE_BITMAP   (__EXFAT_TYPE_BITMAP | EXFAT_TYPE_VALID)
#define __EXFAT_TYPE_UPCASE 0x02
#define EXFAT_TYPE_UPCASE   (__EXFAT_TYPE_UPCASE | EXFAT_TYPE_VALID)
#define __EXFAT_TYPE_LABEL  0x03
#define EXFAT_TYPE_LABEL    (__EXFAT_TYPE_LABEL | EXFAT_TYPE_VALID)
//#define __EXFAT_TYPE_XXXX04   0x04
//#define EXFAT_TYPE_XXXX04 (__EXFAT_TYPE_XXXX04 | EXFAT_TYPE_VALID)
#define __EXFAT_TYPE_DIRENT 0x05
#define EXFAT_TYPE_DIRENT   (__EXFAT_TYPE_DIRENT | EXFAT_TYPE_VALID)

/* Type of sub chunk in directory. */
#define __EXFAT_TYPE_DATA   (0x00 | EXFAT_TYPE_SUBCHUNK)
#define EXFAT_TYPE_DATA     (__EXFAT_TYPE_DATA | EXFAT_TYPE_VALID)
#define __EXFAT_TYPE_NAME   (0x01 | EXFAT_TYPE_SUBCHUNK)
#define EXFAT_TYPE_NAME     (__EXFAT_TYPE_NAME | EXFAT_TYPE_VALID)

/* Chunk for free space bitmap. */
struct exfat_chunk_bitmap
{
    __u8    type;           /* 0x01 */
    __u8    xxxx01[19];     /* ??? (0x00...)*/
    __le32  clusnr;         /* start cluster number of bitmap */
    __le64  size;           /* size of bitmap */
};

/* Chunk for upper-case table. */
struct exfat_chunk_upcase
{
    __u8    type;           /* 0x02 */
    __u8    xxxx01[3];      /* ??? (0x00...) */
    __le32  checksum;       /* checksum of upcase table */
    __u8    xxxx02[12];     /* ??? (0x00...) */
    __le32  clusnr;         /* start cluster number of table */
    __le64  size;           /* size of table */
};

/* Maximum name length in a chunk. */
#define EXFAT_CHUNK_NAME_SIZE   15

/* Chunk for volume lable. */
struct exfat_chunk_label
{
    __u8    type;           /* 0x03 */
    __u8    len;            /* length of volume lable */
    __le16  name[EXFAT_CHUNK_NAME_SIZE];/* name of volume label */
};

/* Attribute flags. */
#define EXFAT_ATTR_RO       0x0001
#define EXFAT_ATTR_HIDDEN   0x0002
#define EXFAT_ATTR_SYSTEM   0x0004
#define EXFAT_ATTR_VOLUME   0x0008
#define EXFAT_ATTR_DIR      0x0010
#define EXFAT_ATTR_ARCH     0x0020

/* Chunk for directory entry. */
struct exfat_chunk_dirent
{
    __u8    type;           /* 0x05 */
    __u8    sub_chunks;     /* number of sub chunks */
    __le16  checksum;       /* checksum of these chunks */
    __le16  attrib;         /* attribute flags */
    __le16  xxxx01;         /* ??? (0x00...) */
    __le16  crtime, crdate; /* creation time/date */
    __le16  mtime, mdate;   /* modification time/date */
    __le16  atime, adate;   /* access time/date */
    __u8    crtime_cs;      /* creation centi seconds time */
    __u8    mtime_cs;       /* modification centi seconds time */
    __u8    xxxx02[10];     /* ??? (0x00...) */
};

/* Flag for cluster number. */
#define EXFAT_DATA_NORMAL       0x01    /* clusnr == 0, or FAT has these
                                         * cluster number */
#define EXFAT_DATA_CONTIGUOUS   0x03    /* clusters are contiguous (FAT may not
                                         * be updated yet) */

/* Maximum filename length */
#define EXFAT_MAX_NAMELEN   255U

/* Sub chunk for file name info and data clusters info. */
struct exfat_chunk_data
{
    __u8    type;           /* 0x40 */
    __u8    flag;           /* flag for cluster number */
    __u8    xxxx01;         /* ??? (0x00) */
    __u8    name_len;       /* length of filename */
    __le16  hash;           /* hash of up-cased filename */
    __le16  xxxx02;         /* ??? (0x00...) */
    __le64  size2;          /* ??? */
    __le32  xxxx03;         /* ??? (0x00...)*/
    __le32  clusnr;         /* start cluster number of data */
    __le64  size;           /* size of data */
};

/* Sub chunk for file name. */
struct exfat_chunk_name
{
    __u8    type;                       /* 0x41 */
    __u8    xxxx01;                     /* ??? (0x00) */
    __le16  name[EXFAT_CHUNK_NAME_SIZE];/* file name */
};

/* normal slot for exfat chuncks */
#define  EXFAT_SLOTS    21          /* FIXME : maybe use EXFAT_DE_MAX_CHUNKS, 
                                     * but is too big now */
struct exfat_dir_slot       /* maybe rename for exfat_de_chunk */
{
    __u8    type;           /* chunck type  */
    __u8    data[31];       /* data area    */
};

#define EXFAT_DELETE_CHUNK(type)    ((type) &= (~EXFAT_TYPE_VALID))

#define EXFAT_EOD_CHUNK(type)       ((type) == (EXFAT_TYPE_EOD))

#define EXFAT_VALID_CHUNK(type)     ((type)&(EXFAT_TYPE_VALID))

#define EXFAT_FREE_CHUNK(type)      (!EXFAT_VALID_CHUNK(type))

static inline __u32 __exfat_checksum32(__u32 sum, __u8 val)
{
    return ((sum << 31) | (sum >> 1)) + val;
}

static inline __u16 __exfat_checksum16(__u16 sum, __u8 val)
{
    return ((sum << 15) | (sum >> 1)) + val;
}

#endif /* __EXFAT_FS_H__ */