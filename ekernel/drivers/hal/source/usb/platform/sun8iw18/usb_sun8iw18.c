#include "usb_sun8iw18.h"
#include "usb_os_platform.h"

static const struct platform_usb_config *usb_ehci_table = 0;
static const struct platform_usb_config *usb_ohci_table = 0;

static const struct platform_usb_config usb_otg_table = {
	.name    = "sunxi-otg",
	.pbase   = SUNXI_USB_OTG_PBASE,
	.irq     = 69,
	.usb_clk = HAL_CLK_PERIPH_USBOTG,
	.usb_rst = 0,
	.phy_clk = HAL_CLK_PERIPH_USB0,
	.phy_rst = 0
};

struct platform_usb_config *platform_get_ehci_table(void)
{
	return (struct platform_usb_config *)usb_ehci_table;
}

struct platform_usb_config *platform_get_ohci_table(void)
{
	return (struct platform_usb_config *)usb_ohci_table;
}

struct platform_usb_config *platform_get_otg_table(void)
{
	return (struct platform_usb_config *)&usb_otg_table;
}
