/*
 * for modules (sensor/actuator/flash) power supply helper.
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *	Yang Feng <yangfeng@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <hal_clk.h>
#include <hal_gpio.h>
#include <sunxi_hal_regulator.h>

#include "../vin-video/vin_core.h"
#include "vin_os.h"
#include "vin_supply.h"
#include "../platform/platform_cfg.h"
#include "../vin-csi/sunxi_csi.h"
#include "../vin-cci/cci_helper.h"
#include "../modules/sensor/sensor_helper.h"
#include "../vin.h"

extern struct sensor_helper_dev *glb_sensor_helper[VIN_MAX_CSI];

/*
 * called by subdev in power on/off sequency
 */
struct modules_config *sd_to_modules(struct v4l2_subdev *sd)
{
	struct vin_md *vind = dev_get_drvdata(sd->v4l2_dev->dev);
	struct modules_config *module = NULL;
	int i, j;

	for (i = 0; i < VIN_MAX_DEV; i++) {
		module = &vind->modules[i];

		for (j = 0; j < MAX_DETECT_NUM; j++) {
			if (!strcmp(module->sensors->inst[j].cam_name, sd->name))
				return module;

			if ((sd == module->modules.sensor[j].sd) ||
			    (sd == module->modules.act[j].sd) ||
			    (sd == module->modules.flash.sd))
				return module;
		}
	}
	vin_err("%s cannot find the match modules\n", sd->name);
	return NULL;
}

#ifdef CONFIG_SUNXI_REGULATOR_DT
static int find_power_pmic(struct v4l2_subdev *sd, struct vin_power *power, enum pmic_channel pmic_ch)
{
	int i;

	for (i = 0; i < VIN_MAX_CSI; i++) {
		if (glb_sensor_helper[i] && (!strcmp(glb_sensor_helper[i]->name, sd->name))) {
			if (glb_sensor_helper[i]->pmic[pmic_ch] == NULL)
				return -1;
			power[pmic_ch].pmic = glb_sensor_helper[i]->pmic[pmic_ch];
			return 0;
		}
	}

	power[pmic_ch].pmic = NULL;
	vin_err("%s cannot find the match sensor_helper\n", sd->name);
	return -1;
}
#endif
/*
 *enable/disable pmic channel
 */
int vin_set_pmu_channel(struct v4l2_subdev *sd, enum pmic_channel pmic_ch,
			enum on_off on_off)
{
	int ret = 0;
#ifndef FPGA_VER
	struct modules_config *modules = sd_to_modules(sd);
	static int def_vol[MAX_POW_NUM] = {3300000, 3300000, 1800000,
					3300000, 3300000, 3300000};
	struct vin_power *power = NULL;

	if (modules == NULL)
		return -1;

	power = &modules->sensors->power[0];

	if (on_off == OFF) {
		if (power[pmic_ch].pmic == NULL)
			return 0;

		ret = hal_regulator_disable(power[pmic_ch].pmic);
		vin_log(VIN_LOG_POWER, "regulator %s disabled\n", power[pmic_ch].power_str);
	} else {
		ret = hal_regulator_set_voltage(power[pmic_ch].pmic,
					  power[pmic_ch].power_vol);

		vin_log(VIN_LOG_POWER, "set regulator %s = %d,return %x\n",
			power[pmic_ch].power_str, power[pmic_ch].power_vol, ret);
		ret = hal_regulator_enable(power[pmic_ch].pmic);
	}
#endif
	return ret;
}

/*
 *enable/disable master clock
 */
int vin_set_mclk(struct v4l2_subdev *sd, enum on_off on_off)
{
	struct vin_md *vind = dev_get_drvdata(sd->v4l2_dev->dev);
	struct modules_config *modules = sd_to_modules(sd);
	struct vin_mclk_info *mclk = NULL;
	struct vin_power *power = NULL;
	unsigned int mclk_mode;
	char pin_name[30] = "";
	int mclk_id = 0;
	int ret;

	if (modules == NULL)
		return -1;

	if (modules->sensors->mclk_id == -1)
		mclk_id = modules->sensors->csi_sel;
	else
		mclk_id = modules->sensors->mclk_id;
	if (mclk_id < 0) {
		vin_err("get mclk id failed\n");
		return -1;
	}

	mclk = vind->mclk[mclk_id];

	switch (on_off) {
	case ON:
		csi_cci_init_helper(modules->sensors->sensor_bus_sel);
		sprintf(pin_name, "mclk%d-default", mclk_id);
		break;
	case OFF:
		csi_cci_exit_helper(modules->sensors->sensor_bus_sel);
		sprintf(pin_name, "mclk%d-sleep", mclk_id);
		break;
	default:
		return -1;
	}
#ifndef FPGA_VER

	if (on_off && mclk->use_count++ > 0)
		return 0;
	else if (!on_off && (mclk->use_count == 0 || --mclk->use_count > 0))
		return 0;

	switch (on_off) {
	case ON:
		vin_log(VIN_LOG_POWER, "sensor mclk on, use_count %d!\n", mclk->use_count);
		if (mclk->mclk != HAL_CLK_UNINITIALIZED) {
			if (hal_clock_enable(mclk->mclk)) {
				vin_err("csi master clock enable error\n");
				return -1;
			}
		} else {
			vin_err("csi master%d clock is null\n", mclk_id);
			return -1;
		}
		break;
	case OFF:
		vin_log(VIN_LOG_POWER, "sensor mclk off, use_count %d!\n", mclk->use_count);
		if (mclk->mclk != HAL_CLK_UNINITIALIZED) {
			hal_clock_disable(mclk->mclk);
		} else {
			vin_err("csi master%d clock is null\n", mclk_id);
			return -1;
		}
		break;
	default:
		return -1;
	}
	if (on_off) {
		hal_gpio_pinmux_set_function(mclk->pin[0].gpio, mclk->pin[0].mul_sel);
		hal_gpio_set_driving_level(mclk->pin[0].gpio, mclk->pin[0].drv_level);
		hal_gpio_set_pull(mclk->pin[0].gpio, mclk->pin[0].pull);
	} else {
		hal_gpio_pinmux_set_function(mclk->pin[1].gpio, mclk->pin[1].mul_sel);
		hal_gpio_set_driving_level(mclk->pin[1].gpio, mclk->pin[1].drv_level);
		hal_gpio_set_pull(mclk->pin[1].gpio, mclk->pin[1].pull);
	}

	if (on_off) {
		power = &modules->sensors->power[0];
		if (power[IOVDD].power_vol && (power[IOVDD].power_vol <= 1800000))
			mclk_mode = MCLK_POWER_VOLTAGE_1800;
		else
			mclk_mode = MCLK_POWER_VOLTAGE_3300;
		vin_log(VIN_LOG_POWER, "IOVDD power vol is %d, mclk mode is %d\n", power[IOVDD].power_vol, mclk_mode);

		ret = hal_gpio_sel_vol_mode(mclk->pin[0].gpio, mclk_mode);
		if (ret) {
			vin_err("Cannot select mclk pio power mode!\n");
			return ret;
		}
	}
#endif
	return 0;
}

/*
 *set frequency of master clock
 */
int vin_set_mclk_freq(struct v4l2_subdev *sd, unsigned long freq)
{
#ifndef FPGA_VER
	struct vin_md *vind = dev_get_drvdata(sd->v4l2_dev->dev);
	struct modules_config *modules = sd_to_modules(sd);
	hal_clk_id_t mclk_src = HAL_CLK_UNINITIALIZED;
	int mclk_id = 0;
	u32 rate = 0;

	if (modules == NULL)
		return -1;

	if (modules->sensors->mclk_id == -1)
		mclk_id = modules->sensors->csi_sel;
	else
		mclk_id = modules->sensors->mclk_id;
	if (mclk_id < 0) {
		vin_err("get mclk id failed\n");
		return -1;
	}

	if (freq == 24000000 || freq == 12000000 || freq == 6000000) {
		if (vind->mclk[mclk_id]->clk_24m != HAL_CLK_UNINITIALIZED) {
			mclk_src = vind->mclk[mclk_id]->clk_24m;
		} else {
			vin_err("csi master clock 24M source is null\n");
			return -1;
		}
	} else {
		if (vind->mclk[mclk_id]->clk_pll != HAL_CLK_UNINITIALIZED) {
			mclk_src = vind->mclk[mclk_id]->clk_pll;
		} else {
			vin_err("csi master clock pll source is null\n");
			return -1;
		}
	}

	if (vind->mclk[mclk_id]->mclk != HAL_CLK_UNINITIALIZED) {
		if (hal_clk_set_parent(vind->mclk[mclk_id]->mclk, mclk_src)) {
			vin_err("set mclk%d source failed!\n", mclk_id);
			return -1;
		}
		if (hal_clk_set_rate(vind->mclk[mclk_id]->mclk, freq)) {
			vin_err("set csi master%d clock error\n", mclk_id);
			return -1;
		}
		rate = hal_clk_get_rate(vind->mclk[mclk_id]->mclk);
		vin_log(VIN_LOG_POWER, "mclk%d set rate %ld, get rate %ld\n", mclk_id,
			freq, rate);
	} else {
		vin_err("csi master clock is null\n");
		return -1;
	}
#endif
	return 0;
}

int vin_set_sync_mclk(struct v4l2_subdev *sd, int id, unsigned long freq, enum on_off on_off)
{
	struct vin_md *vind = dev_get_drvdata(sd->v4l2_dev->dev);
	struct modules_config *modules = sd_to_modules(sd);
	struct vin_mclk_info *mclk = NULL;
	hal_clk_id_t mclk_src = HAL_CLK_UNINITIALIZED;
	u32 rate = 0;

	if (modules == NULL)
		return -1;

	if (id < 0) {
		vin_err("get mclk id failed\n");
		return -1;
	}

	mclk = vind->mclk[id];

	if (on_off && mclk->use_count++ > 0)
		return 0;
	else if (!on_off && (mclk->use_count == 0 || --mclk->use_count > 0))
		return 0;

	switch (on_off) {
	case ON:
		vin_log(VIN_LOG_POWER, "sensor mclk on, use_count %d!\n", mclk->use_count);
		if (freq == 24000000 || freq == 12000000 || freq == 6000000) {
			if (mclk->clk_24m != HAL_CLK_UNINITIALIZED) {
				mclk_src = mclk->clk_24m;
			} else {
				vin_err("mclk%d 24M source is null\n", id);
				return -1;
			}
		} else {
			if (mclk->clk_pll != HAL_CLK_UNINITIALIZED) {
				mclk_src =  mclk->clk_pll;
			} else {
				vin_err("mclk%d pll source is null\n", id);
				return -1;
			}
		}

		if (mclk->mclk != HAL_CLK_UNINITIALIZED) {
			if (hal_clk_set_parent(mclk->mclk, mclk_src)) {
				vin_err("set mclk%d source failed!\n", id);
				return -1;
			}
			if (hal_clk_set_rate(mclk->mclk, freq)) {
				vin_err("set mclk%d error\n", id);
				return -1;
			}
			rate = hal_clk_get_rate(vind->mclk[id]->mclk);
			vin_log(VIN_LOG_POWER, "mclk%d set rate %ld, get rate %ld\n", id,
				freq, rate);
			if (hal_clock_enable(mclk->mclk)) {
				vin_err("mclk%d enable error\n", id);
				return -1;
			}
		} else {
			vin_err("mclk%d is null\n", id);
			return -1;
		}
		break;
	case OFF:
		vin_log(VIN_LOG_POWER, "sensor mclk off, use_count %d!\n", mclk->use_count);
		if (mclk->mclk != HAL_CLK_UNINITIALIZED) {
			hal_clock_disable(mclk->mclk);
		} else {
			vin_err("mclk%d is null\n", id);
			return -1;
		}
		break;
	default:
		return -1;
	}

	return 0;
}

/*
 *set the gpio io status
 */
int vin_gpio_write(struct v4l2_subdev *sd, enum gpio_type gpio_id,
		   unsigned int out_value)
{
#ifndef FPGA_VER
	int force_value_flag = 1;
	struct modules_config *modules = sd_to_modules(sd);
	struct gpio_config *gc = NULL;

	if (modules == NULL)
		return -1;

	gc = &modules->sensors->gpio[gpio_id];

/*
#ifndef CONFIG_ARCH_SUN8IW17P1
	if ((gpio_id == PWDN) || (gpio_id == RESET))
		force_value_flag = 0;
#endif
*/

	return os_gpio_write(gc->gpio, out_value, force_value_flag);
#endif
	return 0;
}

/*
 *set the gpio io status
 */
int vin_gpio_set_status(struct v4l2_subdev *sd, enum gpio_type gpio_id,
			unsigned int status)
{
#ifndef FPGA_VER
	struct modules_config *modules = sd_to_modules(sd);
	struct gpio_config gc_def;
	struct gpio_config *gc = NULL;

	if (modules == NULL)
		return -1;

	gc = &modules->sensors->gpio[gpio_id];

	memcpy(&gc_def, gc, sizeof(struct gpio_config));

	if (status == 0)
		gc_def.mul_sel = GPIO_DISABLE;
	if (status == 3)
		gc_def.mul_sel = 3;
	if (os_gpio_set(&gc_def) < 0)
		return -1;
	if (status == 3)
		return 0;

	if (status == 1) {
		hal_gpio_set_direction(gc->gpio, 1);
		hal_gpio_set_data(gc->gpio, 0);
	} else
		hal_gpio_set_direction(gc->gpio, 0);

#endif
	return 0;
}
