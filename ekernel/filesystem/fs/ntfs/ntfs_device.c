/**
 * device.c - Low level device io functions. Originated from the Linux-NTFS project.
 *
 * Copyright (c) 2004-2006 Anton Altaparmakov
 * Copyright (c) 2004-2006 Szabolcs Szakacsits
 *
 * This program/include file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program/include file is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the NTFS-3G
 * distribution in the file COPYING); if not, write to the Free Software
 * Foundation,Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <string.h>
#include "types.h"
#include "mst.h"
#include "device.h"
#include "ntfs.h"
#include "errno.h"
#include "blk_dev.h"
#include "fsys_libs.h"
#include "fsys_debug.h"

/**
 * ntfs_pread - positioned read from disk
 * @dev:    device to read from
 * @pos:    position in device to read from
 * @count:  number of bytes to read
 * @b:      output data buffer
 *
 * This function will read @count bytes from device @dev at position @pos into
 * the data buffer @b.
 *
 * On success, return the number of successfully read bytes. If this number is
 * lower than @count this means that we have either reached end of file or
 * encountered an error during the read so that the read is partial. 0 means
 * end of file or nothing to read (@count is 0).
 *
 * On error and nothing has been read, return -1 with fs_err set appropriately
 * to the return code of either seek, read, or set to EINVAL in case of
 * invalid arguments.
 *
 * for improve read performance, this part rewrite by sunny 2009.11.03
 *
 */
s64 ntfs_pread(__hdle part, const s64 pos, s64 count, void *b)
{
    s32 br;
    s64 offset, chunk_size, left, cur_pos;
    unsigned char *alignedbuffer = NULL;
    u32 allocsize;

    offset = pos & (NTFS_BLOCK_SIZE - 1);
    cur_pos = pos;
    left = count;
    if (((uintptr_t)b >= CONFIG_DRAM_VIRTBASE) && ((uintptr_t)b <= (CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_SIZE)))
    {

        /* Read first sector if start of read not sector aligned. */
        if (offset)
        {
            allocsize = NTFS_BLOCK_SIZE;
            alignedbuffer = (unsigned char *)malloc(allocsize);
            if (!alignedbuffer)
            {
                fs_err = ENOMEM;
                fs_log_trace01("esMEMS_Balloc failed for read.\n");
                cur_pos = pos - 1;  /* for return -1 */
                goto out;
            }
            br = esFSYS_pread(alignedbuffer, cur_pos >> NTFS_BLOCK_SIZE_BITS, 1, part);
            if (br != 1)
            {
                fs_err = EIO;
                fs_log_trace01("Read block %d failed.\n", cur_pos >> NTFS_BLOCK_SIZE_BITS);
                goto out;
            }
            chunk_size = left <= (NTFS_BLOCK_SIZE - offset) ? left : (NTFS_BLOCK_SIZE - offset);
            memcpy(b, alignedbuffer + offset, chunk_size);
            left -= chunk_size;
            b = (void *)((char *)b + chunk_size);
            cur_pos += chunk_size;
        }
        /* Read aligned sectors */
        if (left >> NTFS_BLOCK_SIZE_BITS)
        {
            br = esFSYS_pread(b, cur_pos >> NTFS_BLOCK_SIZE_BITS, left >> NTFS_BLOCK_SIZE_BITS, part);
            if (br != left >> NTFS_BLOCK_SIZE_BITS)
            {
                fs_err = EIO;
                fs_log_trace01("Read block %d failed.\n", cur_pos >> NTFS_BLOCK_SIZE_BITS);
                goto out;
            }
            chunk_size = left & ~(s64)(NTFS_BLOCK_SIZE - 1);
            left -= chunk_size;
            b = (void *)((char *)b + chunk_size);
            cur_pos += chunk_size;
        }
        /* Read last sector if end of read not sector aligned,
         * if the start of read is aligned by sector,
         * we should allocate alignedbuffer here.*/
        if (left)
        {
            if (!alignedbuffer)
            {
                allocsize = NTFS_BLOCK_SIZE;
                alignedbuffer = (unsigned char *)malloc(allocsize);
                if (!alignedbuffer)
                {
                    fs_err = ENOMEM;
                    fs_log_trace01("esMEMS_Balloc failed for read.\n");
                    cur_pos = pos - 1;  /* for return -1 */
                    goto out;
                }
            }
            br = esFSYS_pread(alignedbuffer, cur_pos >> NTFS_BLOCK_SIZE_BITS, 1, part);
            if (br != 1)
            {
                fs_err = EIO;
                fs_log_trace01("Read block %d failed.\n", cur_pos >> NTFS_BLOCK_SIZE_BITS);
                goto out;
            }
            chunk_size = left;
            memcpy(b, alignedbuffer, chunk_size);
            left -= chunk_size;
            b = (void *)((char *)b + chunk_size);
            cur_pos += chunk_size;
        }
    }
    else
    {
        left = (offset + count + NTFS_BLOCK_SIZE - 1) & ~(s64)(NTFS_BLOCK_SIZE - 1);
        allocsize = left;
        alignedbuffer = (unsigned char *)malloc(allocsize);
        if (!alignedbuffer)
        {
            fs_err = ENOMEM;
            fs_log_trace01("esMEMS_Balloc failed for read.\n");
            cur_pos = pos - 1;  /* for return -1 */
            goto out;
        }
        br = esFSYS_pread(alignedbuffer, cur_pos >> NTFS_BLOCK_SIZE_BITS, left >> NTFS_BLOCK_SIZE_BITS, part);
        if (br == left >> NTFS_BLOCK_SIZE_BITS)
        {
            memcpy(b, alignedbuffer + offset, count);
            cur_pos += count;
        }
    }
out:
    if (alignedbuffer)
    {
        free(alignedbuffer);
    }
    return cur_pos - pos;
}

/**
 * ntfs_mst_pread - multi sector transfer (mst) positioned read
 * @dev:    device to read from
 * @pos:    position in file descriptor to read from
 * @count:  number of blocks to read
 * @bksize: size of each block that needs mst deprotecting
 * @b:      output data buffer
 *
 * Multi sector transfer (mst) positioned read. This function will read @count
 * blocks of size @bksize bytes each from device @dev at position @pos into the
 * the data buffer @b.
 *
 * On success, return the number of successfully read blocks. If this number is
 * lower than @count this means that we have reached end of file, that the read
 * was interrupted, or that an error was encountered during the read so that
 * the read is partial. 0 means end of file or nothing was read (also return 0
 * when @count or @bksize are 0).
 *
 * On error and nothing was read, return -1 with fs_err set appropriately to the
 * return code of either seek, read, or set to EINVAL in case of invalid
 * arguments.
 *
 * NOTE: If an incomplete multi sector transfer has been detected the magic
 * will have been changed to magic_BAAD but no error will be returned. Thus it
 * is possible that we return count blocks as being read but that any number
 * (between zero and count!) of these blocks is actually subject to a multi
 * sector transfer error. This should be detected by the caller by checking for
 * the magic being "BAAD".
 */
s64 ntfs_mst_pread(__hdle part, const s64 pos, s64 count,
                   const u32 bksize, void *b)
{
    s64 br, i;

    if (bksize & (bksize - 1) || bksize % NTFS_BLOCK_SIZE)
    {
        fs_err = EINVAL;
        return -1;
    }
    /* Do the read. */
    br = ntfs_pread(part, pos, count * bksize, b);
    if (br < 0)
    {
        return br;
    }
    /*
     * Apply fixups to successfully read data, disregarding any errors
     * returned from the MST fixup function. This is because we want to
     * fixup everything possible and we rely on the fact that the "BAAD"
     * magic will be detected later on.
     */
    count = br / bksize;
    for (i = 0; i < count; ++i)
        ntfs_mst_post_read_fixup((NTFS_RECORD *)
                                 ((u8 *)b + i * bksize), bksize);
    /* Finally, return the number of complete blocks read. */
    return count;
}

/**
 * ntfs_device_partition_start_sector_get - get starting sector of a partition
 * @dev:    open device
 *
 * On success, return the starting sector of the partition @dev in the parent
 * block device of @dev.  On error return -1 with fs_err set to the error code.
 *
 * The following error codes are defined:
 *  EINVAL      Input parameter error
 *  EOPNOTSUPP  System does not support HDIO_GETGEO ioctl
 *  ENOTTY      @dev is a file or a device not supporting HDIO_GETGEO
 */
s64 ntfs_device_partition_start_sector_get(__hdle part)
{
    return 0;
}

/**
 * ntfs_device_heads_get - get number of heads of device
 * @dev:        open device
 *
 * On success, return the number of heads on the device @dev.  On error return
 * -1 with fs_err set to the error code.
 *
 * The following error codes are defined:
 *  EINVAL      Input parameter error
 *  EOPNOTSUPP  System does not support HDIO_GETGEO ioctl
 *  ENOTTY      @dev is a file or a device not supporting HDIO_GETGEO
 */
int ntfs_device_heads_get(__hdle part)
{
    return -1;
}

/**
 * ntfs_device_sectors_per_track_get - get number of sectors per track of device
 * @dev:        open device
 *
 * On success, return the number of sectors per track on the device @dev.  On
 * error return -1 with fs_err set to the error code.
 *
 * The following error codes are defined:
 *  EINVAL      Input parameter error
 *  EOPNOTSUPP  System does not support HDIO_GETGEO ioctl
 *  ENOTTY      @dev is a file or a device not supporting HDIO_GETGEO
 */
int ntfs_device_sectors_per_track_get(__hdle part)
{
    return -1;
}

/**
 * ntfs_device_sector_size_get - get sector size of a device
 * @dev:    open device
 *
 * On success, return the sector size in bytes of the device @dev.
 * On error return -1 with fs_err set to the error code.
 *
 * The following error codes are defined:
 *  EINVAL      Input parameter error
 *  EOPNOTSUPP  System does not support BLKSSZGET ioctl
 *  ENOTTY      @dev is a file or a device not supporting BLKSSZGET
 */
int ntfs_device_sector_size_get(__hdle part)
{
    return NTFS_BLOCK_SIZE;
}

/**
 * ntfs_device_block_size_set - set block size of a device
 * @dev:    open device
 * @block_size: block size to set @dev to
 *
 * On success, return 0.
 * On error return -1 with fs_err set to the error code.
 *
 * The following error codes are defined:
 *  EINVAL      Input parameter error
 *  EOPNOTSUPP  System does not support BLKBSZSET ioctl
 *  ENOTTY      @dev is a file or a device not supporting BLKBSZSET
 */
int ntfs_device_block_size_set(__hdle part, int block_size)
{
    /* NTFS not support block size is not 512 byte device now
     * By sunny at 2010-7-18.
     */
    return 0;
}

#if defined CONFIG_FSYS_NTFS_RW

/**
 * ntfs_pwrite - positioned write to disk
 * @dev:    device to write to
 * @pos:    position in file descriptor to write to
 * @count:  number of bytes to write
 * @b:      data buffer to write to disk
 *
 * This function will write @count bytes from data buffer @b to the device @dev
 * at position @pos.
 *
 * On success, return the number of successfully written bytes. If this number
 * is lower than @count this means that the write has been interrupted in
 * flight or that an error was encountered during the write so that the write
 * is partial. 0 means nothing was written (also return 0 when @count is 0).
 *
 * On error and nothing has been written, return -1 with fs_err set
 * appropriately to the return code of either seek, write, or set
 * to EINVAL in case of invalid arguments.
 */
s64 ntfs_pwrite(__hdle part, const s64 pos, s64 count,
                const void *b)
{
    s64 old_pos, to_write, i, bw = 0;
    unsigned char *alignedbuffer;
    int old_ofs, ofs;

    if (!b || count < 0 || pos < 0)
    {
        fs_err = EINVAL;
        return -1;
    }
    if (!count)
    {
        return 0;
    }

    old_pos = pos;
    old_ofs = ofs = old_pos & (NTFS_BLOCK_SIZE - 1);

    to_write = (ofs + count + NTFS_BLOCK_SIZE - 1) &
               ~(s64)(NTFS_BLOCK_SIZE - 1);

    fs_log_trace01("b = %p, count = 0x%x, pos = 0x%x, "
                   "ofs = %d, to_write = 0x%x.\n", b,
                   (int)count, (int)old_pos, ofs,
                   (int)to_write);

    if ((((uintptr_t)b >= CONFIG_DRAM_VIRTBASE) && ((uintptr_t)b <= (CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_SIZE)))
        && !old_ofs && !(count & (NTFS_BLOCK_SIZE - 1)))
    {
        alignedbuffer = (unsigned char *)b;
    }
    else
    {
        s64 end;

        alignedbuffer = (unsigned char *)malloc(to_write);

        if (!alignedbuffer)
        {
            fs_err = ENOMEM;
            fs_log_trace01("VirtualAlloc failed for write.\n");
            return -1;
        }
        /* Read first sector if start of write not sector aligned. */
        if (ofs)
        {
            i = esFSYS_pread(alignedbuffer, old_pos >> NTFS_BLOCK_SIZE_BITS, 1, part);
            //debug
            i =  NTFS_BLOCK_SIZE;
            if (i != NTFS_BLOCK_SIZE)
            {
                if (i >= 0)
                {
                    fs_err = EIO;
                }
                goto write_error;
            }
        }
        /*
         * Read last sector if end of write not sector aligned and last
         * sector is either not the same as the first sector or it is
         * the same as the first sector but this has not been read in
         * yet, i.e. the start of the write is sector aligned.
         */
        end = old_pos + count;
        if ((end & (NTFS_BLOCK_SIZE - 1)) &&
            ((to_write > NTFS_BLOCK_SIZE) || !ofs))
        {
            i = esFSYS_pread(alignedbuffer + to_write - NTFS_BLOCK_SIZE, end >> NTFS_BLOCK_SIZE_BITS, 1, part);
            //debug
            i = NTFS_BLOCK_SIZE;
            if (i != NTFS_BLOCK_SIZE)
            {
                if (i >= 0)
                {
                    fs_err = EIO;
                }
                goto write_error;
            }
        }
        /* Copy the data to be written into @alignedbuffer. */
        memcpy(alignedbuffer + ofs, b, count);
    }

    bw = esFSYS_pwrite(alignedbuffer, old_pos >> NTFS_BLOCK_SIZE_BITS, to_write >> NTFS_BLOCK_SIZE_BITS, part);
    bw <<= NTFS_BLOCK_SIZE_BITS;

    //debug
    if (bw == -1)
    {
        goto write_error;
    }
    if (bw < ofs)
    {
        bw = 0;
        goto write_partial;
    }
    if (bw > count)
    {
        bw = count;
    }

write_partial:
    if (alignedbuffer != b)
    {
        free(alignedbuffer);
    }
    return bw;
write_error:
    bw = -1;
    goto write_partial;
}

/**
 * ntfs_mst_pwrite - multi sector transfer (mst) positioned write
 * @dev:    device to write to
 * @pos:    position in file descriptor to write to
 * @count:  number of blocks to write
 * @bksize: size of each block that needs mst protecting
 * @b:      data buffer to write to disk
 *
 * Multi sector transfer (mst) positioned write. This function will write
 * @count blocks of size @bksize bytes each from data buffer @b to the device
 * @dev at position @pos.
 *
 * On success, return the number of successfully written blocks. If this number
 * is lower than @count this means that the write has been interrupted or that
 * an error was encountered during the write so that the write is partial. 0
 * means nothing was written (also return 0 when @count or @bksize are 0).
 *
 * On error and nothing has been written, return -1 with fs_err set
 * appropriately to the return code of either seek, write, or set
 * to EINVAL in case of invalid arguments.
 *
 * NOTE: We mst protect the data, write it, then mst deprotect it using a quick
 * deprotect algorithm (no checking). This saves us from making a copy before
 * the write and at the same time causes the usn to be incremented in the
 * buffer. This conceptually fits in better with the idea that cached data is
 * always deprotected and protection is performed when the data is actually
 * going to hit the disk and the cache is immediately deprotected again
 * simulating an mst read on the written data. This way cache coherency is
 * achieved.
 */
s64 ntfs_mst_pwrite(__hdle part, const s64 pos, s64 count,
                    const u32 bksize, void *b)
{
    s64 written, i;

    if (count < 0 || bksize % NTFS_BLOCK_SIZE)
    {
        fs_err = EINVAL;
        return -1;
    }
    if (!count)
    {
        return 0;
    }
    /* Prepare data for writing. */
    for (i = 0; i < count; ++i)
    {
        int err;

        err = ntfs_mst_pre_write_fixup((NTFS_RECORD *)
                                       ((u8 *)b + i * bksize), bksize);
        if (err < 0)
        {
            /* Abort write at this position. */
            if (!i)
            {
                return err;
            }
            count = i;
            break;
        }
    }
    /* Write the prepared data. */
    written = ntfs_pwrite(part, pos, count * bksize, b);
    /* Quickly deprotect the data again. */
    for (i = 0; i < count; ++i)
    {
        ntfs_mst_post_write_fixup((NTFS_RECORD *)((u8 *)b + i * bksize));
    }
    if (written <= 0)
    {
        return written;
    }
    /* Finally, return the number of complete blocks written. */
    return written / bksize;
}

/**
 * ntfs_cluster_read - read ntfs clusters
 * @vol:    volume to read from
 * @lcn:    starting logical cluster number
 * @count:  number of clusters to read
 * @b:      output data buffer
 *
 * Read @count ntfs clusters starting at logical cluster number @lcn from
 * volume @vol into buffer @b. Return number of clusters read or -1 on error,
 * with fs_err set to the error code.
 */
s64 ntfs_cluster_read(const ntfs_volume *vol, const s64 lcn, const s64 count,
                      void *b)
{
    s64 br;

    if (!vol || lcn < 0 || count < 0)
    {
        fs_err = EINVAL;
        return -1;
    }
    if (vol->nr_clusters < lcn + count)
    {
        fs_err = ESPIPE;
        fs_log_perror("Trying to read outside of volume "
                      "(%lld < %lld)", (long long)vol->nr_clusters,
                      (long long)lcn + count);
        return -1;
    }
    br = ntfs_pread(vol->_part, lcn << vol->cluster_size_bits,
                    count << vol->cluster_size_bits, b);
    if (br < 0)
    {
        fs_log_perror("Error reading cluster(s)");
        return br;
    }
    return br >> vol->cluster_size_bits;
}

/**
 * ntfs_cluster_write - write ntfs clusters
 * @vol:    volume to write to
 * @lcn:    starting logical cluster number
 * @count:  number of clusters to write
 * @b:      data buffer to write to disk
 *
 * Write @count ntfs clusters starting at logical cluster number @lcn from
 * buffer @b to volume @vol. Return the number of clusters written or -1 on
 * error, with fs_err set to the error code.
 */
s64 ntfs_cluster_write(const ntfs_volume *vol, const s64 lcn,
                       const s64 count, const void *b)
{
    s64 bw;

    if (!vol || lcn < 0 || count < 0)
    {
        fs_err = EINVAL;
        return -1;
    }
    if (vol->nr_clusters < lcn + count)
    {
        fs_err = ESPIPE;
        fs_log_perror("Trying to write outside of volume "
                      "(%lld < %lld)", (long long)vol->nr_clusters,
                      (long long)lcn + count);
        return -1;
    }
    if (!NVolReadOnly(vol))
        bw = ntfs_pwrite(vol->_part, lcn << vol->cluster_size_bits,
                         count << vol->cluster_size_bits, b);
    else
    {
        bw = count << vol->cluster_size_bits;
    }
    if (bw < 0)
    {
        fs_log_perror("Error writing cluster(s)");
        return bw;
    }
    return bw >> vol->cluster_size_bits;
}

/**
 * ntfs_device_size_get - return the size of a device in blocks
 * @dev:    open device
 * @block_size: block size in bytes in which to return the result
 *
 * Return the number of @block_size sized blocks in the device described by the
 * open device @dev.
 *
 * Adapted from e2fsutils-1.19, Copyright (C) 1995 Theodore Ts'o.
 *
 * On error return -1 with fs_err set to the error code.
 */
s64 ntfs_device_size_get(__hdle part, int block_size)
{
    s64 part_sector_count;

    esFSYS_pioctrl(part, PART_IOC_USR_GETPARTSIZE, 0, &part_sector_count);

    return part_sector_count;
}

#endif  /* FSYS_NTFS_RW */
