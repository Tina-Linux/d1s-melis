/*
 * videobuf2-dma-contig.c - DMA contig memory allocator for videobuf2
 *
 * Copyright (C) 2010 Samsung Electronics
 *
 * Author: Pawel Osciak <pawel@osciak.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 */


#include <hal_cache.h>
#include <drivers/sys_fsys.h>

#include "videobuf2-v4l2.h"
#include "videobuf2-dma-contig.h"
#include "videobuf2-memops.h"
//#include "v4l2-types.h"
#include <linux/printk.h>

#include <linux/atomic.h>
#include <linux/device.h>
#include <asm-generic/page.h>

extern unsigned long awos_arch_virt_to_phys(unsigned long virtaddr);
#undef __va_to_pa
#define __va_to_pa(vaddr) awos_arch_virt_to_phys((vaddr))

//#define SUNXI_MEM

struct vb2_dc_buf {
	struct rt_device		*dev;
	void				*vaddr; //virtual address, = cookie
	unsigned long			size; //size of buffer.
	void				*cookie;    //buffer pointer, rt_malloc()
	dma_addr_t			dma_addr;   //physical address.
	unsigned long			attrs;  //DMA_ATTR_NO_KERNEL_MAPPING
	enum dma_data_direction		dma_dir;
	//struct sg_table			*dma_sgt;
	struct frame_vector		*vec;

	/* MMAP related */
	struct vb2_vmarea_handler	handler;
	atomic_t			refcount;
	//struct sg_table			*sgt_base;

	/* DMABUF related */
	struct dma_buf_attachment	*db_attach;
};

/*********************************************/
/*        scatterlist table functions        */
/*********************************************/

#if 0
static unsigned long vb2_dc_get_contiguous_size(struct sg_table *sgt)
{
	struct scatterlist *s;
	dma_addr_t expected = sg_dma_address(sgt->sgl);
	unsigned int i;
	unsigned long size = 0;

	for_each_sg(sgt->sgl, s, sgt->nents, i) {
		if (sg_dma_address(s) != expected)
			break;
		expected = sg_dma_address(s) + sg_dma_len(s);
		size += sg_dma_len(s);
	}
	return 0;
}
#endif

/*********************************************/
/*         callbacks for all buffers         */
/*********************************************/

static void *vb2_dc_cookie(void *buf_priv)
{
	struct vb2_dc_buf *buf = buf_priv;

	return &buf->dma_addr;
}

static void *vb2_dc_vaddr(void *buf_priv)
{
	struct vb2_dc_buf *buf = buf_priv;

#if 0
	if (!buf->vaddr && buf->db_attach)
		buf->vaddr = dma_buf_vmap(buf->db_attach->dmabuf);
#endif
	return buf->vaddr;
}

static unsigned int vb2_dc_num_users(void *buf_priv)
{
	struct vb2_dc_buf *buf = buf_priv;

	return atomic_read(&buf->refcount);
}

static void vb2_dc_prepare(void *buf_priv)
{
	struct vb2_dc_buf *buf = buf_priv;
#if 0
	struct sg_table *sgt = buf->dma_sgt;

	/* DMABUF exporter will flush the cache for us */
	if (!sgt || buf->db_attach)
		return;

	dma_sync_sg_for_device(buf->dev, sgt->sgl, sgt->orig_nents,
		buf->dma_dir);
#endif
	if (buf->dma_dir == DMA_FROM_DEVICE)
	{
		hal_dcache_invalidate((unsigned long)buf->cookie, buf->size);
	}
	else
	{
		hal_dcache_clean((unsigned long)buf->cookie, buf->size);
	}
}

static void vb2_dc_finish(void *buf_priv)
{
	struct vb2_dc_buf *buf = buf_priv;

	/*
	 * struct sg_table *sgt = buf->dma_sgt;
	 *
	 * //DMABUF exporter will flush the cache for us
	 * if (!sgt || buf->db_attach)
	 *         return;
	 *
	 * dma_sync_sg_for_cpu(buf->dev, sgt->sgl, sgt->orig_nents, buf->dma_dir);
	 */

	if (buf->dma_dir != DMA_TO_DEVICE)
	{
		hal_dcache_invalidate((unsigned long)buf->cookie, buf->size);
	}
}

/*********************************************/
/*        callbacks for MMAP buffers         */
/*********************************************/

static void vb2_dc_put(void *buf_priv)
{
	struct vb2_dc_buf *buf = buf_priv;

	if (!atomic_dec_and_test(&buf->refcount))
		return;

	//dma_free_attrs(buf->dev, buf->size, buf->cookie, buf->dma_addr, buf->attrs);
	rt_free(buf->cookie);
	buf->cookie = NULL;
	//put_device(buf->dev);
	rt_free(buf);
}

static void *vb2_dc_alloc(struct rt_device *dev, unsigned long attrs,
			  unsigned long size, enum dma_data_direction dma_dir,
			  gfp_t gfp_flags)
{
	struct vb2_dc_buf *buf;

	if (!dev)
	{
		dev_err(dev, "fatal error dev is null.\n");
		return ERR_PTR(-EINVAL);
	}


	buf  = rt_malloc(sizeof(*buf));
	if (!buf)
		return ERR_PTR(-ENOMEM);

	memset(buf, 0x00, sizeof(*buf));
	if (attrs)
		buf->attrs = attrs;

	/*
	 *buf->cookie = dma_alloc_attrs(dev, size, &buf->dma_addr,
	 *                                GFP_KERNEL | gfp_flags, buf->attrs);
	 */
	buf->cookie = rt_malloc(size);
	if (!buf->cookie) {
		dev_err(dev, "dma_alloc_coherent of size %ld failed\n", size);
		rt_free(buf);
		return ERR_PTR(-ENOMEM);
	}
	buf->dma_addr = (dma_addr_t)__va_to_pa((uint32_t)buf->cookie);
	buf->vaddr = buf->cookie;

	/*
	 *if ((buf->attrs & DMA_ATTR_NO_KERNEL_MAPPING) == 0)
	 *        buf->vaddr = buf->cookie;
	 */

	/* Prevent the device from being released while the buffer is used */
	//buf->dev = get_device(dev);
	buf->size = size;
	buf->dma_dir = dma_dir;

	buf->handler.refcount = &buf->refcount;
	buf->handler.put = vb2_dc_put;
	buf->handler.arg = buf;

	atomic_inc(&buf->refcount);

	return buf;
}

static int vb2_dc_mmap(void *buf_priv, struct map_struct *mp)
{
	struct vb2_dc_buf *buf = buf_priv;
	int ret;

	/*
	 * In rt-thread, all virutal can be accessed.
	 *
	 */
	mp->start = (unsigned long)buf->cookie; 
#if 0
	if (!buf) {
		printk(KERN_ERR "No buffer to map\n");
		return -EINVAL;
	}

	/*
	 * dma_mmap_* uses vm_pgoff as in-buffer offset, but we want to
	 * map whole buffer
	 */
	vma->vm_pgoff = 0;

#ifdef SUNXI_MEM
	ret = ion_mmap_coherent(buf, vma);
#else
	//ret = dma_mmap_attrs(buf->dev, vma, buf->cookie,
	//	buf->dma_addr, buf->size, buf->attrs);
#endif
	if (ret) {
		pr_err("Remapping memory failed, error: %d\n", ret);
		return ret;
	}

	vma->vm_flags		|= VM_DONTEXPAND | VM_DONTDUMP;
	vma->vm_private_data	= &buf->handler;
	vma->vm_ops		= &vb2_common_vm_ops;

	vma->vm_ops->open(vma);

	pr_debug("%s: mapped dma addr 0x%08lx at 0x%08lx, size %ld\n",
		__func__, (unsigned long)buf->dma_addr, vma->vm_start,
		buf->size);
#endif

	return 0;
}

/*********************************************/
/*       callbacks for USERPTR buffers       */
/*********************************************/

static void vb2_dc_put_userptr(void *buf_priv)
{
	struct vb2_dc_buf *buf = buf_priv;
#if 0
	struct sg_table *sgt = buf->dma_sgt;
	int i;
	struct page **pages;

	if (sgt) {
		/*
		 * No need to sync to CPU, it's already synced to the CPU
		 * since the finish() memop will have been called before this.
		 */
		dma_unmap_sg_attrs(buf->dev, sgt->sgl, sgt->orig_nents,
				   buf->dma_dir, DMA_ATTR_SKIP_CPU_SYNC);
		pages = frame_vector_pages(buf->vec);
		/* sgt should exist only if vector contains pages... */
		BUG_ON(IS_ERR(pages));
		for (i = 0; i < frame_vector_count(buf->vec); i++)
			set_page_dirty_lock(pages[i]);
		sg_free_table(sgt);
		rt_free(sgt);
	}
	vb2_destroy_framevec(buf->vec);
#endif
	rt_free(buf);
}

/*
 * For some kind of reserved memory there might be no struct page available,
 * so all that can be done to support such 'pages' is to try to convert
 * pfn to dma address or at the last resort just assume that
 * dma address == physical address (like it has been assumed in earlier version
 * of videobuf2-dma-contig
 */

#ifdef __arch_pfn_to_dma
static inline dma_addr_t vb2_dc_pfn_to_dma(struct rt_device *dev, unsigned long pfn)
{
	return (dma_addr_t)__arch_pfn_to_dma(dev, pfn);
}
#elif defined(__pfn_to_bus)
static inline dma_addr_t vb2_dc_pfn_to_dma(struct rt_device *dev, unsigned long pfn)
{
	return (dma_addr_t)__pfn_to_bus(pfn);
}
#elif defined(__pfn_to_phys)
static inline dma_addr_t vb2_dc_pfn_to_dma(struct rt_device *dev, unsigned long pfn)
{
	return (dma_addr_t)__pfn_to_phys(pfn);
}
#else
static inline dma_addr_t vb2_dc_pfn_to_dma(struct rt_device *dev, unsigned long pfn)
{
	/* really, we cannot do anything better at this point */
	return (dma_addr_t)(pfn) << PAGE_SHIFT;
}
#endif

static void *vb2_dc_get_userptr(struct rt_device *dev, unsigned long vaddr,
	unsigned long size, enum dma_data_direction dma_dir)
{
	struct vb2_dc_buf *buf;
	unsigned long offset;
	int n_pages, i;
	int ret = 0;
#if 0
	struct frame_vector *vec;
	struct sg_table *sgt;
	unsigned long contig_size;
	unsigned long dma_align = dma_get_cache_alignment();

	/* Only cache aligned DMA transfers are reliable */
	if (!IS_ALIGNED(vaddr | size, dma_align)) {
		pr_debug("user data must be aligned to %lu bytes\n", dma_align);
		return ERR_PTR(-EINVAL);
	}
#endif

	if (!size) {
		pr_debug("size is zero\n");
		return ERR_PTR(-EINVAL);
	}

	buf  = rt_malloc(sizeof *buf);
	if (!buf)
		return ERR_PTR(-ENOMEM);

	memset(buf, 0x00, sizeof *buf);
	//buf->dev = dev;
	buf->dma_dir = dma_dir;

#if 0
	offset = vaddr & ~PAGE_MASK;
	vec = vb2_create_framevec(vaddr, size, dma_dir == DMA_FROM_DEVICE);
	if (IS_ERR(vec)) {
		ret = PTR_ERR(vec);
		goto fail_buf;
	}
	buf->vec = vec;
	n_pages = frame_vector_count(vec);
	ret = frame_vector_to_pages(vec);
	if (ret < 0) {
		unsigned long *nums = frame_vector_pfns(vec);

		/*
		 * Failed to convert to pages... Check the memory is physically
		 * contiguous and use direct mapping
		 */
		for (i = 1; i < n_pages; i++)
			if (nums[i-1] + 1 != nums[i])
				goto fail_pfnvec;
		buf->dma_addr = vb2_dc_pfn_to_dma(buf->dev, nums[0]);
		goto out;
	}

	sgt  = rt_malloc(sizeof(*sgt));
	if (!sgt) {
		pr_err("failed to allocate sg table\n");
		ret = -ENOMEM;
		goto fail_pfnvec;
	}

	ret = sg_alloc_table_from_pages(sgt, frame_vector_pages(vec), n_pages,
		offset, size, GFP_KERNEL);
	if (ret) {
		pr_err("failed to initialize sg table\n");
		goto fail_sgt;
	}

	/*
	 * No need to sync to the device, this will happen later when the
	 * prepare() memop is called.
	 */
	sgt->nents = dma_map_sg_attrs(buf->dev, sgt->sgl, sgt->orig_nents,
				      buf->dma_dir, DMA_ATTR_SKIP_CPU_SYNC);
	if (sgt->nents <= 0) {
		pr_err("failed to map scatterlist\n");
		ret = -EIO;
		goto fail_sgt_init;
	}

	contig_size = vb2_dc_get_contiguous_size(sgt);
	if (contig_size < size) {
		pr_err("contiguous mapping is too small %lu/%lu\n",
			contig_size, size);
		ret = -EFAULT;
		goto fail_map_sg;
	}
#endif

	buf->dma_addr = __va_to_pa(vaddr);
	//buf->dma_sgt = NULL;
	buf->size = size;

	return buf;

#if 0
fail_map_sg:
	dma_unmap_sg_attrs(buf->dev, sgt->sgl, sgt->orig_nents,
			   buf->dma_dir, DMA_ATTR_SKIP_CPU_SYNC);

fail_sgt_init:
	sg_free_table(sgt);

fail_sgt:
	rt_free(sgt);

fail_pfnvec:
	vb2_destroy_framevec(vec);

fail_buf:
	rt_free(buf);

	return ERR_PTR(ret);
#endif
}

/*********************************************/
/*       DMA CONTIG exported functions       */
/*********************************************/

const struct vb2_mem_ops vb2_dma_contig_memops = {
	.alloc		= vb2_dc_alloc,
	.put		= vb2_dc_put,
	.cookie		= vb2_dc_cookie,
	.vaddr		= vb2_dc_vaddr,
	.mmap		= vb2_dc_mmap,
	.get_userptr	= vb2_dc_get_userptr,
	.put_userptr	= vb2_dc_put_userptr,
	.prepare	= vb2_dc_prepare,
	.finish		= vb2_dc_finish,
	.num_users	= vb2_dc_num_users,
};

/**
 * vb2_dma_contig_set_max_seg_size() - configure DMA max segment size
 * @dev:	device for configuring DMA parameters
 * @size:	size of DMA max segment size to set
 *
 * To allow mapping the scatter-list into a single chunk in the DMA
 * address space, the device is required to have the DMA max segment
 * size parameter set to a value larger than the buffer size. Otherwise,
 * the DMA-mapping subsystem will split the mapping into max segment
 * size chunks. This function sets the DMA max segment size
 * parameter to let DMA-mapping map a buffer as a single chunk in DMA
 * address space.
 * This code assumes that the DMA-mapping subsystem will merge all
 * scatterlist segments if this is really possible (for example when
 * an IOMMU is available and enabled).
 * Ideally, this parameter should be set by the generic bus code, but it
 * is left with the default 64KiB value due to historical litmiations in
 * other subsystems (like limited USB host drivers) and there no good
 * place to set it to the proper value.
 * This function should be called from the drivers, which are known to
 * operate on platforms with IOMMU and provide access to shared buffers
 * (either USERPTR or DMABUF). This should be done before initializing
 * videobuf2 queue.
 */
int vb2_dma_contig_set_max_seg_size(struct rt_device *dev, unsigned int size)
{
#if 0
	if (!dev->dma_parms) {
		dev->dma_parms  = rt_malloc(sizeof(*dev->dma_parms));
		if (!dev->dma_parms)
			return -ENOMEM;
	}
	if (dma_get_max_seg_size(dev) < size)
		return dma_set_max_seg_size(dev, size);
#endif
	return 0;
}

/*
 * vb2_dma_contig_clear_max_seg_size() - release resources for DMA parameters
 * @dev:	device for configuring DMA parameters
 *
 * This function releases resources allocated to configure DMA parameters
 * (see vb2_dma_contig_set_max_seg_size() function). It should be called from
 * device drivers on driver remove.
 */
void vb2_dma_contig_clear_max_seg_size(struct rt_device *dev)
{
#if 0
	rt_free(dev->dma_parms);
	dev->dma_parms = NULL;
#endif
}

