/*
 * axp2585
 */
#include <sunxi_hal_power.h>
#include <sunxi_hal_power_private.h>
#include "axp2585.h"
#include "type.h"
#ifdef CONFIG_COMPONENTS_PM
#include <pm_devops.h>
#endif
#include <hal_interrupt.h>
#include <hal_atomic.h>
#include <hal_clk.h>
#include <hal_sem.h>
#include <hal_timer.h>

uint32_t set_current_limit;
uint32_t set_current_limit_flags;
static hal_spinlock_t set_current_limit_lock;

struct power_dev rdev_g;
static u8 irq_val;

/* battery ops */
static int axp2585_get_rest_cap(struct power_dev *rdev)
{
	u8 val, temp_val[2], tmp[2];
	u8 ocv_percent = 0;
	u8 coulomb_percent = 0;
	int batt_max_cap, coulumb_counter;
	int rest_vol;
	bool bat_charging;

	hal_power_byte_read(rdev,AXP2585_CAP, &val);
	if (!(val & 0x80))
		return 0;
	rest_vol = (int) (val & 0x7F);
	hal_power_byte_read(rdev, 0xe4, &tmp[0]);
	if (tmp[0] & 0x80) {
		ocv_percent = tmp[0] & 0x7f;
//		pr_info("ocv_percent = %d\n", ocv_percent);
	}
	hal_power_byte_read(rdev, 0xe5, &tmp[0]);
	if (tmp[0] & 0x80) {
		coulomb_percent = tmp[0] & 0x7f;
//		pr_info("coulomb_percent = %d\n", coulomb_percent);
	}
	hal_power_byte_read(rdev, AXP2585_STATUS, &val);
	bat_charging = ((((val & (7 << 2)) > 0))
			&& ((val & (7 << 2)) < 0x14)) ? 1 : 0;
	if (ocv_percent == 100 && bat_charging == 0 && rest_vol == 99
		&& (val & BIT(1))) {
		hal_power_byte_update(rdev, AXP2585_COULOMB_CTL, 0x0, 0x80);
		hal_power_byte_update(rdev, AXP2585_COULOMB_CTL, 0x80, 0x80);
		rest_vol = 100;
	}
	hal_power_byte_bulk_read(rdev, 0xe2, temp_val, 2);
	coulumb_counter = (((temp_val[0] & 0x7f) << 8) + temp_val[1])
						* 1456 / 1000;
//	pr_info("coulumb_counter = %d\n", coulumb_counter);
	hal_power_byte_bulk_read(rdev, 0xe0, temp_val, 2);
	batt_max_cap = (((temp_val[0] & 0x7f) << 8) + temp_val[1])
						* 1456 / 1000;
//	pr_info("batt_max_cap = %d\n", batt_max_cap);
	return rest_vol;
}

static int axp2585_get_coulumb_counter(struct power_dev *rdev)
{
	u8 temp_val[2];
	int coulumb_counter;

	hal_power_byte_bulk_read(rdev, 0xe2, temp_val, 2);
	pr_err("tmp[0]:%x, tmp[1]:%x\n", temp_val[0], temp_val[1]);
	coulumb_counter = (((temp_val[0] & 0x7f) << 8) + temp_val[1])
						* 1456 / 1000;
	return coulumb_counter;
}

static int axp2585_get_bat_present(struct power_dev *rdev)
{
	u8 val;
	struct power_supply *power_spy = rdev->config;

	if (power_spy->pmu_bat_unused == 0) {
		if (power_spy->pmu_version == AXP2585_VERSION_D) {
			hal_power_byte_read(rdev, 0x00, &val);
			if (!(val & 0x02))
				return 1;

			hal_power_byte_read(rdev, 0xbc, &val);
			if (val & 0xff)
				return 1;

			hal_power_byte_read(rdev, 0xbd, &val);
			if (val & 0xff)
				return 1;

			return 0;
		} else {
			hal_power_byte_read(rdev, 0x02, &val);
			if ((val & (3 << 3)) == 0x18)
				return 1;
			else
				return 0;
		}
	} else {
		return 0;
	}
}

static int axp2585_get_direction(struct power_dev *rdev)
{
	u8 val;
	hal_power_byte_read(rdev, 0x02, &val);
	if (val & 0x01)
		return 1;
	else
		return 0;
}

static int axp2585_get_bat_status(struct power_dev *rdev)
{
	u8 data;
	int ret;
	bool bat_det, bat_charging;
	unsigned int rest_cap;
	bool usb_valid;

	ret = hal_power_byte_read(rdev, AXP2585_STATUS, &data);
	if (ret < 0) {
		pr_err("error read AXP210X_COM_STAT1\n");
		return ret;
	}

	bat_charging = ((((data & (7 << 2)) > 0))
			&& ((data & (7 << 2)) < 0x14)) ? 1 : 0;

	usb_valid = data & (1 << 1);

	bat_det = axp2585_get_bat_present(rdev);

	rest_cap = axp2585_get_rest_cap(rdev);

	if (bat_det) {
		if (rest_cap == 100)
			return POWER_SUPPLY_STATUS_FULL;
		else if (bat_charging)
			return POWER_SUPPLY_STATUS_CHARGING;
		else
			return POWER_SUPPLY_STATUS_NOT_CHARGING;
	} else {
		return POWER_SUPPLY_STATUS_UNKNOWN;
	}

	return 0;
}

static int axp2585_get_bat_health(struct power_dev *rdev)
{
	return POWER_SUPPLY_HEALTH_GOOD;
}

static inline int axp2585_vbat_to_mV(u32 reg)
{
	return ((int)(((reg >> 8) << 4) | (reg & 0x000F))) * 1200 / 1000;
}

static inline int axp2585_ibat_to_mA(u32 reg)
{
	return (int)((((reg >> 8) << 4) | (reg & 0x000F)) << 1);
}

static inline int axp2585_icharge_to_mA(u32 reg)
{
	return (int)((((reg >> 8) << 4) | (reg & 0x000F)) << 1);
}

static int axp2585_get_vbat(struct power_dev *rdev)
{
	u8 tmp[2];
	u32 res;

	hal_power_byte_bulk_read(rdev, AXP2585_VBATH_REG, tmp, 2);
	res = (tmp[0] << 8) | tmp[1];

	return axp2585_vbat_to_mV(res);
}

static int axp2585_get_ibat(struct power_dev *rdev)
{
	u8 tmp[2];
	u32 res;

	hal_power_byte_bulk_read(rdev,AXP2585_IBATH_REG, tmp, 2);
	res = (tmp[0] << 8) | tmp[1];

	return axp2585_icharge_to_mA(res);
}

static int axp2585_get_disibat(struct power_dev *rdev)
{
	u8 tmp[2];
	u32 dis_res;

	hal_power_byte_bulk_read(rdev,AXP2585_DISIBATH_REG, tmp, 2);
	dis_res = (tmp[0] << 8) | tmp[1];

	return axp2585_ibat_to_mA(dis_res);
}

static inline int axp_vts_to_mV(u16 reg)
{
	return ((int)(((reg >> 8) << 4) | (reg & 0x000F))) * 800 / 1000;
}

static inline int axp_vts_to_temp(int data, struct power_supply *power_spy)
{
	int temp;

	if (data < 80 || power_spy->pmu_bat_temp_enable != 0)
		return 30;
	else if (data < power_spy->pmu_bat_temp_para[15])
		return 80;
	else if (data <= power_spy->pmu_bat_temp_para[14]) {
		temp = 70 + (power_spy->pmu_bat_temp_para[14]-data)*10/
		(power_spy->pmu_bat_temp_para[14] - power_spy->pmu_bat_temp_para[15]);
	} else if (data <= power_spy->pmu_bat_temp_para[13]) {
		temp = 60 + (power_spy->pmu_bat_temp_para[13]-data)*10/
		(power_spy->pmu_bat_temp_para[13] - power_spy->pmu_bat_temp_para[14]);
	} else if (data <= power_spy->pmu_bat_temp_para[12]) {
		temp = 55 + (power_spy->pmu_bat_temp_para[12]-data)*5/
		(power_spy->pmu_bat_temp_para[12] - power_spy->pmu_bat_temp_para[13]);
	} else if (data <= power_spy->pmu_bat_temp_para[11]) {
		temp = 50 + (power_spy->pmu_bat_temp_para[11]-data)*5/
		(power_spy->pmu_bat_temp_para[11] - power_spy->pmu_bat_temp_para[12]);
	} else if (data <= power_spy->pmu_bat_temp_para[10]) {
		temp = 45 + (power_spy->pmu_bat_temp_para[10]-data)*5/
		(power_spy->pmu_bat_temp_para[10] - power_spy->pmu_bat_temp_para[11]);
	} else if (data <= power_spy->pmu_bat_temp_para[9]) {
		temp = 40 + (power_spy->pmu_bat_temp_para[9]-data)*5/
		(power_spy->pmu_bat_temp_para[9] - power_spy->pmu_bat_temp_para[10]);
	} else if (data <= power_spy->pmu_bat_temp_para[8]) {
		temp = 30 + (power_spy->pmu_bat_temp_para[8]-data)*10/
		(power_spy->pmu_bat_temp_para[8] - power_spy->pmu_bat_temp_para[9]);
	} else if (data <= power_spy->pmu_bat_temp_para[7]) {
		temp = 20 + (power_spy->pmu_bat_temp_para[7]-data)*10/
		(power_spy->pmu_bat_temp_para[7] - power_spy->pmu_bat_temp_para[8]);
	} else if (data <= power_spy->pmu_bat_temp_para[6]) {
		temp = 10 + (power_spy->pmu_bat_temp_para[6]-data)*10/
		(power_spy->pmu_bat_temp_para[6] - power_spy->pmu_bat_temp_para[7]);
	} else if (data <= power_spy->pmu_bat_temp_para[5]) {
		temp = 5 + (power_spy->pmu_bat_temp_para[5]-data)*5/
		(power_spy->pmu_bat_temp_para[5] - power_spy->pmu_bat_temp_para[6]);
	} else if (data <= power_spy->pmu_bat_temp_para[4]) {
		temp = 0 + (power_spy->pmu_bat_temp_para[4]-data)*5/
		(power_spy->pmu_bat_temp_para[4] - power_spy->pmu_bat_temp_para[5]);
	} else if (data <= power_spy->pmu_bat_temp_para[3]) {
		temp = -5 + (power_spy->pmu_bat_temp_para[3]-data)*5/
		(power_spy->pmu_bat_temp_para[3] - power_spy->pmu_bat_temp_para[4]);
	} else if (data <= power_spy->pmu_bat_temp_para[2]) {
		temp = -10 + (power_spy->pmu_bat_temp_para[2]-data)*5/
		(power_spy->pmu_bat_temp_para[2] - power_spy->pmu_bat_temp_para[3]);
	} else if (data <= power_spy->pmu_bat_temp_para[1]) {
		temp = -15 + (power_spy->pmu_bat_temp_para[1]-data)*5/
		(power_spy->pmu_bat_temp_para[1] - power_spy->pmu_bat_temp_para[2]);
	} else if (data <= power_spy->pmu_bat_temp_para[0]) {
		temp = -25 + (power_spy->pmu_bat_temp_para[0]-data)*10/
		(power_spy->pmu_bat_temp_para[0] - power_spy->pmu_bat_temp_para[1]);
	} else
		temp = -25;
	return temp;
}

static int axp2585_get_bat_temp(struct power_dev *rdev)
{
	unsigned char temp_val[2];
	unsigned short ts_res;
	int bat_temp_mv, bat_temp;
	int ret = 0;
	struct power_supply *power_spy = rdev->config;

	ret = hal_power_byte_bulk_read(rdev, AXP2585_VTS_RES, temp_val, 2);
	if (ret < 0)
		return ret;

	ts_res = ((unsigned short) temp_val[0] << 8) | temp_val[1];
	bat_temp_mv = axp_vts_to_mV(ts_res);
	bat_temp = axp_vts_to_temp(bat_temp_mv, power_spy);

//	pr_info("bat_temp: %d\n", bat_temp);

	return bat_temp;
}

static int axp2585_get_temp_ambient(struct power_dev *rdev)
{
	unsigned char temp_val[2];
	unsigned short ts_res;
	float tdie = 0;
	int ret = 0;

	ret = hal_power_byte_bulk_read(rdev, AXP2585_TDIE_REG, temp_val, 2);
	if (ret < 0)
		return ret;

	temp_val[1] &= 0x0f;
	ts_res = ((unsigned short) temp_val[0] << 4) | temp_val[1];
	tdie = -267.7 + 0.10625 * ts_res;

	return (int)tdie;
}

static int axp2585_set_chg_cur(struct power_dev *rdev, int cur)
{
	uint8_t tmp = 0;
	tmp = (cur) / 64;
	if (tmp > 0x3f)
		tmp = 0x3f;
	hal_power_byte_update(rdev, 0x8b, tmp, 0x3F);
	return 0;
}

static int axp2585_set_chg_vol(struct power_dev *rdev, int vol)
{
	uint8_t tmp = 0;

	if (vol > 3840 && vol < 4608)
		tmp = (vol - 3840)/16;
	else {
		pr_warn("unsupported voltage: %dmv, use default 4200mv\n", vol);
		tmp = (4200 - 3840)/16;
	}
	hal_power_byte_update(rdev, 0x8c, tmp << 2, 0xfc);
	return 0;
}

static int axp2585_set_batfet(struct power_dev *rdev, int onoff)
{
	if (onoff == 1)
		hal_power_byte_update(rdev, AXP2585_ILIMIT, 0x80, 0x80);
	else
		hal_power_byte_update(rdev, AXP2585_ILIMIT, 0x00, 0x80);
}

/* usb ops */
static int axp2585_read_vbus_state(struct power_dev *rdev)
{
	u8 data;
	int ret = 0;

	hal_power_byte_read(rdev, AXP2585_STATUS, &data);

	ret = !!(data & (1 << 1));

	return ret;
}

static int axp2585_get_usb_ihold(struct power_dev *rdev)
{
	u8 data;
	int ret = 0;

	hal_power_byte_read(rdev, 0x10, &data);

	data &= 0x3F;

	ret = data * 50 + 100;

	return ret;
}

static int axp2585_get_usb_vhold(struct power_dev *rdev)
{
	u8 data;
	int ret = 0;

	hal_power_byte_read(rdev, 0x11, &data);	
	data &= 0x0F;

	ret = data * 80 + 3880;

	return ret;
}

static int _axp2585_set_usb_ihold(struct power_dev *rdev, int cur)
{
	u8 tmp;

	if (cur) {
		if (cur >= 100 && cur <= 3250) {
			tmp = (cur - 100)/50;
			hal_power_byte_update(rdev, 0x10, tmp, 0x3f);
		}
	}

	return 0;
}

static int axp2585_set_usb_ihold(struct power_dev *rdev, int cur)
{
	_axp2585_set_usb_ihold(rdev, cur);
	set_current_limit_flags = hal_spin_lock_irqsave(&set_current_limit_lock);
	set_current_limit = 1;
	hal_spin_unlock_irqrestore(&set_current_limit_lock, set_current_limit_flags);

	return 0;
}

static int axp2585_set_usb_vhold(struct power_dev *rdev, int vol)
{
	u8 tmp;

	if (vol) {
		if (vol >= 3880 && vol <= 5080) {
			tmp = (vol - 3880)/80;
			hal_power_byte_update(rdev, 0x11, tmp, 0x0f);
		}
	}

	return 0;
}

#if defined(CONFIG_AXP2585_TYPE_C)
static s32 axp2585_get_cc_status(struct power_dev *rdev)
{
	u8 value = 0;
	int ret = 0;

	hal_power_byte_read(rdev, AXP2585_CC_STATUS0, &value);
	if ((value & 0x0f) == 0x06)
		ret = 0;
	else
		ret = 1;

	return ret;
}
#endif

struct bat_power_ops axp2585_bat_power_ops = {
	.get_rest_cap         = axp2585_get_rest_cap,
	.get_coulumb_counter  = axp2585_get_coulumb_counter,
	.get_bat_present      = axp2585_get_bat_present,
	.get_bat_online       = axp2585_get_direction,
	.get_bat_status       = axp2585_get_bat_status,
	.get_bat_health       = axp2585_get_bat_health,
	.get_vbat             = axp2585_get_vbat,
	.get_ibat             = axp2585_get_ibat,
	.get_disibat          = axp2585_get_disibat,
	.get_temp             = axp2585_get_bat_temp,
	.get_temp_ambient     = axp2585_get_temp_ambient,
	.set_chg_cur          = axp2585_set_chg_cur,
	.set_chg_vol          = axp2585_set_chg_vol,
	.set_batfet           = axp2585_set_batfet,
};

struct usb_power_ops axp2585_usb_power_ops = {
	.get_usb_status  = axp2585_read_vbus_state,
	.get_usb_ihold   = axp2585_get_usb_ihold,
	.get_usb_vhold   = axp2585_get_usb_vhold,
	.set_usb_ihold   = axp2585_set_usb_ihold,
	.set_usb_vhold   = axp2585_set_usb_vhold,

#if defined(CONFIG_AXP2585_TYPE_C)
	.get_cc_status     = axp2585_get_cc_status,
#endif
};

#ifdef CONFIG_COMPONENTS_PM
static int axp2585_bat_power_suspend(struct pm_device *dev, suspend_mode_t mode)
{
	struct power_dev *rdev = &rdev_g;
	struct power_supply *power_spy = rdev->config;

	axp2585_set_chg_cur(rdev, power_spy->pmu_suspend_chgcur);

	return 0;
}
static int axp2585_bat_power_resume(struct pm_device *dev, suspend_mode_t mode)
{
	struct power_dev *rdev = &rdev_g;
	struct power_supply *power_spy = rdev->config;

	axp2585_set_chg_cur(rdev, power_spy->pmu_runtime_chgcur);

	return 0;
}

static struct pm_devops axp2585_bat_power_devops = {
	.suspend = axp2585_bat_power_suspend,
	.resume = axp2585_bat_power_resume,
};

static struct pm_device pm_axp2585_bat_power = {
	.name = "axp2585_bat_power",
	.ops = &axp2585_bat_power_devops,
};
#endif

static void axp2585_usb_set_current_fsm(struct power_dev *rdev)
{
	struct power_supply *power_spy = rdev->config;

	hal_msleep(2 * 1000);

	if (set_current_limit) {
		pr_info("current limit setted: usb pc type\n");
	} else {
		_axp2585_set_usb_ihold(rdev, power_spy->pmu_usbad_cur);
		pr_info("current limit not set: usb adapter type\n");
	}
}

static int axp2585_irq_handler_usb_in(struct power_dev *rdev)
{
	struct power_supply *power_spy = rdev->config;

	_axp2585_set_usb_ihold(rdev, power_spy->pmu_usbpc_cur);
	set_current_limit_flags = hal_spin_lock_irqsave(&set_current_limit_lock);
	set_current_limit = 0;
	hal_spin_unlock_irqrestore(&set_current_limit_lock, set_current_limit_flags);
	axp2585_usb_set_current_fsm(rdev);

	return 0;
}


static int axp2585_clean_all_irq(struct power_dev *rdev)
{
	int i = 0, ret = 0;
	uint8_t temp = 0;

	for (i = 0;i < 6; i++) {
		temp = 0x48 + i;
		ret = hal_power_byte_write(rdev, temp, 0xff);
		if (ret != 0) {
			printf("axp2585 clean irq 0x%x failed\n", temp);
			return -1;
		}
	}
	return 0;
}

static void axp_irq_task(void * pvParameters)
{
	struct power_dev *rdev = (struct power_dev *)pvParameters;

	while (1) {
		hal_sem_wait(rdev->irq_schd);

		irq_val = 0;
		hal_power_byte_read(rdev, AXP2585_INTSTS3, &irq_val);

		axp2585_clean_all_irq(rdev);

		hal_msleep(10);
	}
}

static hal_irqreturn_t axp2585_irq_handler(void *dev)
{
	struct power_dev *rdev = (struct power_dev *)dev;
	struct power_supply *power_spy = rdev->config;

	hal_sem_post(rdev->irq_schd);

	if (irq_val &  BIT(7))
		axp2585_irq_handler_usb_in(rdev);

	return HAL_IRQ_OK;
}

/* init chip & power setting*/
int axp2585_init_power(struct power_dev *rdev)
{
	u8 val;
	TaskHandle_t task;
	int cur_coulomb_counter, rdc;
	struct power_supply *power_spy = rdev->config;
	int i, update_min_times[8] = {30, 60, 120, 164, 0, 5, 10, 20};
	int ocv_cou_adjust_time[4] = {60, 120, 15, 30};
	u8 ocv_cap_reg = 0xC0;

	/*Init IRQ wakeup en*/
	if (power_spy->pmu_irq_wakeup)
		hal_power_byte_update(rdev, 0x17, 0x10, 0x10); /* enable */
	else
		hal_power_byte_update(rdev, 0x17, 0x00, 0x10); /* disable */

	/*Init PMU Over Temperature protection*/
	if (power_spy->pmu_hot_shutdown)
		hal_power_byte_update(rdev, 0xf3, 0x08, 0x08); /* enable */
	else
		hal_power_byte_update(rdev, 0xf3, 0x00, 0x08); /* disable */

	/*enable send seq to pmu when power off */
	hal_power_byte_update(rdev, 0x16, 0x40, 0xc0);

	/*set chg time */
	if (power_spy->pmu_init_chg_pretime < 40)
		power_spy->pmu_init_chg_pretime = 40;
	val = (power_spy->pmu_init_chg_pretime - 40)/10;
	if (val >= 3)
		val = 3;
	val = 0x80 + (val<<5);
	hal_power_byte_update(rdev, 0x8e, val, 0xe0);

	if (power_spy->pmu_init_chg_csttime <= 60 * 5)
		val = 0;
	else if (power_spy->pmu_init_chg_csttime <= 60 * 8)
		val = 1;
	else if (power_spy->pmu_init_chg_csttime <= 60 * 12)
		val = 2;
	else if (power_spy->pmu_init_chg_csttime <= 60 * 20)
		val = 3;
	else
		val = 3;
	val = (val << 1) + 0x01;
	hal_power_byte_update(rdev, 0x8d, val, 0x07);

	/* adc set */
	val = AXP2585_ADC_BATVOL_ENABLE | AXP2585_ADC_BATCUR_ENABLE;
	if (power_spy->pmu_bat_temp_enable != 0)
		val = val | AXP2585_ADC_TSVOL_ENABLE;
	hal_power_byte_update(rdev, AXP2585_ADC_CONTROL, val,
					AXP2585_ADC_BATVOL_ENABLE
					| AXP2585_ADC_BATCUR_ENABLE
					| AXP2585_ADC_TSVOL_ENABLE);

	hal_power_byte_read(rdev, AXP2585_TS_PIN_CONTROL, &val);
	switch (power_spy->pmu_init_adc_freq / 100) {
	case 1:
		val &= ~(3 << 5);
		break;
	case 2:
		val &= ~(3 << 5);
		val |= 1 << 5;
		break;
	case 4:
		val &= ~(3 << 5);
		val |= 2 << 5;
		break;
	case 8:
		val &= ~(3 << 5);
		val |= 3 << 5;
		break;
	default:
		break;
	}

	if (power_spy->pmu_bat_temp_enable != 0)
		val &= ~(1 << 7);
	hal_power_byte_write(rdev, AXP2585_TS_PIN_CONTROL, val);

	/* bat para */
	hal_power_byte_write(rdev, AXP2585_WARNING_LEVEL,
		((power_spy->pmu_battery_warning_level1 - 5) << 4)
		+ power_spy->pmu_battery_warning_level2);

	if (power_spy->pmu_init_chgvol < 3840)
		power_spy->pmu_init_chgvol = 3840;
	val = (power_spy->pmu_init_chgvol - 3840)/16;
	if (val > 0x30)
		val = 0x30;
	val <<= 2;
	hal_power_byte_update(rdev, AXP2585_CHARGE_CONTROL2, val, 0xfc);

	for (i = 0; i < 32; i++) {
		hal_power_byte_write(rdev, ocv_cap_reg, power_spy->pmu_bat_para[i]);
		ocv_cap_reg += 0x01;
	}

	/*Init CHGLED function*/
	if (power_spy->pmu_chgled_func)
		hal_power_byte_update(rdev, 0x90, 0x80, 0x80); /* control by charger */
	else
		hal_power_byte_update(rdev, 0x90, 0x00, 0x80); /* drive MOTO */

	hal_power_byte_update(rdev, 0x90, power_spy->pmu_chgled_type & 0x07, power_spy->pmu_chgled_type & 0x07);

	/*Init battery capacity correct function*/
	if (power_spy->pmu_batt_cap_correct)
		hal_power_byte_update(rdev, 0xb8, 0x20, 0x20); /* enable */
	else
		hal_power_byte_update(rdev, 0xb8, 0x00, 0x20); /* disable */

	/*battery detect enable*/
	if (power_spy->pmu_batdeten)
		hal_power_byte_update(rdev, 0x8e, 0x08, 0x08);
	else
		hal_power_byte_update(rdev, 0x8e, 0x00, 0x08);

	/* RDC initial */
	hal_power_byte_read(rdev, AXP2585_RDC0, &val);
	if ((power_spy->pmu_battery_rdc) && (!(val & 0x40))) {
		rdc = (power_spy->pmu_battery_rdc * 10000 + 5371) / 10742;
		hal_power_byte_write(rdev, AXP2585_RDC0, ((rdc >> 8)& 0x1F)|0x80);
		hal_power_byte_write(rdev, AXP2585_RDC1, rdc & 0x00FF);
	}

	hal_power_byte_read(rdev, AXP2585_BATCAP0, &val);
	if ((power_spy->pmu_battery_cap) && (!(val & 0x80))) {
		cur_coulomb_counter = power_spy->pmu_battery_cap
					* 1000 / 1456;
		hal_power_byte_write(rdev, AXP2585_BATCAP0,
					((cur_coulomb_counter >> 8) | 0x80));
		hal_power_byte_write(rdev, AXP2585_BATCAP1,
					cur_coulomb_counter & 0x00FF);
	} else if (!power_spy->pmu_battery_cap) {
		hal_power_byte_write(rdev, AXP2585_BATCAP0, 0x00);
		hal_power_byte_write(rdev, AXP2585_BATCAP1, 0x00);
	}

	if (power_spy->pmu_bat_temp_enable != 0) {
		hal_power_byte_write(rdev, AXP2585_VLTF_CHARGE,
				power_spy->pmu_bat_charge_ltf * 10 / 128);
		hal_power_byte_write(rdev, AXP2585_VHTF_CHARGE,
				power_spy->pmu_bat_charge_htf * 10 / 128);
		hal_power_byte_write(rdev, AXP2585_VLTF_WORK,
				power_spy->pmu_bat_shutdown_ltf * 10 / 128);
		hal_power_byte_write(rdev, AXP2585_VHTF_WORK,
				power_spy->pmu_bat_shutdown_htf * 10 / 128);
	}
	/*enable fast charge */
	hal_power_byte_update(rdev, 0x31, 0x04, 0x04);
	/*set POR time as 16s*/
	hal_power_byte_update(rdev, AXP2585_POK_SET, 0x30, 0x30);
	/* avoid the timer counter error*/
	hal_power_byte_update(rdev, AXP2585_TIMER2_SET, 0x0, 0x10);
	for (i = 0; i < ARRAY_SIZE(update_min_times); i++) {
		if (update_min_times[i] == power_spy->pmu_update_min_time)
			break;
	}
	hal_power_byte_update(rdev, AXP2585_ADJUST_PARA, i, 0x7);
	/*initial the ocv_cou_adjust_time*/
	for (i = 0; i < ARRAY_SIZE(ocv_cou_adjust_time); i++) {
		if (ocv_cou_adjust_time[i] == power_spy->pmu_ocv_cou_adjust_time)
			break;
	}
	i <<= 6;
	hal_power_byte_update(rdev, AXP2585_ADJUST_PARA1, i, 0xc0);

	/* init the chg runtime_cur */
	axp2585_set_chg_cur(rdev, power_spy->pmu_runtime_chgcur);

	/* set bc1.2 */
	if (power_spy->pmu_init_bc_en)
		hal_power_byte_update(rdev, 0x23, 0x40, 0x40);
	else
		hal_power_byte_update(rdev, 0x23, 0x00, 0x40);
	hal_power_byte_update(rdev, 0x23, 0x00, 0x10);

	/* close batfet switch */
	hal_power_byte_read(rdev, AXP2585_ILIMIT, &val);
	if (val & 0x80)
		hal_power_byte_update(rdev, AXP2585_ILIMIT, 0x00, 0x80);

	/*type-c cc logic init*/
#ifdef CONFIG_AXP2585_TYPE_C
	hal_power_byte_update(rdev, AXP2585_CC_EN, 0x02, 0x02);
	hal_power_byte_update(rdev, AXP2585_CC_LOW_POWER_CTRL, 0x00, 0x04);
	hal_power_byte_update(rdev, AXP2585_CC_MODE_CTRL, 0x03, 0x03);
#ifdef CONFIG_AXP2585_TYPE_C_AUDIO
	hal_power_byte_update(rdev, AXP2585_CC_GLOBAL_CTRL, 0x20, 0x20);
#endif
#endif
	axp2585_set_usb_vhold(rdev, power_spy->pmu_usbpc_vol);
	_axp2585_set_usb_ihold(rdev, power_spy->pmu_usbpc_cur);

#ifdef CONFIG_COMPONENTS_PM
	pm_devops_register(&pm_axp2585_bat_power);
#endif

	rdev_g = *rdev;

	hal_power_byte_update(rdev, 0x42, 0xc0, 0xc0); /* enable */
	if (power_spy->pmu_irq_pin) {
		if (hal_gpio_to_irq(power_spy->pmu_irq_pin, &power_spy->pmu_irq_no) < 0) {
			pr_err("request axp_irq gpio to irq error\n");
		}
	}
	if (hal_gpio_irq_request(power_spy->pmu_irq_no, axp2585_irq_handler, IRQ_TYPE_EDGE_FALLING, rdev) < 0) {
		pr_err("request axp_irq irq error\n");
		return -1;
	}

	rdev->irq_schd = hal_sem_create(0);
	xTaskCreate(axp_irq_task, (signed portCHAR *) "axp_irq_task", 4096, (void *)rdev, 31, &task);

	hal_gpio_irq_enable(power_spy->pmu_irq_no);
	axp2585_usb_set_current_fsm(rdev);

	/* axp2585_clean_all_irq(rdev); */

	pr_info("axp2585 init finished !\n");

	return 0;
}

/* get power ops*/
int axp2585_get_power(struct power_dev *rdev)
{
	rdev->bat_ops = &axp2585_bat_power_ops;
	rdev->usb_ops = &axp2585_usb_power_ops;

	return AXP2585_POWER;
}

int axp2585_get_version(struct power_dev *rdev)
{
	int err;
	u8 chip_id = 0;
	static int pmu_type = -1;

	if (pmu_type == -1) {
		err = hal_power_byte_read(rdev, AXP2585_IC_TYPE, &chip_id);
		if (err) {
			pr_err("try to read axp2585 chip id failed!:%d\n", err);
			return err;
		}

		/*only support axp2585*/
		if (((chip_id & 0xcf) == 0x46) ||
			((chip_id & 0xcf) == 0x49)
			) {
			if (chip_id & 0x20) {
				pmu_type = AXP2585_VERSION_D;
				pr_info("axp2585 chip version D !\n");
			} else {
				pmu_type = AXP2585_VERSION_C;
				pr_info("axp2585 chip version C !\n");
			}

			pr_info("axp2585 chip id detect 0x%x !\n", chip_id);
		} else {
			pr_info("axp2585 chip id is error 0x%x !\n", chip_id);
			return err;
		}
	}

	rdev->config->pmu_version = pmu_type;

	return 0;
}
