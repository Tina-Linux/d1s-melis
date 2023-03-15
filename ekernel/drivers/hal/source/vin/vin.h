/*
 * vin.h
 *
 * Copyright (c) 2018 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zequn Zheng <zequnzhengi@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __VIN_H__
#define __VIN_H__

#include <hal_log.h>

#include "utility/vin_supply.h"
#include "utility/vin_common.h"
#include "utility/media-bus-format.h"
#include "platform/platform_cfg.h"
#include "vin_cci/cci_helper.h"
#include "vin_mipi/sunxi_mipi.h"
#include "vin_csi/sunxi_csi.h"
#include "vin_video/vin_core.h"
#include "vin_video/vin_video.h"
#include "vin_isp/sunxi_isp.h"
#include "vin_vipp/sunxi_scaler.h"
#include "vin_tdm/vin_tdm.h"
#include "modules/sensor/camera.h"
#include "modules/sensor/sensor_register.h"
#include "top_reg.h"

#ifndef CONFIG_RTTKERNEL
#error "vin driver only support rtt platform"
#endif

#define VIN_PLL_CSI_RATE (2376UL*1000*1000)

// #define MEMRESERVE 0x5f700000
#define MEMRESERVE 0x43200000
#define MEMRESERVE_SIZE (0xa00000 - 0x2000) //0x2000 reserved for boot0 read flash and write to it
extern struct rt_memheap isp_mempool;

extern unsigned int vin_log_mask;
#define VIN_LOG_MD				(1 << 0) 	/*0x1 */
#define VIN_LOG_FLASH				(1 << 1) 	/*0x2 */
#define VIN_LOG_CCI				(1 << 2) 	/*0x4 */
#define VIN_LOG_CSI				(1 << 3) 	/*0x8 */
#define VIN_LOG_MIPI				(1 << 4) 	/*0x10*/
#define VIN_LOG_ISP				(1 << 5) 	/*0x20*/
#define VIN_LOG_STAT				(1 << 6) 	/*0x40*/
#define VIN_LOG_SCALER				(1 << 7) 	/*0x80*/
#define VIN_LOG_POWER				(1 << 8) 	/*0x100*/
#define VIN_LOG_CONFIG				(1 << 9) 	/*0x200*/
#define VIN_LOG_VIDEO				(1 << 10)	/*0x400*/
#define VIN_LOG_FMT				(1 << 11)	/*0x800*/
#define VIN_LOG_TDM				(1 << 12)	/*0x1000*/
#define VIN_LOG_STAT1				(1 << 13) 	/*0x2000*/

#define vin_log(flag, arg...) do { \
	if (flag & vin_log_mask) { \
		switch (flag) { \
		case VIN_LOG_MD: \
			printk(KERN_DEBUG"[VIN_LOG_MD]" arg); \
			break; \
		case VIN_LOG_FLASH: \
			printk(KERN_DEBUG"[VIN_LOG_FLASH]" arg); \
			break; \
		case VIN_LOG_CCI: \
			printk(KERN_DEBUG"[VIN_LOG_CCI]" arg); \
			break; \
		case VIN_LOG_CSI: \
			printk(KERN_DEBUG"[VIN_LOG_CSI]" arg); \
			break; \
		case VIN_LOG_MIPI: \
			printk(KERN_DEBUG"[VIN_LOG_MIPI]" arg); \
			break; \
		case VIN_LOG_ISP: \
			printk(KERN_DEBUG"[VIN_LOG_ISP]" arg); \
			break; \
		case VIN_LOG_STAT: \
			printk(KERN_DEBUG"[VIN_LOG_STAT]" arg); \
			break; \
		case VIN_LOG_SCALER: \
			printk(KERN_DEBUG"[VIN_LOG_SCALER]" arg); \
			break; \
		case VIN_LOG_POWER: \
			printk(KERN_DEBUG"[VIN_LOG_POWER]" arg); \
			break; \
		case VIN_LOG_CONFIG: \
			printk(KERN_DEBUG"[VIN_LOG_CONFIG]" arg); \
			break; \
		case VIN_LOG_VIDEO: \
			printk(KERN_DEBUG"[VIN_LOG_VIDEO]" arg); \
			break; \
		case VIN_LOG_FMT: \
			printk(KERN_DEBUG"[VIN_LOG_FMT]" arg); \
			break; \
		default: \
			printk(KERN_DEBUG"[VIN_LOG]" arg); \
			break; \
		} \
	} \
} while (0)

#if 0
#define vin_err(x, arg...)   hal_log_err("[VIN_ERR]"x, ##arg)
#define vin_warn(x, arg...)  hal_log_warn("[VIN_WARN]"x, ##arg)
#define vin_print(x, arg...) hal_log_info("[VIN]"x, ##arg)
#else
#define vin_err(x, arg...)   printk(KERN_ERR"[VIN_ERR]"x, ##arg)
#define vin_warn(x, arg...)  printk(KERN_WARNING"[VIN_WARN]"x, ##arg)
#define vin_print(x, arg...) printk(KERN_DEBUG"[VIN]"x, ##arg)
#endif

#endif
