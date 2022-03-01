/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                        Mini ROM Image File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : minfs_device.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-26
* Descript: minfs Low level device io functions.
* Update  : date                auther      ver     notes
*           2011-3-26 15:33:09  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "minfs_i.h"
#include <port.h>
#include <kapi.h>

void *minfs_allocate_temp_buffer(struct super_block *sb, unsigned long len)
{
    //allocate temp use buffer for module section data,
    //maybe will have more reasonable solution for this problem.
    return palloc(len);
}

void minfs_free_temp_buffer(struct super_block *sb, void *buffer, unsigned long len)
{
    pfree(buffer, len);
}

int minfs_cache_init(struct super_block *sb)
{
    struct minfs_sb_info    *sbi = MINFS_SB(sb);
    __u8                    *pbuffer;
    __u32                   i;
    __u8                    err;

    //create lock for minfs cache buffer
    sbi->Cache.Sem = esKRNL_SemCreate(1);
    if (sbi->Cache.Sem == NULL)
    {
        fs_log_error("allocate semaphore for minfs cache buffer failed\n");
        return -EINVAL;
    }

    //initialzie minfs cache manager
    sbi->Cache.CacheItemSectors = 0;
    sbi->Cache.CacheIndex       = 0;
    sbi->Cache.MaxCacheNum      = 0;
    sbi->Cache.pCacheBuffer     = NULL;

    //allocate buffer for cache
    pbuffer = palloc(MINFS_CACHE_SIZE * MINFS_CACHE_MAXNUM);
    if (pbuffer == NULL)
    {
        fs_log_error("allocate memory for minfs cache failed\n");
        esKRNL_SemPost(sbi->Cache.Sem);
        return -ENOMEM;
    }

    //update minfs cache manager parameters
    sbi->Cache.CacheItemSectors = MINFS_CACHE_SIZE >> sbi->SectorBits;
    sbi->Cache.CacheIndex       = 0;
    sbi->Cache.MaxCacheNum      = MINFS_CACHE_MAXNUM;
    sbi->Cache.pCacheBuffer     = pbuffer;
    for (i = 0; i < MINFS_CACHE_MAXNUM; i++)
    {
        sbi->Cache.CacheItems[i].Sector = MINFS_INVALID_SECTOR;
        sbi->Cache.CacheItems[i].pData  = pbuffer + (MINFS_CACHE_SIZE * i);
    }

    return 0;
}

int minfs_cache_exit(struct super_block *sb)
{
    struct minfs_sb_info *sbi = MINFS_SB(sb);
    __u8                  err;

    if (sbi->Cache.pCacheBuffer)
    {
        pfree(sbi->Cache.pCacheBuffer, MINFS_CACHE_SIZE * MINFS_CACHE_MAXNUM);
    }

    if (sbi->Cache.Sem)
    {
        esKRNL_SemPost(sbi->Cache.Sem);
        sbi->Cache.Sem = NULL;
    }

    return 0;
}


int minfs_cache_lock(struct super_block *sb)
{
    struct minfs_sb_info *sbi = MINFS_SB(sb);
    __u8                   err = 0;

    //pend minfs cache buffer semaphore
    esKRNL_SemPend(sbi->Cache.Sem, 0, &err);
    if (err)
    {
        fs_log_error("Pend sem error, error number:%d\n", err);
        return err;
    }
    return 0;
}

int minfs_cache_unlock(struct super_block *sb)
{
    struct minfs_sb_info *sbi = MINFS_SB(sb);

    //post minfs cache buffer semaphore
    esKRNL_SemPost(sbi->Cache.Sem);
    return 0;
}


__minfs_cache_item_t *minfs_get_data_from_cache(struct super_block *sb,
        __u32 sector)
{
    __u32 i;
    struct minfs_sb_info *sbi = MINFS_SB(sb);

    for (i = 0; i < MINFS_CACHE_MAXNUM; i++)
    {
        if (sector == sbi->Cache.CacheItems[i].Sector)
        {
            //target sector find
            return &(sbi->Cache.CacheItems[i]);
        }
    }

    return NULL;
}

__minfs_cache_item_t *minfs_use_cache_load_data(struct super_block *sb,
        __u32 sector)
{
    struct minfs_sb_info *sbi = MINFS_SB(sb);
    __minfs_cache_item_t *pcache_item;
    __u32                 read_num = sbi->Cache.CacheItemSectors;

    pcache_item = &(sbi->Cache.CacheItems[sbi->Cache.CacheIndex]);

    if (esFSYS_pread(pcache_item->pData, sector, read_num, sb->s_part) != read_num)
    {
        //read block device sector failed

        //return NULL;// fix bug, support fs size not aligned by MINFS_CACHE_SIZE, by Bayden 12.05.08
    }
    pcache_item->Sector = sector;

    (sbi->Cache.CacheIndex)++;
    if (sbi->Cache.CacheIndex >= sbi->Cache.MaxCacheNum)
    {
        sbi->Cache.CacheIndex = 0;
    }

    return pcache_item;
}

__u32 minfs_pread(struct super_block *sb, void *buffer, __u32 pos, __u32 count)
{
    struct minfs_sb_info *sbi = MINFS_SB(sb);
    __u32  sector = (pos >> MINFS_CACHE_BITS) << (MINFS_CACHE_BITS - sb->s_blocksize_bits);
    __u32  offset = pos % MINFS_CACHE_SIZE;
    __u32  left;
    __u8  *pCurBuffer = (__u8 *)buffer;
    __u32  chunk_size;
    __minfs_cache_item_t *pcache_item;
    __u32   direct_nr;

    //check if out of minfs size, by Bayden 12.05.08
    if (pos >= sbi->Size)
    {
        return 0;
    }

    if (pos + count > sbi->Size)
    {
        count = sbi->Size - pos;
    }

    left = count;

    //lock cache buffer
    minfs_cache_lock(sb);

    //read first sector if start of read not sector aligned.
    if (offset)
    {
        //try to get from cache
        chunk_size  = left <= (MINFS_CACHE_SIZE - offset) ? left : (MINFS_CACHE_SIZE - offset);
        pcache_item = minfs_get_data_from_cache(sb, sector);
        if (pcache_item == NULL)
        {
            //sector data not in cache, allocate a cache buffer to load sector data.
            pcache_item = minfs_use_cache_load_data(sb, sector);
        }
        if (pcache_item == NULL)
        {
            //read data failed
            goto out;
        }
        memcpy(pCurBuffer, pcache_item->pData + offset, chunk_size);

        left        -= chunk_size;
        pCurBuffer  += chunk_size;
        sector      += (sbi->Cache.CacheItemSectors);
    }

    //read aligned sectors,
    //maybe cache item buffer size more bigger can improve performance.
    //by sunny at 2011-3-28.
    if (left >> MINFS_CACHE_BITS)
    {
        __u8  *PhyBuffer;
        __u32  read_nr;

        //calc the number of read sectors directly,
        //the byte size of read chunk size
        direct_nr = (left >> MINFS_CACHE_BITS) << (MINFS_CACHE_BITS - sb->s_blocksize_bits);

        chunk_size = direct_nr * sbi->SectorSize;

        if (esMEMS_PhyAddrConti(pCurBuffer, count) == EPDK_TRUE)
        {
            //user buffer is phyiscal continuous,
            //use current buffer to read directly.
            read_nr = esFSYS_pread(pCurBuffer, sector, direct_nr, sb->s_part);
            if (read_nr != direct_nr)
            {
                //read block device sector failed
                goto out;
            }
        }
        else
        {
            //allocate a temp phyiscal continuous buffer to read,
            //the buffer input to block device must phyiscal continuous.
            PhyBuffer = (__u8 *)minfs_allocate_temp_buffer(sb, chunk_size);
            if (PhyBuffer == NULL)
            {
                fs_log_error("palloc buffer to read minfs sector failed\n");
                goto out;
            }
            read_nr = esFSYS_pread(PhyBuffer, sector, direct_nr, sb->s_part);
            if (read_nr != direct_nr)
            {
                //read block device sector failed
                minfs_free_temp_buffer(sb, PhyBuffer, chunk_size);
                goto out;
            }
            memcpy(pCurBuffer, PhyBuffer, chunk_size);
            minfs_free_temp_buffer(sb, PhyBuffer, chunk_size);
        }
        sector += direct_nr;
        pCurBuffer += chunk_size;
        left -= chunk_size;
    }

    //read last sector if end of read not sector aligned,
    //if the start of read is aligned by sector,
    //we should allocate cache buffer to read.
    if (left)
    {
        chunk_size = left;
        pcache_item = minfs_get_data_from_cache(sb, sector);
        if (pcache_item == NULL)
        {
            //sector data not in cache, allocate a cache buffer to load sector data.
            pcache_item = minfs_use_cache_load_data(sb, sector);
        }
        if (pcache_item == NULL)
        {
            //read data failed
            goto out;
        }
        memcpy(pCurBuffer, pcache_item->pData, chunk_size);
        left -= chunk_size;
        pCurBuffer += chunk_size;
    }

out:
    //unlock cache buffer
    minfs_cache_unlock(sb);

    return count - left;
}
