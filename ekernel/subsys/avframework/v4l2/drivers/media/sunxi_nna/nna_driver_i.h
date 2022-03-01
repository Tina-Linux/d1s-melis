/* NNA_driver_i.h
 *
 * Copyright (c)    2011 Allwinnertech Co., Ltd.
 *                    2011 Yupu Tang
 *
 * @ NNA driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 */

#ifndef __NNA_DRIVER_I_H
#define __NNA_DRIVER_I_H

#include <linux/device.h>
#include <linux/ioport.h>
#include <linux/mutex.h>
#include <linux/clk.h>
#include <linux/clk/sunxi.h>

#define NNA_DEBUG    1

#ifdef NNA_DEBUG
#define    NNA_DBG(format, args...) pr_debug("%s: " format, "NNA", ## args)
#else
#define    NNA_DBG(format, args...)
#endif

#define NNA_ERR(format, args...)     pr_err("%s: " format, "NNA", ## args)
#define NNA_WARN(format, args...)    pr_warn("%s: " format, "NNA", ## args)
#define NNA_INFO(format, args...)    pr_info("%s: " format, "NNA", ## args)

#define MAX_NNA_MEM_INDEX    10
#define INTC_IRQNO_DE_MIX    SUNXI_IRQ_MP

#define NNAALIGN(value, align) ((align == 0) ? value : (((value) + ((align) - 1)) & ~((align) - 1)))

#define OFFSET_NNA_GLB_S_INTR_STATUS_0 0x100c

typedef struct nna_context {
	struct device *platform_dev;
	struct device *nna_dev;
	struct resource *mem;
	void __iomem *io;
	__u32 irq;
	struct mutex mutex;
	struct clk *clk;
	struct clk *clk_rst;
	bool opened;
	__u32 user_cnt;
	struct class  *nna_class;
	struct cdev   *nna_cdev;
	dev_t         devid;
} nna_context;

#endif /* __NNA_DRIVER_I_H */
