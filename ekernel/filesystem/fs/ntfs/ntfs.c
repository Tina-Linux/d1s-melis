
#include "ntfs.h"
#include "fsys_debug.h"
#include "nls.h"
#include "volume.h"
//#include "mkntfs.h"
#include "errno.h"
#include "fsys_libs.h"
#include "blk_dev.h"

struct file_system_type ntfs_fs_type;

int ntfs_format(__hdle p_part, __hdle fmtpara)
{
    int res;

    fs_log_trace01("ntfs fomat enter...\n");

    res = EPDK_FAIL;  //mkntfs(p_part);

    fs_log_trace01("ntfs fomat return(%x)\n", res);

    return res;
}

int ntfs_identify(__hdle pPart)
{
    int res;
    int sctsize;

    fs_log_trace01("ntfs identify enter...\n");

    /* First of all, get the hardware sector size for this device.
     * we can't read data size smaller then hardware sector size.
     * sector size must be a power of two, and between 512 and PAGE_SIZE.
     */
    sctsize = dev_hardsect_size(pPart);
    if (sctsize > PAGE_SIZE || sctsize < 512 || !is_power_of_2(sctsize))
    {
        fs_log_error("invalid hardware sector size.\n");
        return  EPDK_FALSE;
    }

    /* NTFS just support 512 byte size sector device now,
     * By Sunny at 2010.09.12.
     */
    if (sctsize != 512)
    {
        fs_log_info("ntfs just support 512 byte sector device.\n");
        return  EPDK_FALSE;
    }

    /* Now set the device block size to hardware sctsize for correct read. */
    if (ntfs_device_block_size_set(pPart, sctsize))
    {
        fs_log_error("Failed to set the device block size.");
        return EPDK_FALSE;
    }
    res = ntfs_part_identify(pPart);

    fs_log_trace01("ntfs identify return(%x)\n", res);
    return res;
}

int ntfs_mount(__hdle part)
{
    int res;

#if defined CONFIG_FSYS_NTFS_RW
    int flags = MS_RECOVER | MS_IGNORE_HIBERFILE;
#else
    int flags = MS_RDONLY;
#endif

    fs_log_trace01("fsmount:\n");

    res = get_sb_bdev(&ntfs_fs_type, flags, part, &flags, ntfs_fill_super);

    fs_log_trace01("O%d\n", res);

    return !res ? EPDK_OK : EPDK_FAIL;
}

int ntfs_unmount(struct super_block *sb, int force)
{
    int res;

    fs_log_trace01("ntfs unmount...\n");

    res = deactivate_super(sb, force);

    fs_log_trace01("O\n");
    return !res ? EPDK_OK : EPDK_FAIL;
}

struct file_system_type ntfs_fs_type =
{
    .name       = "ntfs",
    .mount      = ntfs_mount,
    .unmount    = ntfs_unmount,
    .identify   = ntfs_identify,
    .format     = ntfs_format,
    .fs_flags   = FS_REQUIRES_DEV,
};

__s32 fsys_ntfs_init(void)
{
    int err = 0;

    err = esFSYS_fsreg((__hdle)&ntfs_fs_type);
    if (!err)
    {
        fs_log_trace01("NTFS driver registered successfully.");
        return 0;
    }

    fs_log_error("NTFS: Failed to register NTFS filesystem driver!\n");
    err = -ENOMEM;
    return err;
}

__s32 fsys_ntfs_exit(void)
{
    int res;

    res = esFSYS_fsunreg(&ntfs_fs_type);

    //if(nls)
    //   unload_nls(nls);

    return res;
}


