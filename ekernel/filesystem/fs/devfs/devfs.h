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
#ifndef __DEVFS_FS_H__
#define __DEVFS_FS_H__

#include "fs.h"
#include "supports.h"

#define DEVFS_ROOT_INO      1
#define DEVFS_SUPER_MAGIC   0x5d55

#define DEVFS_MAX_LINK      1000

struct devfs_mount_options
{
    unsigned short codepage;  /* Codepage for shortname conversions */
    char *iocharset;          /* Charset used for filename input/display */
    unsigned char name_check; /* r = relaxed, n = normal, s = strict */
};

struct devfs_sb_info
{
    const void *dir_ops;             /* Opaque; default directory operations */

    struct devfs_mount_options options;

    struct list_head inode_list;
};

struct devfs_inode_info
{
    __hdle       i_hnode;
    unsigned int i_key;
    struct list_head i_list;    /* hash by i_location */
    struct inode     vfs_inode;
};

typedef  struct DmsNodeInfo_dev dms_node_info_fs_t;
typedef  struct DmsDirInfo_dev  dms_dir_info_fs_t;

static inline struct devfs_sb_info *DEVFS_SB(struct super_block *sb)
{
    return sb->s_fs_info;
}

static inline struct devfs_inode_info *DEVFS_I(struct inode *inode)
{
    return container_of(inode, struct devfs_inode_info, vfs_inode);
}


#endif  /* __DEVFS_FS_H__ */