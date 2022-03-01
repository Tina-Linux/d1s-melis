/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_namei.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-26
* Descript: minfs path lookup handing functions.
* Update  : date                auther      ver     notes
*           2011-3-26 16:42:09  Sunny       1.0     Create this file.
*********************************************************************************************************
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
