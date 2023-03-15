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
#include "udf_fs_i.h"
#include "fs.h"
#include "buffer_head.h"
#include "udf_fs.h"

struct fileIdentDesc *udf_fileident_read(struct inode *dir, loff_t *nf_pos,
        struct udf_fileident_bh *fibh,
        struct fileIdentDesc *cfi,
        struct extent_position *epos,
        struct kernel_lb_addr *eloc, uint32_t *elen,
        sector_t *offset)
{
    struct fileIdentDesc *fi;
    int i, num, block;
    struct buffer_head *tmp, *bha[16];
    struct udf_inode_info *iinfo = UDF_I(dir);

    fibh->soffset = fibh->eoffset;

    if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB)
    {
        fi = udf_get_fileident(iinfo->i_ext.i_data -
                               (iinfo->i_efe ?
                                sizeof(struct extendedFileEntry) :
                                sizeof(struct fileEntry)),
                               dir->i_sb->s_blocksize,
                               &(fibh->eoffset));
        if (!fi)
        {
            return NULL;
        }

        *nf_pos += fibh->eoffset - fibh->soffset;

        memcpy((uint8_t *)cfi, (uint8_t *)fi,
               sizeof(struct fileIdentDesc));

        return fi;
    }

    if (fibh->eoffset == dir->i_sb->s_blocksize)
    {
        int lextoffset = epos->offset;
        unsigned char blocksize_bits = dir->i_sb->s_blocksize_bits;

        if (udf_next_aext(dir, epos, eloc, elen, 1) !=
            (EXT_RECORDED_ALLOCATED >> 30))
        {
            return NULL;
        }

        block = udf_get_lb_pblock(dir->i_sb, eloc, *offset);

        (*offset)++;

        if ((*offset << blocksize_bits) >= *elen)
        {
            *offset = 0;
        }
        else
        {
            epos->offset = lextoffset;
        }

        brelse(fibh->sbh);
        fibh->sbh = fibh->ebh = udf_tread(dir->i_sb, block);
        if (!fibh->sbh)
        {
            return NULL;
        }
        fibh->soffset = fibh->eoffset = 0;

        if (!(*offset & ((16 >> (blocksize_bits - 9)) - 1)))
        {
            i = 16 >> (blocksize_bits - 9);
            if (i + *offset > (*elen >> blocksize_bits))
            {
                i = (*elen >> blocksize_bits) - *offset;
            }
            for (num = 0; i > 0; i--)
            {
                block = udf_get_lb_pblock(dir->i_sb, eloc,
                                          *offset + i);
                tmp = udf_tgetblk(dir->i_sb, block);
                if (tmp && !buffer_uptodate(tmp) &&
                    !buffer_locked(tmp))
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
    else if (fibh->sbh != fibh->ebh)
    {
        brelse(fibh->sbh);
        fibh->sbh = fibh->ebh;
    }

    fi = udf_get_fileident(fibh->sbh->b_data, dir->i_sb->s_blocksize,
                           &(fibh->eoffset));

    if (!fi)
    {
        return NULL;
    }

    *nf_pos += fibh->eoffset - fibh->soffset;

    if (fibh->eoffset <= dir->i_sb->s_blocksize)
    {
        memcpy((uint8_t *)cfi, (uint8_t *)fi,
               sizeof(struct fileIdentDesc));
    }
    else if (fibh->eoffset > dir->i_sb->s_blocksize)
    {
        int lextoffset = epos->offset;

        if (udf_next_aext(dir, epos, eloc, elen, 1) !=
            (EXT_RECORDED_ALLOCATED >> 30))
        {
            return NULL;
        }

        block = udf_get_lb_pblock(dir->i_sb, eloc, *offset);

        (*offset)++;

        if ((*offset << dir->i_sb->s_blocksize_bits) >= *elen)
        {
            *offset = 0;
        }
        else
        {
            epos->offset = lextoffset;
        }

        fibh->soffset -= dir->i_sb->s_blocksize;
        fibh->eoffset -= dir->i_sb->s_blocksize;

        fibh->ebh = udf_tread(dir->i_sb, block);
        if (!fibh->ebh)
        {
            return NULL;
        }

        if (sizeof(struct fileIdentDesc) > -fibh->soffset)
        {
            int fi_len;

            memcpy((uint8_t *)cfi, (uint8_t *)fi, -fibh->soffset);
            memcpy((uint8_t *)cfi - fibh->soffset,
                   fibh->ebh->b_data,
                   sizeof(struct fileIdentDesc) + fibh->soffset);

            fi_len = (sizeof(struct fileIdentDesc) +
                      cfi->lengthFileIdent +
                      le16_to_cpu(cfi->lengthOfImpUse) + 3) & ~3;

            *nf_pos += fi_len - (fibh->eoffset - fibh->soffset);
            fibh->eoffset = fibh->soffset + fi_len;
        }
        else
        {
            memcpy((uint8_t *)cfi, (uint8_t *)fi,
                   sizeof(struct fileIdentDesc));
        }
    }
    return fi;
}

struct fileIdentDesc *udf_get_fileident(void *buffer, int bufsize, int *offset)
{
    struct fileIdentDesc *fi;
    int lengthThisIdent;
    uint8_t *ptr;
    int padlen;

    if ((!buffer) || (!offset))
    {
        udf_debug("invalidparms\n, buffer=%p, offset=%p\n", buffer, offset);
        return NULL;
    }

    ptr = buffer;

    if ((*offset > 0) && (*offset < bufsize))
    {
        ptr += *offset;
    }
    fi = (struct fileIdentDesc *)ptr;
    if (fi->descTag.tagIdent != cpu_to_le16(TAG_IDENT_FID))
    {
        udf_debug("0x%x != TAG_IDENT_FID\n",
                  le16_to_cpu(fi->descTag.tagIdent));
        udf_debug("offset: %u sizeof: %lu bufsize: %u\n",
                  *offset, (unsigned long)sizeof(struct fileIdentDesc),
                  bufsize);
        return NULL;
    }
    if ((*offset + sizeof(struct fileIdentDesc)) > bufsize)
    {
        lengthThisIdent = sizeof(struct fileIdentDesc);
    }
    else
        lengthThisIdent = sizeof(struct fileIdentDesc) +
                          fi->lengthFileIdent + le16_to_cpu(fi->lengthOfImpUse);

    /* we need to figure padding, too! */
    padlen = lengthThisIdent % UDF_NAME_PAD;
    if (padlen)
    {
        lengthThisIdent += (UDF_NAME_PAD - padlen);
    }
    *offset = *offset + lengthThisIdent;

    return fi;
}

struct short_ad *udf_get_fileshortad(uint8_t *ptr, int maxoffset, uint32_t *offset,
                                     int inc)
{
    struct short_ad *sa;

    if ((!ptr) || (!offset))
    {
        udf_error("udf: udf_get_fileshortad() invalidparms\n");
        return NULL;
    }

    if ((*offset + sizeof(struct short_ad)) > maxoffset)
    {
        return NULL;
    }
    else
    {
        sa = (struct short_ad *)ptr;
        if (sa->extLength == 0)
        {
            return NULL;
        }
    }

    if (inc)
    {
        *offset += sizeof(struct short_ad);
    }
    return sa;
}

struct long_ad *udf_get_filelongad(uint8_t *ptr, int maxoffset, uint32_t *offset, int inc)
{
    struct long_ad *la;

    if ((!ptr) || (!offset))
    {
        udf_error("udf: udf_get_filelongad() invalidparms\n");
        return NULL;
    }

    if ((*offset + sizeof(struct long_ad)) > maxoffset)
    {
        return NULL;
    }
    else
    {
        la = (struct long_ad *)ptr;
        if (la->extLength == 0)
        {
            return NULL;
        }
    }

    if (inc)
    {
        *offset += sizeof(struct long_ad);
    }
    return la;
}