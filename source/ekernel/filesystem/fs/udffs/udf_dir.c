/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : udf_dir.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-17
* Descript: Directory handling routines for the OSTA-UDF(tm) filesystem.
* Update  : date                auther      ver     notes
*           2011-3-17 8:56:47   Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "udf_decl.h"
#include "slab.h"
#include "buffer_head.h"
#include "page_pool.h"
#include "udf_fs_i.h"
#include "udf_sb.h"
#include "udf_fs.h"

static int do_udf_readdir(struct inode *dir, struct file *filp,
                          filldir_t filldir, void *dirent)
{
    struct udf_fileident_bh fibh = { .sbh = NULL, .ebh = NULL};
    struct fileIdentDesc *fi = NULL;
    struct fileIdentDesc cfi;
    int block, iblock;
    loff_t nf_pos = (filp->f_pos - 1) << 2;
    int flen;
    unsigned char *fname = NULL;
    unsigned char *nameptr;
    uint16_t liu;
    uint8_t lfi;
    loff_t size = udf_ext0_offset(dir) + dir->i_size;
    struct buffer_head *tmp, *bha[16];
    struct kernel_lb_addr eloc;
    uint32_t elen;
    sector_t offset;
    int i, num, ret = 0;
    unsigned int dt_type;
    struct extent_position epos = { NULL, 0, {0, 0} };
    struct udf_inode_info *iinfo;

    if (nf_pos >= size)
    {
        goto out;
    }

    fname = kmalloc(UDF_NAME_LEN, GFP_NOFS);
    if (!fname)
    {
        ret = -ENOMEM;
        goto out;
    }

    if (nf_pos == 0)
    {
        nf_pos = udf_ext0_offset(dir);
    }

    fibh.soffset = fibh.eoffset = nf_pos & (dir->i_sb->s_blocksize - 1);
    iinfo = UDF_I(dir);
    if (iinfo->i_alloc_type != ICBTAG_FLAG_AD_IN_ICB)
    {
        if (inode_bmap(dir, nf_pos >> dir->i_sb->s_blocksize_bits,
                       &epos, &eloc, &elen, &offset)
            != (EXT_RECORDED_ALLOCATED >> 30))
        {
            ret = -ENOENT;
            goto out;
        }
        block = udf_get_lb_pblock(dir->i_sb, &eloc, offset);
        if ((++offset << dir->i_sb->s_blocksize_bits) < elen)
        {
            if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_SHORT)
            {
                epos.offset -= sizeof(struct short_ad);
            }
            else if (iinfo->i_alloc_type ==
                     ICBTAG_FLAG_AD_LONG)
            {
                epos.offset -= sizeof(struct long_ad);
            }
        }
        else
        {
            offset = 0;
        }

        if (!(fibh.sbh = fibh.ebh = udf_tread(dir->i_sb, block)))
        {
            ret = -EIO;
            goto out;
        }

        if (!(offset & ((16 >> (dir->i_sb->s_blocksize_bits - 9)) - 1)))
        {
            i = 16 >> (dir->i_sb->s_blocksize_bits - 9);
            if (i + offset > (elen >> dir->i_sb->s_blocksize_bits))
            {
                i = (elen >> dir->i_sb->s_blocksize_bits) - offset;
            }
            for (num = 0; i > 0; i--)
            {
                block = udf_get_lb_pblock(dir->i_sb, &eloc, offset + i);
                tmp = udf_tgetblk(dir->i_sb, block);
                if (tmp && !buffer_uptodate(tmp) && !buffer_locked(tmp))
                {
                    bha[num++] = tmp;
                }
                else
                {
                    brelse(tmp);
                }
            }
            if (num)
            {
                ll_rw_block(READA, num, bha);
                for (i = 0; i < num; i++)
                {
                    brelse(bha[i]);
                }
            }
        }
    }

    while (nf_pos < size)
    {
        filp->f_pos = (nf_pos >> 2) + 1;

        fi = udf_fileident_read(dir, &nf_pos, &fibh, &cfi, &epos, &eloc,
                                &elen, &offset);
        if (!fi)
        {
            goto out;
        }

        liu = le16_to_cpu(cfi.lengthOfImpUse);
        lfi = cfi.lengthFileIdent;

        if (fibh.sbh == fibh.ebh)
        {
            nameptr = fi->fileIdent + liu;
        }
        else
        {
            int poffset;    /* Unpaded ending offset */

            poffset = fibh.soffset + sizeof(struct fileIdentDesc) + liu + lfi;

            if (poffset >= lfi)
            {
                nameptr = (char *)(fibh.ebh->b_data + poffset - lfi);
            }
            else
            {
                nameptr = fname;
                memcpy(nameptr, fi->fileIdent + liu,
                       lfi - poffset);
                memcpy(nameptr + lfi - poffset,
                       fibh.ebh->b_data, poffset);
            }
        }

        if ((cfi.fileCharacteristics & FID_FILE_CHAR_DELETED) != 0)
        {
            if (!UDF_QUERY_FLAG(dir->i_sb, UDF_FLAG_UNDELETE))
            {
                continue;
            }
        }

        if ((cfi.fileCharacteristics & FID_FILE_CHAR_HIDDEN) != 0)
        {
            if (!UDF_QUERY_FLAG(dir->i_sb, UDF_FLAG_UNHIDE))
            {
                continue;
            }
        }

        if (cfi.fileCharacteristics & FID_FILE_CHAR_PARENT)
        {
            //iblock = parent_ino(filp->f_path.dentry);
            //flen = 2;
            //memcpy(fname, "..", flen);
            //dt_type = DT_DIR;
            /* FIXME : can't support "." and ".." now,
             * 2009.10.29, sunny*/
            continue;
        }
        else
        {
            struct kernel_lb_addr tloc = lelb_to_cpu(cfi.icb.extLocation);

            iblock = udf_get_lb_pblock(dir->i_sb, &tloc, 0);
            flen = udf_get_filename(dir->i_sb, nameptr, fname, lfi);
            if (cfi.fileCharacteristics & FID_FILE_CHAR_DIRECTORY)
            {
                dt_type = FSYS_ATTR_DIRECTORY;
            }
            else
            {
                dt_type = 0;
            }
        }
        if (flen)
        {
            /* FIXME : file size temp set to zero */
            if (filldir(dirent, fname, flen, 0, filp->f_pos, iblock, dt_type, /*size*/0) < 0)
            {
                if (fibh.sbh != fibh.ebh)
                {
                    brelse(fibh.ebh);
                }
                brelse(fibh.sbh);
                brelse(epos.bh);
                return 0;
            }
        }
    } /* end while */

    filp->f_pos = (nf_pos >> 2) + 1;

out:
    if (fibh.sbh != fibh.ebh)
    {
        brelse(fibh.ebh);
    }
    brelse(fibh.sbh);
    brelse(epos.bh);
    kfree(fname);

    return ret;
}

static __s32 udf_readdir(struct file *filp, void *dirent, filldir_t filldir)
{
    struct inode *dir = filp->f_dentry->d_inode;
    int result;

    //lock_kernel();
    /* FIXME : can't support "." and ".." now,
     * 2009.10.29, sunny*/
    if (filp->f_pos == 0)
    {

        //if (filldir(dirent, ".", 1, 0, filp->f_pos, dir->i_ino, DT_DIR, dir->i_size) < 0) {
        //  unlock_kernel();
        //  return 0;
        // }
        filp->f_pos++;
    }

    result = do_udf_readdir(dir, filp, filldir, dirent);
    //unlock_kernel();
    return result;
}

/* readdir and lookup functions */
const struct file_operations udf_dir_operations =
{
    //  .read           = generic_read_dir,
    .readdir        = udf_readdir,
    .ioctl          = udf_ioctl,
    //  .fsync          = udf_fsync_file,
    .fsync          = file_fsync,
};
