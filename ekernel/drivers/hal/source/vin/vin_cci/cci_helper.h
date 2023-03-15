/*
 * cci_helper.h
 *
 * Copyright (c) 2018 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zequn Zheng <zequnzhengi@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __CCI_HELPER_H__
#define __CCI_HELPER_H__

#include <sunxi_hal_common.h>

#define REG_DLY  0xffff

typedef unsigned short addr_type;
typedef unsigned short data_type;

struct regval_list {
	addr_type addr;
	data_type data;
};

//u64 v4l2_get_timestamp(void);
int sensor_read(int id, addr_type reg, data_type *value);
int sensor_write(int id, addr_type reg, data_type value);
int sensor_write_array(int id, struct regval_list *regs, int array_size);

#endif

