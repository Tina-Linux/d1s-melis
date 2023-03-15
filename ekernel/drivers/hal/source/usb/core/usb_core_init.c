/*
********************************************************************************************************************
*                                              usb host module
*
*                             Copyright(C), 2006-2008, SoftWinners Co., Ltd.
*                                                  All Rights Reserved
*
* File Name :
*
* Author : GLHuang(HoLiGun)
*
* Version : 1.0
*
* Date : 2008.05.xx
*
* Description :
*
* History :
********************************************************************************************************************
*/
#include "usb_gen_hub.h"
#include "usb_virt_bus.h"
#include "usb_core_base.h"
#include "usb_driver_init.h"

s32 usb_core_init(void)
{
	if (!usb_core_is_enabled()) {
		usb_core_enable();
		usb_virt_bus_init();
		usb_drivers_init();
		usb_gen_hcd_init();
		usb_gen_hub_init();
		return 0;
	} else {
		hal_log_err("host enabled,no need init!");
		return -1;
	}
}

s32 usb_core_exit(void)
{
	if (usb_core_is_enabled()) {
		usb_gen_hub_exit();
		usb_gen_hcd_exit();
		usb_drivers_exit();
		usb_virt_bus_exit();
		usb_core_disable();
		return 0;
	} else {
		hal_log_err("host disabled,no need exit!");
		return -1;
	}
}
