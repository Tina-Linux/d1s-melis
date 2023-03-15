// SPDX-License-Identifier: (GPL-2.0+ or MIT)
/*
 * Copyright (c) 2020 huangzhenwei@allwinnertech.com
 */

#ifndef _DT_BINDINGS_RESET_SUN20IW2_H_
#define _DT_BINDINGS_RESET_SUN20IW2_H_

#define RST_USB_EHCI    0
#define RST_USB_OHCI    1
#define RST_CSI_JPE     2
#define RST_LEDC        3
#define RST_USB_OTG     4
#define RST_SMCARD      5
#define RST_USB_PHY     6
#define RST_FLASH_ENC   7
#define RST_FLASH_CTRL  8
#define RST_HSPSRAM_CTRL 9
#define RST_LSPSRAM_CTRL 10
#define RST_IRRX        11
#define RST_IRTX        12
#define RST_PWM         13
#define RST_TWI1        14
#define RST_TWI0        15
#define RST_UART2       16
#define RST_UART1       17
#define RST_UART0       18
#define RST_SDC0        19
#define RST_SPI1        20
#define RST_SPI0        21
#define RST_G2D         22
#define RST_DE          23
#define RST_DISPLAY     24
#define RST_LCD         25
#define RST_BT_CORE     26
#define RST_WLAN_CTRL   27
#define RST_TRNG        28
#define RST_SPC         29
#define RST_SS          30
#define RST_TIMER       31
#define RST_SPINLOCK    32
#define RST_DMA1        33
#define RST_DMA0        34
#define RST_SPDIF       35
#define RST_I2S         36
#define RST_RISCV_TIMESTAMP 37
#define RST_RISCV_SYS_APB_SOFT 38
#define RST_RISCV_CFG   39
#define RST_RISCV_MSGBOX  40
#define RST_RISCV_WDG   41
#define RST_RISCV_CORE  42
#define RST_DSP_DEBUG   43
#define RST_DSP_INTC    44
#define RST_DSP_TZMA    45
#define RST_DSP_CFG     46
#define RST_DSP_MSGBOX  47
#define RST_DSP_WDG     48
#define RST_DSP_CORE    49
#define RST_CPU_CFG     50
#define RST_CPU_MSGBOX  51
#define RST_CPU_WDG     52

#define RST_BUS_NUMBER (RST_CPU_WDG + 1)

#endif /* _DT_BINDINGS_RESET_SUN20IW2_H_ */
