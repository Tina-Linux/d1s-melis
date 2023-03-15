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
#ifndef __CDFS_I_H__
#define __CDFS_I_H__

#include "fstypedef.h"
#include "nls.h"

/*
 * The isofs filesystem constants/structures
 */

/* This part borrowed from the bsd386 isofs */
#define ISODCL(from, to) (to - from + 1)

struct iso_volume_descriptor
{
    char type       [ISODCL(1, 1)]; /* 711 */
    char id         [ISODCL(2, 6)];
    char version    [ISODCL(7, 7)];
    char data       [ISODCL(8, 2048)];
};

/* volume descriptor types */
#define ISO_VD_PRIMARY          1
#define ISO_VD_SUPPLEMENTARY    2
#define ISO_VD_END              255

#define ISO_STANDARD_ID         "CD001"

struct iso_primary_descriptor
{
    char type                   [ISODCL(1,   1)];    /* 711 */
    char id                     [ISODCL(2,   6)];
    char version                [ISODCL(7,   7)];    /* 711 */
    char unused1                [ISODCL(8,   8)];
    char system_id              [ISODCL(9,  40)];    /* achars */
    char volume_id              [ISODCL(41,  72)];   /* dchars */
    char unused2                [ISODCL(73,  80)];
    char volume_space_size      [ISODCL(81,  88)];   /* 733 */
    char unused3                [ISODCL(89, 120)];
    char volume_set_size        [ISODCL(121, 124)];  /* 723 */
    char volume_sequence_number [ISODCL(125, 128)];  /* 723 */
    char logical_block_size     [ISODCL(129, 132)];  /* 723 */
    char path_table_size        [ISODCL(133, 140)];  /* 733 */
    char type_l_path_table      [ISODCL(141, 144)];  /* 731 */
    char opt_type_l_path_table  [ISODCL(145, 148)];  /* 731 */
    char type_m_path_table      [ISODCL(149, 152)];  /* 732 */
    char opt_type_m_path_table  [ISODCL(153, 156)];  /* 732 */
    char root_directory_record  [ISODCL(157, 190)];  /* 9.1 */
    char volume_set_id          [ISODCL(191, 318)];  /* dchars */
    char publisher_id           [ISODCL(319, 446)];  /* achars */
    char preparer_id            [ISODCL(447, 574)];  /* achars */
    char application_id         [ISODCL(575, 702)];  /* achars */
    char copyright_file_id      [ISODCL(703, 739)];  /* 7.5 dchars */
    char abstract_file_id       [ISODCL(740, 776)];  /* 7.5 dchars */
    char bibliographic_file_id  [ISODCL(777, 813)];  /* 7.5 dchars */
    char creation_date          [ISODCL(814, 830)];  /* 8.4.26.1 */
    char modification_date      [ISODCL(831, 847)];  /* 8.4.26.1 */
    char expiration_date        [ISODCL(848, 864)];  /* 8.4.26.1 */
    char effective_date         [ISODCL(865, 881)];  /* 8.4.26.1 */
    char file_structure_version [ISODCL(882, 882)];  /* 711 */
    char unused4                [ISODCL(883, 883)];
    char application_data       [ISODCL(884, 1395)];
    char unused5                [ISODCL(1396, 2048)];
};

/* Almost the same as the primary descriptor but two fields are specified */
struct iso_supplementary_descriptor
{
    char type                   [ISODCL(1,   1)];    /* 711 */
    char id                     [ISODCL(2,   6)];
    char version                [ISODCL(7,   7)];    /* 711 */
    char flags                  [ISODCL(8,   8)];    /* 853 */
    char system_id              [ISODCL(9,  40)];    /* achars */
    char volume_id              [ISODCL(41,  72)];   /* dchars */
    char unused2                [ISODCL(73,  80)];
    char volume_space_size      [ISODCL(81,  88)];   /* 733 */
    char escape                 [ISODCL(89, 120)];   /* 856 */
    char volume_set_size        [ISODCL(121, 124)];  /* 723 */
    char volume_sequence_number [ISODCL(125, 128)];  /* 723 */
    char logical_block_size     [ISODCL(129, 132)];  /* 723 */
    char path_table_size        [ISODCL(133, 140)];  /* 733 */
    char type_l_path_table      [ISODCL(141, 144)];  /* 731 */
    char opt_type_l_path_table  [ISODCL(145, 148)];  /* 731 */
    char type_m_path_table      [ISODCL(149, 152)];  /* 732 */
    char opt_type_m_path_table  [ISODCL(153, 156)];  /* 732 */
    char root_directory_record  [ISODCL(157, 190)];  /* 9.1 */
    char volume_set_id          [ISODCL(191, 318)];  /* dchars */
    char publisher_id           [ISODCL(319, 446)];  /* achars */
    char preparer_id            [ISODCL(447, 574)];  /* achars */
    char application_id         [ISODCL(575, 702)];  /* achars */
    char copyright_file_id      [ISODCL(703, 739)];  /* 7.5 dchars */
    char abstract_file_id       [ISODCL(740, 776)];  /* 7.5 dchars */
    char bibliographic_file_id  [ISODCL(777, 813)];  /* 7.5 dchars */
    char creation_date          [ISODCL(814, 830)];  /* 8.4.26.1 */
    char modification_date      [ISODCL(831, 847)];  /* 8.4.26.1 */
    char expiration_date        [ISODCL(848, 864)];  /* 8.4.26.1 */
    char effective_date         [ISODCL(865, 881)];  /* 8.4.26.1 */
    char file_structure_version [ISODCL(882, 882)];  /* 711 */
    char unused4                [ISODCL(883, 883)];
    char application_data       [ISODCL(884, 1395)];
    char unused5                [ISODCL(1396, 2048)];
};


#define HS_STANDARD_ID      "CDROM"

struct  hs_volume_descriptor
{
    char foo            [ISODCL(1,   8)];    /* 733 */
    char type           [ISODCL(9,   9)];    /* 711 */
    char id             [ISODCL(10,  14)];
    char version        [ISODCL(15,  15)];   /* 711 */
    char data[ISODCL(16, 2048)];
};


struct hs_primary_descriptor
{
    char foo                    [ISODCL(1,   8)];    /* 733 */
    char type                   [ISODCL(9,   9)];    /* 711 */
    char id                     [ISODCL(10,  14)];
    char version                [ISODCL(15,  15)];   /* 711 */
    char unused1                [ISODCL(16,  16)];   /* 711 */
    char system_id              [ISODCL(17,  48)];   /* achars */
    char volume_id              [ISODCL(49,  80)];   /* dchars */
    char unused2                [ISODCL(81,  88)];   /* 733 */
    char volume_space_size      [ISODCL(89,  96)];   /* 733 */
    char unused3                [ISODCL(97, 128)];   /* 733 */
    char volume_set_size        [ISODCL(129, 132)];  /* 723 */
    char volume_sequence_number [ISODCL(133, 136)];  /* 723 */
    char logical_block_size     [ISODCL(137, 140)];  /* 723 */
    char path_table_size        [ISODCL(141, 148)];  /* 733 */
    char type_l_path_table      [ISODCL(149, 152)];  /* 731 */
    char unused4                [ISODCL(153, 180)];  /* 733 */
    char root_directory_record  [ISODCL(181, 214)];  /* 9.1 */
};

/* We use this to help us look up the parent inode numbers. */

struct iso_path_table
{
    unsigned char   name_len[2];    /* 721 */
    char            extent[4];      /* 731 */
    char            parent[2];      /* 721 */
    char            name[0];
} __attribute__((packed));


/* high sierra is identical to iso, except that the date is only 6 bytes, and
   there is an extra reserved byte after the flags */

struct iso_directory_record
{
    char length                 [ISODCL(1, 1)];     /* 711 */
    char ext_attr_length        [ISODCL(2, 2)];     /* 711 */
    char extent                 [ISODCL(3, 10)];    /* 733 */
    char size                   [ISODCL(11, 18)];   /* 733 */
    char date                   [ISODCL(19, 25)];   /* 7 by 711 */
    char flags                  [ISODCL(26, 26)];
    char file_unit_size         [ISODCL(27, 27)];   /* 711 */
    char interleave             [ISODCL(28, 28)];   /* 711 */
    char volume_sequence_number [ISODCL(29, 32)];   /* 723 */
    unsigned char name_len      [ISODCL(33, 33)];   /* 711 */
    char name           [0];
} __attribute__((packed));

#define ISOFS_BLOCK_BITS            11
#define ISOFS_BLOCK_SIZE            2048

#define ISOFS_BUFFER_SIZE(INODE)    ((INODE)->i_sb->s_blocksize)
#define ISOFS_BUFFER_BITS(INODE)    ((INODE)->i_sb->s_blocksize_bits)

#endif /* __CDFS_I_H__ */