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
#include <stdarg.h>
#include "fs.h"
#include "fatfs.h"
#include "buffer_head.h"
#include "endians.h"
#include "errno.h"
#include "fsys_debug.h"

extern const struct timezone sys_tz;

/* Linear day numbers of the respective 1sts in non-leap years. */
static int day_n[] =
{
    /* Jan  Feb  Mar  Apr   May  Jun  Jul  Aug  Sep  Oct  Nov  Dec */
    0,  31,  59,  90,  120, 151, 181, 212, 243, 273, 304, 334, 0, 0, 0, 0
};

/* Convert a MS-DOS time/date pair to a UNIX date (seconds since 1 1 70). */
int date_dos2unix(unsigned short time, unsigned short date)
{
    int month, year, secs;

    /*
     * first subtract and mask after that... Otherwise, if
     * date == 0, bad things happen
     */
    month = ((date >> 5) - 1) & 15;
    year = date >> 9;
    secs = (time & 31) * 2 + 60 * ((time >> 5) & 63) + (time >> 11) * 3600 + 86400 *
           ((date & 31) - 1 + day_n[month] + (year / 4) + year * 365 - ((year & 3) == 0 &&
                   month < 2 ? 1 : 0) + 3653);
    /* days since 1.1.70 plus 80's leap day */
    secs += sys_tz.tz_minuteswest * 60;
    return secs;
}

#if defined CONFIG_FSYS_FAT_RW
/* Convert linear UNIX date to a MS-DOS time/date pair. */
void fat_date_unix2dos(int unix_date, __le16 *time, __le16 *date)
{
    int day, year, nl_day, month;
    unix_date -= sys_tz.tz_minuteswest * 60;

    /* Jan 1 GMT 00:00:00 1980. But what about another time zone? */
    if (unix_date < 315532800)
    {
        unix_date = 315532800;
    }

    *time = cpu_to_le16((unix_date % 60) / 2 + (((unix_date / 60) % 60) << 5) +
                        (((unix_date / 3600) % 24) << 11));
    day = unix_date / 86400 - 3652;
    year = day / 365;
    if ((year + 3) / 4 + 365 * year > day)
    {
        year--;
    }
    day -= (year + 3) / 4 + 365 * year;
    if (day == 59 && !(year & 3))
    {
        nl_day = day;
        month = 2;
    }
    else
    {
        nl_day = (year & 3) || day <= 59 ? day : day - 1;
        for (month = 0; month < 12; month++)
        {
            if (day_n[month] > nl_day)
            {
                break;
            }
        }
    }
    *date = cpu_to_le16(nl_day - day_n[month - 1] + 1 + (month << 5) + (year << 9));
}

/* Flushes the number of free clusters on FAT32 */
/* XXX: Need to write one per FSINFO block.  Currently only writes 1 */
void fat_clusters_flush(struct super_block *sb)
{
    struct msdos_sb_info *sbi = MSDOS_SB(sb);
    struct buffer_head *bh;
    struct fat_boot_fsinfo *fsinfo;

    if (sbi->fat_bits != 32)
    {
        return;
    }

    bh = sb_bread(sb, sbi->fsinfo_sector);
    if (bh == NULL)
    {
        fs_log_error("FAT: bread failed in fat_clusters_flush\n");
        return;
    }

    fsinfo = (struct fat_boot_fsinfo *)bh->b_data;
    /* Sanity check */
    if (!IS_FSINFO(fsinfo))
    {
        fs_log_error("FAT: Did not find valid FSINFO signature.\n"
                     "     Found signature1 0x%08x signature2 0x%08x"
                     " (sector = %lu)\n",
                     le32_to_cpu(fsinfo->signature1),
                     le32_to_cpu(fsinfo->signature2),
                     sbi->fsinfo_sector);
    }
    else
    {
        if ((int)sbi->free_clusters != -1)
        {
            fsinfo->free_clusters = cpu_to_le32(sbi->free_clusters);
        }
        if ((int)sbi->prev_free != -1)
        {
            fsinfo->next_cluster = cpu_to_le32(sbi->prev_free);
        }
        mark_buffer_dirty(bh);
    }
    brelse(bh);
}

/*
 * fat_chain_add() adds a new cluster to the chain of clusters represented
 * by inode.
 */
int fat_chain_add(struct inode *inode, int new_dclus, int nr_cluster)
{
    struct super_block *sb = inode->i_sb;
    struct msdos_sb_info *sbi = MSDOS_SB(sb);
    int ret, new_fclus, last;

    //debug
    struct msdos_inode_info *ms_inode = MSDOS_I(inode);

    /*
     * We must locate the last cluster of the file to add this new
     * one (new_dclus) to the end of the link list (the FAT).
     */
    last = new_fclus = 0;
    if (ms_inode->i_start)
    {
        int fclus, dclus;

        ret = fat_get_cluster(inode, FAT_ENT_EOF, &fclus, &dclus, NULL);
        if (ret < 0)
        {
            return ret;
        }
        new_fclus = fclus + 1;
        last = dclus;
    }

    /* add new one to the last of the cluster chain */
    if (last)
    {
        struct fat_entry fatent;

        fatent_init(&fatent);
        ret = fat_ent_read(inode, &fatent, last);
        if (ret >= 0)
        {
            int wait = inode_needs_sync(inode);
            ret = fat_ent_write(inode, &fatent, new_dclus, wait);
            fatent_brelse(&fatent);
        }
        if (ret < 0)
        {
            return ret;
        }
        //      fat_cache_add(inode, new_fclus, new_dclus);
    }
    else
    {
        ms_inode->i_start = new_dclus;
        ms_inode->i_logstart = new_dclus;
        /*
         * Since generic_osync_inode() synchronize later if
         * this is not directory, we don't here.
         */
        if (S_ISDIR(inode->i_mode) && IS_DIRSYNC(inode))
        {
            ret = fat_sync_inode(inode);
            if (ret)
            {
                return ret;
            }
        }
        else
        {
            mark_inode_dirty(inode);
        }
    }
    if (new_fclus != (inode->i_blocks >> (sbi->cluster_bits - 9)))
    {
        fat_fs_panic(sb, "%s: clusters badly computed (%d != %lu)",
                     __FUNCTION__, new_fclus, inode->i_blocks >> (sbi->cluster_bits - 9));
        fat_cache_inval_inode(inode);
    }
    inode->i_blocks += nr_cluster << (sbi->cluster_bits - 9);

    return 0;
}

int fat_sync_bhs(struct buffer_head **bhs, int nr_bhs)
{
    int i, err = 0;

    ll_rw_block(WRITE, nr_bhs, bhs);
    for (i = 0; i < nr_bhs; i++)
    {
        wait_on_buffer(bhs[i]);
        //      if (buffer_eopnotsupp(bhs[i])) {
        //          clear_buffer_eopnotsupp(bhs[i]);
        //          err = -EOPNOTSUPP;
        //      } else
        if (!err && !buffer_uptodate(bhs[i]))
        {
            err = -EIO;
        }
    }
    return err;
}
#endif