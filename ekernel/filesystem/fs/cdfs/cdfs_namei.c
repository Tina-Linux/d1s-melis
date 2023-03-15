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
#include "err.h"
#include "fsys_debug.h"
#include "page_pool.h"
#include "cdfs.h"

/*
 * ok, we cannot use strncmp, as the name is not in our data space.
 * Thus we'll have to use isofs_match. No big problem. Match also makes
 * some sanity tests.
 */
static int isofs_cmp(struct dentry *dentry,
                     const char *compare,
                     int dlen)
{
    struct qstr qstr;

    if (!compare)
    {
        return 1;
    }

    /* check special "." and ".." files */
    if (dlen == 1)
    {
        /* "." */
        if (compare[0] == 0)
        {
            if (!dentry->d_name.len)
            {
                return 0;
            }
            compare = ".";
        }
        else if (compare[0] == 1)
        {
            compare = "..";
            dlen = 2;
        }
    }

    qstr.name = compare;
    qstr.len = dlen;
    return dentry->d_op->d_compare(dentry, &dentry->d_name, &qstr);
}

/*
 *  isofs_find_entry()
 *
 * finds an entry in the specified directory with the wanted name. It
 * returns the inode number of the found entry, or 0 on error.
 */
static unsigned long isofs_find_entry(struct inode *dir,
                                      struct dentry *dentry,
                                      unsigned long *block_rv,
                                      unsigned long *offset_rv,
                                      char *tmpname,
                                      struct iso_directory_record *tmpde)
{
    unsigned long bufsize = ISOFS_BUFFER_SIZE(dir);
    unsigned char bufbits = ISOFS_BUFFER_BITS(dir);
    unsigned long block, f_pos, offset, block_saved, offset_saved;
    struct buffer_head *bh = NULL;
    struct isofs_sb_info *sbi = ISOFS_SB(dir->i_sb);

    if (!ISOFS_I(dir)->i_first_extent)
    {
        return 0;
    }

    f_pos = 0;
    offset = 0;
    block = 0;

    while (f_pos < dir->i_size)
    {
        struct iso_directory_record *de;
        int de_len, match, i, dlen;
        char *dpnt;

        if (!bh)
        {
            bh = isofs_bread(dir, block);
            if (!bh)
            {
                return 0;
            }
        }

        de = (struct iso_directory_record *)(bh->b_data + offset);

        de_len = *(unsigned char *) de;
        if (!de_len)
        {
            brelse(bh);
            bh = NULL;
            f_pos = (f_pos + ISOFS_BLOCK_SIZE) & ~(ISOFS_BLOCK_SIZE - 1);
            block = f_pos >> bufbits;
            offset = 0;
            continue;
        }

        block_saved = bh->b_blocknr;
        offset_saved = offset;
        offset += de_len;
        f_pos += de_len;

        /* Make sure we have a full directory entry */
        if (offset >= bufsize)
        {
            int slop = bufsize - offset + de_len;
            memcpy(tmpde, de, slop);
            offset &= bufsize - 1;
            block++;
            brelse(bh);
            bh = NULL;
            if (offset)
            {
                bh = isofs_bread(dir, block);
                if (!bh)
                {
                    return 0;
                }
                //              memcpy((void *) tmpde + slop, bh->b_data, offset);
                memcpy((char *)tmpde + slop, bh->b_data, offset);
            }
            de = tmpde;
        }

        dlen = de->name_len[0];
        dpnt = de->name;

        if (sbi->s_rock &&
            ((i = get_rock_ridge_filename(de, tmpname, dir))))
        {
            dlen = i;   /* possibly -1 */
            dpnt = tmpname;
#ifdef CONFIG_JOLIET
        }
        else if (sbi->s_joliet_level)
        {
            dlen = get_joliet_filename(de, tmpname, dir);
            dpnt = tmpname;
#endif
        }
        else if (sbi->s_mapping == 'a')
        {
            dlen = get_acorn_filename(de, tmpname, dir);
            dpnt = tmpname;
        }
        else if (sbi->s_mapping == 'n')
        {
            dlen = isofs_name_translate(de, tmpname, dir);
            dpnt = tmpname;
        }

        /*
         * Skip hidden or associated files unless hide or showassoc,
         * respectively, is set
         */
        match = 0;
        if (dlen > 0 &&
            (sbi->s_hide == 'n' ||
             (!(de->flags[-sbi->s_high_sierra] & 1))) &&
            (sbi->s_showassoc == 'y' ||
             (!(de->flags[-sbi->s_high_sierra] & 4))))
        {
            match = (isofs_cmp(dentry, dpnt, dlen) == 0);
        }
        if (match)
        {
            isofs_normalize_block_and_offset(de,
                                             &block_saved,
                                             &offset_saved);
            *block_rv = block_saved;
            *offset_rv = offset_saved;
            brelse(bh);
            return 1;
        }
    }
    brelse(bh);
    return 0;
}

struct dentry *isofs_lookup(struct inode *dir, struct dentry *dentry, struct nameidata *nd)
{
    int found;
    //unsigned long uninitialized_var(block);
    //unsigned long uninitialized_var(offset);
    unsigned long block;
    unsigned long offset;
    struct inode *inode;
    struct page *page;

    dentry->d_op = dir->i_sb->s_root->d_op;

    /* maybe one page too bigger */
    page = alloc_page(GFP_USER);
    if (!page)
    {
        return ERR_PTR(-ENOMEM);
    }

    //lock_kernel();
    found = isofs_find_entry(dir, dentry,
                             &block, &offset,
                             page_address(page),
                             (struct iso_directory_record *)(1024 + page_address(page)));
    __free_page(page);

    inode = NULL;
    if (found)
    {
        inode = isofs_iget(dir->i_sb, block, offset);
        if (!inode)
        {
            //unlock_kernel();
            return ERR_PTR(-EACCES);
        }
    }
    //unlock_kernel();
    return d_splice_alias(inode, dentry);
}