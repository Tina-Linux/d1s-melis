/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2020 frank@allwinnertech.com
 */

#ifndef _CCU_SUN20IW2_R_H
#define _CCU_SUN20IW2_R_H

#define SUNXI20_R_CCU_BASE      0x40050000  /* PRCM_BASE */
#define PRCM_SYS_LFCLK_CTRL     0x080       //TODO: use prcm define
#define PRCM_BLE_CLK32K_SWITCH  0x014c      //TODO: use prcm define

#define	CLK_HOSC_DIV_32K	0
#define	CLK_RCOSE_DIV_32K	1
#define	CLK_OSC32K_EN		2
#define	CLK_RC32K_EN		3
#define	RC_HF_EN		4
#define	CLK_SYS_32K_SEL		5
#define	CLK_BLE_SEL		6
#define	CLK_SYSRTC32K		7
#define	CLK_LF_SEL		8
#define	CLK_PAD			9
#define	CLK_PAD_OUT		10
#define	CLK_RCCAL32K		11
#define	CLK_RCO_WUP_EN		12
#define	CLK_RCO_WUP_MODE_SEL	13
#define	CLK_DIV			14
#define	CLK_32K_AUTO_SWITCH	15

#define	CLK_R_NUMBER		(CLK_32K_AUTO_SWITCH + 1)

#endif /* _CCU_SUN8IW20_R_H */
