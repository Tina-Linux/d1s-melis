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
#include "sys_fsys_i.h"
#include "errno.h"
#include "fsys_debug.h"
#include "nls.h"
#include <string.h>

extern struct file_system_type *pFSRoot;

/*
**********************************************************************************************************************
*
*             esFSYS_format
*
*  Description:
*  format one partiton
*
*  Parameters:
*   ppartName   - like 'c' or 'd' etc.
*   fstype      - wanna filesystem type
*
*  Return value:
*   <0          - Unable to find the device.
*   >=0         - Index of the device in the device information table.
**********************************************************************************************************************
*/
int32_t esFSYS_format(const char *partname, const char *fstype, __hdle fmtpara)
{
    uint8_t     err;
    int32_t     res, semap_on = 0;
    __hdle      pPart = NULL;
    struct file_system_type     *pFS;

    fs_log_trace0("fmt:%s,%s", partname, fstype);

    if (esFSYS_vfslock())
    {
        fs_log_error("format error when enter vfs mutex\n");
        return EPDK_FAIL;
    }

    semap_on = 1;

    /* 识别文件系统                 */
    for (pFS = pFSRoot; pFS; pFS = pFS->next)
    {
        if (!strcmp(pFS->name, fstype))
        {
            if (pFS->format)
            {
                break;
            }
            else
            {
                fs_log_info("format failed!\n%s filesystem has no format function!\n", fstype);
                res = EPDK_FAIL;
                goto out;
            }
        }
    }
    if (!pFS)
    {
        fs_log_info("format failed!\n%s filesystem not found!maybe you need to install the filesystem first.\n", fstype);
        res = EPDK_FAIL;
        goto out;
    }
    esFSYS_vfsunlock();
    semap_on = 0;

    fs_log_info("open part \"%s\"\n", partname);
    pPart   = esFSYS_popen(partname, "r+");
    if (!pPart)
    {
        fs_log_info("format failed!\npart %s can not be open!\n", partname);
        res     = EPDK_FAIL;
        goto out;
    }

    if (esFSYS_vfslock())
    {
        fs_log_error("format error when enter vfs mutex\n");
        return EPDK_FAIL;
    }
    semap_on    = 1;

    res         = pFS->format((__hdle)pPart, fmtpara);
    if (res)
    {
        fs_log_info("format failed!\n");
        res     = EPDK_FAIL;
        goto out;
    }

    fs_log_info("complete.\n\n");
    res         = EPDK_OK;

out:
    if (semap_on)
    {
        esFSYS_vfsunlock();
    }
    if (pPart)
    {
        esFSYS_pclose(pPart);
    }

    fs_log_trace0("O\n");
    return res;
}

int32_t esFSYS_statfs(const char *path, __hdle buf, uint32_t flags)
{
    char                *s;
    int32_t             res;
    struct super_block  *sb;
    uint8_t             err;
    struct kstatfs      *stat = (struct kstatfs *)buf;

    fs_log_trace0("stafs:%s,", path);

    if (esFSYS_vfslock())
    {
        fs_log_error("statfs error when enter vfs mutex\n");
        return EPDK_FAIL;
    }

    /* 获取由pFileName路径名中的盘符指定的分区指针，并析取盘符以外的路径名 */
    sb  = path_to_sb(path, NULL);
    if (!sb)
    {
        fs_log_warning("part \"%c\" not accessable!\n", *path);
        res = EPDK_FAIL;
        fs_err = -ENOENT;
        goto out;
    }

    if (sb->s_op->statfs)
    {
        res     = sb->s_op->statfs(sb, stat, flags);
    }
    else
    {
        res     = EPDK_FAIL;
        fs_err  = -EPERM;
        fs_log_warning("no statfs operation!\n");
    }

out:
    esFSYS_vfsunlock();
    fs_log_trace0("O\n");
    return res;
}

/*
**********************************************************************************************************************
*
*             esFSYS_partfslock
*
*  Description:
*               - lock one disk from been fs-unmounted.
*
*  Parameters:
*   diskname    - disk letter follow up with ":\", such as "c:\" , 'd:\' etc.
*
*  Return value:
*   EPDK_OK     - ok
*   EPDK_FAIL   - some thing error
**********************************************************************************************************************
*/
int32_t esFSYS_partfslck(char *partname)
{
    uint8_t             err;
    int32_t             res;
    struct super_block  *sb;
    struct inode        *ino;

    fs_log_trace0("pfslk:%s", partname);

    if (esFSYS_vfslock())
    {
        fs_log_error("partfslck error when enter vfs mutex\n");
        return EPDK_FAIL;
    }

    sb = path_to_sb(partname, NULL);
    if (!sb)
    {
        fs_log_warning("part \"%c\" can not be accessed!\n", *partname);
        res = EPDK_FAIL;
        fs_err = -ENOENT;
        goto out;
    }

    ino = igrab(sb->s_root->d_inode);
    if (ino)
    {
        res = EPDK_OK;
    }
    else
    {
        res = EPDK_FAIL;
    }

out:
    esFSYS_vfsunlock();

    fs_log_trace0("%d\n", res);
    return res;
}

/*
**********************************************************************************************************************
*
*             esFSYS_partfsunlock
*
*  Description:
*               - give up hold the fs of one disk. the disk then can be unmount from fs
*
*  Parameters:
*   diskname    - disk letter follow up with ":\", such as "c:\" , 'd:\' etc.
*
*  Return value:
*   EPDK_OK     - ok
*   EPDK_FAIL   - some thing error.
**********************************************************************************************************************
*/
int32_t esFSYS_partfsunlck(char *partname)
{
    uint8_t             err;
    int32_t             res;
    struct super_block  *sb;

    fs_log_trace0("pfsulk:%s", partname);

    if (esFSYS_vfslock())
    {
        fs_log_error("partfsunlck error when enter vfs mutex\n");
        return EPDK_FAIL;
    }

    sb = path_to_sb(partname, NULL);
    if (!sb)
    {
        fs_log_warning("part \"%c\" can not be accessed!\n", *partname);
        res = EPDK_FAIL;
        fs_err = -ENOENT;
        goto out;
    }

    if (atomic_read(&sb->s_root->d_inode->i_count) > 1)
    {
        iput(sb->s_root->d_inode);
    }

    res = EPDK_OK;

out:
    esFSYS_vfsunlock();

    fs_log_trace0("O%d\n", res);
    return res;
}

int32_t esFSYS_setfs(char *partname, uint32_t cmd, int32_t aux, char *para)
{
    struct nls_table   *new_nls;

    fs_log_trace0("setfs: path(%s), cmd(%d), para(%s)", partname, cmd, para);

    if (esFSYS_vfslock())
    {
        fs_log_error("setfs error when enter vfs mutex\n");
        return EPDK_FAIL;
    }

    new_nls = load_nls(aux);
    if (new_nls)
    {
        fs_log_info("Set nls to [%d]\n", aux);
        unload_nls(nls);
        nls = new_nls;
    }

    esFSYS_vfsunlock();
    fs_log_trace0("O\n");
    return EPDK_OK;
}

int32_t esFSYS_getfscharset(const char *partname, int32_t *pCharset)
{
    int32_t         ret = EPDK_OK;
    uint8_t         err = 0;

    fs_log_trace0("getfscharset: path(%s)", partname);

    if (esFSYS_vfslock())
    {
        fs_log_error("getfscharset error when enter vfs mutex\n");
        return EPDK_FAIL;
    }

    /* 获取文件系统当前的编码 */
    if (pCharset)
    {
        *pCharset = get_current_charset();
    }
    else
    {
        ret = EPDK_FAIL;
    }
    esFSYS_vfsunlock();
    fs_log_trace0("O\n");
    return ret;
}