/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2020 frank@allwinnertech.com
 */

#ifndef _CCU_SUN55IW3_R_H
#define _CCU_SUN55IW3_R_H

#define SUNXI55_R_CCU_BASE      0x7010000  /* PRCM_BASE */

#define CLK_R_AHB		0
#define CLK_R_APBS0		1
#define CLK_R_APBS1		2
#define CLK_R_TIMER0		3
#define CLK_R_TIMER1		4
#define CLK_R_TIMER2		5
#define CLK_BUS_R_TIMER		6
#define CLK_BUS_R_TWD		7
#define CLK_R_PWM		8
#define CLK_BUS_R_PWM		9
#define CLK_R_SPI		11
#define CLK_BUS_R_SPI		12
#define CLK_BUS_R_SPLOCK	13
#define CLK_BUS_R_MBOX		14
#define CLK_BUS_R_UART1		15
#define CLK_BUS_R_UART0		16
#define CLK_BUS_R_TWI2		17
#define CLK_BUS_R_TWI1		18
#define CLK_BUS_R_TWI0		19
#define CLK_R_PPU1		20
#define CLK_R_PPU		21
#define CLK_BUS_R_TZMA		22
#define CLK_BUS_R_BIST		23
#define CLK_R_IRRX		24
#define CLK_BUS_R_IRRX		25
#define CLK_DMA_CLKEN_SW	26
#define CLK_BUS_R_RTC		27
#define CLK_BUS_R_CPUCFG	28
#define	CLK_R_NUMBER		(CLK_BUS_R_CPUCFG + 1)

#endif
