/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : page_pool.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: page pool manager for file system, code is extracted from linux.
* Update  : date                auther      ver     notes
*           2011-3-15 15:54:54  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __PAGE_POOL_H__
#define __PAGE_POOL_H__

#include "list.h"
#include "supports.h"
#include "atomic.h"

/*
 * Each physical page in the system has a struct page associated with
 * it to keep track of whatever it is we are using the page for at the
 * moment. Note that we have no way to track which tasks are using
 * a page.
 *
 * Try to keep the most commonly accessed fields in single cache lines
 * here (16 bytes or greater).  This ordering should be particularly
 * beneficial on 32-bit processors.
 *
 * The first line is data used in page cache lookup, the second line
 * is used for linear searches (eg. clock algorithm scans).
 *
 * TODO: make this structure smaller, it could be as small as 32 bytes.
 */
typedef struct page
{
    struct list_head list;      /* ->mapping has some page lists. */
    struct address_space *mapping;  /* The inode (or ...) we belong to. */
    unsigned long index;        /* Our offset within mapping. */
    struct page *next_hash;     /* Next page sharing our hash bucket in
                       the pagecache hash table. */
    atomic_t count;         /* Usage count, see below. */
    unsigned long flags;        /* atomic flags, some possibly
                       updated asynchronously */
    struct list_head lru;       /* Pageout list, eg. active_list;
                       protected by pagemap_lru_lock !! */
    struct page **pprev_hash;   /* Complement to *next_hash. */
    struct buffer_head *buffers;    /* Buffer maps us to a disk block. */
    char *virtual;          /* Kernel virtual address (NULL if
                       not kmapped, ie. highmem) */
} mem_map_t;

typedef struct free_area_struct
{
    struct list_head    free_list;
    unsigned long       *map;
} free_area_t;

#define page_address(page) ((page)->virtual)

/*
 * The page cache can done in larger chunks than
 * one page, because it allows for more efficient
 * throughput (it can then be mapped into user
 * space in smaller chunks for same flexibility).
 *
 * Or rather, it _will_ be done in larger chunks.
 */
#define PAGE_SHIFT  12
#define PAGE_SIZE   (1UL << PAGE_SHIFT)
#define PAGE_MASK   (~(PAGE_SIZE-1))
#define PAGE_CACHE_SHIFT    PAGE_SHIFT
#define PAGE_CACHE_SIZE     PAGE_SIZE
#define PAGE_CACHE_MASK     PAGE_MASK
#define PAGE_CACHE_ALIGN(addr)  (((addr)+PAGE_CACHE_SIZE-1)&PAGE_CACHE_MASK)
#define SWAP_CLUSTER_MAX    8
#define NR_TOTAL_PAGE     64//  256
#define MAX_ORDER           2

typedef struct mem_map_info_s
{
    free_area_t free_area[MAX_ORDER];
    int free_pages;
    int pages_min;
    int pages_low;
    int pages_high;
} mem_map_info_t;

#define DEF_PRIORITY (6)
/*
 * The maximum number of pages to writeout in a single bdflush/kupdate
 * operation.  We do this so we don't hold I_LOCK against an inode for
 * enormous amounts of time, which would block a userspace task which has
 * been forced to throttle against that inode.  Also, the code reevaluates
 * the dirty each time it has written this many pages.
 */
#define MAX_WRITEBACK_PAGES 1024

#define page_cache_get(x)   get_page(x)
#define page_cache_release(x)   __free_page(x)
#define __get_free_page(gfp_mask) __get_free_pages((gfp_mask),0)
#define __free_page(page) __free_pages((page), 0)
#define free_page(addr) free_pages(((unsigned long)addr),0)


/*
 * Methods to modify the page usage count.
 *
 * What counts for a page usage:
 * - cache mapping   (page->mapping)
 * - disk mapping    (page->buffers)
 * - page mapped in a task's page tables, each mapping
 *   is counted separately
 *
 * Also, many kernel routines increase the page count before a critical
 * routine so they can be sure the page doesn't go away from under them.
 */
#define get_page(p)     atomic_inc(&(p)->count)
#define put_page(p)     __free_page(p)
#define put_page_testzero(p)    atomic_dec_and_test(&(p)->count)
#define page_count(p)       atomic_read(&(p)->count)
#define set_page_count(p,v)  atomic_set(&(p)->count, v)

/*
 * Various page->flags bits:
 *
 * PG_reserved is set for special pages, which can never be swapped
 * out. Some of them might not even exist (eg empty_bad_page)...
 *
 * Multiple processes may "see" the same page. E.g. for untouched
 * mappings of /dev/null, all processes see the same page full of
 * zeroes, and text pages of executables and shared libraries have
 * only one copy in memory, at most, normally.
 *
 * For the non-reserved pages, page->count denotes a reference count.
 *   page->count == 0 means the page is free.
 *   page->count == 1 means the page is used for exactly one purpose
 *   (e.g. a private data page of one process).
 *
 * A page may be used for kmalloc() or anyone else who does a
 * __get_free_page(). In this case the page->count is at least 1, and
 * all other fields are unused but should be 0 or NULL. The
 * management of this page is the responsibility of the one who uses
 * it.
 *
 * The other pages (we may call them "process pages") are completely
 * managed by the Linux memory manager: I/O, buffers, swapping etc.
 * The following discussion applies only to them.
 *
 * A page may belong to an inode's memory mapping. In this case,
 * page->mapping is the pointer to the inode, and page->index is the
 * file offset of the page, in units of PAGE_CACHE_SIZE.
 *
 * A page may have buffers allocated to it. In this case,
 * page->buffers is a circular list of these buffer heads. Else,
 * page->buffers == NULL.
 *
 * For pages belonging to inodes, the page->count is the number of
 * attaches, plus 1 if buffers are allocated to the page, plus one
 * for the page cache itself.
 *
 * All pages belonging to an inode are in these doubly linked lists:
 * mapping->clean_pages, mapping->dirty_pages and mapping->locked_pages;
 * using the page->list list_head. These fields are also used for
 * freelist managemet (when page->count==0).
 *
 * There is also a hash table mapping (mapping,index) to the page
 * in memory if present. The lists for this hash table use the fields
 * page->next_hash and page->pprev_hash.
 *
 * All process pages can do I/O:
 * - inode pages may need to be read from disk,
 * - inode pages which have been modified and are MAP_SHARED may need
 *   to be written to disk,
 * - private pages which have been modified may need to be swapped out
 *   to swap space and (later) to be read back into memory.
 * During disk I/O, PG_locked is used. This bit is set before I/O
 * and reset when I/O completes. page->wait is a wait queue of all
 * tasks waiting for the I/O on this page to complete.
 * PG_uptodate tells whether the page's contents is valid.
 * When a read completes, the page becomes uptodate, unless a disk I/O
 * error happened.
 *
 * For choosing which pages to swap out, inode pages carry a
 * PG_referenced bit, which is set any time the system accesses
 * that page through the (mapping,index) hash table. This referenced
 * bit, together with the referenced bit in the page tables, is used
 * to manipulate page->age and move the page across the active,
 * inactive_dirty and inactive_clean lists.
 *
 * Note that the referenced bit, the page->lru list_head and the
 * active, inactive_dirty and inactive_clean lists are protected by
 * the pagemap_lru_lock, and *NOT* by the usual PG_locked bit!
 *
 * PG_skip is used on sparc/sparc64 architectures to "skip" certain
 * parts of the address space.
 *
 * PG_error is set to indicate that an I/O error occurred on this page.
 *
 * PG_arch_1 is an architecture specific page state bit.  The generic
 * code guarantees that this bit is cleared for a page when it first
 * is entered into the page cache.
 *
 * PG_highmem pages are not permanently mapped into the kernel virtual
 * address space, they need to be kmapped separately for doing IO on
 * the pages. The struct page (these bits with information) are always
 * mapped into kernel address space...
 */
#define PG_locked        0  /* Page is locked. Don't touch. */
#define PG_error         1
#define PG_referenced        2
#define PG_uptodate      3
#define PG_dirty         4
#define PG_decr_after        5
#define PG_active        6
#define PG_inactive      7
#define PG_slab          8
#define PG_skip         10
#define PG_highmem      11
#define PG_checked      12  /* kill me in 2.5.<early>. */
#define PG_arch_1       13
#define PG_reserved     14

/* Make it prettier to test the above... */
#define UnlockPage(page)    clear_bit(PG_locked, &(page)->flags)
#define Page_Uptodate(page) test_bit(PG_uptodate, &(page)->flags)
#define SetPageUptodate(page)   set_bit(PG_uptodate, &(page)->flags)
#define ClearPageUptodate(page) clear_bit(PG_uptodate, &(page)->flags)
#define PageDirty(page)     test_bit(PG_dirty, &(page)->flags)
#define SetPageDirty(page)  set_bit(PG_dirty, &(page)->flags)
#define ClearPageDirty(page)    clear_bit(PG_dirty, &(page)->flags)
#define PageLocked(page)    test_bit(PG_locked, &(page)->flags)
#define LockPage(page)      set_bit(PG_locked, &(page)->flags)
#define TryLockPage(page)   test_and_set_bit(PG_locked, &(page)->flags)
#define PageChecked(page)   test_bit(PG_checked, &(page)->flags)
#define SetPageChecked(page)    set_bit(PG_checked, &(page)->flags)

extern void __set_page_dirty(struct page *);

static inline void set_page_dirty(struct page *page)
{
    if (!test_and_set_bit(PG_dirty, &page->flags))
    {
        __set_page_dirty(page);
    }
}

#define PageError(page)     test_bit(PG_error, &(page)->flags)
#define SetPageError(page)  set_bit(PG_error, &(page)->flags)
#define ClearPageError(page)    clear_bit(PG_error, &(page)->flags)
#define PageReferenced(page)    test_bit(PG_referenced, &(page)->flags)
#define SetPageReferenced(page) set_bit(PG_referenced, &(page)->flags)
#define ClearPageReferenced(page)   clear_bit(PG_referenced, &(page)->flags)
#define PageTestandClearReferenced(page)    test_and_clear_bit(PG_referenced, &(page)->flags)
#define PageDecrAfter(page) test_bit(PG_decr_after, &(page)->flags)
#define SetPageDecrAfter(page)  set_bit(PG_decr_after, &(page)->flags)
#define PageTestandClearDecrAfter(page) test_and_clear_bit(PG_decr_after, &(page)->flags)
#define PageSlab(page)      test_bit(PG_slab, &(page)->flags)
#define PageSetSlab(page)   set_bit(PG_slab, &(page)->flags)
#define PageClearSlab(page) clear_bit(PG_slab, &(page)->flags)
//#define PageReserved(page)    test_bit(PG_reserved, &(page)->flags)

#define PageActive(page)    test_bit(PG_active, &(page)->flags)
#define SetPageActive(page) set_bit(PG_active, &(page)->flags)
#define ClearPageActive(page)   clear_bit(PG_active, &(page)->flags)
#define TestandSetPageActive(page)  test_and_set_bit(PG_active, &(page)->flags)
#define TestandClearPageActive(page)    test_and_clear_bit(PG_active, &(page)->flags)

#define PageInactive(page)  test_bit(PG_inactive, &(page)->flags)
#define SetPageInactive(page)   set_bit(PG_inactive, &(page)->flags)
#define ClearPageInactive(page) clear_bit(PG_inactive, &(page)->flags)
#define TestandSetPageInactive(page)    test_and_set_bit(PG_inactive, &(page)->flags)
#define TestandClearPageInactive(page)  test_and_clear_bit(PG_inactive, &(page)->flags)

#ifdef CONFIG_HIGHMEM
#define PageHighMem(page)       test_bit(PG_highmem, &(page)->flags)
#else
#define PageHighMem(page)       0 /* needed to optimize away at compile time */
#endif

//#define SetPageReserved(page)     set_bit(PG_reserved, &(page)->flags)
//#define ClearPageReserved(page)       clear_bit(PG_reserved, &(page)->flags)

/*
 * GFP bitmasks..
 */
/* Zone modifiers in GFP_ZONEMASK (see linux/mmzone.h - low four bits) */
#define __GFP_DMA   0x01
#define __GFP_HIGHMEM   0x02

/* Action modifiers - doesn't change the zoning */
#define __GFP_WAIT  0x10    /* Can wait and reschedule? */
#define __GFP_HIGH  0x20    /* Should access emergency pools? */
#define __GFP_IO    0x40    /* Can start low memory physical IO? */
#define __GFP_HIGHIO    0x80    /* Can start high mem physical IO? */
#define __GFP_FS    0x100   /* Can call down to low-level FS? */

#define GFP_NOHIGHIO    (__GFP_HIGH | __GFP_WAIT | __GFP_IO)
#define GFP_NOIO    (__GFP_HIGH | __GFP_WAIT)
#define GFP_NOFS    (__GFP_HIGH | __GFP_WAIT | __GFP_IO | __GFP_HIGHIO)
#define GFP_ATOMIC  (__GFP_HIGH)
#define GFP_USER    (             __GFP_WAIT | __GFP_IO | __GFP_HIGHIO | __GFP_FS)
#define GFP_HIGHUSER    (             __GFP_WAIT | __GFP_IO | __GFP_HIGHIO | __GFP_FS | __GFP_HIGHMEM)
#define GFP_KERNEL  (__GFP_HIGH | __GFP_WAIT | __GFP_IO | __GFP_HIGHIO | __GFP_FS)
#define GFP_NFS     (__GFP_HIGH | __GFP_WAIT | __GFP_IO | __GFP_HIGHIO | __GFP_FS)
#define GFP_KSWAPD  (             __GFP_WAIT | __GFP_IO | __GFP_HIGHIO | __GFP_FS)

#define memlist_init(x) INIT_LIST_HEAD(x)
#define memlist_add_head list_add
#define memlist_add_tail list_add_tail
#define memlist_del list_del
#define memlist_entry list_entry
#define memlist_next(x) ((x)->next)
#define memlist_prev(x) ((x)->prev)

/* Flag - indicates that the buffer will be suitable for DMA.  Ignored on some
   platforms, used as appropriate on others */

#define GFP_DMA     __GFP_DMA

/*
 * List add/del helper macros. These must be called
 * with the pagemap_lru_lock held!
 */
#define DEBUG_LRU_PAGE(page)            \
    do {                        \
        if (PageActive(page))           \
            BUG();              \
        if (PageInactive(page))         \
            BUG();              \
        if (page_count(page) == 0)      \
            BUG();              \
    } while (0)

#define add_page_to_active_list(page)       \
    do {                        \
        DEBUG_LRU_PAGE(page);           \
        SetPageActive(page);            \
        list_add(&(page)->lru, &active_list);   \
        nr_active_pages++;          \
    } while (0)

#define add_page_to_inactive_list(page)     \
    do {                        \
        DEBUG_LRU_PAGE(page);           \
        SetPageInactive(page);      \
        list_add(&(page)->lru, &inactive_list); \
        nr_inactive_pages++;            \
    } while (0)

#define del_page_from_active_list(page)     \
    do {                        \
        list_del(&(page)->lru);         \
        ClearPageActive(page);          \
        nr_active_pages--;          \
        DEBUG_LRU_PAGE(page);           \
    } while (0)

#define del_page_from_inactive_list(page)   \
    do {                        \
        list_del(&(page)->lru);         \
        ClearPageInactive(page);        \
        nr_inactive_pages--;            \
        DEBUG_LRU_PAGE(page);           \
    } while (0)


unsigned int nr_free_buffer_pages(void);
struct page *alloc_pages(unsigned int, int);
struct page *alloc_page(unsigned int);
unsigned long __get_free_pages(unsigned int, unsigned int);
void free_pages(unsigned long, unsigned int);
void __free_pages(struct page *, unsigned int);

void lru_cache_add(struct page *);
void __lru_cache_del(struct page *);
void lru_cache_del(struct page *);

void deactivate_page(struct page *);
void deactivate_page_nolock(struct page *);
void activate_page(struct page *);
void activate_page_nolock(struct page *);

void mark_page_accessed(struct buffer_head *);
int page_pool_ini(void);
int mem_map_index(struct page *);
struct page *virt_to_page(unsigned long);

#endif  /* __PAGE_POOL_H__ */
