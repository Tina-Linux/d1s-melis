#ifndef _USB_MSG_H_
#define _USB_MSG_H_

int usb_string(struct usb_device *dev, int index, char *buf, uint32_t size);

int usb_control_msg(struct usb_device *dev, uint32_t pipe, uint8_t request, uint8_t requesttype,
		    uint16_t value, uint16_t index, void *data, uint16_t size, int32_t timeout);
int32_t usb_set_configuration(struct usb_device *dev, int32_t configuration);

int32_t usb_get_descriptor(struct usb_device *dev, uint8_t type, uint8_t index, void *buf, int32_t size);

int32_t usb_get_device_descriptor(struct usb_device *dev, uint32_t size);

int __usb_get_extra_descriptor(unsigned char *buffer,
			       unsigned size,
			       unsigned char type,
			       void **ptr);

#define usb_get_extra_descriptor(ifpoint, type, ptr) \
	__usb_get_extra_descriptor((ifpoint)->extra, (ifpoint)->extralen, type, (void **)ptr)

int usb_get_status(struct usb_device *dev, int32_t type, int32_t target, void *data);

int usb_clear_halt(struct usb_device *dev, int pipe);

int usb_set_interface(struct usb_device *dev, int interface, int alternate);

void usb_disable_interface(struct usb_device *dev, struct usb_interface *intf);

int usb_bulk_msg(struct usb_device *usb_dev, unsigned int pipe,
			void *data, int len, int *actual_length, int timeout);
#endif
