/* SPDX-License-Identifier: (GPL-2.0+ or MIT) */
/*
 * Copyright (C) 2020 huangzhenwei@allwinnertech.com
 */

#ifndef _RST_SUN55IW3_R_CCU_H_
#define _RST_SUN55IW3_R_CCU_H_


#define RST_R_TIMER		0
#define RST_R_PWM		1
#define RST_R_CAN		2
#define RST_R_SPI		3
#define RST_R_SPLOCK		4
#define RST_R_MBOX		5
#define RST_R_UART1		6
#define RST_R_UART0		7
#define RST_R_TWI2		8
#define RST_R_TWI1		9
#define RST_R_TWI0		10
#define RST_R_PPU1		11
#define RST_R_PPU		12
#define RST_R_IRRX		13
#define RST_R_RTC		14
#define RST_R_CPUCFG		15

#define RST_R_BUS_NUMBER   (RST_R_CPUCFG + 1)

#endif /* _DT_BINDINGS_RST_SUN20IW2_R_CCU_H_ */
