/*
 * power ops
 */
#include <sunxi_hal_power.h>
#include "type.h"

/* battery ops */
int hal_power_get_bat_cap(struct power_dev *rdev)
{
	if ((rdev) && (rdev->bat_ops) && (rdev->bat_ops->get_rest_cap))
		return rdev->bat_ops->get_rest_cap(rdev);
	pr_err("can't read bat_cap\n");
	return -1;
}

int hal_power_get_coulumb_counter(struct power_dev *rdev)
{
	if ((rdev) && (rdev->bat_ops) && (rdev->bat_ops->get_coulumb_counter))
		return rdev->bat_ops->get_coulumb_counter(rdev);
	pr_err("can't read bat_cap\n");
	return -1;
}

int hal_power_get_bat_present(struct power_dev *rdev)
{
	if ((rdev) && (rdev->bat_ops) && (rdev->bat_ops->get_bat_present))
		return rdev->bat_ops->get_bat_present(rdev);
	pr_err("can't read bat_cap\n");
	return -1;
}

int hal_power_get_bat_online(struct power_dev *rdev)
{
	if ((rdev) && (rdev->bat_ops) && (rdev->bat_ops->get_bat_online))
		return rdev->bat_ops->get_bat_online(rdev);
	pr_err("can't read bat_cap\n");
	return -1;
}

int hal_power_get_bat_status(struct power_dev *rdev)
{
	if ((rdev) && (rdev->bat_ops) && (rdev->bat_ops->get_bat_status))
		return rdev->bat_ops->get_bat_status(rdev);
	pr_err("can't read bat_cap\n");
	return -1;
}

int hal_power_get_bat_health(struct power_dev *rdev)
{
	if ((rdev) && (rdev->bat_ops) && (rdev->bat_ops->get_bat_health))
		return rdev->bat_ops->get_bat_health(rdev);
	pr_err("can't read bat_health\n");
	return -1;
}

int hal_power_get_vbat(struct power_dev *rdev)
{
	if ((rdev) && (rdev->bat_ops) && (rdev->bat_ops->get_vbat))
		return rdev->bat_ops->get_vbat(rdev);
	pr_err("can't read vbat\n");
	return -1;
}

int hal_power_get_ibat(struct power_dev *rdev)
{
	if ((rdev) && (rdev->bat_ops) && (rdev->bat_ops->get_ibat))
		return rdev->bat_ops->get_ibat(rdev);
	pr_err("can't read ibat\n");
	return -1;
}

int hal_power_get_disibat(struct power_dev *rdev)
{
	if ((rdev) && (rdev->bat_ops) && (rdev->bat_ops->get_disibat))
		return rdev->bat_ops->get_disibat(rdev);
	pr_err("can't read disibat\n");
	return -1;
}

int hal_power_get_temp(struct power_dev *rdev)
{
	if ((rdev) && (rdev->bat_ops) && (rdev->bat_ops->get_temp))
		return rdev->bat_ops->get_temp(rdev);
	pr_err("can't read disibat\n");
	return -1;
}

int hal_power_get_temp_ambient(struct power_dev *rdev)
{
	if ((rdev) && (rdev->bat_ops) && (rdev->bat_ops->get_temp_ambient))
		return rdev->bat_ops->get_temp_ambient(rdev);
	pr_err("can't read disibat\n");
	return -1;
}

int hal_power_set_chg_cur(struct power_dev *rdev, int cur)
{
	if ((rdev) && (rdev->bat_ops) && (rdev->bat_ops->set_chg_cur))
		return rdev->bat_ops->set_chg_cur(rdev, cur);
	pr_err("can't set_chg_cur\n");
	return -1;
}

int hal_power_set_chg_vol(struct power_dev *rdev, int vol)
{
	if ((rdev) && (rdev->bat_ops) && (rdev->bat_ops->set_chg_vol))
		return rdev->bat_ops->set_chg_vol(rdev, vol);
	pr_err("can't set_chg_vol\n");
	return -1;
}

int hal_power_set_batfet(struct power_dev *rdev, int onoff)
{
	if ((rdev) && (rdev->bat_ops) && (rdev->bat_ops->set_batfet))
		return rdev->bat_ops->set_batfet(rdev, onoff);
	pr_err("can't set_batfet\n");
	return -1;
}

/* usb ops */
int hal_power_get_usb_status(struct power_dev *rdev)
{
	if ((rdev) && (rdev->usb_ops) && (rdev->usb_ops->get_usb_status))
		return rdev->usb_ops->get_usb_status(rdev);
	pr_err("can't read usb_status\n");
	return -1;
}

int hal_power_get_usb_ihold(struct power_dev *rdev)
{
	if ((rdev) && (rdev->usb_ops) && (rdev->usb_ops->get_usb_ihold))
		return rdev->usb_ops->get_usb_ihold(rdev);
	pr_err("can't read usb_ihold\n");
	return -1;
}

int hal_power_get_usb_vhold(struct power_dev *rdev)
{
	if ((rdev) && (rdev->usb_ops) && (rdev->usb_ops->get_usb_vhold))
		return rdev->usb_ops->get_usb_vhold(rdev);
	pr_err("can't read usb_vhold\n");
	return -1;
}

int hal_power_set_usb_ihold(struct power_dev *rdev, int cur)
{
	if ((rdev) && (rdev->usb_ops) && (rdev->usb_ops->set_usb_ihold))
		return rdev->usb_ops->set_usb_ihold(rdev, cur);
	pr_err("can't set_usb_ihold\n");
	return -1;
}

int hal_power_set_usb_vhold(struct power_dev *rdev, int vol)
{
	if ((rdev) && (rdev->usb_ops) && (rdev->usb_ops->set_usb_vhold))
		return rdev->usb_ops->set_usb_vhold(rdev, vol);
	pr_err("can't set_usb_vhold\n");
	return -1;
}

int hal_power_get_cc_status(struct power_dev *rdev)
{
	if ((rdev) && (rdev->usb_ops) && (rdev->usb_ops->get_cc_status))
		return rdev->usb_ops->get_cc_status(rdev);
	pr_err("can't get_cc_status\n");
	return -1;
}

