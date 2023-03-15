#ifndef _USB_GEN_HUB_H_
#define _USB_GEN_HUB_H_

#include "usb_gen_hcd.h"

#define usb_endpoint_out(ep_dir)        (!((ep_dir) & USB_DIR_IN))

struct usb_hub {
	// struct device     *intfdev;   /* the "interface" device */
	struct usb_device *hdev;
	struct urb *urb;  //就是用来查询status的urb//for interrupt polling pipe

	/* buffer for urb ... with extra space in case of babble */
	// char (*buffer)[8];        //是status urb和 hub交互status的中介
	char *status_urb_ubff;
	// u32 buffer_dma;                 /* DMA address for buffer */

	union {
		struct usb_hub_status hub;
		struct usb_port_status port;
	} * status; /* buffer for status reports    */

	int error;   /* last reported error          */
	int nerrors; /* track consecutive errors     */

	//  struct list_head    event_list; /* hubs w/data or errs ready    */
	u32 event_bits[1];  /* status change bitmask        */
	u32 change_bits[1]; /* ports with logical connect status change */
	u32 busy_bits[1];   /* ports being reset            */
	hal_mutex_t port_status_lock[5];

	struct usb_hub_descriptor *descriptor; /* class descriptor         */
	struct usb_tt tt;		       /* Transaction Translator   */

	u8 power_budget; /* in 2mA units; or zero    */

	unsigned quiescing : 1;
	unsigned activating : 1;
	unsigned resume_root_hub : 1;

	unsigned has_indicators : 1;
	enum hub_led_mode indicator[USB_MAXCHILDREN];
	// struct work_struct    leds;

	// USB_OS_KERNEL_EVENT *HubStatusSemi;     /* hub status thread的睡眠和唤醒 */
	// USB_OS_KERNEL_EVENT *notify_complete;   /* 同步thread创建/删除           */
	hal_sem_t HubStatusSemi;   /* hub status thread的睡眠和唤醒 */
	hal_sem_t notify_complete; /* 同步thread创建/删除           */

	hal_thread_t thread;
	__u32 HubStatusThrdId; /* hub线程ID                     */
	__u8 HubRemove;	       /* flag. Hub是否拔出             */
	__u32 hub_req_busy;    /* 正在发送urb请求               */
};

void usb_set_device_state(struct usb_device *udev, enum usb_device_state new_state);
void usb_disable_endpoint(struct usb_device *dev, unsigned int epaddr);

void usb_disconnect(struct usb_device **pdev);

void usb_disable_device(struct usb_device *dev, int skip_ep0);
int usb_reset_device(struct usb_device *udev);

int usb_new_device(struct usb_device *udev);

uint32_t hub_GetHubNo(struct usb_device *udev);
uint32_t hub_GetHubSeries(struct usb_device *udev);

void kick_khubd(struct usb_hub *hub);
void hub_activate(struct usb_hub *hub);

int32_t usb_gen_hub_init(void);
int32_t usb_gen_hub_exit(void);

#endif
