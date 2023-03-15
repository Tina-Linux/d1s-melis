#ifndef _USB_VIRT_BUS_H_
#define _USB_VIRT_BUS_H_

#include "usb_gen_hcd.h"

int32_t usb_virt_bus_drv_reg(struct usb_driver *drv);
int32_t usb_virt_bus_drv_unreg(struct usb_driver *drv);
int32_t usb_virt_bus_dev_add(struct usb_host_virt_sub_dev *dev);
int32_t usb_virt_bus_dev_del(struct usb_host_virt_sub_dev *dev);

int32_t usb_virt_bus_init(void);
int32_t usb_virt_bus_exit(void);

#endif
