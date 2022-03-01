/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : exfat_cluster.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-16
* Descript: exfat file system cluster handing functions.
* Update  : date                auther      ver     notes
*           2011-3-16 14:21:29  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "exfatfs.h"
#include "exfat_cache.h"
#include "exfat_fatent.h"
#include "exfat_cluster.h"
#include "exfat_bitmap.h"
#include "fsys_debug.h"

static void exfat_calc_cmap(struct exfat_clus_map *cmap,
                            struct exfat_cache_id *cid,
                            u32 iclusnr)
{
    u32 offset = iclusnr - cid->iclusnr;
    cmap->iclusnr = iclusnr;
    cmap->clusnr = cid->clusnr + offset;
    cmap->len = cid->len - offset;
}

/*
 * Caller must be guarantee it have no race with truncate().  I.e. the
 * range of "iclusnr" and "iclusnr + len" must not be truncated.
 */
int exfat_get_cluster(struct inode *inode, u32 iclusnr, u32 clus_len,
                      struct exfat_clus_map *cmap)
{
    struct super_block *sb = inode->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct exfat_inode_info *exi = EXFAT_I(inode);
    struct exfat_cache_id cid;
    struct exfat_ent fatent;
    u32 clusnr;
    int err = 0, found_target;

    EXFAT_DEBUG0_ON(!exi->clusnr);
    if (exi->data_flag == EXFAT_DATA_CONTIGUOUS)
    {

        EXFAT_DEBUG_ON(clus_len > (exi->phys_size + (sbi->clus_size - 1)) >> sbi->clus_bits
                       && exi->phys_size,   /* expect first time add cluster space to inode */
                       "clus_len %u, i_size %lld, clus_size %lu\n",
                       clus_len, exi->phys_size, sbi->clus_size);
        /* FIXME: should check clus_len limit from i_size? */
        cmap->iclusnr = iclusnr;
        cmap->clusnr = exi->clusnr + iclusnr;
        cmap->len = clus_len;
        //      fs_log_info("%s: contig: iclusnr %u, clusnr %u, len %u\n",
        //                  __func__, cmap->iclusnr, cmap->clusnr, cmap->len);
        return 0;
    }
    /*
     * FIXME: use rw_semaphore (r: here, w: truncate), then we can
     * do readahead of contiguous clusters and cache it. (This
     * should optimize disk-seek very much if clusters is not
     * fragmented heavily)
     */

    /* Setup the start cluster to walk */
    if (!exfat_cache_lookup(inode, iclusnr, &cid))
    {
        exfat_cache_init(&cid, 0, exi->clusnr);
    }
    clusnr = cid.clusnr + cid.len - 1;

    /* Walk the cluster-chain */
    found_target = 0;
    exfat_ent_init(&fatent);
    fs_log_info("%s: iclusnr %u, clus_len %u: cid.iclusnr %u, cid.clusnr %u,"
                " cid.len %u, search start clusnr %u\n",
                __func__, iclusnr, clus_len, cid.iclusnr,
                cid.clusnr, cid.len, clusnr);
    while (cid.iclusnr + cid.len < iclusnr + clus_len)
    {
        /* Target iclusnr was found, so find contiguous clusters */
        if (!found_target && (iclusnr < cid.iclusnr + cid.len))
        {
            found_target = 1;
        }

        /* FIXME: should be readahead FAT blocks */
#if 0
        err = exfat_ent_read(inode, &fatent, clusnr, &clusnr);
#else
        {
            u32 prev = clusnr;
            err = exfat_ent_read(inode, &fatent, prev, &clusnr);
            //          fs_log_info("%s: ent_read: clusnr %u, next clusnr %u\n",
            //                      __func__, prev, clusnr);
        }
#endif
        if (err < 0)
        {
            goto out;
        }

        if (exfat_ent_eof(sbi, clusnr))
        {
            exfat_cache_add(inode, &cid);

            /* Return special cmap */
            cmap->iclusnr = cid.iclusnr + cid.len;
            cmap->clusnr = clusnr;
            cmap->len = 0;
            goto out;
        }
        else if (exfat_ent_bad(sbi, clusnr))
        {
            fs_log_error("exFAT: found bad cluster entry"
                         " (start cluster 0x%08x)\n", exi->clusnr);
            err = -EIO;
            goto out;
        }
        else if (!exfat_ent_valid(sbi, clusnr))
        {
            fs_log_error("found invalid cluster chain"
                         " (start cluster 0x%08x, entry 0x%08x)",
                         exi->clusnr, clusnr);
            err = -EIO;
            goto out;
        }

        if (exfat_cache_contiguous(&cid, clusnr))
        {
            cid.len++;
        }
        else
        {
            if (found_target)
            {
                /* Found a discontiguous cluster */
                exfat_calc_cmap(cmap, &cid, iclusnr);
                exfat_cache_add(inode, &cid);

                exfat_cache_init(&cid, cid.iclusnr + cid.len, clusnr);
                exfat_cache_add(inode, &cid);
                goto out;
            }
            exfat_cache_init(&cid, cid.iclusnr + cid.len, clusnr);
        }

        /* Prevent the infinite loop of cluster chain */
        if (cid.iclusnr + cid.len > sbi->total_clusters)
        {
            fs_log_error("detected the cluster chain loop"
                         " (start cluster 0x%08x)", exi->clusnr);
            err = -EIO;
            goto out;
        }
    }

    exfat_calc_cmap(cmap, &cid, iclusnr);
    exfat_cache_add(inode, &cid);
out:
    exfat_ent_release(&fatent);
    fs_log_info("%s: result cmap: iclusnr %u, clusnr %u, len %u\n",
                __func__, cmap->iclusnr, cmap->clusnr, cmap->len);

    return err;
}

/*
 * get inode last cluster number
 */
static int exfat_get_last_clus(struct inode *inode, int *dclus, int *iclus)
{
    struct exfat_sb_info *sbi = EXFAT_SB(inode->i_sb);
    struct exfat_inode_info *exi = EXFAT_I(inode);
    struct exfat_clus_map cmap;
    int err;

    *iclus = 0;
    *dclus = exi->clusnr;
    if (exi->data_flag == EXFAT_DATA_CONTIGUOUS)
    {
        /* inode last logic cluser number,
         * NOTE : logic cluster base is 0 */
        *iclus = ((exi->phys_size + sbi->clus_size - 1) >> sbi->clus_bits) - 1;
        *dclus = exi->clusnr + *iclus;
    }
    else
    {
        /* inode have no space allocated */
        if (!exi->clusnr || !exi->phys_size)
        {
            *dclus = 0;
            *iclus = 0;
            return 0;
        }
        /* Get inode last disk cluster numebr by logic cluster number,
         * NOTE : logic cluster base is 0 */
        *iclus = ((exi->phys_size + sbi->clus_size - 1) >> sbi->clus_bits) - 1;
        err = exfat_get_cluster(inode, *iclus, 0, &cmap);
        if (err)
        {
            return err;
        }
        *dclus = cmap.clusnr;
    }
    return 0;
}

#if defined CONFIG_FSYS_EXFAT_RW

/*
 * free bitmap allocated bits space
 * NOTE : free bits must be coutiguous
 */

static int exfat_free_bitmap_bits(struct inode *inode,
                                  int cluster, int nr_cluster)
{
    struct super_block *sb = inode->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct exfat_inode_info *exi_bitmap = EXFAT_I(sbi->bitmap_inode);
    struct buffer_head *bh;
    int bits_nr, left, err, blocknr, bitmap_clusnr;
    u64 offset;

    err = 0;
    bits_nr = 0;
    left = nr_cluster;

    /* can't free reserved clusters */
    EXFAT_DEBUG_ON(cluster < EXFAT_START_ENT,
                   "invalid cluster %x for freed\n", cluster);

    /* bits of bitmap mapping cluster number */
    bitmap_clusnr = cluster - EXFAT_START_ENT;

    /* maybe get from sbi directly, improve performance */
    blocknr = exfat_clus_to_blknr(sbi, exi_bitmap->clusnr);
    while (left)
    {
        bh = sb_bread(sb, blocknr + (bitmap_clusnr >> sbi->cpbb_bits));
        if (!bh)
        {
            return -EIO;
        }
        offset = (u64)(bitmap_clusnr & (int)(sbi->cpbb - 1));
        bits_nr = min(left, (int)(sbi->cpbb - (u32)offset));

        /* set bits as free state */
        exfat_set_bits((u8 *)(bh->b_data), offset,
                       bits_nr, EXFAT_BIT_FREE);
        set_buffer_uptodate(bh);
        mark_buffer_dirty(bh);
        if (sb->s_flags & MS_SYNCHRONOUS)
        {
            err = exfat_sync_bhs(&bh, 1);
            if (err)
            {
                brelse(bh);
                return err;
            }
        }
        brelse(bh);
        left -= bits_nr;
        bitmap_clusnr += bits_nr;
    }
    return 0;
}

/*
 * write one cluster to FAT table
 * cluster  :   write new cluster position
 * next_clus:   new cluster number
 */
static int exfat_write_one_cluster(struct inode *inode,
                                   int cluster, int next_clus)
{
    struct exfat_ent fatent;
    int err = 0;
    u32 next;

    /* add cluster chain to the end of inode */
    exfat_ent_init(&fatent);
    err = exfat_ent_prepare(inode, &fatent,
                            cluster, &next);
    if (err >= 0)
    {
        int wait = inode_needs_sync(inode);
        err = exfat_ent_write(inode, &fatent,
                              next_clus, wait);
        exfat_ent_release(&fatent);
    }
    return err;
}

/*
 * write contiguous clusters to FAT table
 * first_clus   :   the first cluster of contiguous
 * nr_clus      :   the number of contiguous clusters
 */
static int exfat_write_conti_chain(struct inode *inode,
                                   int first_clus, int nr_clus)
{
    struct exfat_sb_info *sbi = EXFAT_SB(inode->i_sb);
    struct exfat_ent fatent;
    int add_nr, wait, err, left;
    u32 prev, next;

    err = 0;
    add_nr = 0;
    left = nr_clus - 1;  /* ???? */
    prev = first_clus;
    wait = inode_needs_sync(inode);

    /* write contiguous clusters */
    while (left)
    {
        prev += add_nr;
        exfat_ent_init(&fatent);
        err = exfat_ent_prepare(inode, &fatent, prev, &next);
        if (err >= 0)
        {
            add_nr = min((int)(sbi->fpb - fatent.offset), left);
            err = exfat_ent_write_conti(inode, &fatent,
                                        prev + 1, add_nr, wait);
            exfat_ent_release(&fatent);
        }
        if (err < 0)
        {
            fs_log_error("exFAT write contiguous cluster chain failed\n");
            goto out;
        }
        left -= add_nr;
    }
    /* write last EOF FAT entry */
    prev = first_clus + nr_clus - 1;
    exfat_ent_init(&fatent);
    err = exfat_ent_prepare(inode, &fatent, prev, &next);
    if (err >= 0)
    {
        err = exfat_ent_write(inode, &fatent, EXFAT_ENT_EOF, wait);
        exfat_ent_release(&fatent);
    }
out:
    return err;
}

/*
 * add number of contiguous clusters to inode end
 * first_clus   :   the first cluster of contiguous clusters
 * nr_clus      :   the number of contiguous cluster
 */
static int exfat_add_inode_chain(struct inode *inode, int first_clus,
                                 int nr_clus, int data_flag)
{
    struct super_block *sb = inode->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct exfat_inode_info *exi = EXFAT_I(inode);
    int err, last, next;

    /* sanity check */
    if (exi->data_flag != EXFAT_DATA_NORMAL && exi->clusnr)
    {
        fs_log_error("exFAT data flag is normal,"
                     "and exi->clusnr : %d\n", exi->clusnr);
        return -EPERM;
    }
    /* We must locate the last cluster of the file to add this new
     * one (new_dclus) to the end of the link list (the FAT).
     */
    last = 0;
    if (exi->clusnr)
    {
        int fclus, dclus;
        err = exfat_get_last_clus(inode, &dclus, &fclus);
        if (err < 0)
        {
            return err;
        }
        last = dclus;
    }
    if (last)
    {
        /* write first_clus to last cluster */
        err = exfat_write_one_cluster(inode, last, first_clus);
        if (err)
        {
            fs_log_error("write inode cluster failed\n");
            return err;
        }
    }
    else
    {
        /* inode have no space current,
         * set allocated cluster to inode start cluster,
         * change inode data flag.
         */
        exi->clusnr = first_clus;
        exi->data_flag = (u8)data_flag;
        if (S_ISDIR(inode->i_mode) && IS_DIRSYNC(inode))
        {
            err = exfat_sync_inode(inode);
            if (err)
            {
                return err;
            }
        }
        else
        {
            mark_inode_dirty(inode);
        }
    }
    return 0;
}

/*
 * write coutiguous inode cluster chain to FAT
 * the last cluster write EOF of cluster
 * inode    :   the write inode
 */
static int exfat_write_inode_chain(struct inode *inode)
{
    struct exfat_sb_info *sbi = EXFAT_SB(inode->i_sb);
    struct exfat_inode_info *exi = EXFAT_I(inode);
    int nr_cluster, err = 0;

    /* sanity check */
    if (exi->data_flag != EXFAT_DATA_CONTIGUOUS)
    {
        fs_log_error("inode data flag is already normal\n");
        return -EPERM;
    }
    /* write inode contiguous cluster chain */
    /* ????? maybe other method get total cluster number of inode */
    nr_cluster = (exi->phys_size + sbi->clus_size - 1) >> (sbi->clus_bits);
    err = exfat_write_conti_chain(inode, exi->clusnr, nr_cluster);
    if (err)
    {
        fs_log_error("exFAT add inode contiguous cluster chain failed\n");
        return err;
    }
    return 0;
}

/* inode data space state flag */
#define  EXFAT_WRITE_NOTHING        0   /* inode data contiguous, no need write anything */
#define  EXFAT_WRITE_INODE_SPACE    1   /* write inode data space                        */
#define  EXFAT_WRITE_NEW_ALLOCATED  2   /* write new allocated space                     */
#define  EXFAT_ADD_NEW_ALLOCTED     4   /* add new allocated space to inode end          */

/*
 * get inode clusters chain state
 */
int exfat_inode_clusters_state(struct inode *inode, int clus,
                               int nr_clus, u8 data_flag)
{
    struct exfat_inode_info *exi = EXFAT_I(inode);
    int  last, state, err, last_iclus;

    /* get inode last cluster first */
    err = exfat_get_last_clus(inode, &last, &last_iclus);
    if (err)
    {
        fs_log_error("exFAT get last cluster failed\n");
        return err;
    }
    state = EXFAT_WRITE_NOTHING;
    if (exi->data_flag == EXFAT_DATA_CONTIGUOUS)
    {
        if (data_flag != EXFAT_DATA_CONTIGUOUS)
        {
            state |= EXFAT_WRITE_INODE_SPACE;
            state |= EXFAT_ADD_NEW_ALLOCTED;
        }
        else
        {
            if ((last + 1) != clus)
            {
                state |= EXFAT_WRITE_INODE_SPACE;
                state |= EXFAT_WRITE_NEW_ALLOCATED;
                state |= EXFAT_ADD_NEW_ALLOCTED;
            }
        }
    }
    else
    {
        /* check is the first time add cluster */
        if (!exi->clusnr || !exi->phys_size)
        {
            exi->clusnr = clus;
            exi->data_flag = data_flag;
            if (IS_DIRSYNC(inode))
            {
                exfat_sync_inode(inode);
            }
            else
            {
                mark_inode_dirty(inode);
            }
            goto out;
        }
        if (data_flag == EXFAT_DATA_CONTIGUOUS)
        {
            state |= EXFAT_WRITE_NEW_ALLOCATED;
            state |= EXFAT_ADD_NEW_ALLOCTED;
        }
        else
        {
            state |= EXFAT_ADD_NEW_ALLOCTED;
        }
    }
out:
    return state;
}

int exfat_chain_add(struct inode *inode, int clus, int nr_clus, u8 data_flag)
{
    struct exfat_sb_info *sbi = EXFAT_SB(inode->i_sb);
    struct exfat_inode_info *exi = EXFAT_I(inode);
    int state, err;

    state = exfat_inode_clusters_state(inode, clus, nr_clus, data_flag);
    if (state < 0)
    {
        fs_log_error("exFAT get inode cluster state failed\n");
        return state;
    }
    if (state == EXFAT_WRITE_NOTHING)
    {
        fs_log_trace01("exFAT inode clusters chain contiguous, "
                       "no any operations on FAT\n");
        inode->i_blocks += nr_clus << (sbi->clus_bits - 9);
        return 0;
    }
    /* write contiguous inode clusters chain if necessary */
    if (state & EXFAT_WRITE_INODE_SPACE)
    {
        /* sanity check */
        if (exi->data_flag != EXFAT_DATA_CONTIGUOUS)
        {
            fs_log_error("exFAT write inode clusters chain not conti\n");
            return -EPERM;
        }
        /* write all inode cluster chain to FAT table */
        err = exfat_write_inode_chain(inode);
        if (err)
        {
            fs_log_error("exFAT write all inode clusters chain failed\n");
            return err;
        }
        /* set inode data space state */
        exi->data_flag = EXFAT_DATA_NORMAL;
        if (IS_DIRSYNC(inode))
        {
            err = exfat_sync_inode(inode);
            if (err)
            {
                return err;
            }
        }
        else
        {
            mark_inode_dirty(inode);
        }
    }
    /* write allocated new clusters chain if necessary */
    if (state & EXFAT_WRITE_NEW_ALLOCATED)
    {
        /* write new allocated clusters to FAT table,
         * NOTE: the new allocated clusters must be contiguous
         */
        /* sanity check */
        if (data_flag != EXFAT_DATA_CONTIGUOUS)
        {
            fs_log_error("exFAT write conti clusters chain not conti\n");
            return -EPERM;
        }
        err = exfat_write_conti_chain(inode, clus, nr_clus);
        if (err)
        {
            fs_log_error("exFAT write contiguous clusters chain failed\n");
            return err;
        }
    }
    /* add cluster chain to inode end if nessary */
    if (state & EXFAT_ADD_NEW_ALLOCTED)
    {
        /* add new allocated clusters chain to inode end */
        err = exfat_add_inode_chain(inode, clus, nr_clus, data_flag);
        if (err)
        {
            fs_log_error("exFAT add clusters chain to inode end failed\n");
            return err;
        }
    }
    /* maybe should check i_blocks here ????? */
    //if (new_fclus != (inode->i_blocks >> (sbi->clus_bits - 9))) {
    //  fs_log_error("clusters badly computed (%d != %lu)",
    //               new_fclus, inode->i_blocks>>(sbi->clus_bits - 9));
    //  exfat_cache_inval(inode);
    //}
    inode->i_blocks += nr_clus << (sbi->clus_bits - 9);
    return 0;
}

/*
 * the max allocate cluster number at one time
 * FIXME : sunny
 */

#define   EXFAT_ALLOC_INFO_MAX_BH   2

/* calc method : 1<<12 = 512 * 8, 1 block : 512 bytes, 1 byte : 8bits */
#define   EXFAT_MAX_ALLOC_CLUSNR    ((EXFAT_ALLOC_INFO_MAX_BH - 1)<<12)


struct exfat_alloc_data
{
    /* buffer heads for store bitmap data */
    struct buffer_head *bhs[EXFAT_ALLOC_INFO_MAX_BH];
    int                 bhs_nr;     /* number of buffer heads */
    u64                 offset;     /* first free bit offset */
    int                 clusnr;     /* the first free bit cluster number */
    u32                 bitsnr;     /* allocated contiguous bits number */
};

static void exfat_alloc_data_init(struct exfat_alloc_data *alloc_data)
{
    alloc_data->clusnr = 0;
    alloc_data->bitsnr = 0;
    alloc_data->bhs_nr = 0;
}

static void exfat_alloc_data_brelse(struct exfat_alloc_data *alloc_data)
{
    struct buffer_head *bh;
    while (alloc_data->bhs_nr)
    {
        alloc_data->bhs_nr--;
        bh = alloc_data->bhs[alloc_data->bhs_nr];
        brelse(bh);
    }
    exfat_alloc_data_init(alloc_data);
}


/*  ---NOTE : performace relative-------
 * get allocate position of bitmap inode
 * allocate position maybe affect by :
 * 1. inode space mode : contiguous or not
 * 2. inode physic size
 * 3. allocate bits
 * 4. the free space size behind inode space
 */
static int exfat_get_alloc_position(struct inode *inode,
                                    int bits_nr, u64 *position)
{
    int err = 0;

    /* rethink this
     * should more intelligent
     */
    *position = 0;
    return err;
}


/*
 * count coutiguous free bits number in bitmap
 *
 */
static int exfat_count_conti_free_bits(u8 *bitmap, u64 bits_nr, s64 offset)
{
    int number = 0;
    while (exfat_bit_get(bitmap, offset) == EXFAT_BIT_FREE)
    {

        offset++;
        number++;

        if (offset == bits_nr)
        {
            break;
        }
    }
    return number;
}

/*
 * find first free bit in bitmap
 *
 */
static s64 exfat_find_first_free_bit(u8 *bitmap, u64 bits_nr, s64 offset)
{
    s64 position;
    while (exfat_bit_get(bitmap, offset) != EXFAT_BIT_FREE)
    {

        offset++;

        /* not find free bit */
        if (offset == bits_nr)
        {
            return -1;
        }
    }
    position = offset;
    return position;
}


/*
 * allocate free space for one inode
 * bits_info    : struct pointer of fill allocate bits info
 * bits_nr      : expect allocate bit number
 * alloc_pos    : allocate start seach position
 */
static int exfat_alloc_bits(struct inode *inode,
                            struct exfat_alloc_data *alloc_data,
                            int bits_nr, u64 alloc_pos)
{
    struct super_block *sb = inode->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct exfat_inode_info *exi_bitmap = EXFAT_I(sbi->bitmap_inode);
    struct buffer_head *bh;
    int free_bits, left, err, alloc_bits;
    int blocknr, i;
    u64 offset, pos, left_bits;
    s64 first;

    err = 0;
    free_bits = 0;
    alloc_bits = 0;
    left = bits_nr;
    pos = alloc_pos;

    /* get bitmap first block number, maybe cache it for performance */
    blocknr = exfat_clus_to_blknr(sbi, exi_bitmap->clusnr);
    while (1)
    {
        /* avoid beyond bitmap inode size */
        if (pos >= exi_bitmap->phys_size << 3)
        {
            return -ENOSPC;
        }
        bh = sb_bread(sb, blocknr + (pos >> sbi->cpbb_bits));
        if (!bh)
        {
            return -EIO;
        }
        offset = pos & (sbi->cpbb - 1);

        /* scan first free bit position */
        left_bits = min((u64)(bh->b_size << 3), (u64)(exi_bitmap->phys_size << 3) - pos);
        first = exfat_find_first_free_bit((u8 *)(bh->b_data), left_bits, offset);
        if (first < 0)
        {
            /* no free space in this block, seach next block */
            brelse(bh);
            pos += (sbi->cpbb - offset);
            continue;
        }
        /* count free bits number after first free bit */
        free_bits = exfat_count_conti_free_bits((u8 *)(bh->b_data),
                                                left_bits, (u64)first);
        alloc_bits = min(left, free_bits);

        get_bh(bh);
        alloc_data->bhs[alloc_data->bhs_nr] = bh;
        alloc_data->bhs_nr++;

        /* get first free bit mapping cluster number */
        alloc_data->clusnr = (pos - offset) + first + EXFAT_START_ENT;
        alloc_data->offset = first;
        alloc_data->bitsnr += alloc_bits;
        left -= alloc_bits;
        pos += (first + alloc_bits);
        brelse(bh);
        /* allocate have not finished,
         * current block is not full free bits
         * allocate discrete bitmap space
         */
        if (left && (free_bits != (int)((s64)(bh->b_size << 3) - first)))
        {
            goto allocated;
        }
        /* find first block */
        break;
    }
    /* left space allocated */
    while (left)
    {
        /* avoid beyond bitmap inode size */
        if (pos >= exi_bitmap->phys_size << 3)
        {
            break;
        }
        bh = sb_bread(sb, blocknr + (pos >> sbi->cpbb_bits));
        if (!bh)
        {
            break;
        }

        /* scan first free bit position */
        first = exfat_find_first_free_bit((u8 *)(bh->b_data),
                                          bh->b_size << 3, 0);
        if (first != 0)
        {
            /* first bit have been allocated,
             * can't get contiguous space */
            brelse(bh);
            break;
        }
        /* get free bits number after first free bit */
        free_bits = exfat_count_conti_free_bits((u8 *)(bh->b_data),
                                                bh->b_size << 3, (u64)first);
        alloc_bits = min(left, free_bits);
        get_bh(bh);
        alloc_data->bhs[alloc_data->bhs_nr] = bh;
        alloc_data->bhs_nr++;
        alloc_data->bitsnr += alloc_bits;
        left -= alloc_bits;
        pos += alloc_bits;
        brelse(bh);
        /* current block have bits been allocated
         * can't allocate contiguous space */
        if (left && (free_bits != (int)(bh->b_size << 3)))
        {
            goto allocated;
        }
    }
allocated:
    /* write allocated bits to bitmap inode */
    left = alloc_data->bitsnr;
    offset = alloc_data->offset;
    i = 0;
    while (left && i < alloc_data->bhs_nr)
    {
        /* set bits as allocated state */
        get_bh(alloc_data->bhs[i]);
        bh = alloc_data->bhs[i];
        free_bits = min(left, (int)((int)(bh->b_size << 3) - (int)offset));
        exfat_set_bits((u8 *)(bh->b_data), (u64)offset,
                       free_bits, EXFAT_BIT_ALLOCATED);

        set_buffer_uptodate(bh);
        mark_buffer_dirty(bh);
        if (sb->s_flags & MS_SYNCHRONOUS)
        {
            err = exfat_sync_bhs(&bh, 1);
            if (err)
            {
                brelse(bh);
                break;
            }
        }
        brelse(bh);
        offset = 0;
        left -= free_bits;
        i++;
    }
    return err;
}

/*
 * check should move inode data or not
 * to avoid generate fragment
 * mainly use for add to small file end
 * move or not maybe affect by :
 * 1. inode space mode : contiguous or not
 * 2. inode physic size : small file
 * 4. the free space size behind inode space
 */
int exfat_file_data_move_check(struct inode *inode, int new_size)
{
    int err = 0;
    return err;
}

/*
 * move inode data to a new position
 * move one inode data area to a new special position
 * to avoid generate fragment
 * mainly use for add to small file end
 * finished things :
 * 1. move inode data to new position
 * 3. update inode info : start cluster number
 * 2. free inode old bits space(must deal after inode been update finished)
 */
int exfat_file_data_move(struct inode *inode, u64 position)
{
    int err = 0;
    return err;
}

int exfat_alloc_clusters(struct inode *inode, int *clus, int nr_clus, u8 *data_flag)
{
    struct super_block *sb = inode->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct exfat_inode_info *exi = EXFAT_I(inode);
    struct exfat_alloc_data alloc_data;
    int  err, left, prev, index, i;
    u64  position;

    BUG_ON(nr_clus > EXFAT_MAX_ALLOC_CLUSNR);   /* fixed limit */

    /* check total free cluster number enough or not */
    if (((int)sbi->free_clusters != -1) &&
        (sbi->free_clusters < nr_clus))
    {
        return -ENOSPC;
    }
    /* get allocate start position */
    err = exfat_get_alloc_position(inode, nr_clus, &position);
    if (err)
    {
        fs_log_error("exFAT get allocate position failed\n");
        return err;
    }
    /* allocate free space bits from bitmap inode */
    prev = 0;
    index = 0;
    left = nr_clus;
    *data_flag = EXFAT_DATA_CONTIGUOUS;
    while (left)
    {
        exfat_alloc_data_init(&alloc_data);
        err = exfat_alloc_bits(inode, &alloc_data, left, position);
        if (err)
        {
            fs_log_error("exFAT allocate free space bits failed\n");
            return err;
        }
        left -= min(left, (int)(alloc_data.bitsnr));
        /* allocated will write FAT table:
         * 1.inode data space not coutiguous.
         * 2.can't allocated coutiguous space, allocate generate fragment .*/
        if (left || (*data_flag != EXFAT_DATA_CONTIGUOUS))
        {

            /* set allocated data space state flag */
            if (*data_flag == EXFAT_DATA_CONTIGUOUS)
            {
                *data_flag = EXFAT_DATA_NORMAL;
            }
            /* add first cluster number to last time chain end */
            if (prev)
            {
                err = exfat_write_one_cluster(inode, prev, alloc_data.clusnr);
                if (err)
                {
                    fs_log_error("exFAT write one cluster failed\n");
                    goto error;
                }
            }
            /* write contiguous cluster chain */
            err = exfat_write_conti_chain(inode, alloc_data.clusnr,
                                          alloc_data.bitsnr);
            if (err)
            {
                fs_log_error("exFAT write cluster chain failed\n");
                goto error;
            }
        }
        /* dec free clusters number, and set sb dirty */
        if ((int)sbi->free_clusters != -1)
        {
            sbi->free_clusters -= alloc_data.bitsnr;
            sb->s_dirt = 1;
        }
        prev = alloc_data.clusnr + alloc_data.bitsnr - 1;

        /* set cluster number to clus[] */
        for (i = 0; i < alloc_data.bitsnr && index < nr_clus; i++)
        {
            clus[index++] = alloc_data.clusnr + i;
        }
        exfat_alloc_data_brelse(&alloc_data);
    }
    return 0;

error:
    /* free clusters we allocated */
    if (exfat_free_clusters(inode, *clus, nr_clus - left, *data_flag))
    {
        fs_log_error("exFAT free cluster failed\n");
    }
    return err;
}

int exfat_free_clusters(struct inode *inode, int clus, int nr_clus, u8 data_flag)
{
    struct super_block *sb = inode->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct exfat_inode_info *exi = EXFAT_I(inode);
    struct exfat_ent fatent;
    u32 prev, next;
    int err, conti_bits;

    if (!nr_clus)
    {
        return 0;
    }
    /* free data space is contiguous, free contiguous bits in bitmap directly
     * free data space is discrete , get free bits chain information from FAT table
     */
    if (data_flag == EXFAT_DATA_CONTIGUOUS)
    {
        /* free bitmap bits */
        err = exfat_free_bitmap_bits(inode, clus, nr_clus);
        if (err)
        {
            fs_log_error("exFAT free bitmap bits failed\n");
            return err;
        }
        /* maybe should deal with FAT table */
        if ((int)sbi->free_clusters != -1)
        {
            sbi->free_clusters += nr_clus;
            sb->s_dirt = 1;
        }
    }
    else
    {
        /* free discrete clusters
         * get cluster chain relative from FAT table
         */
        exfat_ent_init(&fatent);
        while (nr_clus)
        {
            next = clus;
            conti_bits = 1;

read_next:
            prev = next;
            err = exfat_ent_read(inode, &fatent, prev, &next);
            if (err)
            {
                fs_log_error("exFAT: read fat entry failed\n");
                goto out;
            }
            /* encount bad cluster, what can we do ???? */
            if (exfat_ent_bad(sbi, next))
            {
                fs_log_error("exFAT: found bad cluster entry\n");
                err = -EIO;
                break;
            }
            if (exfat_ent_free(sbi, next))
            {
                fs_log_error("Deleting FAT entry beyond EOF\n");
                err = -EIO;
                break;
            }
            if (exfat_ent_contiguous(prev, next))
            {
                conti_bits++;
                if (conti_bits < nr_clus)
                {
                    goto read_next;
                }
            }
            /* write contiguous bits */
            err = exfat_free_bitmap_bits(inode, clus, conti_bits);
            if (err)
            {
                fs_log_error("exFAT free bitmap bits failed\n");
                break;
            }
            if ((int)sbi->free_clusters != -1)
            {
                sbi->free_clusters += conti_bits;
                sb->s_dirt = 1;
            }
            /* check encount EOF cluster of inode */
            if (exfat_ent_eof(sbi, next))
            {
                fs_log_info("exFAT: found EOF of inode\n");
                err = 0;
                break;
            }
            nr_clus -= conti_bits;
            clus = next;
        }
        exfat_ent_release(&fatent);
    }
out:
    return err;
}

/*
 * free all clusters after the skip'th cluster.
 */
int exfat_free(struct inode *inode, int skip, int nr_clus)
{
    struct super_block *sb = inode->i_sb;
    struct exfat_sb_info *sbi = EXFAT_SB(sb);
    struct exfat_inode_info *exi = EXFAT_I(inode);
    int err, wait, free_start, start;
    u8 data_flag;

    /* inode have not allocated clusters */
    if (exi->clusnr == 0)
    {
        return 0;
    }
    /* inode cluster chain changed,
     * so invalid inode fat cache here */
    exfat_cache_inval(inode);
    wait = IS_DIRSYNC(inode);
    start = free_start = exi->clusnr;

    /* get inode data flag first */
    data_flag = exi->data_flag;

    /* First, we write the new file size. */
    if (!skip)
    {
        exi->clusnr = 0;
        exi->data_flag = EXFAT_DATA_NORMAL;
    }
    //MSDOS_I(inode)->i_attrs |= ATTR_ARCH;  /* ????? */
    inode->i_ctime = inode->i_mtime = CURRENT_TIME_SEC;
    if (wait)
    {
        err = exfat_sync_inode(inode);
        if (err)
        {
            exi->clusnr = start;
            return err;
        }
    }
    else
    {
        mark_inode_dirty(inode);
    }
    /* Write a new EOF, and get the remaining cluster chain for freeing. */
    if (skip)
    {
        struct exfat_ent fatent;
        int    dclus;
        u32    next;
        struct exfat_clus_map cmap;

        /* get prev free physic cluster number */
        err = exfat_get_cluster(inode, skip - 1, 1, &cmap);
        if (err)
        {
            return err;
        }
        dclus = cmap.clusnr;
        if (exi->data_flag != EXFAT_DATA_CONTIGUOUS)
        {
            exfat_ent_init(&fatent);
            err = exfat_ent_read(inode, &fatent, dclus, &next);
            if (err)
            {
                return err;
            }
            if (next == EXFAT_ENT_EOF)
            {
                fs_log_error("invalid cluster chain");
                exfat_ent_release(&fatent);
                return -EIO;
            }
            else if (next == EXFAT_ENT_FREE)
            {
                fs_log_error("invalid cluster chain");
                exfat_ent_release(&fatent);
                return -EIO;
            }
            err = exfat_ent_write(inode, &fatent, EXFAT_ENT_EOF, wait);
            if (err)
            {
                exfat_ent_release(&fatent);
                return err;
            }
            exfat_ent_release(&fatent);
            free_start = next;
        }
        else
        {
            /* inode data space is contiguous,
             * inc directly to get next cluster number */
            free_start = dclus + 1;
        }
    }
    inode->i_blocks = skip << (sbi->clus_bits - 9);

    /* Freeing the remained cluster chain */
    return exfat_free_clusters(inode, free_start, nr_clus, data_flag);
}
#endif

