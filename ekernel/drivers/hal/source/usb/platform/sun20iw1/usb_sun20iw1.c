#include "usb_sun20iw1.h"
#include "usb_os_platform.h"

#ifdef CONFIG_SOC_SUN20IW1
#define USB_GIC_OFFSET 16
#else
#define USB_GIC_OFFSET 0
#endif

static const struct platform_usb_config usb_ehci_table[] = {
	{//ehci-0
		.name    = "sunxi-ehci0",
		.pbase   = SUNXI_USB_HCI0_PBASE,
		.irq     = 62 - USB_GIC_OFFSET,
		.usb_clk = CLK_BUS_EHCI0,
		.usb_rst = RST_BUS_EHCI0,
		.phy_clk = 0,
		.phy_rst = RST_USB_PHY0
	},

	{//ehci-1
		.name    = "sunxi-ehci1",
		.pbase   = SUNXI_USB_HCI1_PBASE,
		.irq     = 65 - USB_GIC_OFFSET,
		.usb_clk = CLK_BUS_EHCI1,
		.usb_rst = RST_BUS_EHCI1,
		.phy_clk = 0,
		.phy_rst = RST_USB_PHY1
	}
};

static const struct platform_usb_config usb_ohci_table[] = {
	{//ohci-0
		.name     = "sunxi-ohci0",
		.pbase    = SUNXI_USB_HCI0_PBASE,
		.irq      = 63 - USB_GIC_OFFSET,
		.ohci_clk = CLK_USB_OHCI0,
		.usb_clk  = CLK_BUS_OHCI0,
		.usb_rst  = RST_BUS_OHCI0,
		.phy_clk  = 0,
		.phy_rst  = RST_USB_PHY0
	},

	{//ohci-1
		.name     = "sunxi-ohci1",
		.pbase    = SUNXI_USB_HCI1_PBASE,
		.irq      = 66 - USB_GIC_OFFSET,
		.ohci_clk = CLK_USB_OHCI1,
		.usb_clk  = CLK_BUS_OHCI1,
		.usb_rst  = RST_BUS_OHCI1,
		.phy_clk  = 0,
		.phy_rst  = RST_USB_PHY1
	}
};

static const struct platform_usb_config usb_otg_table = {
	.name    = "sunxi-otg",
	.pbase   = SUNXI_USB_OTG_PBASE,
	.irq     = 61 - USB_GIC_OFFSET,
	.usb_clk = CLK_BUS_OTG,
	.usb_rst = RST_BUS_OTG,
	.phy_clk = 0,
	.phy_rst = RST_USB_PHY0
};

#if defined(CONFIG_ARCH_SUN8IW20)
static const struct platform_usb_port_config usb_port_table = {
	.enable = 1,
	.port_type = 2,
	.detect_type = 1,
	.detect_mode = 0,
	.id.valid = 1,
	.id.gpio = GPIO_PD12,
	.det_vbus.valid = 1,
	.det_vbus.gpio = GPIO_PB8,
	.drv_vbus[0].valid = 1,
	.drv_vbus[0].gpio = GPIO_PB7,
	.drv_vbus[1].valid = 1,
	.drv_vbus[1].gpio = GPIO_PD22,
};
#endif

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

#if defined(CONFIG_ARCH_SUN8IW20)
struct platform_usb_port_config *platform_get_port_table(void)
{
	return (struct platform_usb_port_config *)&usb_port_table;
}
#endif