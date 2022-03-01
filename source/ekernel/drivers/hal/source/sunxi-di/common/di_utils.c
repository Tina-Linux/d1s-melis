/*
 * Copyright (c) 2007-2018 Allwinnertech Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include "di_utils.h"
#include "di_debug.h"

#include <hal_sem.h>
#include <hal_dma.h>
#include <stdlib.h>
#include "sunxi_hal_common.h"
#include "../include/drm_fourcc.h"

static struct info_mem di_mem[MAX_DI_MEM_INDEX];
void *di_malloc(__u32 bytes_num, uintptr_t *phy_addr)
{
	void *address = NULL;

	if (bytes_num != 0) {
		address = dma_alloc_coherent(bytes_num);
		if (address) {
			DI_INFO("malloc ok, address=0x%p, size=0x%x\n",
			    (void *)(*(unsigned long *)phy_addr), bytes_num);
			*phy_addr = __va_to_pa((unsigned long)address);
			return address;
		}

		DI_ERR("dma_alloc_coherent fail, size=0x%x\n", bytes_num);
		return NULL;
	}
	DI_ERR("%s size is zero\n", __func__);

	return NULL;
}

void di_free(void *virt_addr, void *phy_addr, unsigned int size)
{
	if (phy_addr && virt_addr)
		dma_free_coherent(virt_addr);

}

__s32 di_get_free_mem_index(void)
{
	__u32 i = 0;

	for (i = 0; i < MAX_DI_MEM_INDEX; i++) {
		if (di_mem[i].b_used == 0)
			return i;
	}
	return -1;
}

int di_mem_request(__u32 size, u64 *phyaddr)
{
	__s32 sel;
	unsigned long ret = 0;
	uintptr_t phy_addr;

	sel = di_get_free_mem_index();
	if (sel < 0) {
		DI_ERR("di_get_free_mem_index fail!\n");
		return -DI_EINVAL;
	}

	ret = (unsigned long)di_malloc(size, &phy_addr);
	if (ret != 0) {
		di_mem[sel].virt_addr = (void *)ret;
		memset(di_mem[sel].virt_addr, 0, size);
		di_mem[sel].phy_addr = phy_addr;
		di_mem[sel].mem_len = size;
		di_mem[sel].b_used = 1;
		*phyaddr = phy_addr;

		DI_INFO("map_di_memory[%d]: pa=%08lx va=%p size:%x\n", sel,
		     di_mem[sel].phy_addr, di_mem[sel].virt_addr, size);
		return sel;
	}
	DI_ERR("fail to alloc reserved memory!\n");
	return -DI_ENOMEM;
}

int di_mem_release(__u32 sel)
{
	if (di_mem[sel].b_used == 0) {
		DI_ERR("mem not used in di_mem_release,%d\n", sel);
		return -DI_EINVAL;
	}

	di_free((void *)di_mem[sel].virt_addr, (void *)di_mem[sel].phy_addr,
		 di_mem[sel].mem_len);
	memset(&di_mem[sel], 0, sizeof(struct info_mem));

	return 0;
}

struct di_mapped_buf *di_dma_buf_alloc_map(u32 size)
{
	struct di_mapped_buf *mbuf =
		hal_malloc(sizeof(*mbuf));

	if (mbuf == NULL) {
		DI_ERR("kzalloc for mapped buf fail, size=%d\n",
			(u32)sizeof(*mbuf));
		return NULL;
	}

	mbuf->size_request = size;
	mbuf->size_alloced = DI_ALIGN(size, 2);
	mbuf->vir_addr = dma_alloc_coherent(mbuf->size_alloced);
	if (mbuf->vir_addr == NULL) {
		DI_ERR("dma_alloc_coherent fail, size=0x%x->0x%x\n",
			mbuf->size_request, mbuf->size_alloced);
		hal_free(mbuf);
		return NULL;
	}

	DI_DEBUG("%s: addr[%p], size=0x%x\n", __func__,
		mbuf->vir_addr, mbuf->size_alloced);

	return mbuf;
}

void di_dma_buf_unmap_free(struct di_mapped_buf *mbuf)
{
	DI_DEBUG("%s: addr[%p], size=0x%x\n", __func__,
		mbuf->vir_addr, mbuf->size_alloced);
	dma_free_coherent(mbuf->vir_addr);
	hal_free(mbuf);
}

const char *di_format_to_string(u32 format)
{
	switch (format) {
	case DRM_FORMAT_YUV420:
		return "YU12";
	case DRM_FORMAT_YVU420:
		return "YV12";
	case DRM_FORMAT_YUV422:
		return "YU16";
	case DRM_FORMAT_YVU422:
		return "YV16";
	case DRM_FORMAT_NV12:
		return "NV12";
	case DRM_FORMAT_NV21:
		return "NV21";
	case DRM_FORMAT_NV16:
		return "NV16";
	case DRM_FORMAT_NV61:
		return "NV61";
	default:
		DI_INFO("%s format(0x%x)\n", __func__, format);
		return "unknown format";
	}
}

u32 di_format_get_planar_num(u32 format)
{
	switch (format) {
	case DRM_FORMAT_YUV420:
	case DRM_FORMAT_YVU420:
	case DRM_FORMAT_YUV422:
	case DRM_FORMAT_YVU422:
		return 3;
	case DRM_FORMAT_NV12:
	case DRM_FORMAT_NV21:
	case DRM_FORMAT_NV16:
	case DRM_FORMAT_NV61:
		return 2;
	default:
		DI_INFO("%s:%s\n", __func__,
			di_format_to_string(format));
		return 1;
	}
}

u32 di_format_get_uvseq(u32 format)
{
	switch (format) {
	case DRM_FORMAT_NV21:
	case DRM_FORMAT_NV61:
	case DRM_FORMAT_YVU420:
	case DRM_FORMAT_YVU422:
		/* TODO: add more format to support */
		return DI_UVSEQ_VU;
	default:
		return DI_UVSEQ_UV;
	}
}
