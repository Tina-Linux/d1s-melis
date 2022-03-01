
/*
 * config.c for device tree and sensor list parser.
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 * Yang Feng <yangfeng@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <hal_gpio.h>
#include <sunxi_hal_regulator.h>
#include "config.h"
#include "../platform/platform_cfg.h"

#ifdef CONFIG_SOC_SUN8IW19P1
#include "vin_config_sun8iw19p1.h"
#endif

int parse_modules_from_device_tree(struct vin_md *vind)
{
	struct vin_power *power = NULL;
	int i = 0;
	int j = 0;

	for (j = 0; j < VIN_MAX_DEV; j++) {
		for (i = 0; i < MAX_POW_NUM; i++) {
			power = &sensors_default[j].power[i];

			if (power->regulator_id < AXP2101_ID_MAX && power->power_vol != 0) {
				power->pmic = rt_calloc(1, sizeof(struct regulator_dev));
				if (power->pmic == NULL) {
					vin_log(VIN_LOG_CONFIG, "calloc %d error\n", sizeof(struct regulator_dev));
					return -1;
				}
				hal_regulator_get(REGULATOR_GET(AXP2101_REGULATOR, power->regulator_id), power->pmic);
			}
		}
	}

	for (i = 0; i < VIN_MAX_DEV; i++) {
		vind->modules[i].sensors = &sensors_default[i];
	}

	return 0;
}

void *sunxi_vin_get_csi_base(int id)
{
	return (void *)vin_csi_base[id];
}

void *sunxi_vin_get_scaler_base(int id)
{
	return (void *)vin_scaler_base[id];
}

void *sunxi_vin_get_isp_base(int id)
{
	return (void *)vin_isp_base[id];
}

int sunxi_vin_get_isp_irq(int id)
{
	return vin_isp_irq[id];
}

void *sunxi_vin_get_mipi_base(int id)
{
	return (void *)vin_mipi_base[id];
}

void *sunxi_vin_get_csi_top_base(void)
{
	return (void *)CSI_TOP_REGS_BASE;
}

void *sunxi_vin_get_csi_ccu_base(void)
{
	return (void *)CSI_CCU_REGS_BASE;
}

int sunxi_vin_get_csi_top_irq(void)
{
	return SUNXI_IRQ_CSI_TOP_PKT;
}

struct vin_clk_info *sunxi_vin_get_clk(int id)
{
	return &vind_default_clk[id];
}

struct vin_mclk_info *sunxi_vin_get_mclk(int id)
{
	return &vind_default_mclk[id];
}

struct vin_clk_info *sunxi_vin_get_isp_clk(int id)
{
	return &vind_default_isp_clk[id];
}

struct vin_core *sunxi_vin_get_vin_core(int id)
{
	return &sunxi_vinc[id];
}
