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
#include "minfs_i.h"

static int minfs_dentry_hashi(struct dentry *dentry, struct qstr *qstr)
{
    const unsigned char *name;
    unsigned int        len;
    unsigned long       hash;

    name    = qstr->name;
    len     = qstr->len;

    hash = init_name_hash();
    while (len--)
    {
        hash = partial_name_hash(nls_tolower(nls, *name++), hash);
    }
    qstr->hash = end_name_hash(hash);

    return 0;
}

static int minfs_dentry_cmpi(struct dentry *dentry, struct qstr *a, struct qstr *b)
{
    unsigned int alen, blen;

    /* A filename cannot end in '.' or we treat it like it has none */
    alen    = a->len;
    blen    = b->len;

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
int minfs_dentry_delete(struct dentry *d)
{
    int deleted = TRUE;

    return deleted;
}

struct dentry_operations minfs_dentry_ops =
{
    .d_hash     = minfs_dentry_hashi,
    .d_compare  = minfs_dentry_cmpi,
    .d_delete   = minfs_dentry_delete,
};

static __minfs_dentry_t *minfs_find(struct inode *dir, struct qstr *qname, __u32 *dentry_offset)
{
    struct super_block      *sb = dir->i_sb;
    struct minfs_sb_info    *sbi = MINFS_SB(sb);
    struct minfs_inode_info *exi = MINFS_I(dir);
    __minfs_dentry_t        *pdentry;
    __u32                   offset;

    //seach target dentry
    offset  = 0;
    pdentry = (__minfs_dentry_t *)(sbi->pDEntryData + exi->DataOffset - sbi->RootDataOffet);

    while (offset < exi->DataLen)
    {
        if (qname->len == pdentry->NameLen)
        {
            if (memcmp(qname->name, pdentry->MetaData, pdentry->NameLen) == 0)
            {
                //seach target dentry failed
                (*dentry_offset) = exi->DataOffset + offset;
                return pdentry;
            }
        }

        //next dentry
        offset  += (pdentry->RecLen);
        pdentry = (__minfs_dentry_t *)((uintptr_t)(pdentry) + pdentry->RecLen);
    }

    //seach target dentry failed
    return NULL;
}


static struct dentry *minfs_lookup(struct inode *dir, struct dentry *dentry, struct nameidata *nd)
{
    struct inode     *inode = NULL;
    struct dentry    *alias;
    __minfs_dentry_t *pdentry;
    __u32             dentry_offset;

    dentry->d_op    = &minfs_dentry_ops;

    pdentry = minfs_find(dir, &dentry->d_name, &dentry_offset);
    if (pdentry)
    {
        inode = minfs_iget(dir->i_sb, pdentry, dentry_offset);
        if (inode == NULL)
        {
            return ERR_PTR(-EACCES);
        }
    }
    return d_splice_alias(inode, dentry);
}

const struct inode_operations minfs_dir_inode_operations =
{
    .lookup = minfs_lookup,
};