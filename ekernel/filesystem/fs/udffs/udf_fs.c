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