/*
 * Copyright (c) 2020 Allwinner Technology Co., Ltd. ALL rights reserved.
 */

#ifndef __SUNXI_HAL_POWER_H__
#define __SUNXI_HAL_POWER_H__
#include <hal_debug.h>
#include <hal_log.h>
#include <hal_gpio.h>
#include <sunxi_hal_common.h>
#include <hal_sem.h>
#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************
 * power structs
 ********************************************************************/
struct power_dev{
	unsigned int flag;
	struct bat_power_ops *bat_ops;
	struct usb_power_ops *usb_ops;
	struct power_supply  *config;
	hal_sem_t irq_schd;
};

struct bat_power_ops{
	int (*get_rest_cap) (struct power_dev *);
	int (*get_coulumb_counter) (struct power_dev *);
	int (*get_bat_present) (struct power_dev *);
	int (*get_bat_online) (struct power_dev *);
	int (*get_bat_status) (struct power_dev *);
	int (*get_bat_health) (struct power_dev *);
	int (*get_vbat) (struct power_dev *);
	int (*get_ibat) (struct power_dev *);
	int (*get_disibat) (struct power_dev *);
	int (*get_temp) (struct power_dev *);
	int (*get_temp_ambient) (struct power_dev *);
	int (*set_chg_cur) (struct power_dev *, int cur);
	int (*set_chg_vol) (struct power_dev *, int vol);
	int (*set_batfet) (struct power_dev *, int onoff);
};

struct usb_power_ops{
	int (*get_usb_status) (struct power_dev *);
	int (*get_usb_ihold) (struct power_dev *);
	int (*get_usb_vhold) (struct power_dev *);
	int (*set_usb_ihold) (struct power_dev *, int cur);
	int (*set_usb_vhold) (struct power_dev *, int vol);
	int (*get_cc_status) (struct power_dev *);
};

/********************************************************************
 * power request_flag
 *
 *         +---> buffer
 *         |
 *         |                --->Rsv
 *         |               /             --->pmu addr
 *         |              /             /        --->twi port
 *         |             /             /        /           --->buffer
 *         |            /             /        /           /
 *      +--------------------------------------------------------+
 *      |31   28|27             20|19     12|11    8|7          0|
 *      +--------------------------------------------------------+
 ********************************************************************/
#define TWI_PORT_SHIFT		8
#define AXP_ADDR_SHIFT		12

#define TWI_PORT(x)		(((x) & TWI_PORT_MASK) >> TWI_PORT_SHIFT)
#define AXP_ADDR(x)		(((x) & AXP_ADDR_MASK) >> AXP_ADDR_SHIFT)

#define TWI_PORT_MASK		0x00f00
#define AXP_ADDR_MASK		0xff000

#define AXP_OF_PROP_SET(_name, _value)\
{\
	power_spy->_name = (int)_value; \
}

enum POWER_TYPE_ENUM{
	NO_POWER,
	AXP2585_POWER,
};

enum POWER_AXP2585_VERSION_ENUM{
	AXP2585_UNKNOWN,
	AXP2585_VERSION_C,
	AXP2585_VERSION_D,
};

enum POWER_SUPPLY_STATUS{
	POWER_SUPPLY_STATUS_UNKNOWN = 0,
	POWER_SUPPLY_STATUS_CHARGING,
	POWER_SUPPLY_STATUS_DISCHARGING,
	POWER_SUPPLY_STATUS_NOT_CHARGING,
	POWER_SUPPLY_STATUS_FULL,
};

enum POWER_SUPPLY_HEALTH{
	POWER_SUPPLY_HEALTH_UNKNOWN = 0,
	POWER_SUPPLY_HEALTH_GOOD,
	POWER_SUPPLY_HEALTH_OVERHEAT,
	POWER_SUPPLY_HEALTH_DEAD,
	POWER_SUPPLY_HEALTH_OVERVOLTAGE,
	POWER_SUPPLY_HEALTH_UNSPEC_FAILURE,
	POWER_SUPPLY_HEALTH_COLD,
	POWER_SUPPLY_HEALTH_WATCHDOG_TIMER_EXPIRE,
	POWER_SUPPLY_HEALTH_SAFETY_TIMER_EXPIRE,
};

struct power_supply {
	u32 pmu_used;
	u32 pmu_id;
	u32 pmu_version;
	u32 pmu_battery_rdc;
	u32 pmu_battery_cap;
	u32 pmu_batdeten;
	u32 pmu_chg_ic_temp;
	u32 pmu_runtime_chgcur;
	u32 pmu_suspend_chgcur;
	u32 pmu_shutdown_chgcur;
	u32 pmu_init_chgvol;
	u32 pmu_init_chgend_rate;
	u32 pmu_init_chg_enabled;
	u32 pmu_init_bc_en;
	u32 pmu_init_adc_freq;
	u32 pmu_init_adcts_freq;
	u32 pmu_init_chg_pretime;
	u32 pmu_init_chg_csttime;
	u32 pmu_batt_cap_correct;
	u32 pmu_chg_end_on_en;
	u32 pmu_ocv_coulumb_100;

	u32 pmu_bat_para[32];

	u32 pmu_ac_vol;
	u32 pmu_ac_cur;
	u32 pmu_usbpc_vol;
	u32 pmu_usbpc_cur;
	u32 pmu_usbad_vol;
	u32 pmu_usbad_cur;
	u32 pmu_pwroff_vol;
	u32 pmu_pwron_vol;
	u32 pmu_powkey_off_time;
	u32 pmu_powkey_off_en;
	u32 pmu_powkey_off_delay_time;
	u32 pmu_powkey_off_func;
	u32 pmu_powkey_long_time;
	u32 pmu_powkey_on_time;
	u32 pmu_pwrok_time;
	u32 pmu_pwrnoe_time;
	u32 pmu_reset_shutdown_en;
	u32 pmu_battery_warning_level1;
	u32 pmu_battery_warning_level2;
	u32 pmu_restvol_adjust_time;
	u32 pmu_ocv_cou_adjust_time;
	u32 pmu_chgled_func;
	u32 pmu_chgled_type;
	u32 pmu_vbusen_func;
	u32 pmu_reset;
	u32 pmu_irq_wakeup;
	u32 pmu_hot_shutdown;
	u32 pmu_inshort;
	u32 power_start;
	u32 pmu_as_slave;
	u32 pmu_bat_unused;
	u32 pmu_ocv_en;
	u32 pmu_cou_en;
	u32 pmu_update_min_time;

	u32 pmu_bat_temp_enable;
	u32 pmu_bat_charge_ltf;
	u32 pmu_bat_charge_htf;
	u32 pmu_bat_shutdown_ltf;
	u32 pmu_bat_shutdown_htf;
	u32 pmu_bat_temp_para[16];

	u32 pmu_irq_no;
	u32 pmu_bc12_en;
	gpio_pin_t pmu_irq_pin;
};

/********************************************************************
 * power init
 ********************************************************************/
int hal_power_twi_init(struct power_dev *rdev);
int hal_power_init(void);
int hal_power_get(struct power_dev *rdev);
int hal_power_put(struct power_dev *rdev);

/********************************************************************
 * power ops
 ********************************************************************/

int hal_power_get_bat_cap(struct power_dev *rdev);
int hal_power_get_coulumb_counter(struct power_dev *rdev);
int hal_power_get_bat_present(struct power_dev *rdev);
int hal_power_get_bat_online(struct power_dev *rdev);
int hal_power_get_bat_status(struct power_dev *rdev);
int hal_power_get_bat_health(struct power_dev *rdev);
int hal_power_get_vbat(struct power_dev *rdev);
int hal_power_get_ibat(struct power_dev *rdev);
int hal_power_get_disibat(struct power_dev *rdev);
int hal_power_get_temp(struct power_dev *rdev);
int hal_power_get_temp_ambient(struct power_dev *rdev);
int hal_power_set_chg_cur(struct power_dev *rdev, int cur);
int hal_power_set_chg_vol(struct power_dev *rdev, int vol);
int hal_power_set_batfet(struct power_dev *rdev, int onoff);

int hal_power_get_usb_status(struct power_dev *rdev);
int hal_power_get_usb_ihold(struct power_dev *rdev);
int hal_power_get_usb_vhold(struct power_dev *rdev);
int hal_power_set_usb_ihold(struct power_dev *rdev, int cur);
int hal_power_set_usb_vhold(struct power_dev *rdev, int vol);
int hal_power_get_cc_status(struct power_dev *rdev);

#ifdef __cplusplus
}
#endif

#endif /* __SUNXI_HAL_POWER_H__ */
