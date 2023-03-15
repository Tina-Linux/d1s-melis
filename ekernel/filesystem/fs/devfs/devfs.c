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
#include "devfs.h"
#include "fsys_debug.h"
#include <sys_device.h>
#include <string.h>

struct list_head            devfs_suppers;
struct file_system_type     devfs_fs_type;

extern int32_t  devfs_fill_super(struct super_block *, void *, int32_t);
extern int32_t  devfs_init_inodecache(void);

int32_t devfs_format(__hdle p_part, __hdle fmtpara)
{
    return EPDK_FAIL;
}

int32_t devfs_identify(__hdle pPart)
{
    int32_t     res = EPDK_FALSE;
    char        *clsname;
    __hdle      dev;

    fs_log_trace01("devfs identify...");

    if (esFSYS_pioctrl(pPart, PART_IOC_SYS_GETDEV, 0, &dev) == EPDK_OK)
    {
        if (esDEV_Ioctl((__hdle)dev, DEV_IOC_SYS_GET_CLSNAME, 0, &clsname) == EPDK_OK)
        {
            if (!strcmp(clsname, DEV_CLASS_DMS))
            {
                res = EPDK_TRUE;
            }
        }
    }
    fs_log_trace01("O%d\n", res);

    return res;
}

int32_t devfs_mount(__hdle part)
{
    int32_t     res;

    fs_log_trace01("devfs mount...");

    res = get_sb_bdev(&devfs_fs_type, 0, part, NULL, devfs_fill_super);

    fs_log_trace01("O%d\n", res);

    return !res ? EPDK_OK : EPDK_FAIL;
}

int32_t devfs_unmount(struct super_block *sb, int force)
{
    int32_t     res;

    fs_log_trace01("devfs unmount...");

    res = deactivate_super(sb, force);

    fs_log_trace01("O\n");
    return !res ? EPDK_OK : EPDK_FAIL;
}

struct file_system_type devfs_fs_type =
{
    .name       = "melis_devfs",
    .mount      = devfs_mount,
    .unmount    = devfs_unmount,
    .identify   = devfs_identify,
    .format     = devfs_format,
    .fs_flags   = FS_REQUIRES_DEV,
};

/* ----------------------- init && exit --------------------*/
int32_t fsys_devfs_init(void)
{
    int32_t err = 0;

    err = devfs_init_inodecache();
    if (err)
    {
        goto out;
    }

    err = esFSYS_fsreg((__hdle)&devfs_fs_type);

out:
    return err;
}

int32_t fsys_devfs_exit(void)
{
    esFSYS_fsunreg((__hdle)&devfs_fs_type);

    return EPDK_OK;
}