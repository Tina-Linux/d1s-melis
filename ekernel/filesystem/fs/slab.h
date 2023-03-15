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
#ifndef __SLAB_H__
#define __SLAB_H__

#define L1_CACHE_SHIFT 5
#define L1_CACHE_BYTES 32
#define L1_CACHE_ALIGN(x) FSYS_ALIGN(x, L1_CACHE_BYTES)

typedef struct kmem_cache_s kmem_cache_t;

/* flags for kmem_cache_alloc() */
#define SLAB_NOFS       GFP_NOFS
#define SLAB_NOIO       GFP_NOIO
#define SLAB_NOHIGHIO       GFP_NOHIGHIO
#define SLAB_ATOMIC     GFP_ATOMIC
#define SLAB_USER       GFP_USER
#define SLAB_KERNEL     GFP_KERNEL
#define SLAB_NFS        GFP_NFS
#define SLAB_DMA        GFP_DMA

#define SLAB_LEVEL_MASK     (__GFP_WAIT|__GFP_HIGH|__GFP_IO|__GFP_HIGHIO|__GFP_FS)
#define SLAB_NO_GROW        0x00001000UL    /* don't grow a cache */

/* flags to pass to kmem_cache_create().
 * The first 3 are only valid when the allocator as been build
 * SLAB_DEBUG_SUPPORT.
 */
#define SLAB_DEBUG_FREE     0x00000100UL    /* Peform (expensive) checks on free */
#define SLAB_DEBUG_INITIAL  0x00000200UL    /* Call constructor (as verifier) */
#define SLAB_RED_ZONE       0x00000400UL    /* Red zone objs in a cache */
#define SLAB_POISON     0x00000800UL    /* Poison objects */
#define SLAB_NO_REAP        0x00001000UL    /* never reap from the cache */
#define SLAB_HWCACHE_ALIGN  0x00002000UL    /* align objs on a h/w cache lines */
#define SLAB_CACHE_DMA      0x00004000UL    /* use GFP_DMA memory */

/* flags passed to a constructor func */
#define SLAB_CTOR_CONSTRUCTOR   0x001UL     /* if not set, then deconstructor */
#define SLAB_CTOR_ATOMIC    0x002UL     /* tell constructor it can't sleep */
#define SLAB_CTOR_VERIFY    0x004UL     /* tell constructor it's a verify call */

/* prototypes */
extern int kmem_cache_init(void);
extern int kmem_cache_sizes_init(void);

extern kmem_cache_t *kmem_find_general_cachep(unsigned int, int gfpflags);
extern kmem_cache_t *kmem_cache_create(const char *, unsigned int, unsigned int, unsigned long,
                                       void (*)(void *, kmem_cache_t *, unsigned long),
                                       void (*)(void *, kmem_cache_t *, unsigned long));
extern int kmem_cache_destroy(kmem_cache_t *);
extern int kmem_cache_shrink(kmem_cache_t *);
extern void *kmem_cache_alloc(kmem_cache_t *, int);
extern void kmem_cache_free(kmem_cache_t *, void *);

extern void *kmalloc(unsigned int, int);
extern void kfree(const void *);

extern int kmem_cache_reap(int);

/* System wide caches */
//extern kmem_cache_t   *vm_area_cachep;
//extern kmem_cache_t   *mm_cachep;
//extern kmem_cache_t   *names_cachep;
//extern kmem_cache_t   *files_cachep;
//extern kmem_cache_t   *filp_cachep;
//extern kmem_cache_t   *dquot_cachep;
//extern kmem_cache_t   *bh_cachep;
//extern kmem_cache_t   *fs_cachep;
//extern kmem_cache_t   *sigact_cachep;

#endif  /* __SLAB_H__ */