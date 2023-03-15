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

static __s32 minfs_romldr_ioctl(struct inode *inode, __u32 cmd, __u32 aux, void *buffer)
{
    struct super_block      *sb  = inode->i_sb;
    struct minfs_sb_info    *sbi = MINFS_SB(sb);
    struct minfs_inode_info *exi = MINFS_I(inode);
    __minfs_dentry_t        *pdentry;
    __section_rom_hdr_t     *promhdr;
    __minfs_section_hdr_t   *psectionhdr;
    __u32                    sections_num;

    if (!(exi->Attribute & MINFS_ATTR_MODULE) || (buffer == NULL))
    {
        fs_log_error("invalid module access\n");
        return -EINVAL;
    }

    //file dentry metadata
    pdentry     = (__minfs_dentry_t *)(sbi->pDEntryData + exi->DEntryOffset - sbi->RootDataOffet);

    //melis module file section header
    psectionhdr = (__minfs_section_hdr_t *)(pdentry->MetaData + MINFS_ALIGN(pdentry->NameLen, MINFS_NAME_ALIGN));

    //sections number
    sections_num    = (pdentry->ExtentLen / MINFS_SECTION_HDR_LEN);

    switch (cmd)
    {
        case ROMLDR_IOC_GET_MAGIC_INDEX   :
        {
            __u32 index = 0;

            while (index < sections_num)
            {
                if (psectionhdr->Attribute & MINFS_SECTION_ATTR_MAGIC)
                {
                    //magic section find
                    *((__u32 *)buffer) = index;
                    return 0;
                }
                //seach next section
                psectionhdr++;
                index++;
            }
            return -EINVAL;
        }
        case ROMLDR_IOC_GET_SECTION_NUM   :
        {
            //get the number of sections
            *((__u32 *)buffer) = sections_num;
            return 0;
        }
        case ROMLDR_IOC_GET_SECTION_HDR   :
        {
            //get special section header
            promhdr = (__section_rom_hdr_t *)buffer;
            if (aux >= sections_num)
            {
                fs_log_warning("invalid section index\n");
                return -EINVAL;
            }
            //seach to target section
            psectionhdr     = &(psectionhdr[aux]);

            //user just have interesting in original section size,
            //by sunny at 2011-3-29 21:32:57.
            promhdr->Size   = psectionhdr->Size;
            promhdr->VAddr  = psectionhdr->VAddr;
            promhdr->Type   = psectionhdr->Type;
            promhdr->Flags  = psectionhdr->Flags;
            return 0;
        }
        case ROMLDR_IOC_GET_SECTION_DATA  :
        {
            //get special section data
            psectionhdr = &(psectionhdr[aux]);

            if (psectionhdr->RecSize == 0)
            {
                fs_log_error("section data have no any data record failed\n");
                return -EINVAL;
            }
            if (psectionhdr->Attribute & MINFS_SECTION_ATTR_COMPRESS)
            {
                __u8            *ptmpdata;
                unsigned long   uncompresslen = 0;
                unsigned long   compresslen = 0;

                //allocate a temp buffer
                compresslen = psectionhdr->RecSize;
                ptmpdata    = minfs_allocate_temp_buffer(sb, compresslen);
                if (ptmpdata == NULL)
                {
                    fs_log_error("allocate temp buffer for section data failed\n");
                    return -ENOMEM;
                }
                //load compressed section data to temp buffer
                if (compresslen != minfs_pread(sb, ptmpdata, exi->DataOffset + psectionhdr->Offset, compresslen))
                {
                    minfs_free_temp_buffer(sb, ptmpdata, compresslen);
                    fs_log_error("read module section data failed\n");
                    return -EIO;
                }
                //uncompress section data to use buffer,
                //the user buffer can't be smaller then uncomress length.
                uncompresslen = psectionhdr->RecUnPackSize;
                if (minfs_uncompress(buffer, &uncompresslen, ptmpdata, &compresslen)
                    != EPDK_OK)
                {
                    fs_log_error("uncompress section data failed\n");
                    minfs_free_temp_buffer(sb, ptmpdata, compresslen);
                    return -EIO;
                }
                //section data load succeeded
                minfs_free_temp_buffer(sb, ptmpdata, compresslen);
                return 0;
            }
            else
            {
                //uncompress section, read to user data buffer directly.
                if (psectionhdr->RecSize != minfs_pread(sb, buffer, exi->DataOffset + psectionhdr->Offset, psectionhdr->RecSize))
                {
                    fs_log_error("read module section data failed\n");
                    return -EIO;
                }
                //section data load succeeded
                return 0;
            }
        }
        default:
        {
            //invalid ioctl command for minfs
            return -EINVAL;
        }
    }
    //unreached, make some compiler happy
    return 0;
}


static __s32 minfs_file_ioctl(struct inode *inode, struct file *filp, __u32 cmd, __u32 aux, void *buffer)
{
    int                     ret = 0;
    struct super_block      *sb  = inode->i_sb;
    struct minfs_sb_info    *sbi = MINFS_SB(sb);
    struct minfs_inode_info *exi = MINFS_I(inode);
    __minfs_dentry_t        *pdentry;

    //the ioctl function is not been protected by vfs lock,
    //so we have to protect it by self.
    minfs_lock(sb);
    if (ROMLDR_IOC_CMD(cmd))
    {
        ret = minfs_romldr_ioctl(inode, cmd, aux, buffer);
        goto out;
    }

    if (cmd == MINFS_IOC_GET_UNCOMPRESS_FILE_SIZE)
    {
        pdentry = (__minfs_dentry_t *)(sbi->pDEntryData + exi->DEntryOffset - sbi->RootDataOffet);

        //the file must been compressed
        if (!(exi->Attribute & MINFS_ATTR_COMPRESS) || (buffer == NULL))
        {
            fs_log_error("invalid compress file access\n");
            ret = -EINVAL;
            goto out;
        }
        *((__u32 *)buffer) = pdentry->UnPackSize;
        ret = 0;
        goto out;
    }

    if (cmd == MINFS_IOC_GET_UNCOMPRESS_FILE_DATA)
    {
        __u8            *ptmpdata;
        unsigned long   uncompresslen;
        unsigned long   compresslen;

        pdentry = (__minfs_dentry_t *)(sbi->pDEntryData + exi->DEntryOffset - sbi->RootDataOffet);

        //the file must been compressed
        if (!(exi->Attribute & MINFS_ATTR_COMPRESS) || (buffer == NULL))
        {
            fs_log_error("invalid compress file access\n");
            ret = -EINVAL;
            goto out;
        }

        //allocate a temp buffer
        compresslen = pdentry->Size;
        ptmpdata = minfs_allocate_temp_buffer(sb, compresslen);
        if (ptmpdata == NULL)
        {
            fs_log_error("allocate temp buffer for file data failed\n");
            ret = -ENOMEM;
            goto out;
        }

        //load compressed section data to temp buffer
        if (compresslen != minfs_pread(sb, ptmpdata, pdentry->Offset, compresslen))
        {
            minfs_free_temp_buffer(sb, ptmpdata, compresslen);
            fs_log_error("read module section data failed\n");
            ret = -EIO;
            goto out;
        }

        //uncompress section data to use buffer,
        //the user buffer can't be smaller then uncomress length.
        uncompresslen = pdentry->UnPackSize;

        if (minfs_uncompress(buffer, &uncompresslen, ptmpdata, &compresslen)!= EPDK_OK)
        {
            fs_log_error("uncompress section data failed\n");
            minfs_free_temp_buffer(sb, ptmpdata, compresslen);
            ret = -EIO;
            goto out;
        }

        //section data load succeeded
        ret = 0;
        goto out;
    }
    fs_log_warning("invalid command for minfs ioctl\n");

out:
    //unlock minfs super block
    minfs_unlock(sb);
    return ret;
}

static __s32 minfs_file_read(struct file *filp, char *buffer, __u32 len, __s64 *pos)
{
    int     count;
    int     read_number;
    struct  inode *inode = filp->f_dentry->d_inode;
    struct minfs_inode_info *exi = MINFS_I(inode);

    if (*pos >= inode->i_size)
    {
        //read beyond file end
        return 0;
    }

    //you can't read beyond file end, adjust read length
    read_number = *pos + len < inode->i_size ? len : inode->i_size - *pos;

    //read low lelvel device directly
    count = minfs_pread(filp->f_dentry->d_sb, buffer, (__u32)(*pos + exi->DataOffset), read_number);
    (*pos) += count;

    return count;
}

const struct file_operations minfs_file_operations =
{
    .llseek     = generic_file_llseek,
    .read       = minfs_file_read,
    .ioctl      = minfs_file_ioctl,
};