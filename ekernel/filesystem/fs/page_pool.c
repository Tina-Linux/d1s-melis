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
#include "page_pool.h"
#include "buffer_head.h"
#include "slab.h"
#include "errno.h"
#include "fsys_debug.h"

atomic_t nr_async_pages = ATOMIC_INIT(0);
unsigned int num_physpages;
int nr_active_pages;
int nr_inactive_pages;
struct list_head inactive_list;
struct list_head active_list;

char *pagepool;
mem_map_t  mem_map[NR_TOTAL_PAGE];
mem_map_info_t mem_map_info =
{
    .free_pages = 0,
    .pages_min = NR_TOTAL_PAGE / 16,
    .pages_low = NR_TOTAL_PAGE / 3,
    .pages_high = 3 * NR_TOTAL_PAGE / 4,
};

//extern atomic_t buffermem_pages;

int mem_map_index(struct page *p)
{
    return ((p->virtual - pagepool) >> PAGE_SHIFT);
}

static void ClearBufsReferenced(struct page *page)
{
    struct buffer_head *___bh = page->buffers;

    do
    {
        clear_buffer_refered(___bh);
        ___bh = ___bh->b_this_page;
    } while (___bh != page->buffers);
}

unsigned int nr_free_buffer_pages(void)
{
    return mem_map_info.free_pages;
}

static int is_page_cache_freeable(struct page *page)
{
    return page_count(page) - !!page->buffers == 1;
}

/*
 * This moves pages from the active list to
 * the inactive list.
 *
 * We move them the other way when we see the
 * reference bit on the page.
 */
static void refill_inactive(int nr_pages)
{
    struct list_head *entry;

    entry = active_list.prev;
    while (nr_pages-- && entry != &active_list)
    {
        struct page *page;

        page = list_entry(entry, struct page, lru);
        entry = entry->prev;
        if (PageTestandClearReferenced(page))
        {
            ClearBufsReferenced(page);
            list_del(&page->lru);
            list_add(&page->lru, &active_list);
            continue;
        }

        del_page_from_active_list(page);
        add_page_to_inactive_list(page);
    }
}

static int shrink_cache(int nr_pages, int max_scan, unsigned int gfp_mask)
{
    struct list_head *entry;

    while (max_scan && (entry = inactive_list.prev) != &inactive_list)
    {
        struct page *page;

        page = list_entry(entry, struct page, lru);

        if (!PageInactive(page) && !PageActive(page))
        {
            BUG();
        }

        list_del(entry);
        list_add(entry, &inactive_list);
        if (PageTestandClearReferenced(page))
        {
            continue;
        }

        max_scan--;

        //      /* Racy check to avoid trylocking when not worthwhile */
        //      if (!page->buffers && page_count(page) != 1)
        //          continue;

        /*
         * The page is locked. IO in progress?
         * Move it to the back of the list.
         */
        if (TryLockPage(page))
        {
            continue;
        }

        /*
         * If the page has buffers, try to free the buffer mappings
         * associated with this page. If we succeed we try to free
         * the page as well.
         */
        if (page->buffers)
        {
            //          /* avoid to free a locked page */
            //          page_cache_get(page);

            if (try_to_free_buffers(page, gfp_mask))
            {
                /*
                 * Account we successfully freed a page
                 * of buffer cache.
                 */
                //              atomic_dec(&buffermem_pages);

                /*
                 * We must not allow an anon page
                 * with no buffers to be visible on
                 * the LRU, so we unlock the page after
                 * taking the lru lock
                 */
                UnlockPage(page);
                __lru_cache_del(page);

                /* effectively free the page here */
                page_cache_release(page);

                if (--nr_pages)
                {
                    continue;
                }
                break;

            }
            else
            {
                /* failed to drop the buffers so stop here */
                UnlockPage(page);
                //              page_cache_release(page);

                continue;
            }
        }

        /*
         * this is the non-racy check, it is critical to check
         * PageDirty _after_ we made sure the page is freeable
         * so not in use by anybody.
         */
        if (!is_page_cache_freeable(page) || PageDirty(page))
        {
            UnlockPage(page);
            continue;
        }

        __lru_cache_del(page);

        UnlockPage(page);

        /* effectively free the page here */
        page_cache_release(page);

        if (--nr_pages)
        {
            continue;
        }
        break;
    }

    return nr_pages;
}

static int shrink_caches(int priority, unsigned int gfp_mask, int nr_pages)
{
    int max_scan = nr_inactive_pages / priority;

    nr_pages -= kmem_cache_reap(gfp_mask);
    if (nr_pages <= 0)
    {
        return 0;
    }

    /* Do we want to age the active list? */
    if (nr_inactive_pages < nr_active_pages * 2)
    {
        refill_inactive(nr_pages);
    }

    nr_pages = shrink_cache(nr_pages, max_scan, gfp_mask);
    if (nr_pages <= 0)
    {
        return 0;
    }

    shrink_dcache_memory(priority);
    shrink_icache_memory(priority);

    return nr_pages;
}

int try_to_free_pages(void)
{
    int priority = DEF_PRIORITY;
    int ret = 0;

    do
    {
        int nr_pages = SWAP_CLUSTER_MAX;
        nr_pages = shrink_caches(priority, GFP_KSWAPD, nr_pages);
        if (nr_pages <= 0)
        {
            return 1;
        }
    } while (--priority);

    return ret;
}

/*
 * Mark a page as having seen activity.
 *
 * If it was already so marked, move it
 * to the active queue and drop the referenced
 * bit. Otherwise, just mark it for future
 * action..
 */
void mark_page_accessed(struct buffer_head *bh)
{
    struct page *page = bh->b_page;

    if (!PageActive(page) && PageReferenced(page) && Buffer_Refered(bh))
    {
        activate_page(page);
        ClearBufsReferenced(page);
        ClearPageReferenced(page);
        return;
    }

    set_buffer_refered(bh);
    SetPageReferenced(page);
}

#define MARK_USED(index, order, area) \
    change_bit((index) >> (1+(order)), (area)->map)

static struct page *expand(struct page *page,
                           unsigned long index, int low, int high, free_area_t *area)
{
    unsigned long size = 1 << high;

    while (high > low)
    {
        area--;
        high--;
        size >>= 1;
        memlist_add_head(&(page)->list, &(area)->free_list);
        MARK_USED(index, high, area);
        index += size;
        page += size;
    }
    return page;
}

static struct page *rmqueue(unsigned int order)
{
    free_area_t *area = mem_map_info.free_area + order;
    unsigned int curr_order = order;
    struct list_head *head, *curr;
    unsigned long flags;
    struct page *page;

    do
    {
        head = &area->free_list;
        curr = memlist_next(head);

        if (curr != head)
        {
            unsigned int index;

            page = memlist_entry(curr, struct page, list);
            memlist_del(curr);
            index = page - mem_map;
            if (curr_order != MAX_ORDER - 1)
            {
                MARK_USED(index, curr_order, area);
            }
            mem_map_info.free_pages -= 1UL << order;

            page = expand(page, index, order, curr_order, area);

            set_page_count(page, 1);
            DEBUG_LRU_PAGE(page);
            return page;
        }
        curr_order++;
        area++;
    } while (curr_order < MAX_ORDER);

    return NULL;
}

static inline unsigned long left_free_pages(unsigned int order)
{
    long free = mem_map_info.free_pages - (1UL << order);
    return free >= 0 ? free : 0;
}

struct page *alloc_pages(unsigned int gfp_mask, int order)
{
    struct list_head *curr;
    struct page *page;

    int nr_pages;
    int max_scan;
    int priority;
    int min;


    if (left_free_pages(order) > mem_map_info.pages_low)
    {
        page = rmqueue(order);
        if (page)
        {
            return page;
        }
    }

repeat:
    min = mem_map_info.pages_min;
    if (!(gfp_mask & __GFP_WAIT))
    {
        min >>= 2;
    }
    if (left_free_pages(order) > min)
    {
        page = rmqueue(order);
        if (page)
        {
            return page;
        }
    }

    try_to_free_pages();
    goto repeat;
}

struct page *alloc_page(unsigned int gfp_mask)
{
    struct page *pg;

    pg = alloc_pages(gfp_mask, 0);
    if (pg != NULL)
    {
        set_page_count(pg, 1);
    }

    return pg;
}

unsigned long __get_free_pages(unsigned int gfp_mask,  unsigned int order)
{
    struct page *page;

    page = alloc_pages(gfp_mask, order);
    if (!page)
    {
        return 0;
    }
    return (unsigned long) page_address(page);
}

struct page *virt_to_page(unsigned long addr)
{
    return &mem_map[(addr - (unsigned long)pagepool) >> PAGE_SHIFT];
}

void __free_pages(struct page *page, unsigned int order)
{
    unsigned long index, page_idx, mask, flags;
    free_area_t *area;
    struct page *base;

    if (put_page_testzero(page))
    {
        if (page->buffers)
        {
            BUG();
        }
        if (page->mapping)
        {
            BUG();
        }
        if (PageLocked(page))
        {
            BUG();
        }
        if (PageDecrAfter(page))
        {
            BUG();
        }
        if (PageActive(page))
        {
            BUG();
        }
        if (PageInactive(page))
        {
            BUG();
        }
        page->flags &= ~((1 << PG_referenced) | (1 << PG_dirty));

        mask = (~0UL) << order;
        base = mem_map;
        page_idx = page - base;
        if (page_idx & ~mask)
        {
            BUG();
        }
        index = page_idx >> (1 + order);

        area = mem_map_info.free_area + order;

        mem_map_info.free_pages -= mask;

        while (mask + (1 << (MAX_ORDER - 1)))
        {
            struct page *buddy1, *buddy2;

            if (area >= mem_map_info.free_area + MAX_ORDER)
            {
                BUG();
            }
            if (!test_and_change_bit(index, area->map))
                /*
                 * the buddy page is still allocated.
                 */
            {
                break;
            }
            /*
             * Move the buddy up one level.
             */
            buddy1 = base + (page_idx ^ -mask);
            buddy2 = base + page_idx;

            memlist_del(&buddy1->list);
            mask <<= 1;
            area++;
            index >>= 1;
            page_idx &= mask;
        }
        memlist_add_head(&(base + page_idx)->list, &area->free_list);
    }
}

void free_pages(unsigned long addr, unsigned int order)
{
    if (addr != 0)
    {
        __free_pages(virt_to_page(addr), order);
    }
}

/**
 * (de)activate_page - move pages from/to active and inactive lists
 * @page: the page we want to move
 * @nolock - are we already holding the pagemap_lru_lock?
 *
 * Deactivate_page will move an active page to the right
 * inactive list, while activate_page will move a page back
 * from one of the inactive lists to the active list. If
 * called on a page which is not on any of the lists, the
 * page is left alone.
 */
void deactivate_page_nolock(struct page *page)
{
    if (PageActive(page))
    {
        del_page_from_active_list(page);
        add_page_to_inactive_list(page);
    }
}

void deactivate_page(struct page *page)
{
    deactivate_page_nolock(page);
}

/*
 * Move an inactive page to the active list.
 */
void activate_page_nolock(struct page *page)
{
    if (PageInactive(page))
    {
        del_page_from_inactive_list(page);
        add_page_to_active_list(page);
    }
}

void activate_page(struct page *page)
{
    activate_page_nolock(page);
}

/**
 * lru_cache_add: add a page to the page lists
 * @page: the page to add
 */
void lru_cache_add(struct page *page)
{
    if (!PageLocked(page))
    {
        BUG();
    }
    add_page_to_inactive_list(page);
}

/**
 * __lru_cache_del: remove a page from the page lists
 * @page: the page to add
 *
 * This function is for when the caller already holds
 * the pagemap_lru_lock.
 */
void __lru_cache_del(struct page *page)
{
    if (PageActive(page))
    {
        del_page_from_active_list(page);
    }
    else if (PageInactive(page))
    {
        del_page_from_inactive_list(page);
    }
    else
    {
        fs_log_warning("VM: __lru_cache_del, found unknown page ?!\n");
    }

    DEBUG_LRU_PAGE(page);
}

/**
 * lru_cache_del: remove a page from the page lists
 * @page: the page to remove
 */
void lru_cache_del(struct page *page)
{
    if (!PageLocked(page))
    {
        BUG();
    }
    __lru_cache_del(page);
}

char *__pagepool;
int page_pool_ini(void)
{
    int i;
    int err;
    struct page *p;

#ifdef simulate
    __pagepool = malloc((NR_TOTAL_PAGE + 2) << PAGE_SHIFT);
    if (__pagepool == NULL)
    {
        return -1;
    }
    if ((int)__pagepool & ((1 << PAGE_SHIFT) - 1))
    {
        pagepool = (char *)(((int)__pagepool + ((1 << PAGE_SHIFT) - 1)) & ~((1 << PAGE_SHIFT) - 1));
    }
#else
    pagepool = k_malloc((NR_TOTAL_PAGE) << PAGE_SHIFT);
#endif

    err = -ENOMEM;
    if (pagepool == NULL)
    {
        goto out;
    }

    num_physpages = NR_TOTAL_PAGE;
    fs_log_verbose("Fat file system has alloc %dk bytes memory for page pool(page size:%dk, page count:%d).\n",
                   (NR_TOTAL_PAGE << PAGE_SHIFT) >> 10,
                   1 << (PAGE_SHIFT - 10),
                   NR_TOTAL_PAGE
                  );

    INIT_LIST_HEAD(&active_list);
    INIT_LIST_HEAD(&inactive_list);

    for (i = 0; i < MAX_ORDER; i++)
    {
        unsigned long bitmap_size;

        memlist_init(&mem_map_info.free_area[i].free_list);
        mem_map_info.free_area[i].map = NULL;

        bitmap_size = (NR_TOTAL_PAGE - 1) >> (i + 4);
        bitmap_size = LONG_ALIGN(bitmap_size + 1);
        mem_map_info.free_area[i].map = malloc(bitmap_size);
        if (mem_map_info.free_area[i].map)
        {
            memset(mem_map_info.free_area[i].map, 0, bitmap_size);
        }
        else
        {
            goto free_out;
        }
    }
    fs_log_verbose("Max page alloc order is %d.\n", --i);

    for (i = 0; i < NR_TOTAL_PAGE; i++)
    {
        struct page *page;

        page = &mem_map[i];
        atomic_set(&page->count, 1);
        mem_map[i].virtual = pagepool + (i << PAGE_SHIFT);
        __free_pages(page, 0);
    }

    nr_active_pages = 0;
    nr_inactive_pages = 0;

    err = 0;
    goto out;

free_out:
    if (pagepool != NULL)
    {
        k_free(pagepool);
    }

    for (i = 0; i < MAX_ORDER; i++)
    {
        if (mem_map_info.free_area[i].map)
        {
            free(mem_map_info.free_area[i].map);
        }
    }

out:
    return err;
}

void page_pool_exit(void)
{
#ifdef simulate
    free(__pagepool);
#else
    k_free(pagepool);
#endif
}
/*---------------- debug -----------------------*/
#if     FSYS_DEBUG_ON
void debug_show_all_page_list(char *info)
{
    fs_log_info("%s", info);

    //   debug_show_free_page(&freep_list, "free pages", 0, detail);
    debug_show_lru_page(&inactive_list, "inactive pages", 0);
    debug_show_lru_page(&active_list, "active pages", 0);
}
#endif  /* FSYS_DEBUG_ON */