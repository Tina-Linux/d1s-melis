/*
 * soc.h
 *
 * Copyright (c) 2007-2021 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
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
#ifndef _SOC_H
#define _SOC_H

#if defined (CONFIG_ARCH_SUN20IW1)
#define TVD0_IRQ_NO 123
#define TVD0_REG_BASE 0x05c01000
#else
#error "NULL ARCH!!"
#endif

//1, auto agc, 0, manual mode
#define AGC_AUTO_ENABLE 1
//agc value when AGC_AUTO_ENABLE == 0
#define AGC_MANUAL_VALUE 64
//enable cagc
#define CAGC_ENABLE 1
// enable 3D filter
#define FLITER_USED 1

#endif /*End of file*/
