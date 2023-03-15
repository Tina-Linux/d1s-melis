/*
 * vin_supply.c
 *
 * Copyright (c) 2018 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zequn Zheng <zequnzhengi@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <sunxi_hal_twi.h>

#include "vin_supply.h"
#include "../platform/platform_cfg.h"
#include "../vin_video/vin_core.h"

extern struct vin_core *global_vinc[VIN_MAX_VIDEO];

int sunxi_vin_get_csi_top_irq(void)
{
	return SUNXI_IRQ_CSI_TOP_PKT;
}

int sunxi_vin_get_isp_irq(int id)
{
	return vin_isp_irq[id];
}

unsigned long sunxi_vin_get_top_base(void)
{
	return (unsigned long)CSI_TOP_REGS_BASE;
}

unsigned long sunxi_vin_get_ccu_base(void)
{
	return (unsigned long)CSI_CCU_REGS_BASE;
}

unsigned long sunxi_vin_get_mipi_base(void)
{
	return (unsigned long)vin_mipi_base[0];
}

unsigned long sunxi_vin_get_mipiphy_base(int id)
{
	return (unsigned long)vin_mipi_base[1 + id];
}

unsigned long sunxi_vin_get_mipiport_base(int id)
{
	return (unsigned long)vin_mipi_base[3 + id];
}

unsigned long sunxi_vin_get_csi_base(int id)
{
	return (unsigned long)vin_csi_base[id];
}

unsigned long sunxi_vin_get_isp_base(int id)
{
	return (unsigned long)vin_isp_base[id/DEV_VIRT_NUM];
}

unsigned long sunxi_vin_get_scaler_base(int id)
{
	return (unsigned long)vin_scaler_base[id/DEV_VIRT_NUM];
}

int sunxi_vin_get_vipp_irq(int id)
{
	return vin_vipp_irq[id/DEV_VIRT_NUM];
}

int vin_vinc_parser(unsigned int id)
{
	struct vin_core *vinc = global_vinc[id];

	vinc->used = global_video[id].used;
	vinc->csi_sel = global_video[id].csi_sel;
	vinc->mipi_sel = global_video[id].mipi_sel;
	vinc->isp_sel = global_video[id].isp_sel;
	vinc->tdm_rx_sel = global_video[id].tdm_rx_sel;
	vinc->isp_tx_ch = global_video[id].isp_tx_ch;
	vinc->rear_sensor = global_video[id].rear_sensor;

	return 0;
}

int sunxi_twi_init(int id)
{
	int slave_addr;
	twi_port_t port;

	slave_addr = global_sensors[id].sensor_twi_addr >> 1;
	port = global_sensors[id].sensor_twi_id;

	hal_twi_init(port);
	hal_twi_control(port, I2C_SLAVE, &slave_addr);
	hal_writel(0x135, 0x02000340);

	vin_log(VIN_LOG_POWER, "%s:id=%d, slave=0x%x\n", __func__, port, slave_addr);

	return 0;
}

int sunxi_twi_exit(int id)
{
	twi_port_t port;

	port = global_sensors[id].sensor_twi_id;
	hal_twi_uninit(port);

	return 0;
}

int vin_set_pmu_channel(int on)
{
	return 0;
}

int vin_gpio_set_status(int id, enum gpio_type gpio_id, unsigned int status)
{
	gpio_pin_t gpio;

	if (gpio_id == PWDN)
		gpio = global_sensors[id].pwdn_gpio;
	else if (gpio_id == RESET)
		gpio = global_sensors[id].reset_gpio;
	else if (gpio_id == IR_CUT0)
		gpio = global_sensors[id].ir_cut_gpio[0];
	else if (gpio_id == IR_CUT1)
		gpio = global_sensors[id].ir_cut_gpio[1];
	else if (gpio_id == IR_LED)
		gpio = global_sensors[id].ir_led_gpio;
	else
		return -1;

	if (gpio == 0xffff)
		return -1;

	if (status == 1) { //output
		hal_gpio_set_pull(gpio, GPIO_PULL_UP);
		hal_gpio_set_direction(gpio, GPIO_DIRECTION_OUTPUT);
		hal_gpio_set_data(gpio, GPIO_DATA_LOW);
	} else if (status == 0) {
		hal_gpio_set_pull(gpio, GPIO_PULL_DOWN);
		hal_gpio_set_direction(gpio, GPIO_DIRECTION_INPUT);
		hal_gpio_set_data(gpio, GPIO_DATA_LOW);
	} else {
		hal_gpio_pinmux_set_function(gpio, GPIO_MUXSEL_DISABLED);
	}

	return 0;
}

int vin_gpio_write(int id, enum gpio_type gpio_id, unsigned int out_value)
{
	gpio_pin_t gpio;

	if (gpio_id == PWDN)
		gpio = global_sensors[id].pwdn_gpio;
	else if (gpio_id == RESET)
		gpio = global_sensors[id].reset_gpio;
	else if (gpio_id == IR_CUT0)
		gpio = global_sensors[id].ir_cut_gpio[0];
	else if (gpio_id == IR_CUT1)
		gpio = global_sensors[id].ir_cut_gpio[1];
	else if (gpio_id == IR_LED)
		gpio = global_sensors[id].ir_led_gpio;
	else
		return -1;

	if (gpio == 0xffff)
		return -1;

	hal_gpio_set_direction(gpio, GPIO_DIRECTION_OUTPUT);
	if (out_value) {
		hal_gpio_set_data(gpio, GPIO_DATA_HIGH);
	} else {
		hal_gpio_set_data(gpio, GPIO_DATA_LOW);
	}

	return 0;
}

int vin_set_mclk_freq(int id, unsigned long freq)
{
	hal_clk_t mclk_src = 0;
	int mclk_id = global_sensors[id].mclk_id;

	if (freq == 24000000 || freq == 12000000 || freq == 6000000) {
		mclk_src = vind_default_mclk[mclk_id].clk_24m;
	} else {
		mclk_src = vind_default_mclk[mclk_id].clk_pll;
	}

	if (hal_clk_set_parent(vind_default_mclk[mclk_id].mclk, mclk_src)) {
		vin_err("set mclk%d source failed!\n", mclk_id);
		return -1;
	}
	if (hal_clk_set_rate(vind_default_mclk[mclk_id].mclk, freq)) {
		vin_err("set csi master%d clock error\n", mclk_id);
		return -1;
	}
	vin_log(VIN_LOG_POWER, "mclk%d set rate %ld, get rate %ld\n", mclk_id,
		freq, hal_clk_get_rate(vind_default_mclk[mclk_id].mclk));

	return 0;
}

int vin_set_mclk(int id, unsigned int on_off)
{
	struct vin_mclk_info *mclk = NULL;
	int mclk_id = global_sensors[id].mclk_id;

	mclk = &vind_default_mclk[mclk_id];

	if (on_off && mclk->use_count++ > 0)
		return 0;
	else if (!on_off && (mclk->use_count == 0 || --mclk->use_count > 0))
		return 0;

	switch (on_off) {
	case 1:
		vin_log(VIN_LOG_POWER, "sensor mclk on, use_count %d!\n", mclk->use_count);
		hal_gpio_pinmux_set_function(mclk->pin, mclk->pin_func[0]);
		if (mclk->mclk) {
			if (hal_clock_enable(mclk->mclk)) {
				vin_err("csi master clock enable error\n");
				return -1;
			}
		} else {
			vin_err("csi master%d clock is null\n", mclk_id);
			return -1;
		}
		break;
	case 0:
		hal_gpio_pinmux_set_function(mclk->pin, mclk->pin_func[1]);
		vin_log(VIN_LOG_POWER, "sensor mclk off, use_count %d!\n", mclk->use_count);
		if (mclk->mclk) {
			hal_clock_disable(mclk->mclk);
		} else {
			vin_err("csi master%d clock is null\n", mclk_id);
			return -1;
		}
		break;
	default:
		return -1;
	}

	return 0;
}