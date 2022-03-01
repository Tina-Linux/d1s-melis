#include <usb/hal_hci.h>
#include <init.h>
void sunxi_usb_hci_init(void)
{
	/*except for usb0， other usb hci should be init in OS.
	 *usb0 hci depends on otg.
	 */
#ifndef CONFIG_DRIVER_USB_MANAGER
#ifdef CONFIG_DRIVER_USB_HOST_0
	hal_usb_hcd_init(0);
#endif
#endif
#ifdef CONFIG_DRIVER_USB_HOST_1
	hal_usb_hcd_init(1);
#endif
}
late_initcall(sunxi_usb_hci_init);
