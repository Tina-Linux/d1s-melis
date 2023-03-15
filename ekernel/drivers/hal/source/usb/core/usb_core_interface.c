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
* Date : 2008.06.xx
*
* Description :
*       功能驱动reg/unreg接口
* History :
********************************************************************************************************************
*/

#include "usb_virt_bus.h"
#include "usb_core_base.h"

//return
//  0   :   成功reg
int32_t usb_host_func_drv_reg(struct usb_driver *drv)
{
	if (usb_core_is_enabled() == 0) {
		hal_log_err("ERR: usb host is disabled.");
		return -ENODEV;
	}

	hal_log_info("[usb bus]: driver \"%s\" detect usb bus.",
		     drv->func_drv_name != NULL ? drv->func_drv_name : " ");
	drv->func_drv_ext.probe_ext = _usb_core_func_drv_probe_interface;
	drv->func_drv_ext.disconnect_ext = _usb_core_func_drv_disconncet_interface;
	return usb_virt_bus_drv_reg(drv);
}

// return
//  0   :   成功unreg
int32_t usb_host_func_drv_unreg(struct usb_driver *drv)
{
	hal_log_info("[usb bus]: driver \"%s\" disconnect usb bus.",
		     drv->func_drv_name != NULL ? drv->func_drv_name : " ");
	return usb_virt_bus_drv_unreg(drv);
}
