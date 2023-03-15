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
#ifndef __SMBR_H__
#define __SMBR_H__

#define MSDOS_LABEL_MAGIC       0xAA55

enum
{
    /* These three have identical behaviour; use the second one if DOS FDISK gets
       confused about extended/logical partitions starting past cylinder 1023. */
    DOS_EXTENDED_PARTITION    = 5,
    LINUX_EXTENDED_PARTITION  = 0x85,
    WIN98_EXTENDED_PARTITION  = 0x0f,

    SUN_WHOLE_DISK            = DOS_EXTENDED_PARTITION,

    LINUX_SWAP_PARTITION      = 0x82,
    LINUX_DATA_PARTITION      = 0x83,
    LINUX_LVM_PARTITION       = 0x8e,
    LINUX_RAID_PARTITION      = 0xfd,  /* autodetect RAID partition */

    SOLARIS_X86_PARTITION     = LINUX_SWAP_PARTITION,
    NEW_SOLARIS_X86_PARTITION = 0xbf,

    DM6_AUX1PARTITION         = 0x51, /* no DDO:  use xlated geom */
    DM6_AUX3PARTITION         = 0x53, /* no DDO:  use xlated geom */
    DM6_PARTITION             = 0x54, /* has DDO: use xlated geom & offset */
    EZD_PARTITION             = 0x55, /* EZ-DRIVE */

    FREEBSD_PARTITION         = 0xa5, /* FreeBSD Partition ID */
    OPENBSD_PARTITION         = 0xa6, /* OpenBSD Partition ID */
    NETBSD_PARTITION          = 0xa9, /* NetBSD Partition ID */
    BSDI_PARTITION            = 0xb7, /* BSDI Partition ID */
    MINIX_PARTITION           = 0x81, /* Minix Partition ID */
    UNIXWARE_PARTITION        = 0x63, /* Same as GNU_HURD and SCO Unix */
};

struct partition
{
    unsigned char boot_ind;       /* 0x80 - active */
    unsigned char head;           /* starting head */
    unsigned char sector;         /* starting sector */
    unsigned char cyl;            /* starting cylinder */
    unsigned char sys_ind;        /* What partition type */
    unsigned char end_head;       /* end head */
    unsigned char end_sector;     /* end sector */
    unsigned char end_cyl;        /* end cylinder */
    unsigned int  start_sect;     /* starting sector counting from 0 */
    unsigned int  nr_sects;       /* nr of sectors in partition */
} __attribute__((__packed__));

#define ADDPART_FLAG_NONE       0
#define ADDPART_FLAG_RAID       1
#define ADDPART_FLAG_WHOLEDISK  2

/*
 * add_gd_partition adds a partitions details to the devices partition
 * description.
 */
enum { MAX_PART = 16 };

struct part_geometry
{
    unsigned int from;
    unsigned int size;
    int flags;
};

struct parsed_partitions
{
    char name[32];
    struct part_geometry parts[MAX_PART];
    int next;
    int limit;
};

#endif  /*  __SMBR_H__  */