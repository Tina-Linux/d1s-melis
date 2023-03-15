
/*
 * vin platform config header file
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __PLATFORM_CFG__H__
#define __PLATFORM_CFG__H__

#include <sunxi_hal_common.h>
//#include "../vin_mipi/combo_common.h"

#if defined CONFIG_ARCH_SUN20IW3
#include "vin_config_sun8iw21p1.h"
#endif

struct mbus_framefmt_res {
	u8 res_wdr_mode;
	u8 res_time_hs;
};

#define clamp(val, lo, hi) min(max(val, lo), hi)
//#define clamp_t(type, val, lo, hi) min(type, max(type, val, lo), hi)
#define roundup(x, y) (					\
{							\
	const typeof(y) __y = y;			\
	(((x) + (__y - 1)) / __y) * __y;		\
}							\
)
#define ALIGN(x, a)		__ALIGN_KERNEL((x), (a))
#define __ALIGN_KERNEL(x, a)		__ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask)	(((x) + (mask)) & ~(mask))
#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))

#define ALIGN_4K(x)	(((x) + (4095)) & ~(4095))
#define ALIGN_32B(x)	(((x) + (31)) & ~(31))
#define ALIGN_16B(x)	(((x) + (15)) & ~(15))
#define CLIP(a, i, s)	(((a) > (s)) ? (s) : MAX(a, i))

#define CSI_CH_0	(1 << 20)
#define CSI_CH_1	(1 << 21)
#define CSI_CH_2	(1 << 22)
#define CSI_CH_3	(1 << 23)

#define MAX_DETECT_NUM	3

#define VIN_ALIGN_WIDTH 16
#define VIN_ALIGN_HEIGHT 16

#endif /*__PLATFORM_CFG__H__*/
