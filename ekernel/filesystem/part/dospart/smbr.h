/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : smbr.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-15
* Descript: mbr parser header file.
* Update  : date                auther      ver     notes
*           2011-3-15 15:05:27  Sunny       1.0     Create this file.
*********************************************************************************************************
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

