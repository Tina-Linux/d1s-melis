/*
 * Copyright (C) 2020 allwinnertech Ltd.
 */

#include <sunxi_hal_power.h>
#include <sunxi_hal_power_private.h>
#include "../axp2585.h"
#include <stdlib.h>

struct bat_power_ops bat_ops;
struct usb_power_ops usb_ops;
static int power_init = -1;
static struct power_dev *rdev;

#define POWER_PMU_ADDR 0x34
#define POWER_TWI_PORT 1

void power_function_set(struct power_supply *power_spy)
{
	AXP_OF_PROP_SET(pmu_irq_wakeup, 1);
	AXP_OF_PROP_SET(pmu_hot_shutdown, 1);
	AXP_OF_PROP_SET(pmu_bat_unused, 0);
	AXP_OF_PROP_SET(pmu_irq_pin, GPIOA(27));

	/* usb_power_supply */
	AXP_OF_PROP_SET(pmu_usbad_vol, 4600);
	AXP_OF_PROP_SET(pmu_usbad_cur, 1500);

	AXP_OF_PROP_SET(pmu_usbpc_vol, 4600);
	AXP_OF_PROP_SET(pmu_usbpc_cur, 500);

	/* battery_power_supply */
	AXP_OF_PROP_SET(pmu_chg_ic_temp, 0);
	AXP_OF_PROP_SET(pmu_battery_rdc, 100);
	AXP_OF_PROP_SET(pmu_battery_cap, 3568);
	AXP_OF_PROP_SET(pmu_runtime_chgcur, 900);
	AXP_OF_PROP_SET(pmu_suspend_chgcur, 1200);
	AXP_OF_PROP_SET(pmu_shutdown_chgcur, 1200);
	AXP_OF_PROP_SET(pmu_init_chgvol, 4200);
	AXP_OF_PROP_SET(pmu_init_chg_pretime, 50);
	AXP_OF_PROP_SET(pmu_init_chg_csttime, 1200);
	AXP_OF_PROP_SET(pmu_chgled_type, 0);
	AXP_OF_PROP_SET(pmu_init_bc_en, 1);
	AXP_OF_PROP_SET(pmu_bat_para[0], 0);
	AXP_OF_PROP_SET(pmu_bat_para[1], 0);
	AXP_OF_PROP_SET(pmu_bat_para[2], 0);
	AXP_OF_PROP_SET(pmu_bat_para[3], 0);
	AXP_OF_PROP_SET(pmu_bat_para[4], 0);
	AXP_OF_PROP_SET(pmu_bat_para[5], 0);
	AXP_OF_PROP_SET(pmu_bat_para[6], 1);
	AXP_OF_PROP_SET(pmu_bat_para[7], 1);
	AXP_OF_PROP_SET(pmu_bat_para[8], 2);
	AXP_OF_PROP_SET(pmu_bat_para[9], 4);
	AXP_OF_PROP_SET(pmu_bat_para[10], 5);
	AXP_OF_PROP_SET(pmu_bat_para[11], 12);
	AXP_OF_PROP_SET(pmu_bat_para[12], 19);
	AXP_OF_PROP_SET(pmu_bat_para[13], 32);
	AXP_OF_PROP_SET(pmu_bat_para[14], 41);
	AXP_OF_PROP_SET(pmu_bat_para[15], 45);
	AXP_OF_PROP_SET(pmu_bat_para[16], 48);
	AXP_OF_PROP_SET(pmu_bat_para[17], 51);
	AXP_OF_PROP_SET(pmu_bat_para[18], 54);
	AXP_OF_PROP_SET(pmu_bat_para[19], 59);
	AXP_OF_PROP_SET(pmu_bat_para[20], 63);
	AXP_OF_PROP_SET(pmu_bat_para[21], 68);
	AXP_OF_PROP_SET(pmu_bat_para[22], 71);
	AXP_OF_PROP_SET(pmu_bat_para[23], 74);
	AXP_OF_PROP_SET(pmu_bat_para[24], 78);
	AXP_OF_PROP_SET(pmu_bat_para[25], 81);
	AXP_OF_PROP_SET(pmu_bat_para[26], 82);
	AXP_OF_PROP_SET(pmu_bat_para[27], 84);
	AXP_OF_PROP_SET(pmu_bat_para[28], 88);
	AXP_OF_PROP_SET(pmu_bat_para[29], 92);
	AXP_OF_PROP_SET(pmu_bat_para[30], 96);
	AXP_OF_PROP_SET(pmu_bat_para[31], 100);
	AXP_OF_PROP_SET(pmu_bat_temp_enable, 0);
	AXP_OF_PROP_SET(pmu_bat_charge_ltf, 2261);
	AXP_OF_PROP_SET(pmu_bat_charge_htf, 388);
	AXP_OF_PROP_SET(pmu_bat_shutdown_ltf, 3200);
	AXP_OF_PROP_SET(pmu_bat_shutdown_htf, 237);
	AXP_OF_PROP_SET(pmu_bat_temp_para[0], 7466);
	AXP_OF_PROP_SET(pmu_bat_temp_para[1], 4480);
	AXP_OF_PROP_SET(pmu_bat_temp_para[2], 3518);
	AXP_OF_PROP_SET(pmu_bat_temp_para[3], 2786);
	AXP_OF_PROP_SET(pmu_bat_temp_para[4], 2223);
	AXP_OF_PROP_SET(pmu_bat_temp_para[5], 1788);
	AXP_OF_PROP_SET(pmu_bat_temp_para[6], 1448);
	AXP_OF_PROP_SET(pmu_bat_temp_para[7], 969);
	AXP_OF_PROP_SET(pmu_bat_temp_para[8], 664);
	AXP_OF_PROP_SET(pmu_bat_temp_para[9], 466);
	AXP_OF_PROP_SET(pmu_bat_temp_para[10], 393);
	AXP_OF_PROP_SET(pmu_bat_temp_para[11], 333);
	AXP_OF_PROP_SET(pmu_bat_temp_para[12], 283);
	AXP_OF_PROP_SET(pmu_bat_temp_para[13], 242);
	AXP_OF_PROP_SET(pmu_bat_temp_para[14], 179);
	AXP_OF_PROP_SET(pmu_bat_temp_para[15], 134);
}

int hal_power_init(void)
{
	struct power_supply power_spy;
	int pmu_id;

	rdev = malloc(sizeof(struct power_dev));
	if (!rdev) {
		printf("%s:malloc rdev failed\n", __func__);
		return -1;
	}

	rdev->config = malloc(sizeof(struct power_supply));
	if (!rdev->config) {
		printf("%s:malloc rdev.config failed\n", __func__);
		return -1;
	}
	rdev->flag = 0;
	rdev->flag |= (POWER_PMU_ADDR << AXP_ADDR_SHIFT) |
					(POWER_TWI_PORT << TWI_PORT_SHIFT);
	hal_power_twi_init(rdev);
	axp2585_get_power(rdev);
	pmu_id = axp2585_get_version(rdev);
	if (pmu_id == 0) {
		power_function_set(rdev->config);
		hal_gpio_pinmux_set_function(GPIO_PA27, GPIO_MUXSEL_EINT);
		axp2585_init_power(rdev);
		power_init = 1;
	} else {
		pr_err("hal power init axp2585 failed\n");
		power_init = 0;
	}

	return 0;
}

void hal_power_deinit(void)
{
	free(rdev);
}

int hal_power_get(struct power_dev *rdev)
{
	struct power_supply power_spy;
	if(power_init != 1) {
		pr_err("hal power init failed\n");
		return 0;
	}
	rdev->flag = 0;
	rdev->flag |= (POWER_PMU_ADDR << AXP_ADDR_SHIFT) |
					(POWER_TWI_PORT << TWI_PORT_SHIFT);
	axp2585_get_power(rdev);
	rdev->config = malloc(sizeof(struct power_supply));
	if (!rdev->config) {
		printf("%s:malloc failed\n", __func__);
		return -1;
	}
	axp2585_get_version(rdev);

	power_function_set(rdev->config);

	return 0;
}

int hal_power_put(struct power_dev *rdev)
{
	free(rdev->config);

	return 0;
}

