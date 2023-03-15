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
#include "errno.h"
#include "fsys_debug.h"
#include <sys_device.h>

__s32 devfs_generic_ioctl(struct inode *inode, struct file *filp,
                          __u32 cmd, long aux, void *pBuffer)
{
    return esDEV_Ioctl(filp->private_data, cmd, aux, pBuffer);
}

static __s32 devfs_file_read(struct file *filp, char *buf, __u32 len, __s64 *ppos)
{
    return esDEV_Read(buf, *ppos, len, filp->private_data);
}

static __s32 devfs_file_write(struct file *filp, const char *buf, __u32 len, __s64 *ppos)
{
    return esDEV_Write(buf, *ppos, len, filp->private_data);
}

static __s32 devfs_file_open(struct inode *finode, struct file *filp)
{
    struct devfs_inode_info *devi = DEVFS_I(finode);

    filp->f_dev = 1;
    filp->private_data = esDEV_Open(devi->i_hnode, 0);                    /* 打开设备                                                     */
    if (filp->private_data == 0)
    {
        fs_log_warning("dev cannot opened!\n");
        return -EIO;
    }

    return 0;
}

static __s32 devfs_file_close(struct file *filp)
{
    return esDEV_Close(filp->private_data);
}

static __s32 devfs_file_fsync(struct file *filp, struct dentry *dentry, __s32 datasync)
{
    return 0;
}

/* ------------------- file ops -------------------------------*/
const struct file_operations devfs_file_operations =
{
    .read       = devfs_file_read,
    .write      = devfs_file_write,
    .ioctl      = devfs_generic_ioctl,
    .open       = devfs_file_open,
    .close      = devfs_file_close,
    .fsync      = devfs_file_fsync,
};

const struct inode_operations devfs_file_inode_operations =
{
};