#ifndef _USB_CORE_INTERFACE_H_
#define _USB_CORE_INTERFACE_H_

#include "usb_os_platform.h"

/* 是连接usb_host_virt_dev和usb_interface的桥梁
 * 在usb_set_configuration()的时候被使用
 *
 * 可以看做是一个lun
 */
struct usb_host_virt_sub_dev {
	struct usb_driver *func_drv;  //指向功能驱动
	// USB_OS_KERNEL_EVENT    *usb_virt_sub_dev_semi;
	hal_sem_t usb_virt_sub_dev_semi;

	struct usb_device *father_dev;	  //指向其father
	struct usb_interface *sub_dev_interface;  //指向其下游，既interface

	void *drv_pirv_data;  //存储临时数据
};

struct usb_host_func_drv_ext {
	int32_t (*probe_ext)(struct usb_host_virt_sub_dev *dev); //对usb_host_func_drv{}的probe的extend
	//目前为_usb_core_func_drv_probe_interface()
	int32_t (*disconnect_ext)(struct usb_host_virt_sub_dev *dev); /* 指向真实的drv */
};

int32_t usb_host_func_drv_reg(struct usb_driver *drv);
int32_t usb_host_func_drv_unreg(struct usb_driver *drv);

#endif
