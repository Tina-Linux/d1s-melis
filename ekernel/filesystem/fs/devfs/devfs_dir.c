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
#include "fs.h"
#include "devfs.h"
#include "errno.h"
#include "fsys_debug.h"


/**
  *return value:
  *     1: fill buffer overflow
  *     0: end of dir
  *    <0: ENOMEM error or other fatal error, need stop to go on scaning the directory
  */
static __s32 devfs_readdir(struct file *filp, void *dirent, filldir_t filldir)
{
    struct super_block *sb = filp->f_dentry->d_sb;
    struct inode *inode = filp->f_dentry->d_inode;
    dms_dir_info_fs_t itemInfo;
    dms_node_info_fs_t dinfo;
    __s64  cpos;
    __u64  cino;
    int rc, ret = 0;

    memset(&itemInfo, 0x00, sizeof(dms_dir_info_fs_t));
    memset(&dinfo, 0x00, sizeof(dms_node_info_fs_t));

    cpos = filp->f_pos;

    /* 判断是否是合法的目录节点 */
    if (! S_ISDIR(inode->i_mode))
    {
        fs_log_debug("fail, readdir on a none dir node!\n");
        ret = -ENOTDIR;
        goto out;
    }

    for (;;)
    {
        /* 读取目录下的条目 */
        itemInfo.dir = DEVFS_I(inode)->i_hnode;
        itemInfo.itemPos = cpos;
        ret = esFSYS_pioctrl(sb->s_part, PART_IOC_USR_GETITEM, (long)&itemInfo, &dinfo);
        if (ret == EPDK_FAIL) /* 到达目录末尾 */
        {
            ret = 0;
            goto EOD;
        }

        /* 填充目录结构 */
        cino = cpos + 1;
        rc = filldir(dirent, dinfo.name, strlen((const char *)dinfo.name), 0, cpos ++, cino,
                     (dinfo.type == DEVFS_CLASS_TYPE ? FSYS_ATTR_DIRECTORY : 0) | FSYS_ATTR_READONLY | FSYS_ATTR_SYSTEM, 0);
        if (rc)    /* 目录缓存空间不足 */
        {
            ret = 1;
            goto NoEnoughFillBuff;
        }

        filp->f_pos = cpos;
    }

EOD:
NoEnoughFillBuff:

out:
    return ret;
}

/* ------------------- dir ops -------------------------------*/
const struct file_operations devfs_dir_operations =
{
    .readdir    = devfs_readdir,
};