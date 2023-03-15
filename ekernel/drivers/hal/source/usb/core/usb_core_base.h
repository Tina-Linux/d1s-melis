#ifndef _USB_CORE_BASE_H_
#define _USB_CORE_BASE_H_

// usb host开启
void usb_core_enable(void);
// usb host 禁止
void usb_core_disable(void);

//查询usb host是否开启
// return    :   非0为enable
uint8_t usb_core_is_enabled(void);

// usb_device{}的分配与释放
struct usb_device *usb_host_alloc_virt_dev(struct usb_device *parent,
						  struct usb_bus *bus,
						  uint32_t port1);
void usb_host_free_virt_dev(struct usb_device *virt_dev);

struct usb_host_interface *usb_altnum_to_altsetting(struct usb_interface *intf, int32_t altnum);

void usb_lock_device(struct usb_device *virt_dev);
void usb_unlock_device(struct usb_device *virt_dev);

void *usb_buffer_alloc(struct usb_device *dev, uint32_t size, uint32_t mem_flags, void *dma);
void usb_buffer_free(struct usb_device *dev, uint32_t size, void *addr, void *dma);

struct usb_interface *usb_ifnum_to_if(struct usb_device *dev, uint32_t ifnum);

int32_t _usb_core_func_drv_probe_interface(struct usb_host_virt_sub_dev *sub_dev);
int32_t _usb_core_func_drv_disconncet_interface(struct usb_host_virt_sub_dev *dev);

#endif
