/*
 * disp_board_config.c
 *
 * Copyright (c) 2007-2020 Allwinnertech Co., Ltd.
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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <hal_clk.h>
#include <hal_gpio.h>
#include "../lcd_fb/lcd_fb_intf.h"
#include "disp_board_config.h"

#ifndef CONFIG_DRIVER_SYSCONFIG
struct property_t g_lcd0_config[] = {
	{
		.name = "lcd_used",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "lcd_driver_name",
		.type = PROPERTY_STRING,
		.v.str = "kld2844b",
	},
	{
		.name = "lcd_backlight",
		.type = PROPERTY_INTGER,
		.v.value = 100,
	},
	{
		.name = "lcd_if",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "lcd_dbi_if",
		.type = PROPERTY_INTGER,
		.v.value = 4,
	},
	{
		.name = "lcd_data_speed",
		.type = PROPERTY_INTGER,
		.v.value = 60,
	},
	{
		.name = "lcd_spi_bus_num",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "lcd_x",
		.type = PROPERTY_INTGER,
		.v.value = 240,
	},
	{
		.name = "lcd_y",
		.type = PROPERTY_INTGER,
		.v.value = 320,
	},
	{
		.name = "lcd_width",
		.type = PROPERTY_INTGER,
		.v.value = 60,
	},
	{
		.name = "lcd_height",
		.type = PROPERTY_INTGER,
		.v.value = 95,
	},
	{
		.name = "lcd_pwm_used",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "lcd_pwm_ch",
		.type = PROPERTY_INTGER,
		.v.value = 6,
	},
	{
		.name = "lcd_pwm_freq",
		.type = PROPERTY_INTGER,
		.v.value = 5000,
	},
	{
		.name = "lcd_pwm_pol",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_pixel_fmt",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_dbi_fmt",
		.type = PROPERTY_INTGER,
		.v.value = 3,
	},
	{
		.name = "lcd_rgb_order",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
	{
		.name = "lcd_frm",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "lcd_gamma_en",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "lcd_buffer_num",
		.type = PROPERTY_INTGER,
		.v.value = 2,
	},
	{
		.name = "lcd_dbi_te",
		.type = PROPERTY_INTGER,
		.v.value = 1,
	},
	{
		.name = "lcd_fps",
		.type = PROPERTY_INTGER,
		.v.value = 60,
	},

//gpio
	// dc
	{
		.name = "lcd_spi_dc_pin",
		.type = PROPERTY_GPIO,
		.v.gpio_list = {
			.gpio = GPIOA(19),
			.mul_sel = GPIO_DIRECTION_OUTPUT,
			.pull = 0,
			.drv_level = 3,
			.data = 1,
		},
	},

};

struct property_t g_lcd1_config[] = {
	{
		.name = "lcd_used",
		.type = PROPERTY_INTGER,
		.v.value = 0,
	},
};

u32 g_lcd0_config_len = sizeof(g_lcd0_config) / sizeof(struct property_t);
u32 g_lcd1_config_len = sizeof(g_lcd1_config) / sizeof(struct property_t);
#endif
