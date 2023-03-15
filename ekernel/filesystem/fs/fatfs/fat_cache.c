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
#include "list.h"
#include "fs.h"
#include "fatfs.h"
#include "buffer_head.h"
#include "slab.h"
#include "errno.h"
#include "fsys_debug.h"

/* this must be > 0. */
#define FAT_MAX_CACHE   8

#if     FSYS_DEBUG_ON
void    debug_show_lru_fatcache(struct list_head *head, struct fat_cache_id *cid, char *name, __u32 fclus);
#else
#define debug_show_lru_fatcache(head, cid, name, fclus)
#endif

static int fat_max_cache(struct inode *inode)
{
    return FAT_MAX_CACHE;
}

static kmem_cache_t *fat_cache_cachep;

static void init_once(void *foo, kmem_cache_t *cachep, unsigned long flags)
{
    struct fat_cache *cache = (struct fat_cache *)foo;

    INIT_LIST_HEAD(&cache->cache_list);
}

int fat_cache_init(void)
{
    fat_cache_cachep = kmem_cache_create("fat_cache",
                                         sizeof(struct fat_cache),
                                         0, 0,    //SLAB_RECLAIM_ACCOUNT|SLAB_MEM_SPREAD,
                                         init_once, NULL);
    if (fat_cache_cachep == NULL)
    {
        return -ENOMEM;
    }
    return 0;
}

void fat_cache_destroy(void)
{
    kmem_cache_destroy(fat_cache_cachep);
}

static   struct fat_cache *fat_cache_alloc(struct inode *inode)
{
    struct fat_cache *tmp;

    tmp = kmem_cache_alloc(fat_cache_cachep, GFP_KERNEL);
    init_once(tmp, fat_cache_cachep, 0);
    return tmp;
}

static   void fat_cache_free(struct fat_cache *cache)
{
    BUG_ON(!list_empty(&cache->cache_list));
    kmem_cache_free(fat_cache_cachep, cache);
}

static   void fat_cache_update_lru(struct inode *inode,
                                   struct fat_cache *cache)
{
    if (MSDOS_I(inode)->cache_lru.next != &cache->cache_list)
    {
        list_move(&cache->cache_list, &MSDOS_I(inode)->cache_lru);
    }
}

static int fat_cache_lookup(struct inode *inode, int fclus,
                            struct fat_cache_id *cid,
                            int *cached_fclus, int *cached_dclus)
{
    static struct fat_cache nohit = { .fcluster = 0, };

    struct fat_cache *hit = &nohit, *p;
    int offset = -1;

    list_for_each_entry(p, &MSDOS_I(inode)->cache_lru, cache_list)
    {
        /* Find the cache of "fclus" or nearest cache. */
        if (p->fcluster <= fclus && hit->fcluster < p->fcluster)
        {
            hit = p;
            if ((hit->fcluster + hit->nr_contig) < fclus)
            {
                offset = hit->nr_contig;
            }
            else
            {
                offset = fclus - hit->fcluster;
                break;
            }
        }
    }
    if (hit != &nohit)
    {
        fat_cache_update_lru(inode, hit);

        cid->id = MSDOS_I(inode)->cache_valid_id;
        cid->nr_contig = hit->nr_contig;
        cid->fcluster = hit->fcluster;
        cid->dcluster = hit->dcluster;
        *cached_fclus = cid->fcluster + offset;
        *cached_dclus = cid->dcluster + offset;
    }

    return offset;
}

static struct fat_cache *fat_cache_merge(struct inode *inode,
        struct fat_cache_id *new)
{
    struct fat_cache *p;

    list_for_each_entry(p, &MSDOS_I(inode)->cache_lru, cache_list)
    {
        /* Find the same part as "new" in cluster-chain. */
        if (p->fcluster == new->fcluster)
        {
            BUG_ON(p->dcluster != new->dcluster);
            if (new->nr_contig > p->nr_contig)
            {
                p->nr_contig = new->nr_contig;
            }
            return p;
        }
    }
    return NULL;
}

static void fat_cache_add(struct inode *inode, struct fat_cache_id *new)
{
    struct fat_cache *cache, *tmp;

    if (new->fcluster == -1) /* dummy cache */
    {
        return;
    }

    if (new->id != FAT_CACHE_VALID &&
        new->id != MSDOS_I(inode)->cache_valid_id)
    {
        goto out;    /* this cache was invalidated */
    }

    cache = fat_cache_merge(inode, new);
    if (cache == NULL)
    {
        if (MSDOS_I(inode)->nr_caches < fat_max_cache(inode))
        {
            MSDOS_I(inode)->nr_caches++;

            tmp = fat_cache_alloc(inode);
            cache = fat_cache_merge(inode, new);
            if (cache != NULL)
            {
                MSDOS_I(inode)->nr_caches--;
                fat_cache_free(tmp);
                goto out_update_lru;
            }
            cache = tmp;
        }
        else
        {
            struct list_head *p = MSDOS_I(inode)->cache_lru.prev;
            cache = list_entry(p, struct fat_cache, cache_list);
        }
        cache->fcluster = new->fcluster;
        cache->dcluster = new->dcluster;
        cache->nr_contig = new->nr_contig;
    }
out_update_lru:
    fat_cache_update_lru(inode, cache);
out:
    return;
}

/*
 * Cache invalidation occurs rarely, thus the LRU chain is not updated. It
 * fixes itself after a while.
 */
static void __fat_cache_inval_inode(struct inode *inode)
{
    struct msdos_inode_info *i = MSDOS_I(inode);
    struct fat_cache *cache;

    while (!list_empty(&i->cache_lru))
    {
        cache = list_entry(i->cache_lru.next, struct fat_cache, cache_list);
        list_del_init(&cache->cache_list);
        i->nr_caches--;
        fat_cache_free(cache);
    }
    /* Update. The copy of caches before this id is discarded. */
    i->cache_valid_id++;
    if (i->cache_valid_id == FAT_CACHE_VALID)
    {
        i->cache_valid_id++;
    }
}

void fat_cache_inval_inode(struct inode *inode)
{
    __fat_cache_inval_inode(inode);
}

static   int cache_contiguous(struct fat_cache_id *cid, int dclus)
{
    cid->nr_contig++;
    return ((cid->dcluster + cid->nr_contig) == dclus);
}

static   void cache_init(struct fat_cache_id *cid, int fclus, int dclus)
{
    cid->id = FAT_CACHE_VALID;
    cid->fcluster = fclus;
    cid->dcluster = dclus;
    cid->nr_contig = 0;
}

/**
 * @descript:
 *          get the disk scope cluster number by in-file-scope cluster number
 * @para:   cluster -- wanted in-file cluster number
 *          fclus   -- maximum reached in-file cluster number
 *          dclus   -- maximum reached disk scope cluster number
 *          contig  -- contiguity cluster number
 * @return: 0 -- ok, and *fclus==cluster;  <0 -- error meet
 */
int fat_get_cluster(struct inode *inode, int cluster, int *fclus, int *dclus, int *mapped)
{
    struct super_block *sb = inode->i_sb;
    const int limit = sb->s_maxbytes >> MSDOS_SB(sb)->cluster_bits;
    struct fat_entry fatent;
    struct fat_cache_id cid;
    int nr;

    BUG_ON(MSDOS_I(inode)->i_start == 0);

    *fclus = 0;
    *dclus = MSDOS_I(inode)->i_start;
    if (mapped)
    {
        *mapped = 1;
    }
    if (cluster == 0)
    {
        return 0;
    }

    debug_timerstart(fatcachetime);
    if (fat_cache_lookup(inode, cluster, &cid, fclus, dclus) < 0)
    {
        /*
         * dummy, always not contiguous
         * This is reinitialized by cache_init(), later.
         */
        cache_init(&cid, -1, -1);
    }
    debug_timerend(fatcachetime);

    fatent_init(&fatent);

    debug_timerstart(scanfattime);
    while (*fclus < cluster)
    {

        /* prevent the infinite loop of cluster chain */
        if (*fclus > limit)
        {
            fat_fs_panic(sb, "%s: detected the cluster chain loop"
                         " (i_pos %d)", __FUNCTION__,
                         (int)MSDOS_I(inode)->i_pos);
            nr = -EIO;
            goto out;
        }

        nr = fat_ent_read(inode, &fatent, *dclus);
        if (nr < 0)
        {
            goto out;
        }
        else if (nr == FAT_ENT_FREE)
        {
            fat_fs_panic(sb, "%s: invalid cluster chain"
                         " (i_pos %d)", __FUNCTION__,
                         (int)MSDOS_I(inode)->i_pos);
            nr = -EIO;
            goto out;
        }
        else if (nr == FAT_ENT_EOF)
        {
            debug_show_lru_fatcache(&MSDOS_I(inode)->cache_lru, &cid, "fat scaned, fat cache lru", cluster);
            fat_cache_add(inode, &cid);
            goto out;
        }
        (*fclus)++;
        *dclus = nr;
        if (!cache_contiguous(&cid, *dclus))
        {
            debug_show_lru_fatcache(NULL, &cid, "fat scaned, discuded cid", cluster);
            cache_init(&cid, *fclus, *dclus);
        }
    }
    /* get continue clusters whose fat entrys belone to the already readout sector */
    if (fatent.nr_bhs)
    {
        int __dclus = *dclus;

        while (1)
        {
            nr = fat_ent_read_inbh(inode, &fatent, __dclus);
            if (nr < 0 || nr == FAT_ENT_FREE || nr == FAT_ENT_EOF)
            {
                break;
            }
            else
            {
                if ((cid.dcluster + cid.nr_contig + 1) == nr)
                {
                    cid.nr_contig++;
                }
                else
                {
                    break;
                }
            }
            __dclus = nr;
        }
    }
    if (mapped)
    {
        *mapped = cid.nr_contig - (cluster - cid.fcluster) + 1;
    }

    debug_timerend(scanfattime);

    nr = 0;
    fat_cache_add(inode, &cid);
out:
    fatent_brelse(&fatent);
    return nr;
}

static int fat_bmap_cluster(struct inode *inode, int cluster, int *mapped)
{
    struct super_block *sb = inode->i_sb;
    int ret, fclus, dclus;

    if (MSDOS_I(inode)->i_start == 0)
    {
        return 0;
    }

    ret = fat_get_cluster(inode, cluster, &fclus, &dclus, mapped);
    if (ret < 0)
    {
        return ret;
    }
    else if (ret == FAT_ENT_EOF)
    {
        fat_fs_panic(sb, "%s: request beyond EOF (i_pos %d)",
                     __FUNCTION__, (int)MSDOS_I(inode)->i_pos);
        return -EIO;
    }
    return dclus;
}

int fat_bmap(struct inode *inode, int sector, int *phys,
             unsigned long *mapped_blocks)
{
    struct super_block *sb = inode->i_sb;
    struct msdos_sb_info *sbi = MSDOS_SB(sb);
    int last_block;
    int cluster, offset, mapped;

    //debug
    struct msdos_inode_info *ms_inode = MSDOS_I(inode);

    *phys = 0;
    *mapped_blocks = 0;
    if ((sbi->fat_bits != 32) && (inode->i_ino == MSDOS_ROOT_INO))
    {
        if (sector < (sbi->dir_entries >> sbi->dir_per_block_bits))
        {
            *phys = sector + sbi->dir_start;
            *mapped_blocks = 1;
        }
        return 0;
    }
    last_block = (ms_inode->mmu_private + (sb->s_blocksize - 1))
                 >> sb->s_blocksize_bits;
    if (sector >= last_block)
    {
        return 0;
    }

    cluster = sector >> (sbi->cluster_bits - sb->s_blocksize_bits);
    offset  = sector & (sbi->sec_per_clus - 1);
    cluster = fat_bmap_cluster(inode, cluster, &mapped);
    if (cluster < 0)
    {
        return cluster;
    }
    else if (cluster)
    {
        *phys = fat_clus_to_blknr(sbi, cluster) + offset;
        *mapped_blocks = (mapped << (sbi->cluster_bits - sb->s_blocksize_bits))
                         - offset;
        if (*mapped_blocks > last_block - sector)
        {
            *mapped_blocks = last_block - sector;
        }
    }
    return 0;
}

/* -------------------------------- debug -------------------------*/
#if     FSYS_DEBUG_ON
int __debug_dump_fatchunk(struct inode *inode)
{
    struct super_block *sb = inode->i_sb;
    const int limit = sb->s_maxbytes >> MSDOS_SB(sb)->cluster_bits;
    struct fat_entry fatent;
    struct fat_cache_id cid;
    int fclus, dclus;
    int nr = 0;
    int chunk = 0;

    BUG_ON(MSDOS_I(inode)->i_start == 0);

    fclus = 0;
    dclus = MSDOS_I(inode)->i_start;

    cache_init(&cid, -1, -1);
    fatent_init(&fatent);

    while (nr != FAT_ENT_EOF)
    {
        /* prevent the infinite loop of cluster chain */
        if (fclus > limit)
        {
            fat_fs_panic(sb, "%s: detected the cluster chain loop"
                         " (i_pos %d)", __FUNCTION__,
                         (int)MSDOS_I(inode)->i_pos);
            nr = -EIO;
            goto out;
        }

        nr = fat_ent_read(inode, &fatent, dclus);
        if (nr < 0)
        {
            fat_fs_panic(sb, "%s: ent read error"
                         " (i_pos %d)", __FUNCTION__,
                         (int)MSDOS_I(inode)->i_pos);
            nr = -EIO;
            goto out;
        }
        else if (nr == FAT_ENT_FREE)
        {
            fat_fs_panic(sb, "%s: invalid cluster chain"
                         " (i_pos %d)", __FUNCTION__,
                         (int)MSDOS_I(inode)->i_pos);
            nr = -EIO;
            goto out;
        }
        fclus++;
        dclus = nr;
        if (!cache_contiguous(&cid, dclus))
        {
            if (cid.fcluster != -1)
            {
                fs_log_info("fat chunk %d = fclus:%d, dclus:%d, cont_nr:%d\n", chunk++, cid.fcluster, cid.dcluster, cid.nr_contig);
            }
            cache_init(&cid, fclus, dclus);
        }
    }

out:
    if (cid.nr_contig)
    {
        if (cid.fcluster != -1)
        {
            fs_log_info("fat chunk %d = fclus:%d, dclus:%d, cont_nr:%d\n", chunk++, cid.fcluster, cid.dcluster, cid.nr_contig);
        }
    }
    fs_log_info("there are %d clusters and %d chunks in the file.\n", fclus, chunk);

    fatent_brelse(&fatent);
    return nr;
}


void debug_show_lru_fatcache(struct list_head *head, struct fat_cache_id *cid, char *name, __u32 fclus)
{
    int i = 0;
    struct fat_cache *p;

    if (!can_dump(show_lru_fatcache))
    {
        return;
    }

    if (name)
    {
        __inf("required clus:%d. %s:\n", fclus, name);
    }
    if (head)
    {
        list_for_each_entry(p, head, cache_list)
        {
            __inf("lru = fclus:%d, dclus:%d, cont_nr:%d", p->fcluster, p->dcluster, p->nr_contig);
        }
    }
    if (cid)
    {
        __inf("cid = fclus:%d, dclus:%d, cont_nr:%d", cid->fcluster, cid->dcluster, cid->nr_contig);
    }
}
#endif  /* FSYS_DEBUG_ON */