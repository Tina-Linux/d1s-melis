#include "usb_sun20iw2.h"
#include "usb_os_platform.h"

#if defined(CONFIG_ARCH_ARM)
#define USB_GIC_OFFSET 16
#elif defined(CONFIG_ARCH_RISCV)
#define USB_GIC_OFFSET 0
#endif

static const struct platform_usb_config usb_ehci_table[] = {
	{//enci-0
		.name    = "sunxi-ehci0",
		.pbase   = SUNXI_USB_HCI0_PBASE,
		.irq     = 79 - USB_GIC_OFFSET,
		.usb_clk = CLK_BUS_EHCI0,
		.usb_rst = RST_USB_EHCI,
		.phy_clk = 0,
		.phy_rst = RST_USB_PHY
	}
};

static const struct platform_usb_config usb_ohci_table[] = {
	{//ohci-0
		.name     = "sunxi-ohci0",
		.pbase    = SUNXI_USB_HCI0_PBASE,
		.irq      = 80 - USB_GIC_OFFSET,
		.ohci_clk = CLK_USB_PLL,
		.usb_clk  = CLK_BUS_OHCI0,
		.usb_rst  = RST_USB_OHCI,
		.phy_clk  = 0,
		.phy_rst  = RST_USB_PHY
	}
};

static const struct platform_usb_config usb_otg_table = {
	.name    = "sunxi-otg",
	.pbase   = SUNXI_USB_OTG_PBASE,
	.irq     = 81 - USB_GIC_OFFSET,
	.usb_clk = CLK_BUS_OTG,
	.usb_rst = RST_USB_OTG,
	.phy_clk = 0,
	.phy_rst = RST_USB_PHY
};

static const struct platform_usb_port_config usb_port_table = {
	.enable = 1,
	.port_type = 2,
	.detect_type = 1,
	.detect_mode = 0,
	.id.valid = 1,
	.id.gpio = GPIO_PB4,
	.det_vbus.valid = 1,
	.det_vbus.gpio = GPIO_PA24,
	.drv_vbus[0].valid = 1,
	.drv_vbus[0].gpio = GPIO_PA29
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

struct platform_usb_port_config *platform_get_port_table(void)
{
	return (struct platform_usb_port_config *)&usb_port_table;
}