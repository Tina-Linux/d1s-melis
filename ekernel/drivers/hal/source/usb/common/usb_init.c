#include <sunxi_hal_usb.h>

/********************************************************
 *
 * init usb driver for Melis os
 *
 ********************************************************/
#ifdef CONFIG_OS_MELIS
#include <init.h>

#ifdef CONFIG_USB_HOST
subsys_initcall(hal_usb_core_init);
#endif //CONFIG_USB_HOST

#ifdef CONFIG_USB_HOST
late_initcall(hal_usb_hci_init);
#endif //CONFIG_USB_HOST

#ifdef CONFIG_USB_MANAGER
device_initcall(hal_usb_manager_init);
#else
#ifdef CONFIG_USB_GADGET
/*for adb auto init*/
device_initcall(hal_gadget_init);
#endif //CONFIG_USB_GADGET
#endif //CONFIG_USB_MANAGER

#endif //CONFIG_OS_MELIS


/********************************************************
 *
 * init usb driver for FreeRtos os
 *
 ********************************************************/

#ifdef CONFIG_KERNEL_FREERTOS

void sunxi_usb_init(void)
{
#ifdef CONFIG_USB_HOST
	hal_usb_core_init();
	hal_usb_hci_init();
#endif

#ifdef CONFIG_USB_MANAGER
	hal_usb_manager_init();
#else
#ifdef CONFIG_USB_GADGET
	/*for adb auto init*/
	hal_gadget_init();
#endif //CONFIG_USB_GADGET
#endif //CONFIG_USB_MANAGER
}

#endif //CONFIG_KERNEL_FREERTOS

