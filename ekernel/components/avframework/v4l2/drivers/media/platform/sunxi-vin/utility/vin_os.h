/*
 * ekernel/components/avframework/v4l2/platform/sunxi-vin/utility/vin_os.h
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

#ifndef __VIN__OS__H__
#define __VIN__OS__H__

#include <stdio.h>
//#include <v4l2-types.h>
#include <melis/typedef.h>
#include <melis/log.h>

#include "../platform/platform_cfg.h"

#define IS_FLAG(x, y) (((x)&(y)) == y)

#define VIN_LOG_MD				(1 << 0)	/*0x1 */
#define VIN_LOG_FLASH				(1 << 1)	/*0x2 */
#define VIN_LOG_CCI				(1 << 2)	/*0x4 */
#define VIN_LOG_CSI				(1 << 3)	/*0x8 */
#define VIN_LOG_MIPI				(1 << 4)	/*0x10*/
#define VIN_LOG_ISP				(1 << 5)	/*0x20*/
#define VIN_LOG_STAT				(1 << 6)	/*0x40*/
#define VIN_LOG_SCALER				(1 << 7)	/*0x80*/
#define VIN_LOG_POWER				(1 << 8)	/*0x100*/
#define VIN_LOG_CONFIG				(1 << 9)	/*0x200*/
#define VIN_LOG_VIDEO				(1 << 10)	/*0x400*/
#define VIN_LOG_FMT				(1 << 11)	/*0x800*/
#define VIN_LOG_TDM				(1 << 12)	/*0x1000*/

//#define CONFIG_VIN_LOG
extern unsigned int vin_log_mask;
#if defined CONFIG_VIN_LOG
#define vin_log(flag, arg...) do { \
	if (flag & vin_log_mask) { \
		switch (flag) { \
		case VIN_LOG_MD: \
			printk(KERN_DEBUG "[VIN_LOG_MD]" arg); \
			break; \
		case VIN_LOG_FLASH: \
			printk(KERN_DEBUG "[VIN_LOG_FLASH]" arg); \
			break; \
		case VIN_LOG_CCI: \
			printk(KERN_DEBUG "[VIN_LOG_CCI]" arg); \
			break; \
		case VIN_LOG_CSI: \
			printk(KERN_DEBUG "[VIN_LOG_CSI]" arg); \
			break; \
		case VIN_LOG_MIPI: \
			printk(KERN_DEBUG "[VIN_LOG_MIPI]" arg); \
			break; \
		case VIN_LOG_ISP: \
			printk(KERN_DEBUG "[VIN_LOG_ISP]" arg); \
			break; \
		case VIN_LOG_STAT: \
			printk(KERN_DEBUG "[VIN_LOG_STAT]" arg); \
			break; \
		case VIN_LOG_SCALER: \
			printk(KERN_DEBUG "[VIN_LOG_SCALER]" arg); \
			break; \
		case VIN_LOG_POWER: \
			printk(KERN_DEBUG "[VIN_LOG_POWER]" arg); \
			break; \
		case VIN_LOG_CONFIG: \
			printk(KERN_DEBUG "[VIN_LOG_CONFIG]" arg); \
			break; \
		case VIN_LOG_VIDEO: \
			printk(KERN_DEBUG "[VIN_LOG_VIDEO]" arg); \
			break; \
		case VIN_LOG_FMT: \
			printk(KERN_DEBUG "[VIN_LOG_FMT]" arg); \
			break; \
		case VIN_LOG_TDM: \
			printk(KERN_DEBUG "[VIN_LOG_TDM]" arg); \
			break; \
		default: \
			printk(KERN_DEBUG "[VIN_LOG]" arg); \
			break; \
		} \
	} \
} while (0)
#else
#define vin_log(flag, arg...) do { } while (0)
#endif
#define vin_err(x, arg...) pr_err("[VIN_ERR]"x, ##arg)
#define vin_warn(x, arg...) pr_warn("[VIN_WARN]"x, ##arg)
#define vin_print(x, arg...) pr_info("[VIN]"x, ##arg)

struct vin_mm {
	size_t size;
	void *phy_addr;
	void *vir_addr;
	void *dma_addr;
	//struct ion_client *client;
	//struct ion_handle *handle;
};

/* pio end, invalid macro */
#define GPIO_INDEX_INVALID      (0xFFFFFFF0)
#define GPIO_CFG_INVALID        (0xEEEEEEEE)
#define GPIO_PULL_INVALID       (0xDDDDDDDD)
#define GPIO_DRVLVL_INVALID     (0xCCCCCCCC)
#define IRQ_NUM_INVALID         (0xFFFFFFFF)
#define AXP_PORT_VAL            (0x0000FFFF)

/* pio default macro */
#define GPIO_PULL_DEFAULT       ((u32)-1)
#define GPIO_DRVLVL_DEFAULT     ((u32)-1)
#define GPIO_DATA_DEFAULT       ((u32)-1)

/*
 * struct gpio_config - gpio config info
 * @gpio:      gpio global index, must be unique
 * @mul_sel:   multi sel val: 0 - input, 1 - output.
 * @pull:      pull val: 0 - pull up/down disable, 1 - pull up
 * @drv_level: driver level val: 0 - level 0, 1 - level 1
 * @data:      data val: 0 - low, 1 - high, only valid when mul_sel is input/output
 */
struct gpio_config {
        u32     data;
        u32     gpio;
        u32     mul_sel;
        u32     pull;
        u32     drv_level;
};

extern int os_gpio_set(struct gpio_config *gc);
extern int os_gpio_write(u32 gpio, __u32 out_value, int force_value_flag);
extern int os_mem_alloc(struct vin_mm *mem_man);
extern void os_mem_free(struct vin_mm *mem_man);

#endif	/*__VIN__OS__H__*/
