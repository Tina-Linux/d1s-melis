/*
********************************************************************************************************************
*                                            sun project ---- usb host module
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

#ifndef __USB_HCD_H__
#define __USB_HCD_H__

#include "usb_core_interface.h"
#include "usb.h"
#include "hcd.h"

#define HC_GEN_DEV___ACTIVE		0x01
#define HC_GEN_DEV___SUSPEND		0x04
#define HC_GEN_DEV___TRANSIENT		0x80

#define HC_GEN_DEV_STATE_HALT		0
#define HC_GEN_DEV_STATE_RUNNING	(HC_GEN_DEV___ACTIVE)
#define HC_GEN_DEV_STATE_QUIESCING	(HC_GEN_DEV___SUSPEND | HC_GEN_DEV___TRANSIENT | HC_GEN_DEV___ACTIVE)
#define HC_GEN_DEV_STATE_RESUMING	(HC_GEN_DEV___SUSPEND | HC_GEN_DEV___TRANSIENT)
#define HC_GEN_DEV_STATE_SUSPENDED	(HC_GEN_DEV___SUSPEND)
#define HC_GEN_DEV_IS_RUNNING(state)	((state)&HC_GEN_DEV___ACTIVE)
#define HC_GEN_DEV_IS_SUSPENDED(state)	((state)&HC_GEN_DEV___SUSPEND)


#define HC_DRIVER_FLAG_HCD_MEMORY	0x0001 /* HC regs use memory (else I/O) */
#define HC_DRIVER_FLAG_HCD_USB11	0x0010 /* USB 1.1 */
#define HC_DRIVER_FLAG_HCD_USB2		0x0020 /* USB 2.0 */

#define HUB_EVNET_PLUGIN_COMPLETE	0x01
#define HUB_EVNET_PLUGOUT_COMPLETE	0x02


int usb_hcd_link_urb_to_ep(struct usb_hcd *hcd, struct urb *urb);
void usb_hcd_unlink_urb_from_ep(struct usb_hcd *hcd, struct urb *urb);
void usb_hcd_start_port_resume(struct usb_bus *bus, int portnum);
void usb_hcd_end_port_resume(struct usb_bus *bus, int portnum);
int usb_hcd_check_unlink_urb(struct usb_hcd *hcd, struct urb *urb);

struct usb_hcd *usb_create_hc_gen_dev(const struct hc_driver *driver, const char *bus_name);
s32 usb_add_hc_gen_dev(struct usb_hcd *hcd, u32 irqnum, u32 irqflags);
void usb_remove_hc_gen_dev(struct usb_hcd *hcd);
void usb_hcd_poll_rh_status(struct usb_hcd *hcd);
void usb_hcd_giveback_urb(struct usb_hcd *hcd, struct urb *urb);

int hcd_ops_get_frame_number(struct usb_device *udev);
int hcd_ops_submit_urb(struct urb *urb, unsigned mem_flags);
int hcd_ops_unlink_urb(struct urb *urb, int status);
void *hcd_ops_buffer_alloc(struct usb_bus *bus, u32 size, u32 mem_flags, u32 *dma);
void hcd_ops_buffer_free(struct usb_bus *bus, u32 size, u32 *addr, u32 *dma);
void hcd_ops_endpoint_disable(struct usb_device *udev, struct usb_host_endpoint *ep);
void usb_hc_died(struct usb_hcd *hcd);
int usb_hub_clear_tt_buffer(struct urb *urb);
void ehci_clear_tt_buffer_complete(struct usb_hcd *hcd, struct usb_host_endpoint *ep);
long usb_calc_bus_time(int speed, int is_input, int isoc, int bytecount);

void usb_gen_hcd_init(void);
void usb_gen_hcd_exit(void);

void usb_hc_died(struct usb_hcd *hcd);

#endif
