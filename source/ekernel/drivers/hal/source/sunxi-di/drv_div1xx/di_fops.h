/*
 * Copyright (c) 2020-2031 Allwinnertech Co., Ltd.
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

#ifndef _DI_FOPS_H_
#define _DI_FOPS_H_

int sunxi_di_release(void);
int sunxi_di_open(void);
long sunxi_di_control(unsigned int cmd, void *arg);
#endif /* #ifndef _DI_FOPS_H_ */
