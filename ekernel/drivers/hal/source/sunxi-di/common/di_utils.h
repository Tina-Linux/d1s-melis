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

#ifndef _DI_UTILS_H_
#define _DI_UTILS_H_

#include <typedef.h>
#include <hal_dma.h>
#include <hal_mem.h>
#include <hal_osal.h>

#include <string.h>

#define TAG "[DI_UTILS]"

#define DI_ALIGN(value, align) ((align == 0) ? \
				value : \
				(((value) + ((align) - 1)) & ~((align) - 1)))

#define MAX_DI_MEM_INDEX       100

#define DI_TASK_CNT_MAX DI_CLIENT_CNT_MAX

#define DI_EOK                          0               /**< There is no error */
#define DI_ERROR                        1               /**< A generic error happens */
#define DI_ETIMEOUT                     2               /**< Timed out */
#define DI_EFULL                        3               /**< The resource is full */
#define DI_EEMPTY                       4               /**< The resource is empty */
#define DI_ENOMEM                       5               /**< No memory */
#define DI_ENOSYS                       6               /**< No system */
#define DI_EBUSY                        7               /**< Busy */
#define DI_EIO                          8               /**< IO error */
#define DI_EINTR                        9               /**< Interrupted system call */
#define DI_EINVAL                       10              /**< Invalid argument */

struct info_mem {
	unsigned long phy_addr;
	void *virt_addr;
	__u32 b_used;
	__u32 mem_len;
};

struct di_dma_item {
	struct dma_buf *buf;
	struct dma_buf_attachment *attach;
	struct sg_table *sgt_org;
	struct sg_table *sgt_bak;
};

struct di_mapped_buf {
	u32 size_request;
	u32 size_alloced;
	void *vir_addr;
};

int di_mem_release(__u32 sel);
int di_mem_request(__u32 size, u64 *phyaddr);

struct di_dma_item *di_dma_buf_self_map(
	int fd);
void di_dma_buf_self_unmap(struct di_dma_item *item);

struct di_mapped_buf *di_dma_buf_alloc_map(u32 size);
void di_dma_buf_unmap_free(struct di_mapped_buf *mbuf);


enum {
	DI_UVSEQ_UV = 0, /* uv-combined: LSB U0V0U1V1 MSB */
	DI_UVSEQ_VU = 1, /* uv-combined: LSB V0U0V1U1 MSB */
};

const char *di_format_to_string(u32 format);
u32 di_format_get_planar_num(u32 format);
u32 di_format_get_uvseq(u32 format);

#endif /* #ifndef _DI_UTILS_H_ */
