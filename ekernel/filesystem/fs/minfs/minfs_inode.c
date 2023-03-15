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

static kmem_cache_t *minfs_inode_cachep;

struct inode *minfs_alloc_inode(struct super_block *sb)
{
    struct minfs_inode_info *ei;

    ei = kmem_cache_alloc(minfs_inode_cachep, GFP_KERNEL);
    if (!ei)
    {
        return NULL;
    }
    return &ei->vfs_inode;
}

void minfs_destroy_inode(struct inode *inode)
{
    kmem_cache_free(minfs_inode_cachep, MINFS_I(inode));
}

static void minfs_inode_init_once(void *foo, kmem_cache_t *cachep, unsigned long flags)
{
    struct minfs_inode_info *ei = foo;

    inode_init_once(&ei->vfs_inode);
}

int minfs_init_inodecache(void)
{
    minfs_inode_cachep = kmem_cache_create("minfs_inode_cache",
                                           sizeof(struct minfs_inode_info),
                                           0, SLAB_HWCACHE_ALIGN,
                                           /*(SLAB_RECLAIM_ACCOUNT| SLAB_MEM_SPREAD),*/
                                           minfs_inode_init_once, NULL);
    if (minfs_inode_cachep == NULL)
    {
        return -ENOMEM;
    }
    return 0;
}

void minfs_destroy_inodecache(void)
{
    kmem_cache_destroy(minfs_inode_cachep);
}


static int minfs_subdirs(struct inode *inode)
{
    struct super_block      *sb = inode->i_sb;
    struct minfs_sb_info    *sbi = MINFS_SB(sb);
    struct minfs_inode_info *exi = MINFS_I(inode);
    __minfs_dentry_t        *pdentry;
    __u32                   offset;
    int                     sub_dirs;

    offset      = 0;
    sub_dirs    = 0;
    pdentry     = (__minfs_dentry_t *)(sbi->pDEntryData + exi->DataOffset - sbi->RootDataOffet);

    while (offset < exi->DataLen)
    {
        offset  += (pdentry->RecLen);
        pdentry = (__minfs_dentry_t *)((uintptr_t)(pdentry) + pdentry->RecLen);
        sub_dirs++;
    }

    return sub_dirs;
}

static int minfs_fill_inode(struct inode *inode, __minfs_dentry_t *pdentry, __u32 dentry_offset)
{
    struct super_block      *sb = inode->i_sb;
    struct minfs_sb_info    *sbi = MINFS_SB(sb);
    struct minfs_inode_info *exi = MINFS_I(inode);

    inode->i_version++;
    inode->i_generation = 0;

    //inode extent information
    exi->Attribute      = pdentry->Attribute;
    exi->DataOffset     = pdentry->Offset;
    exi->DataLen        = pdentry->Size;
    exi->DEntryOffset   = dentry_offset;

    //inode basic informations
    inode->i_size   = pdentry->Size;
    inode->i_blocks = (inode->i_size + sbi->SectorSize - 1) >> (sbi->SectorBits);
    inode->i_ino    = dentry_offset;
    if ((pdentry->Attribute & MINFS_ATTR_DIR))
    {
        inode->i_generation &= ~1;
        inode->i_mode = S_IFDIR;
        inode->i_op = &minfs_dir_inode_operations;
        inode->i_fop = &minfs_dir_operations;
        inode->i_nlink = minfs_subdirs(inode);
    }
    else
    {
        inode->i_generation |= 1;
        inode->i_mode = S_IFREG;
        inode->i_fop = &minfs_file_operations;
        inode->i_nlink = 1;
    }

    return 0;
}

int minfs_read_root_inode(struct inode *inode)
{
    struct super_block *sb      = inode->i_sb;
    struct minfs_sb_info *sbi   = MINFS_SB(sb);
    struct minfs_inode_info *exi    = MINFS_I(inode);

    exi->DataOffset = sbi->RootDataOffet;
    exi->DataLen    = sbi->RootDirSize;

    inode->i_version++;
    inode->i_generation     = 0;
    inode->i_mode           = S_IFDIR;
    inode->i_op             = &minfs_dir_inode_operations;
    inode->i_fop            = &minfs_dir_operations;
    inode->i_size           = sbi->RootDirSize;
    inode->i_blocks         = ((inode->i_size + (sbi->SectorSize - 1))& ~((__s64)sbi->SectorSize - 1)) >> 9;
    inode->i_mtime.tv_sec   = 0;
    inode->i_atime.tv_sec   = 0;
    inode->i_ctime.tv_sec   = 0;
    inode->i_mtime.tv_nsec  = 0;
    inode->i_atime.tv_nsec  = 0;
    inode->i_ctime.tv_nsec  = 0;
    inode->i_nlink          = minfs_subdirs(inode) + 2;

    return 0;
}

struct inode *minfs_iget(struct super_block *sb, __minfs_dentry_t *pdentry, __u32 dentry_offset)
{
    struct inode *inode = iget_locked(sb, pdentry->Offset);

    if (inode == NULL)
    {
        fs_log_error("allocate inode for minfs failed\n");
        return NULL;
    }

    if (inode->i_state & I_NEW)
    {
        minfs_fill_inode(inode, pdentry, dentry_offset);
    }
    return inode;
}