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
#include "fs.h"
#include "udf_decl.h"
#include "udf_fs.h"
#include "udf_sb.h"

unsigned int udf_get_last_session(struct super_block *sb)
{
    //  struct cdrom_multisession ms_info;
    //  unsigned int vol_desc_start;
    //  struct block_device *bdev = sb->s_bdev;
    //  int i;
    //
    //  vol_desc_start = 0;
    //  ms_info.addr_format = CDROM_LBA;
    //  i = ioctl_by_bdev(bdev, CDROMMULTISESSION, (unsigned long)&ms_info);
    //
    //  if (i == 0) {
    //      udf_debug("XA disk: %s, vol_desc_start=%d\n",
    //            (ms_info.xa_flag ? "yes" : "no"), ms_info.addr.lba);
    //      if (ms_info.xa_flag) /* necessary for a valid ms_info.addr */
    //          vol_desc_start = ms_info.addr.lba;
    //  } else {
    //      udf_debug("CDROMMULTISESSION not supported: rc=%d\n", i);
    //  }
    //  return vol_desc_start;

    __u32   vol_desc_start = 0;
    int     ret = 0;

    vol_desc_start = 0;
    ret = esFSYS_pioctrl(sb->s_part, PART_IOC_CDROM_MULTISESSION, 0, (void *)(&vol_desc_start));
    if (ret == EPDK_OK)
    {
        udf_debug("vol_desc_start=%d\n", vol_desc_start);
    }
    else
    {
        udf_debug("CDROMMULTISESSION not supported: rc=%d\n", ret);
    }
    return vol_desc_start;
}

unsigned long udf_get_last_block(struct super_block *sb)
{
    //  struct block_device *bdev = sb->s_bdev;
    //  unsigned long lblock = 0;
    //
    //  /*
    //   * ioctl failed or returned obviously bogus value?
    //   * Try using the device size...
    //   */
    //  if (ioctl_by_bdev(bdev, CDROM_LAST_WRITTEN, (unsigned long) &lblock) ||
    //      lblock == 0)
    //      lblock = bdev->bd_inode->i_size >> sb->s_blocksize_bits;
    //
    //  if (lblock)
    //      return lblock - 1;
    //  else
    //      return 0;

    __u32 lblock = 0;

    /* just for temp debug use */
    lblock = 2208992;
    /*
        if (esFSYS_pioctrl(sb->s_part, PART_IOC_CDROM_LAST_WRITTEN, 0, (void *)(&lblock)) != EPDK_OK) {
            udf_error("CDROM get last block failed\n");
            return 0;
        }
    */
    if (lblock)
    {
        return lblock - 1;
    }
    else
    {
        return 0;
    }
}