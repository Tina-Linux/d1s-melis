#ifndef __USB_MSG_CENTER_H__
#define __USB_MSG_CENTER_H__

typedef enum usb_role {
	USB_ROLE_NULL = 0,
	USB_ROLE_HOST,
	USB_ROLE_DEVICE,
} usb_role_t;

typedef struct usb_msg {
	uint8_t hw_insmod_host;
	uint8_t hw_rmmod_host;
	uint8_t hw_insmod_device;
	uint8_t hw_rmmod_device;
} usb_msg_t;

typedef struct usb_msg_center_info {
	struct usb_cfg *cfg;

	struct usb_msg msg;
	enum usb_role role;

	uint32_t skip; /* if skip, not enter msg process */
		       /* mainly to omit invalid msg */
} usb_msg_center_info_t;

enum usb_role get_usb_role(void);
int usb_msg_center_init(void);
int usb_msg_center_exit(void);
void usb_msg_center(usb_cfg_t *cfg);
void hw_insmod_usb_host(void);
void hw_insmod_usb_device(void);
void hw_rmmod_usb_host(void);
void hw_rmmod_usb_device(void);
#endif
