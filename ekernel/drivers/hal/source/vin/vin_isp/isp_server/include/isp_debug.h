/*
 * linux-4.9/drivers/media/platform/sunxi-vin/vin-isp/isp_server/include/isp_debug.h
 *
 * Copyright (c) 2007-2022 Allwinnertech Co., Ltd.
 *
 * Authors:  Yang Feng <yangfeng@allwinnertech.com>
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

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <hal_log.h>
#include <sunxi_hal_common.h>

//#define ISP_DGB_FL

extern unsigned int isp_lib_log_param;
extern unsigned int isp_cfg_log_param;

#if 0
#define ISP_ERR(x, arg...) hal_log_err("[ISP_ERR]" x, ##arg)
#define ISP_WARN(x, arg...) hal_log_warn("[ISP_WARN]" x, ##arg)
#define ISP_PRINT(x, arg...) hal_log_info("[ISP]" x, ##arg)
#else
#define ISP_ERR(x, arg...) printk(KERN_ERR"[ISP_ERR]%s, line: %d," x, __FUNCTION__, __LINE__, ##arg)
#define ISP_WARN(x, arg...) printk(KERN_WARNING"[ISP_WARN]" x, ##arg)
#define ISP_PRINT(x, arg...) printk(KERN_DEBUG"[ISP]" x, ##arg)
#endif

#ifdef ISP_DGB_FL
#define  FUNCTION_LOG do { printk(KERN_DEBUG"%s, line: %d\n", __FUNCTION__, __LINE__); } while (0)
#else
#define  FUNCTION_LOG do { } while(0)
#endif

#define ISP_LOG_AE				(1 << 0)	//0x1
#define ISP_LOG_AWB				(1 << 1)	//0x2
#define ISP_LOG_AF				(1 << 2)	//0x4
#define ISP_LOG_ISO				(1 << 3)	//0x8
#define ISP_LOG_GAMMA				(1 << 4)	//0x10
#define ISP_LOG_COLOR_MATRIX			(1 << 5)	//0x20
#define ISP_LOG_AFS				(1 << 6)	//0x40
#define ISP_LOG_MOTION_DETECT			(1 << 7)	//0x80
#define ISP_LOG_GAIN_OFFSET			(1 << 8)	//0x100
#define ISP_LOG_DEFOG				(1 << 9)	//0x200
#define ISP_LOG_LSC				(1 << 10)	//0x400
#define ISP_LOG_GTM				(1 << 11)	//0x800
#define ISP_LOG_PLTM				(1 << 12)	//0x1000

#define ISP_LOG_SUBDEV				(1 << 13)	//0x2000
#define ISP_LOG_CFG				(1 << 14)	//0x4000
#define ISP_LOG_VIDEO				(1 << 15)	//0x8000
#define ISP_LOG_ISP				(1 << 16)	//0x10000
#define ISP_LOG_FLASH				(1 << 17)	//0x20000
#define ISP_LOG_RP				(1 << 18)	//0x40000

#define ISP_LIB_LOG(flag, msg...)\
	do {\
		if (isp_lib_log_param & flag)\
		printk(KERN_DEBUG"[ISP_DEBUG]: " msg);\
	} while (0);\

#define ISP_CFG_LOG(flag, msg...)\
	do {\
		if (isp_cfg_log_param & flag)\
		printk(KERN_DEBUG"[ISP_DEBUG]: " msg);\
	} while (0);\


#endif /*_DEBUG_H_*/

