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
#include "fsys_debug.h"

/*
 * look if the driver can tell the multi session redirection value
 *
 * don't change this if you don't know what you do, please!
 * Multisession is legal only with XA disks.
 * A non-XA disk with more than one volume descriptor may do it right, but
 * usually is written in a nowhere standardized "multi-partition" manner.
 * Multisession uses absolute addressing (solely the first frame of the whole
 * track is #0), multi-partition uses relative addressing (each first frame of
 * each track is #0), and a track is not a session.
 *
 * A broken CDwriter software or drive firmware does not set new standards,
 * at least not if conflicting with the existing ones.
 *
 * emoenke@gwdg.de
 */
#define WE_OBEY_THE_WRITTEN_STANDARDS 1

unsigned int isofs_get_last_session(struct super_block *sb)
{
    //  struct cdrom_multisession ms_info;
    //  unsigned int vol_desc_start;
    //  struct block_device *bdev = sb->s_bdev;
    //  int i;
    //
    //  vol_desc_start=0;
    //  ms_info.addr_format=CDROM_LBA;
    //  if(session >= 0 && session <= 99) {
    //      struct cdrom_tocentry Te;
    //      Te.cdte_track=session;
    //      Te.cdte_format=CDROM_LBA;
    //      i = ioctl_by_bdev(bdev, CDROMREADTOCENTRY, (unsigned long) &Te);
    //      if (!i) {
    //          __log(KERN_DEBUG "ISOFS: Session %d start %d type %d",
    //              session, Te.cdte_addr.lba,
    //              Te.cdte_ctrl&CDROM_DATA_TRACK);
    //          if ((Te.cdte_ctrl&CDROM_DATA_TRACK) == 4)
    //              return Te.cdte_addr.lba;
    //      }
    //
    //      __log(KERN_ERR "ISOFS: Invalid session number or type of track");
    //  }
    //  i = ioctl_by_bdev(bdev, CDROMMULTISESSION, (unsigned long) &ms_info);
    //  if (session > 0)
    //      __log(KERN_ERR "ISOFS: Invalid session number");
    //#if 0
    //  __log(KERN_DEBUG "isofs.inode: CDROMMULTISESSION: rc=%d",i);
    //  if (i==0) {
    //      __log(KERN_DEBUG "isofs.inode: XA disk: %s",ms_info.xa_flag?"yes":"no");
    //      __log(KERN_DEBUG "isofs.inode: vol_desc_start = %d", ms_info.addr.lba);
    //  }
    //#endif
    //  if (i==0)
    //#if WE_OBEY_THE_WRITTEN_STANDARDS
    //      if (ms_info.xa_flag) /* necessary for a valid ms_info.addr */
    //#endif
    //          vol_desc_start=ms_info.addr.lba;
    //  return vol_desc_start;

    __u32   vol_desc_start = 0;
    int     ret = 0;

    ret = esFSYS_pioctrl(sb->s_part, PART_IOC_CDROM_MULTISESSION, 0, (void *)(&vol_desc_start));
    if (ret == EPDK_OK)
    {
        fs_log_debug("vol_desc_start=%d\n", vol_desc_start);
    }
    else
    {
        fs_log_debug("CDROMMULTISESSION not supported: rc=%d\n", ret);
    }
    return vol_desc_start;
}