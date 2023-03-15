/*
 * ekernel/components/avframework/v4l2/platform/sunxi-vin/utility/vin_os.c
 *
 * Copyright (c) 2007-2017 Allwinnertech Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <hal_cache.h>
#include <hal_mem.h>
#include <hal_gpio.h>
#include "vin_os.h"

/*#include <drivers/sys_fsys.h>*/
#include <errno.h>
#include <string.h>

unsigned int vin_log_mask;

int os_gpio_set(struct gpio_config *gc)
{
#ifndef FPGA_VER
	char pin_name[32];
	__u32 config;

	if (gc == NULL)
		return -1;
	if (gc->gpio == GPIO_INDEX_INVALID)
		return -1;

	hal_gpio_pinmux_set_function(gc->gpio, gc->mul_sel);
	if (gc->pull != GPIO_PULL_DEFAULT)
		hal_gpio_set_pull(gc->gpio, gc->pull);

	if (gc->drv_level != GPIO_DRVLVL_DEFAULT)
		hal_gpio_set_driving_level(gc->gpio, gc->drv_level);

	if (gc->data != GPIO_DATA_DEFAULT)
		hal_gpio_set_data(gc->gpio, gc->data);
#endif
	return 0;
}

int os_gpio_write(u32 gpio, __u32 out_value, int force_value_flag)
{
#ifndef FPGA_VER
	if (gpio == GPIO_INDEX_INVALID)
		return 0;

	if (force_value_flag == 1) {
		hal_gpio_set_direction(gpio, 1); // 1 - outupt, 0 - input
		hal_gpio_set_data(gpio, out_value);
	} else {
		if (out_value == 0) {
			hal_gpio_set_direction(gpio, 1); // 1 - outupt, 0 - input
			hal_gpio_set_data(gpio, out_value);
		} else {
			hal_gpio_set_direction(gpio, 0); // 1 - outupt, 0 - input
		}
	}
#endif
	return 0;
}


extern unsigned long dma_coherent_alloc(unsigned long *virt_addr, unsigned long size);
int os_mem_alloc(struct vin_mm *mem_man)
{
#define CSI_ALIGN_SIZE (0x40)

	unsigned long ptr = 0;
	unsigned long ptr_phy = 0;
	int uintptr_size = 0;
	int align = CSI_ALIGN_SIZE;
	rt_size_t align_size = 0;

	uintptr_size = sizeof(void *);
	uintptr_size -= 1;

	/* align the alignment size to uintptr size byte */
	align = ((align + uintptr_size) & ~uintptr_size);

	/* get total aligned size */
	align_size = ((mem_man->size + uintptr_size) & ~uintptr_size) + align;

	/* allocate memory block from heap */
//	ptr_phy = dma_coherent_alloc(&ptr, align_size);
	if (ptr_phy == 0 || ptr == 0) {
		vin_err("malloc %d failed.", mem_man->size);
		return -ENOMEM;
	}

	memset((void *)ptr, 0, align_size);
	//hal_dcache_clean((unsigned long)ptr, align_size);

	/* the allocated memory block is aligned */
	if (((rt_ubase_t)ptr & (align - 1)) == 0) {
		mem_man->vir_addr = (void *)(ptr + align);
		mem_man->phy_addr = (void *)(ptr_phy + align);
		mem_man->dma_addr = mem_man->phy_addr;
	} else {
		mem_man->vir_addr = (void *)((ptr + (align - 1)) & ~(align - 1));
		mem_man->phy_addr = (void *)((ptr_phy + (align - 1)) & ~(align - 1));
		mem_man->dma_addr = mem_man->phy_addr;
	}

	/* set the pointer before alignment pointer to the real pointer */
	*((rt_ubase_t *)((rt_ubase_t)mem_man->vir_addr - sizeof(void *))) = (rt_ubase_t)ptr;

	vin_print("[%s]: ptr: 0x%lx, align_size: 0x%lx\n", __func__, ptr, align_size);
	vin_print("[%s]: vaddr: %p, paddr: %p, size: 0x%x\n", __func__, mem_man->vir_addr, mem_man->phy_addr, mem_man->size);

	return 0;
}

extern void dma_coherent_free(unsigned long virt_addr);
void os_mem_free(struct vin_mm *mem_man)
{
	unsigned long real_ptr = 0;

	if (mem_man->vir_addr) {
		real_ptr = * (rt_ubase_t *)((rt_ubase_t)mem_man->vir_addr - sizeof(void *));
//		dma_coherent_free(real_ptr);
		vin_print("[%s]: real_ptr: 0x%lx, vaddr: %p, paddr: %p, size: 0x%x \n", __func__, real_ptr, mem_man->vir_addr, mem_man->phy_addr, mem_man->size);
	}

	mem_man->phy_addr = NULL;
	mem_man->dma_addr = NULL;
	mem_man->vir_addr = NULL;
}
