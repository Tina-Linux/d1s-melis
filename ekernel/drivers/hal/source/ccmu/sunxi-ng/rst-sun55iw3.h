// SPDX-License-Identifier: (GPL-2.0+ or MIT)
/*
 * Copyright (c) 2022 Allwinnertech
 */

#ifndef _RESET_SUN55IW3_H_
#define _RESET_SUN55IW3_H_

#define RST_MBUS		0
#define RST_BUS_NSI		1
#define RST_BUS_DE0		2
#define RST_BUS_DI		3
#define RST_BUS_G2D		4
#define RST_BUS_GPU		5
#define RST_BUS_CE_SY		6
#define RST_BUS_CE		7
#define RST_BUS_VE		8
#define RST_BUS_DMA		9
#define RST_BUS_MSGBOX1		10
#define RST_BUS_MSGBOX0		11
#define RST_BUS_SPINLOCK	12
#define RST_BUS_TIME		13
#define RST_BUS_DBGSY		14
#define RST_BUS_PWM1		15
#define RST_BUS_PWM		16
#define RST_BUS_DRAM		17
#define RST_BUS_NAND0		18
#define RST_BUS_SMHC2		19
#define RST_BUS_SMHC1		20
#define RST_BUS_SMHC0		21
#define RST_BUS_SYSDAP		22
#define RST_BUS_UART7		23
#define RST_BUS_UART6		24
#define RST_BUS_UART5		25
#define RST_BUS_UART4		26
#define RST_BUS_UART3		27
#define RST_BUS_UART2		28
#define RST_BUS_UART1		29
#define RST_BUS_UART0		30
#define RST_BUS_TWI5		31
#define RST_BUS_TWI4		32
#define RST_BUS_TWI3		33
#define RST_BUS_TWI2		34
#define RST_BUS_TWI1		35
#define RST_BUS_TWI0		36
#define RST_BUS_SPIF		38
#define RST_BUS_SPI2		39
#define RST_BUS_SPI1		40
#define RST_BUS_SPI0		41
#define RST_BUS_GMAC1		42
#define RST_BUS_GMAC0		43
#define RST_BUS_IRRX		44
#define RST_BUS_IRTX		45
#define RST_BUS_GPADC1		46
#define RST_BUS_GPADC0		47
#define RST_BUS_TH		48
#define RST_USB_PHY0_RSTN	49
#define RST_USB_PHY1_RSTN	50
#define RST_USB_2_PHY		51
#define RST_USB_2		52
#define RST_USB_OTG0		53
#define RST_USB_EHCI1		54
#define RST_USB_EHCI0		55
#define RST_USB_OHCI1		56
#define RST_USB_OHCI0		57
#define RST_BUS_LRADC		58
#define RST_BUS_PCIE_PE		59
#define RST_BUS_PCIE_POWER_UP	60
#define RST_BUS_PCIE		61
#define RST_BUS_DPSS_TOP0	62
#define RST_BUS_DPSS_TOP1	63
#define RST_BUS_HDMI_SUB	64
#define RST_BUS_HDMI_MAIN	65
#define RST_BUS_DSI1		66
#define RST_BUS_DSI0		67
#define RST_BUS_VO1_TCONLCD0	68
#define RST_BUS_VO0_TCONLCD1	69
#define RST_BUS_VO0_TCONLCD0	70
#define RST_BUS_TCONTV1		71
#define RST_BUS_TCONTV		72
#define RST_BUS_LVDS1		73
#define RST_BUS_LVDS0		74
#define RST_BUS_LEDC		75
#define RST_BUS_CSI		76
#define RST_BUS_ISP		77

#define RST_BUS_NUMBER (RST_BUS_ISP + 1)

#endif /* _DT_BINDINGS_RESET_SUN55IW3_H_ */
