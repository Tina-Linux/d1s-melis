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
#include "fs.h"
#include "udf_fs.h"
#include "slab.h"
#include "page_pool.h"
#include "udf_fs_i.h"
#include "udf_sb.h"

#if defined CONFIG_FSYS_UDFFS_RW

void udf_free_inode(struct inode *inode)
{
    struct super_block *sb = inode->i_sb;
    struct udf_sb_info *sbi = UDF_SB(sb);

    /*
     * Note: we must free any quota before locking the superblock,
     * as writing the quota to disk may need the lock as well.
     */
    //  dquot_free_inode(inode);
    //  dquot_drop(inode);

    clear_inode(inode);

    //  mutex_lock(&sbi->s_alloc_mutex);
    if (sbi->s_lvid_bh)
    {
        struct logicalVolIntegrityDescImpUse *lvidiu =
            udf_sb_lvidiu(sbi);
        if (S_ISDIR(inode->i_mode))
        {
            le32_add_cpu(&lvidiu->numDirs, -1);
        }
        else
        {
            le32_add_cpu(&lvidiu->numFiles, -1);
        }
        udf_updated_lvid(sb);
    }
    //  mutex_unlock(&sbi->s_alloc_mutex);

    udf_free_blocks(sb, NULL, &UDF_I(inode)->i_location, 0, 1);
}

struct inode *udf_new_inode(struct inode *dir, int mode, int *err)
{
    struct super_block *sb = dir->i_sb;
    struct udf_sb_info *sbi = UDF_SB(sb);
    struct inode *inode;
    int block, ret;
    uint32_t start = UDF_I(dir)->i_location.logicalBlockNum;
    struct udf_inode_info *iinfo;
    struct udf_inode_info *dinfo = UDF_I(dir);

    inode = new_inode(sb);

    if (!inode)
    {
        *err = -ENOMEM;
        return NULL;
    }
    *err = -ENOSPC;

    iinfo = UDF_I(inode);
    if (UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_USE_EXTENDED_FE))
    {
        iinfo->i_efe = 1;
        if (UDF_VERS_USE_EXTENDED_FE > sbi->s_udfrev)
        {
            sbi->s_udfrev = UDF_VERS_USE_EXTENDED_FE;
        }
        //      iinfo->i_ext.i_data = kzalloc(inode->i_sb->s_blocksize - sizeof(struct extendedFileEntry), GFP_KERNEL);
        iinfo->i_ext.i_data = kmalloc(inode->i_sb->s_blocksize - sizeof(struct extendedFileEntry), GFP_KERNEL);
        memset(iinfo->i_ext.i_data, 0, inode->i_sb->s_blocksize - sizeof(struct extendedFileEntry));
    }
    else
    {
        iinfo->i_efe = 0;
        //      iinfo->i_ext.i_data = kzalloc(inode->i_sb->s_blocksize - sizeof(struct fileEntry), GFP_KERNEL);
        iinfo->i_ext.i_data = kmalloc(inode->i_sb->s_blocksize - sizeof(struct fileEntry), GFP_KERNEL);
        memset(iinfo->i_ext.i_data, 0, inode->i_sb->s_blocksize - sizeof(struct fileEntry));
    }
    if (!iinfo->i_ext.i_data)
    {
        iput(inode);
        *err = -ENOMEM;
        return NULL;
    }

    block = udf_new_block(dir->i_sb, NULL,
                          dinfo->i_location.partitionReferenceNum,
                          start, err);
    if (*err)
    {
        iput(inode);
        return NULL;
    }

    //  mutex_lock(&sbi->s_alloc_mutex);
    if (sbi->s_lvid_bh)
    {
        struct logicalVolIntegrityDesc *lvid =
            (struct logicalVolIntegrityDesc *)
            sbi->s_lvid_bh->b_data;
        struct logicalVolIntegrityDescImpUse *lvidiu =
            udf_sb_lvidiu(sbi);
        struct logicalVolHeaderDesc *lvhd;
        uint64_t uniqueID;
        lvhd = (struct logicalVolHeaderDesc *)
               (lvid->logicalVolContentsUse);
        if (S_ISDIR(mode))
        {
            le32_add_cpu(&lvidiu->numDirs, 1);
        }
        else
        {
            le32_add_cpu(&lvidiu->numFiles, 1);
        }
        iinfo->i_unique = uniqueID = le64_to_cpu(lvhd->uniqueID);
        if (!(++uniqueID & 0x00000000FFFFFFFFUL))
        {
            uniqueID += 16;
        }
        lvhd->uniqueID = cpu_to_le64(uniqueID);
        udf_updated_lvid(sb);
    }
    //  mutex_unlock(&sbi->s_alloc_mutex);
    inode->i_mode = mode;
    //  inode->i_uid = current_fsuid();
    if (dir->i_mode & S_ISGID)
    {
        //      inode->i_gid = dir->i_gid;
        if (S_ISDIR(mode))
        {
            mode |= S_ISGID;
        }
    }
    else
    {
        //      inode->i_gid = current_fsgid();
    }

    iinfo->i_location.logicalBlockNum = block;
    iinfo->i_location.partitionReferenceNum =
        dinfo->i_location.partitionReferenceNum;
    inode->i_ino = udf_get_lb_pblock(sb, &iinfo->i_location, 0);
    inode->i_blocks = 0;
    iinfo->i_lenEAttr = 0;
    iinfo->i_lenAlloc = 0;
    iinfo->i_use = 0;
    if (UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_USE_AD_IN_ICB))
    {
        iinfo->i_alloc_type = ICBTAG_FLAG_AD_IN_ICB;
    }
    else if (UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_USE_SHORT_AD))
    {
        iinfo->i_alloc_type = ICBTAG_FLAG_AD_SHORT;
    }
    else
    {
        iinfo->i_alloc_type = ICBTAG_FLAG_AD_LONG;
    }
    inode->i_mtime = inode->i_atime = inode->i_ctime =
                                          iinfo->i_crtime = current_fs_time(inode->i_sb);
    insert_inode_hash(inode);
    mark_inode_dirty(inode);

    //  dquot_initialize(inode);
    //  ret = dquot_alloc_inode(inode);
    //  if (ret) {
    //      dquot_drop(inode);
    //      inode->i_flags |= S_NOQUOTA;
    //      inode->i_nlink = 0;
    //      iput(inode);
    //      *err = ret;
    //      return NULL;
    //  }

    *err = 0;
    return inode;
}

#endif  /* FSYS_UDF_RW */