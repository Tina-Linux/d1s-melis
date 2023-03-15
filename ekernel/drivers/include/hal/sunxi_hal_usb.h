/*
 * drivers/input/sensor/sunxi_gpadc.h
 *
 * Copyright (C) 2016 Allwinner.
 * fuzhaoke <fuzhaoke@allwinnertech.com>
 *
 * SUNXI GPADC Controller Driver Header
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#ifndef HAL_USB_H
#define HAL_USB_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
/********************************************************************
 * USB driver init
 ********************************************************************/
void sunxi_usb_init(void);

/********************************************************************
 * USB HOST
 ********************************************************************/
int hal_usb_core_init(void);
int hal_usb_core_exit(void);
int hal_usb_hci_init(void);
int hal_usb_hci_deinit(void);
int hal_usb_hcd_init(int hci_num);
int hal_usb_hcd_deinit(int hci_num);

void hal_hci_phy_range_show(int hci_num);
void hal_hci_phy_range_set(int hci_num, int val);
void hal_hci_driverlevel_show(int hci_num);
void hal_hci_driverlevel_adjust(int hci_num, int driverlevel);
void hal_hci_ed_test(int hci_num, const char *buf, unsigned int count);

void hal_usb_hcd_debug_set(int value);
int hal_usb_hcd_debug_get(void);

/********************************************************************
 * USB UDC
 ********************************************************************/

typedef enum {
	UDC_EVENT_RX_STANDARD_REQUEST = 1,
	UDC_EVENT_RX_CLASS_REQUEST = 2,
	UDC_EVENT_RX_DATA = 3,
	UDC_EVENT_TX_COMPLETE = 4,
} udc_callback_event_t;

typedef enum {
	UDC_ERRNO_SUCCESS = 0,
	UDC_ERRNO_CMD_NOT_SUPPORTED = -1,
	UDC_ERRNO_CMD_INVALID = -2,
	UDC_ERRNO_BUF_NULL = -3,
	UDC_ERRNO_BUF_FULL = -4,
	UDC_ERRNO_EP_INVALID = -5,
	UDC_ERRNO_RX_NOT_READY = -6,
	UDC_ERRNO_TX_BUSY = -7,
} udc_errno_t;

typedef udc_errno_t (*udc_callback_t)(uint8_t ep_addr, udc_callback_event_t event,
				void *data, uint32_t len);

int32_t hal_udc_init(void);
int32_t hal_udc_deinit(void);

int32_t hal_udc_ep_read(uint8_t ep_addr, void *buf, uint32_t len);
int32_t hal_udc_ep_write(uint8_t ep_addr, void *buf , uint32_t len);

void hal_udc_device_desc_init(void *device_desc);
void hal_udc_config_desc_init(void *config_desc, uint32_t len);
void hal_udc_string_desc_init(const void *string_desc);
void hal_udc_register_callback(udc_callback_t user_callback);

void hal_udc_ep_disable(uint8_t ep_addr);
void hal_udc_ep_enable(uint8_t ep_addr, uint16_t maxpacket, uint32_t ts_type);
void hal_udc_ep_set_buf(uint8_t ep_addr, void *buf, uint32_t len);

void hal_udc_driverlevel_show(void);
void hal_udc_driverlevel_adjust(int driverlevel);
void hal_udc_phy_range_show(int usbc_num);
void hal_udc_phy_range_set(int usbc_num, int val);
void hal_udc_ed_test(const char *buf, size_t count);

/********************************************************************
 * USB MANAGER
 ********************************************************************/
int hal_usb_manager_init(void);
int hal_usb_manager_deinit(void);

/********************************************************************
 * USB GADGET
 ********************************************************************/
enum {
	USB_GADGET_MANUFACTURER_IDX = 0,
	USB_GADGET_PRODUCT_IDX,
	USB_GADGET_SERIAL_IDX,
	USB_GADGET_LANGUAGE_IDX,
	USB_GADGET_CONFIG_IDX,
	USB_GADGET_INTERFACE_IDX,
	USB_GADGET_MAX_IDX,
};

int hal_gadget_init(void);
void hal_gadget_exit(void);
int usb_gadget_function_enable(const char *name);
int usb_gadget_function_disable(const char *name);
int usb_gadget_function_read(int ep_idx, char *buf, int size);
int usb_gadget_function_write(int ep_idx, char *buf, int size);
int usb_gadget_function_string_set(char *name, char *str, unsigned int idx);

#ifdef __cplusplus
}
#endif

#endif
