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
#include    "slab.h"
#include    "list.h"
#include    "page_pool.h"
#include    "fsys_debug.h"

struct mutex {};
struct semaphore {};
void inline init_MUTEX(struct mutex *lock)
{
}
#define up(x)
#define down(x)

/*
 * DEBUG    - 1 for kmem_cache_create() to honour; SLAB_DEBUG_INITIAL,
 *        SLAB_RED_ZONE & SLAB_POISON.
 *        0 for faster, smaller code (especially in the critical paths).
 *
 * STATS    - 1 to collect stats for /proc/slabinfo.
 *        0 for faster, smaller code (especially in the critical paths).
 *
 * FORCED_DEBUG - 1 enables SLAB_RED_ZONE and SLAB_POISON (if possible)
 */

#ifdef CONFIG_DEBUG_SLAB
#define DEBUG       1
#define STATS       1
#define FORCED_DEBUG    1
#else
#define DEBUG       0
#define STATS       0
#define FORCED_DEBUG    0
#endif

/*
 * Parameters for kmem_cache_reap
 */
#define REAP_SCANLEN    10
#define REAP_PERFECT    10

/* Shouldn't this be in a header file somewhere? */
#define BYTES_PER_WORD      sizeof(void *)

/* Legal flag mask for kmem_cache_create(). */
#define CREATE_MASK (SLAB_HWCACHE_ALIGN | SLAB_NO_REAP | SLAB_CACHE_DMA)

/*
 * kmem_bufctl_t:
 *
 * Bufctl's are used for linking objs within a slab
 * linked offsets.
 *
 * This implementaion relies on "struct page" for locating the cache &
 * slab an object belongs to.
 * This allows the bufctl structure to be small (one int), but limits
 * the number of objects a slab (not a cache) can contain when off-slab
 * bufctls are used. The limit is the size of the largest general cache
 * that does not use off-slab slabs.
 * For 32bit archs with 4 kB pages, is this 56.
 * This is not serious, as it is only for large objects, when it is unwise
 * to have too many per slab.
 * Note: This limit can be raised by introducing a general cache whose size
 * is less than 512 (PAGE_SIZE<<3), but greater than 256.
 */

#define BUFCTL_END 0xffffFFFF
#define SLAB_LIMIT 0xffffFFFE
typedef unsigned int kmem_bufctl_t;

/* Max number of objs-per-slab for caches which use off-slab slabs.
 * Needed to avoid a possible looping condition in kmem_cache_grow().
 */
static unsigned long offslab_limit;

/*
 * slab_t
 *
 * Manages the objs in a slab. Placed either at the beginning of mem allocated
 * for a slab, or allocated from an general cache.
 * Slabs are chained into three list: fully used, partial, fully free slabs.
 */
typedef struct slab_s
{
    struct list_head    list;
    unsigned long       colouroff;
    void            *s_mem;     /* including colour offset */
    unsigned int        inuse;      /* num of objs active in slab */
    kmem_bufctl_t       free;
} slab_t;

#define slab_bufctl(slabp) \
    ((kmem_bufctl_t *)(((slab_t*)slabp)+1))

/*
 * cpucache_t
 *
 * Per cpu structures
 * The limit is stored in the per-cpu structure to reduce the data cache
 * footprint.
 */
typedef struct cpucache_s
{
    unsigned int avail;
    unsigned int limit;
} cpucache_t;

#define cc_entry(cpucache) \
    ((void **)(((cpucache_t*)(cpucache))+1))
#define cc_data(cachep) \
    ((cachep)->cpudata[smp_processor_id()])
/*
 * kmem_cache_t
 *
 * manages a cache.
 */

#define CACHE_NAMELEN   20  /* max name length for a slab cache */

struct kmem_cache_s
{
    /* 1) each alloc & free */
    /* full, partial first, then free */
    struct list_head    slabs_full;
    struct list_head    slabs_partial;
    struct list_head    slabs_free;
    unsigned int        objsize;
    unsigned int        flags;  /* constant flags */
    unsigned int        num;    /* # of objs per slab */

    /* 2) slab additions /removals */
    /* order of pgs per slab (2^n) */
    unsigned int        gfporder;

    /* force GFP flags, e.g. GFP_DMA */
    unsigned int        gfpflags;

    unsigned int            colour;     /* cache colouring range */
    unsigned int        colour_off; /* colour offset */
    unsigned int        colour_next;    /* cache colouring */
    kmem_cache_t        *slabp_cache;
    unsigned int        growing;
    unsigned int        dflags;     /* dynamic flags */

    /* constructor func */
    void (*ctor)(void *, kmem_cache_t *, unsigned long);

    /* de-constructor func */
    void (*dtor)(void *, kmem_cache_t *, unsigned long);

    unsigned long       failures;

    /* 3) cache creation/removal */
    char            name[CACHE_NAMELEN];
    struct list_head    next;
};

/* internal c_flags */
#define CFLGS_OFF_SLAB  0x010000UL  /* slab management in own cache */
#define CFLGS_OPTIMIZE  0x020000UL  /* optimized slab lookup */

/* c_dflags (dynamic flags). Need to hold the spinlock to access this member */
#define DFLGS_GROWN 0x000001UL  /* don't reap a recently grown */

#define OFF_SLAB(x) ((x)->flags & CFLGS_OFF_SLAB)
#define OPTIMIZE(x) ((x)->flags & CFLGS_OPTIMIZE)
#define GROWN(x)    ((x)->dlags & DFLGS_GROWN)

/* maximum size of an obj (in 2^order pages) */
#define MAX_OBJ_ORDER   5   /* 32 pages */

/*
 * Do not go above this order unless 0 objects fit into the slab.
 */
#define BREAK_GFP_ORDER_HI  2
#define BREAK_GFP_ORDER_LO  1
static int slab_break_gfp_order = BREAK_GFP_ORDER_LO;

/*
 * Absolute limit for the gfp order
 */
#define MAX_GFP_ORDER   5   /* 32 pages */


/* Macros for storing/retrieving the cachep and or slab from the
 * global 'mem_map'. These are used to find the slab an obj belongs to.
 * With kfree(), these are used to find the cache which an obj belongs to.
 */
#define SET_PAGE_CACHE(pg,x)  ((pg)->list.next = (struct list_head *)(x))
#define GET_PAGE_CACHE(pg)    ((kmem_cache_t *)(pg)->list.next)
#define SET_PAGE_SLAB(pg,x)   ((pg)->list.prev = (struct list_head *)(x))
#define GET_PAGE_SLAB(pg)     ((slab_t *)(pg)->list.prev)

/* Size description struct for general caches. */
typedef struct cache_sizes
{
    unsigned int         cs_size;
    kmem_cache_t    *cs_cachep;
} cache_sizes_t;

static cache_sizes_t cache_sizes[] =
{
    {    32,    NULL},
    {    64,    NULL},
    {   128,    NULL},
    {   256,    NULL},
    {   512,    NULL},
    {  1024,    NULL},
    {  2048,    NULL},
    {  4096,    NULL},
    {  8192,    NULL},
    {     0,    NULL}
};

/* internal cache of cache description objs */
static kmem_cache_t cache_cache =
{
slabs_full:
    LIST_HEAD_INIT(cache_cache.slabs_full),
slabs_partial:
    LIST_HEAD_INIT(cache_cache.slabs_partial),
slabs_free:
    LIST_HEAD_INIT(cache_cache.slabs_free),
objsize:
    sizeof(kmem_cache_t),
flags:
    SLAB_NO_REAP,
colour_off:
    L1_CACHE_BYTES,
name:       "kmem_cache"
    ,
};

extern unsigned int num_physpages;
int nr_debug_slab_pages = 0;

/* Guard access to the cache-chain. */
static struct semaphore cache_chain_sem;

/* Place maintainer for reaping. */
static kmem_cache_t *clock_searchp = &cache_cache;

#define cache_chain (cache_cache.next)

/* Cal the num objs, wastage, and bytes left over for a given slab size. */
static void kmem_cache_estimate(unsigned long gfporder, unsigned int size,
                                int flags, unsigned int *left_over, unsigned int *num)
{
    unsigned int i;
    unsigned int wastage = PAGE_SIZE << gfporder;
    unsigned int extra = 0;
    unsigned int base = 0;

    if (!(flags & CFLGS_OFF_SLAB))
    {
        base = sizeof(slab_t);
        extra = sizeof(kmem_bufctl_t);
    }
    i = 0;
    while (i * size + L1_CACHE_ALIGN(base + i * extra) <= wastage)
    {
        i++;
    }
    if (i > 0)
    {
        i--;
    }

    if (i > SLAB_LIMIT)
    {
        i = SLAB_LIMIT;
    }

    *num = i;
    wastage -= i * size;
    wastage -= L1_CACHE_ALIGN(base + i * extra);
    *left_over = wastage;
}

/* Initialisation - setup the `cache' cache. */
int kmem_cache_init(void)
{
    unsigned int left_over;

    //  init_MUTEX(&cache_chain_sem);
    INIT_LIST_HEAD(&cache_chain);

    kmem_cache_estimate(0, cache_cache.objsize, 0,
                        &left_over, &cache_cache.num);
    if (!cache_cache.num)
    {
        BUG();
    }

    cache_cache.colour = left_over / cache_cache.colour_off;
    cache_cache.colour_next = 0;

    return 0;
}


/* Initialisation - setup remaining internal and general caches.
 * Called after the gfp() functions have been enabled, and before smp_init().
 */
int kmem_cache_sizes_init(void)
{
    cache_sizes_t *sizes = cache_sizes;
    char name[20];
    /*
     * Fragmentation resistance on low memory - only use bigger
     * page orders on machines with more than 32MB of memory.
     */
    if (num_physpages > (32 << 20) >> PAGE_SHIFT)
    {
        slab_break_gfp_order = BREAK_GFP_ORDER_HI;
    }
    do
    {
        /* For performance, all the general caches are L1 aligned.
         * This should be particularly beneficial on SMP boxes, as it
         * eliminates "false sharing".
         * Note for systems short on memory removing the alignment will
         * allow tighter packing of the smaller caches. */
        sprintf(name, "size-%d", sizes->cs_size);
        if (!(sizes->cs_cachep =
                  kmem_cache_create(name, sizes->cs_size,
                                    0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
        {
            BUG();
        }

        /* Inc off-slab bufctl limit until the ceiling is hit. */
        if (!(OFF_SLAB(sizes->cs_cachep)))
        {
            offslab_limit = sizes->cs_size - sizeof(slab_t);
            offslab_limit /= 2;
        }

        sizes++;
    } while (sizes->cs_size);

    return 0;
}


/* Interface to system's page allocator. No need to hold the cache-lock.
 */
static inline void *kmem_getpages(kmem_cache_t *cachep, unsigned long flags)
{
    void    *addr;

    debug_show_page_info("slab getpages->");
    /*
     * If we requested dmaable memory, we will get it. Even if we
     * did not request dmaable memory, we might get it, but that
     * would be relatively rare and ignorable.
     */
    flags |= cachep->gfpflags;
    addr = (void *) __get_free_pages(flags, cachep->gfporder);

    nr_debug_slab_pages += (1 << cachep->gfporder);

    /* Assume that now we have the pages no one else can legally
     * messes with the 'struct page's.
     * However vm_scan() might try to test the structure to see if
     * it is a named-page or buffer-page.  The members it tests are
     * of no interest here.....
     */
    return addr;
}

/* Interface to system's page release. */
static inline void kmem_freepages(kmem_cache_t *cachep, void *addr)
{
    unsigned long i = (1 << cachep->gfporder);
    struct page *page = virt_to_page((long)addr);

    /* free_pages() does not clear the type bit - we do that.
     * The pages have been unlinked from their cache-slab,
     * but their 'struct page's might be accessed in
     * vm_scan(). Shouldn't be a worry.
     */
    while (i--)
    {
        PageClearSlab(page);
        page++;
    }
    free_pages((unsigned long)addr, cachep->gfporder);
    nr_debug_slab_pages -= (1 << cachep->gfporder);
}

/* Destroy all the objs in a slab, and release the mem back to the system.
 * Before calling the slab must have been unlinked from the cache.
 * The cache-lock is not held/needed.
 */
static void kmem_slab_destroy(kmem_cache_t *cachep, slab_t *slabp)
{
    if (cachep->dtor)
    {
        int i;
        for (i = 0; i < cachep->num; i++)
        {
            void *objp = (void *)((uintptr_t)slabp->s_mem + cachep->objsize * i);
            if (cachep->dtor)
            {
                (cachep->dtor)(objp, cachep, 0);
            }
        }
    }

    kmem_freepages(cachep, (void *)((uintptr_t)slabp->s_mem - slabp->colouroff));
    if (OFF_SLAB(cachep))
    {
        kmem_cache_free(cachep->slabp_cache, slabp);
    }
}

/**
 * kmem_cache_create - Create a cache.
 * @name: A string which is used in /proc/slabinfo to identify this cache.
 * @size: The size of objects to be created in this cache.
 * @offset: The offset to use within the page.
 * @flags: SLAB flags
 * @ctor: A constructor for the objects.
 * @dtor: A destructor for the objects.
 *
 * Returns a ptr to the cache on success, NULL on failure.
 * Cannot be called within a int, but can be interrupted.
 * The @ctor is run when new pages are allocated by the cache
 * and the @dtor is run before the pages are handed back.
 * The flags are
 *
 * %SLAB_POISON - Poison the slab with a known test pattern (a5a5a5a5)
 * to catch references to uninitialised memory.
 *
 * %SLAB_RED_ZONE - Insert `Red' zones around the allocated memory to check
 * for buffer overruns.
 *
 * %SLAB_NO_REAP - Don't automatically reap this cache when we're under
 * memory pressure.
 *
 * %SLAB_HWCACHE_ALIGN - Align the objects in this cache to a hardware
 * cacheline.  This can be beneficial if you're counting cycles as closely
 * as davem.
 */
kmem_cache_t *kmem_cache_create(const char *name, unsigned int size, unsigned int offset,
                                unsigned long flags, void (*ctor)(void *, kmem_cache_t *, unsigned long),
                                void (*dtor)(void *, kmem_cache_t *, unsigned long))
{
    const char *func_nm = "kmem_create: ";
    unsigned int left_over, align, slab_size;
    kmem_cache_t *cachep = NULL;

    /*
     * Sanity checks... these are all serious usage bugs.
     */
    if ((!name) ||
        ((strlen(name) >= CACHE_NAMELEN - 1)) ||
        (size < BYTES_PER_WORD) ||
        (size > (1 << MAX_OBJ_ORDER)*PAGE_SIZE) ||
        (dtor && !ctor) ||
        ((int)offset < 0 || (int)offset > size))
    {
        BUG();
    }

    /*
     * Always checks flags, a caller might be expecting debug
     * support which isn't available.
     */
    if (flags & ~CREATE_MASK)
    {
        BUG();
    }

    /* Get cache's description obj. */
    cachep = (kmem_cache_t *) kmem_cache_alloc(&cache_cache, SLAB_KERNEL);
    if (!cachep)
    {
        goto opps;
    }
    memset(cachep, 0, sizeof(kmem_cache_t));

    /* Check that size is in terms of words.  This is needed to avoid
     * unaligned accesses for some archs when redzoning is used, and makes
     * sure any on-slab bufctl's are also correctly aligned.
     */
    if (size & (BYTES_PER_WORD - 1))
    {
        size += (BYTES_PER_WORD - 1);
        size &= ~(BYTES_PER_WORD - 1);
        fs_log_info("%sForcing size word alignment - %s\n", func_nm, name);
    }

    align = BYTES_PER_WORD;
    if (flags & SLAB_HWCACHE_ALIGN)
    {
        align = L1_CACHE_BYTES;
    }

    /* Determine if the slab management is 'on' or 'off' slab. */
    if (size >= (PAGE_SIZE >> 3))
        /*
         * Size is large, assume best to place the slab management obj
         * off-slab (should allow better packing of objs).
         */
    {
        flags |= CFLGS_OFF_SLAB;
    }

    if (flags & SLAB_HWCACHE_ALIGN)
    {
        /* Need to adjust size so that objs are cache aligned. */
        /* Small obj size, can get at least two per cache line. */
        /* FIXME: only power of 2 supported, was better */
        while (size < align / 2)
        {
            align /= 2;
        }
        size = (size + align - 1) & (~(align - 1));
    }

    /* Cal size (in pages) of slabs, and the num of objs per slab.
     * This could be made much more intelligent.  For now, try to avoid
     * using high page-orders for slabs.  When the gfp() funcs are more
     * friendly towards high-order requests, this should be changed.
     */
    do
    {
        unsigned int break_flag = 0;
cal_wastage:
        kmem_cache_estimate(cachep->gfporder, size, flags,
                            &left_over, &cachep->num);
        if (break_flag)
        {
            break;
        }
        if (cachep->gfporder >= MAX_GFP_ORDER)
        {
            break;
        }
        if (!cachep->num)
        {
            goto next;
        }
        if (flags & CFLGS_OFF_SLAB && cachep->num > offslab_limit)
        {
            /* Oops, this num of objs will cause problems. */
            cachep->gfporder--;
            break_flag++;
            goto cal_wastage;
        }

        /*
         * Large num of objs is good, but v. large slabs are currently
         * bad for the gfp()s.
         */
        if (cachep->gfporder >= slab_break_gfp_order)
        {
            break;
        }

        if ((left_over * 8) <= (PAGE_SIZE << cachep->gfporder))
        {
            break;    /* Acceptable internal fragmentation. */
        }
next:
        cachep->gfporder++;
    } while (1);

    if (!cachep->num)
    {
        fs_log_info("kmem_cache_create: couldn't create cache %s.\n", name);
        kmem_cache_free(&cache_cache, cachep);
        cachep = NULL;
        goto opps;
    }
    slab_size = L1_CACHE_ALIGN(cachep->num * sizeof(kmem_bufctl_t) + sizeof(slab_t));

    /*
     * If the slab has been placed off-slab, and we have enough space then
     * move it on-slab. This is at the expense of any extra colouring.
     */
    if (flags & CFLGS_OFF_SLAB && left_over >= slab_size)
    {
        flags &= ~CFLGS_OFF_SLAB;
        left_over -= slab_size;
    }

    /* Offset must be a multiple of the alignment. */
    offset += (align - 1);
    offset &= ~(align - 1);
    if (!offset)
    {
        offset = L1_CACHE_BYTES;
    }
    cachep->colour_off = offset;
    cachep->colour = left_over / offset;

    /* init remaining fields */
    if (!cachep->gfporder && !(flags & CFLGS_OFF_SLAB))
    {
        flags |= CFLGS_OPTIMIZE;
    }

    cachep->flags = flags;
    cachep->gfpflags = 0;
    if (flags & SLAB_CACHE_DMA)
    {
        cachep->gfpflags |= GFP_DMA;
    }
    cachep->objsize = size;
    INIT_LIST_HEAD(&cachep->slabs_full);
    INIT_LIST_HEAD(&cachep->slabs_partial);
    INIT_LIST_HEAD(&cachep->slabs_free);

    if (flags & CFLGS_OFF_SLAB)
    {
        cachep->slabp_cache = kmem_find_general_cachep(slab_size, 0);
    }
    cachep->ctor = ctor;
    cachep->dtor = dtor;
    /* Copy name over so we don't have problems with unloaded modules */
    strcpy(cachep->name, name);

    /* Need the semaphore to access the chain. */
    down(&cache_chain_sem);
    {
        struct list_head *p;

        list_for_each(p, &cache_chain)
        {
            kmem_cache_t *pc = list_entry(p, kmem_cache_t, next);

            /* The name field is constant - no lock needed. */
            if (!strcmp(pc->name, name))
            {
                BUG();
            }
        }
    }

    /* There is no reason to lock our new cache before we
     * link it in - no one knows about it yet...
     */
    list_add(&cachep->next, &cache_chain);
    up(&cache_chain_sem);
opps:
    return cachep;
}

static int __kmem_cache_shrink(kmem_cache_t *cachep)
{
    slab_t *slabp;
    int ret;

    /* If the cache is growing, stop shrinking. */
    while (!cachep->growing)
    {
        struct list_head *p;

        p = cachep->slabs_free.prev;
        if (p == &cachep->slabs_free)
        {
            break;
        }

        slabp = list_entry(cachep->slabs_free.prev, slab_t, list);
        list_del(&slabp->list);

        kmem_slab_destroy(cachep, slabp);
    }
    ret = !list_empty(&cachep->slabs_full) || !list_empty(&cachep->slabs_partial);
    return ret;
}

/**
 * kmem_cache_shrink - Shrink a cache.
 * @cachep: The cache to shrink.
 *
 * Releases as many slabs as possible for a cache.
 * To help debugging, a zero exit status indicates all slabs were released.
 */
int kmem_cache_shrink(kmem_cache_t *cachep)
{
    if (!cachep)
    {
        BUG();
    }

    return __kmem_cache_shrink(cachep);
}

/**
 * kmem_cache_destroy - delete a cache
 * @cachep: the cache to destroy
 *
 * Remove a kmem_cache_t object from the slab cache.
 * Returns 0 on success.
 *
 * It is expected this function will be called by a module when it is
 * unloaded.  This will remove the cache completely, and avoid a duplicate
 * cache being allocated each time a module is loaded and unloaded, if the
 * module doesn't have persistent in-kernel storage across loads and unloads.
 *
 * The caller must guarantee that noone will allocate memory from the cache
 * during the kmem_cache_destroy().
 */
int kmem_cache_destroy(kmem_cache_t *cachep)
{
    if (!cachep || cachep->growing)
    {
        BUG();
    }

    /* Find the cache in the chain of caches. */
    down(&cache_chain_sem);
    /* the chain is never empty, cache_cache is never destroyed */
    if (clock_searchp == cachep)
        clock_searchp = list_entry(cachep->next.next,
                                   kmem_cache_t, next);
    list_del(&cachep->next);
    up(&cache_chain_sem);

    if (__kmem_cache_shrink(cachep))
    {
        fs_log_error("kmem_cache_destroy: Can't free all objects %p\n",
                     cachep);
        down(&cache_chain_sem);
        list_add(&cachep->next, &cache_chain);
        up(&cache_chain_sem);
        return 1;
    }
    kmem_cache_free(&cache_cache, cachep);

    return 0;
}

/* Get the memory for a slab management obj. */
static inline slab_t *kmem_cache_slabmgmt(kmem_cache_t *cachep,
        void *objp, int colour_off, int local_flags)
{
    slab_t *slabp;

    if (OFF_SLAB(cachep))
    {
        /* Slab management obj is off-slab. */
        slabp = kmem_cache_alloc(cachep->slabp_cache, local_flags);
        if (!slabp)
        {
            return NULL;
        }
    }
    else
    {
        /* FIXME: change to
            slabp = objp
         * if you enable OPTIMIZE
         */
        slabp = (void *)((long)objp + colour_off);
        colour_off += L1_CACHE_ALIGN(cachep->num *
                                     sizeof(kmem_bufctl_t) + sizeof(slab_t));
    }
    slabp->inuse = 0;
    slabp->colouroff = colour_off;
    slabp->s_mem = (void *)((long)objp + colour_off);

    return slabp;
}

static inline void kmem_cache_init_objs(kmem_cache_t *cachep,
                                        slab_t *slabp, unsigned long ctor_flags)
{
    int i;

    for (i = 0; i < cachep->num; i++)
    {
        void *objp = (void *)((uintptr_t)slabp->s_mem + cachep->objsize * i);

        /*
         * Constructors are not allowed to allocate memory from
         * the same cache which they are a constructor for.
         * Otherwise, deadlock. They must also be threaded.
         */
        if (cachep->ctor)
        {
            cachep->ctor(objp, cachep, ctor_flags);
        }
        slab_bufctl(slabp)[i] = i + 1;
    }
    slab_bufctl(slabp)[i - 1] = BUFCTL_END;
    slabp->free = 0;
}

/*
 * Grow (by 1) the number of slabs within a cache.  This is called by
 * kmem_cache_alloc() when there are no active objs left in a cache.
 */
static int kmem_cache_grow(kmem_cache_t *cachep, int flags)
{
    slab_t  *slabp;
    struct page *page;
    void        *objp;
    unsigned int         offset;
    unsigned int     i, local_flags;
    unsigned long    ctor_flags;
    unsigned long    save_flags;

    /* Be lazy and only check for valid flags here,
     * keeping it out of the critical path in kmem_cache_alloc().
     */
    if (flags & ~(SLAB_DMA | SLAB_LEVEL_MASK | SLAB_NO_GROW))
    {
        BUG();
    }
    if (flags & SLAB_NO_GROW)
    {
        return 0;
    }

    ctor_flags = SLAB_CTOR_CONSTRUCTOR;
    local_flags = (flags & SLAB_LEVEL_MASK);
    if (local_flags == SLAB_ATOMIC)
        /*
         * Not allowed to sleep.  Need to tell a constructor about
         * this - it might need to know...
         */
    {
        ctor_flags |= SLAB_CTOR_ATOMIC;
    }

    /* Get colour for the slab, and cal the next value. */
    offset = cachep->colour_next;
    cachep->colour_next++;
    if (cachep->colour_next >= cachep->colour)
    {
        cachep->colour_next = 0;
    }
    offset *= cachep->colour_off;
    cachep->dflags |= DFLGS_GROWN;

    cachep->growing++;

    /* A series of memory allocations for a new slab.
     * Neither the cache-chain semaphore, or cache-lock, are
     * held, but the incrementing c_growing prevents this
     * cache from being reaped or shrunk.
     * Note: The cache could be selected in for reaping in
     * kmem_cache_reap(), but when the final test is made the
     * growing value will be seen.
     */

    /* Get mem for the objs. */
    if (!(objp = kmem_getpages(cachep, flags)))
    {
        goto failed;
    }

    /* Get slab management. */
    if (!(slabp = kmem_cache_slabmgmt(cachep, objp, offset, local_flags)))
    {
        goto opps1;
    }

    /* Nasty!!!!!! I hope this is OK. */
    i = 1 << cachep->gfporder;
    page = virt_to_page((long)objp);
    do
    {
        SET_PAGE_CACHE(page, cachep);
        SET_PAGE_SLAB(page, slabp);
        PageSetSlab(page);
        page++;
    } while (--i);

    kmem_cache_init_objs(cachep, slabp, ctor_flags);

    cachep->growing--;

    /* Make slab active. */
    list_add_tail(&slabp->list, &cachep->slabs_free);
    cachep->failures = 0;

    return 1;
opps1:
    kmem_freepages(cachep, objp);
failed:
    cachep->growing--;
    return 0;
}

static inline void *kmem_cache_alloc_one_tail(kmem_cache_t *cachep,
        slab_t *slabp)
{
    void *objp;

    /* get obj pointer */
    slabp->inuse++;
    objp = (void *)((uintptr_t)slabp->s_mem + slabp->free * cachep->objsize);
    slabp->free = slab_bufctl(slabp)[slabp->free];

    if (slabp->free == BUFCTL_END)
    {
        list_del(&slabp->list);
        list_add(&slabp->list, &cachep->slabs_full);
    }
    return objp;
}

/**
 * kmem_cache_alloc - Allocate an object
 * @cachep: The cache to allocate from.
 * @flags: See kmalloc().
 *
 * Allocate an object from this cache.  The flags are only relevant
 * if the cache has no available objects.
 */
void *kmem_cache_alloc(kmem_cache_t *cachep, int flags)
{
    struct list_head *slabs_partial, * entry;
    unsigned long save_flags;
    slab_t *slabp;
    void *objp;

try_again:
    slabs_partial = &(cachep)->slabs_partial;
    entry = slabs_partial->next;
    if (entry == slabs_partial)
    {
        struct list_head *slabs_free;
        slabs_free = &(cachep)->slabs_free;
        entry = slabs_free->next;
        if (entry == slabs_free)
        {
            goto alloc_new_slab;
        }
        list_del(entry);
        list_add(entry, slabs_partial);
    }

    slabp = list_entry(entry, slab_t, list);

    objp = kmem_cache_alloc_one_tail(cachep, slabp);
    return objp;
alloc_new_slab:
    if (kmem_cache_grow(cachep, flags))
        /* Someone may have stolen our objs.  Doesn't matter, we'll
         * just come back here again.
         */
    {
        goto try_again;
    }
    return NULL;
}

/**
 * kmalloc - allocate memory
 * @size: how many bytes of memory are required.
 * @flags: the type of memory to allocate.
 *
 * kmalloc is the normal method of allocating memory
 * in the kernel.
 *
 * The @flags argument may be one of:
 *
 * %GFP_USER - Allocate memory on behalf of user.  May sleep.
 *
 * %GFP_KERNEL - Allocate normal kernel ram.  May sleep.
 *
 * %GFP_ATOMIC - Allocation will not sleep.  Use inside interrupt handlers.
 *
 * Additionally, the %GFP_DMA flag may be set to indicate the memory
 * must be suitable for DMA.  This can mean different things on different
 * platforms.  For example, on i386, it means that the memory must come
 * from the first 16MB.
 */
void *kmalloc(unsigned int size, int flags)
{
    cache_sizes_t *csizep = cache_sizes;

    for (; csizep->cs_size; csizep++)
    {
        if (size > csizep->cs_size)
        {
            continue;
        }
        return kmem_cache_alloc(csizep->cs_cachep, flags);
    }
    return NULL;
}

/**
 * kmem_cache_free - Deallocate an object
 * @cachep: The cache the allocation was from.
 * @objp: The previously allocated object.
 *
 * Free an object which was previously allocated from this
 * cache.
 */
void kmem_cache_free(kmem_cache_t *cachep, void *objp)
{
    slab_t *slabp;

    /* reduces memory footprint
     *
    if (OPTIMIZE(cachep))
        slabp = (void*)((unsigned long)objp&(~(PAGE_SIZE-1)));
     else
     */
    slabp = GET_PAGE_SLAB(virt_to_page((unsigned long)objp));

    {
        unsigned long objnr = ((unsigned long)objp - (unsigned long)slabp->s_mem) / cachep->objsize;

        slab_bufctl(slabp)[objnr] = slabp->free;
        slabp->free = objnr;
    }

    /* fixup slab chains */
    {
        int inuse = slabp->inuse;
        if (!--slabp->inuse)
        {
            /* Was partial or full, now empty. */
            list_del(&slabp->list);
            list_add(&slabp->list, &cachep->slabs_free);
        }
        else if (inuse == cachep->num)
        {
            /* Was full. */
            list_del(&slabp->list);
            list_add(&slabp->list, &cachep->slabs_partial);
        }
    }
}

/**
 * kfree - free previously allocated memory
 * @objp: pointer returned by kmalloc.
 *
 * Don't free memory not originally allocated by kmalloc()
 * or you will run into trouble.
 */
void kfree(const void *objp)
{
    kmem_cache_t *c;
    unsigned long flags;

    if (!objp)
    {
        return;
    }
    c = GET_PAGE_CACHE(virt_to_page((unsigned long)objp));
    kmem_cache_free(c, (void *)objp);
}

kmem_cache_t *kmem_find_general_cachep(unsigned int size, int gfpflags)
{
    cache_sizes_t *csizep = cache_sizes;

    /* This function could be moved to the header file, and
     * made inline so consumers can quickly determine what
     * cache pointer they require.
     */
    for (; csizep->cs_size; csizep++)
    {
        if (size > csizep->cs_size)
        {
            continue;
        }
        break;
    }
    return (csizep->cs_cachep);
}

/**
 * kmem_cache_reap - Reclaim memory from caches.
 * @gfp_mask: the type of memory required.
 *
 * Called from do_try_to_free_pages() and __alloc_pages()
 */
int kmem_cache_reap(int gfp_mask)
{
    slab_t *slabp;
    kmem_cache_t *searchp;
    kmem_cache_t *best_cachep;
    unsigned int best_pages;
    unsigned int best_len;
    unsigned int scan;
    int ret = 0;

    down(&cache_chain_sem);

    scan = REAP_SCANLEN;
    best_len = 0;
    best_pages = 0;
    best_cachep = NULL;
    searchp = clock_searchp;
    do
    {
        unsigned int pages;
        struct list_head *p;
        unsigned int full_free;

        /* It's safe to test this without holding the cache-lock. */
        if (searchp->flags & SLAB_NO_REAP)
        {
            goto next;
        }
        if (searchp->growing)
        {
            goto next_unlock;
        }
        if (searchp->dflags & DFLGS_GROWN)
        {
            searchp->dflags &= ~DFLGS_GROWN;
            goto next_unlock;
        }

        full_free = 0;
        p = searchp->slabs_free.next;
        while (p != &searchp->slabs_free)
        {
            slabp = list_entry(p, slab_t, list);
            full_free++;
            p = p->next;
        }

        /*
         * Try to avoid slabs with constructors and/or
         * more than one page per slab (as it can be difficult
         * to get high orders from gfp()).
         */
        pages = full_free * (1 << searchp->gfporder);
        if (searchp->ctor)
        {
            pages = (pages * 4 + 1) / 5;
        }
        if (searchp->gfporder)
        {
            pages = (pages * 4 + 1) / 5;
        }
        if (pages > best_pages)
        {
            best_cachep = searchp;
            best_len = full_free;
            best_pages = pages;
            if (pages >= REAP_PERFECT)
            {
                clock_searchp = list_entry(searchp->next.next,
                                           kmem_cache_t, next);
                goto perfect;
            }
        }
next_unlock:
next:
        searchp = list_entry(searchp->next.next, kmem_cache_t, next);
    } while (--scan && searchp != clock_searchp);

    clock_searchp = searchp;

    if (!best_cachep)
        /* couldn't find anything to reap */
    {
        goto out;
    }

perfect:
    /* free only 50% of the free slabs */
    best_len = (best_len + 1) / 2;
    for (scan = 0; scan < best_len; scan++)
    {
        struct list_head *p;

        if (best_cachep->growing)
        {
            break;
        }
        p = best_cachep->slabs_free.prev;
        if (p == &best_cachep->slabs_free)
        {
            break;
        }
        slabp = list_entry(p, slab_t, list);
        list_del(&slabp->list);

        /* Safe to drop the lock. The slab is no longer linked to the
         * cache.
         */
        kmem_slab_destroy(best_cachep, slabp);
    }
    ret = scan * (1 << best_cachep->gfporder);
out:
    up(&cache_chain_sem);
    return ret;
}