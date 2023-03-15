/*
 * vin_supply.h
 *
 * Copyright (c) 2018 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zequn Zheng <zequnzhengi@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __VIN_SUPPLY_H__
#define __VIN_SUPPLY_H__

#include <stdint.h>
#include <hal_gpio.h>
#include <hal_clk.h>

#include "../modules/sensor/camera.h"

enum gpio_type {
	PWDN = 0,
	RESET,
	IR_CUT0,
	IR_CUT1,
	IR_LED,
};

int sunxi_vin_get_top_irq(void);
int sunxi_vin_get_isp_irq(int id);
int sunxi_vin_get_vipp_irq(int id);
unsigned long sunxi_vin_get_top_base(void);
unsigned long sunxi_vin_get_ccu_base(void);
unsigned long sunxi_vin_get_mipi_base(void);
unsigned long sunxi_vin_get_mipiphy_base(int id);
unsigned long sunxi_vin_get_mipiport_base(int id);
unsigned long sunxi_vin_get_csi_base(int id);
unsigned long sunxi_vin_get_isp_base(int id);
unsigned long sunxi_vin_get_scaler_base(int id);

int vin_vinc_parser(unsigned int id);
int vin_set_pmu_channel(int on);
int sunxi_twi_init(int id);
int sunxi_twi_exit(int id);

int vin_gpio_set_status(int id, enum gpio_type gpio_id, unsigned int status);
int vin_gpio_write(int id, enum gpio_type gpio_id, unsigned int out_value);
int vin_set_mclk_freq(int id, unsigned long freq);
int vin_set_mclk(int id, unsigned int on_off);

#endif

