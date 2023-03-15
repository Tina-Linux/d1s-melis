// SPDX-License-Identifier: (GPL-2.0+ or MIT)
/*
 * Copyright (c) 2020 huangzhenwei@allwinnertech.com
 */

#ifndef _DT_BINDINGS_RESET_SUN20IW2_AON_H_
#define _DT_BINDINGS_RESET_SUN20IW2_AON_H_

#define RST_BLE_RTC   	0
#define RST_MADCFG 	1
#define RST_WLAN_CONN 	2
#define RST_WLAN      	3
#define RST_CODEC_DAC 	4
#define RST_RFAS      	5
#define RST_RCCAL     	6
#define RST_LPSD      	7
#define RST_AON_TIMER 	8
#define RST_CODEC_ADC 	9
#define RST_MAD       	10
#define RST_DMIC      	11
#define RST_GPADC     	12
#define RST_LPUART1   	13
#define RST_LPUART0   	14
#define RST_BLE_32M   	15
#define RST_BLE_48M   	16

#define RST_AON_BUS_NUMBER (RST_LPUART0 + 1)

#endif /* _DT_BINDINGS_RESET_SUN20IW2_AON_H_ */
