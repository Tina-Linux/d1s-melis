/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : udf_fs.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-17
* Descript: OSTA-UDF(tm) filesystem.
* Update  : date                auther      ver     notes
*           2011-3-17 9:00:19   Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "udf_decl.h"
#include "udf_fs.h"
#include "udf_sb.h"
#include "udf_fs_i.h"
#include "slab.h"
#include "nls.h"
#include "buffer_head.h"
#include "fsys_debug.h"

struct file_system_type udf_fs_type;

int udf_format(__hdle p_part, __hdle fmtpara)
{
    int res;

    fs_log_trace01("udf fomat enter...\n");

    /* not support format now */
    res = EPDK_FALSE;

    fs_log_trace01("udf fomat return(%x)\n", res);

    return res;
}

int udf_identify(__hdle pPart)
{
    __s32 res = EPDK_TRUE;

    /* must rethink this method */
    struct super_block *sb;
    struct udf_sb_info *sbi;

    sb  = kmalloc(sizeof(struct super_block), GFP_KERNEL);
    if (!sb)
    {
        fs_log_error("udf identify kmalloc failed\n");
        return EPDK_FALSE;
    }
    sbi = kmalloc(sizeof(struct udf_sb_info), GFP_KERNEL);
    if (!sbi)
    {
        fs_log_error("udf identify kmalloc failed\n");
        kfree(sb);
        return EPDK_FALSE;
    }
    memset(sb,  0x00, sizeof(struct super_block));
    memset(sbi, 0x00, sizeof(struct udf_sb_info));

    sb->s_fs_info = sbi;

    fs_log_trace01("udf identify enter...\n");

    /* set super block useful field here */
    sb->s_blocksize = 2048;
    sb->s_blocksize_bits = 11;
    sb->s_part = pPart;

    sb->s_blocksize = sb_min_blocksize(sb, sb->s_blocksize);

    /* Get last session start block number */
    sbi->s_session = udf_get_last_session(sb);

    /* read volume recognition sequences */
    if (udf_check_valid(sb))
    {
        udf_info("UDF-fs: No VRS found\n");
        res = EPDK_FALSE;
    }

    fs_log_trace01("udf identify return(%x)\n", res);
    kfree(sb);
    kfree(sbi);
    return res;
}

int udf_mount(__hdle part)
{
    int res;

#if defined CONFIG_FSYS_UDFFS_RW
    int flags = 0;
#else
    int flags = MS_RDONLY;
#endif

    fs_log_trace01("udf fs mount:\n");

    res = get_sb_bdev(&udf_fs_type, flags, part, NULL, udf_fill_super);

    if (!res)
    {
        char *partName;
        esFSYS_pioctrl(part, PART_IOC_SYS_GETNAME, 0, &partName);
    }

    fs_log_trace01("O%d\n", res);

    return !res ? EPDK_OK : EPDK_FAIL;
}

int udf_unmount(struct super_block *sb, int force)
{
    int res;

    fs_log_trace01("udf fs unmount...\n");

    res = deactivate_super(sb, force);

    fs_log_trace01("O\n");
    return !res ? EPDK_OK : EPDK_FAIL;
}

struct file_system_type udf_fs_type =
{
    .name       = "udf",
    .mount      = udf_mount,
    .unmount    = udf_unmount,
    .identify   = udf_identify,
    .format     = udf_format,
    .fs_flags   = FS_REQUIRES_DEV,
};

/* ----------------------- init && exit --------------------*/
__s32 fsys_udffs_init(void)
{
    int ret;

    /* udf inode cache init */
    ret = udf_init_inodecache();
    if (ret)
    {
        return EPDK_FAIL;
    }

    /* register udf fs */
    ret = esFSYS_fsreg((__hdle)&udf_fs_type);
    if (ret == EPDK_FAIL)
    {
        udf_destroy_inodecache();
    }
    return ret;
}

__s32 fsys_udffs_exit(void)
{
    esFSYS_fsunreg((__hdle)&udf_fs_type);

    udf_destroy_inodecache();

    return EPDK_OK;
}


