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
#include "dcache.h"
#include "buffer_head.h"
#include "namei.h"
#include "nls.h"
#include "endians.h"
#include "errno.h"
#include "page_pool.h"
#include "slab.h"
#include "fsys_debug.h"

extern struct inode *devfs_build_inode(struct super_block *sb, dms_node_info_fs_t *dinfo);

static int devfs_revalidate(struct dentry *dentry, struct nameidata *nd)
{
    int ret = 1;

    if (!dentry->d_inode &&
        nd && !(nd->flags & LOOKUP_CONTINUE) && (nd->flags & LOOKUP_CREATE))
        /*
         * negative dentry is dropped, in order to make sure
         * to use the name which a user desires if this is
         * create path.
         */
    {
        ret = 0;
    }
    else
    {
        //      if (dentry->d_time != dentry->d_parent->d_inode->i_version)
        //          ret = 0;
    }
    return ret;
}

/*
 * Compute the hash for the vfat name corresponding to the dentry.
 * Note: if the name is invalid, we leave the hash code unchanged so
 * that the existing dentry can be used. The vfat fs routines will
 * return ENOENT or EINVAL as appropriate.
 */
static int devfs_hashi(struct dentry *dentry, struct qstr *qstr)
{
    const unsigned char *name;
    unsigned int len;
    unsigned long hash;

    name = qstr->name;
    len = qstr->len;

    hash = init_name_hash();
    while (len--)
    {
        hash = partial_name_hash(nls_tolower(nls, *name++), hash);
    }
    qstr->hash = end_name_hash(hash);

    return 0;
}

/*
 * Case insensitive compare of two vfat names.
 */
static int devfs_cmpi(struct dentry *dentry, struct qstr *a, struct qstr *b)
{
    unsigned int alen, blen;

    /* A filename cannot end in '.' or we treat it like it has none */
    alen = a->len;
    blen = b->len;
    if (alen == blen)
    {
        if (nls_strnicmp(nls, a->name, b->name, alen) == 0)
        {
            return 0;
        }
    }
    return 1;
}

/* tell the vfs to kill the dentry,
 */
int devfs_d_delete(struct dentry *d)
{
    int deleted = TRUE;

    return deleted;
}

struct dentry_operations devfs_dentry_ops[2] =
{
    {
        .d_hash     = devfs_hashi,
        .d_compare  = devfs_cmpi,
        .d_delete   = devfs_d_delete,
    },
    {
        .d_revalidate   = devfs_revalidate,
        .d_hash     = devfs_hashi,
        .d_compare  = devfs_cmpi,
        .d_delete   = devfs_d_delete,
    },
};

static int devfs_find(struct inode *dir, struct qstr *qname, dms_node_info_fs_t *dinfo)
{
    int                       res = 0;
    int                       idx;
    int                       len;
    struct super_block       *sb = dir->i_sb;
    struct devfs_inode_info *devi = DEVFS_I(dir);
    dms_dir_info_fs_t itemInfo;

    for (idx = 0; idx < DEVFS_MAX_LINK; idx++)
    {
        /* 读取目录下的条目 */
        itemInfo.dir = DEVFS_I(dir)->i_hnode;
        itemInfo.itemPos = idx;
        if (esFSYS_pioctrl(sb->s_part, PART_IOC_USR_GETITEM, (long)&itemInfo, dinfo) == EPDK_FAIL)
        {
            fs_log_debug("Dev item not found!\n");
            goto err_out;
        }
        len = strlen((const char *)dinfo->name);
        if (len == qname->len)
            if (!nls_strnicmp(nls, (const char *)dinfo->name, qname->name, qname->len))
            {
                break;
            }
    }

    if (idx == DEVFS_MAX_LINK)
    {
        fs_log_error("Error: dev list is conrupted!\n");
        goto err_out;
    }

    goto out;

err_out:
    res = -ENOENT;
out:
    return res;
}

static struct dentry *devfs_lookup(struct inode *dir, struct dentry *dentry,
                                   struct nameidata *nd)
{
    struct super_block *sb = dir->i_sb;
    dms_node_info_fs_t dinfo;
    struct inode *inode = NULL;
    struct dentry *alias;
    int err;

    memset(&dinfo, 0x00, sizeof(dms_node_info_fs_t));
    dentry->d_op = &devfs_dentry_ops[0];

    err = devfs_find(dir, &dentry->d_name, &dinfo);
    if (err)
    {
        goto error;
    }

    dinfo.key = dentry->d_phash_;
    inode = devfs_build_inode(sb, &dinfo);

    if (IS_ERR(PTR_ERR(inode)))
    {
        return ERR_PTR(PTR_ERR(inode));
    }

error:
    d_splice_alias(inode, dentry);

    return NULL;
}

struct inode_operations devfs_dir_inode_operations =
{
    .lookup     = devfs_lookup,
};