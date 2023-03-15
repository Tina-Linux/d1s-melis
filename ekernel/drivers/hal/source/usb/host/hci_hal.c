#include "usb_os_platform.h"
#include "usb_core_init.h"
#include "sunxi-hci.h"
#include "usb_phy.h"

static int g_sunxi_usb_debug = 0;

int sunxi_ehci_hcd_init(int hci_num);
int sunxi_ehci_hcd_deinit(int hci_num);
int sunxi_ohci_hcd_init(unsigned int hci_num);
int sunxi_ohci_hcd_deinit(unsigned int usbc_no);
void ehci_ed_test(int hci_num, unsigned int test_mode);
unsigned int sunxi_ehci_phy_addr_get(int hci_num);

int hal_usb_core_init(void)
{
	return usb_core_init();
}

int hal_usb_core_exit(void)
{
	return usb_core_exit();
}

int hal_usb_hcd_init(int hci_num)
{
	int ret = 0;

#ifdef CONFIG_USB_EHCI
	ret = sunxi_ehci_hcd_init(hci_num);
	if (ret)
		return ret;
#endif

#ifdef CONFIG_USB_OHCI
	ret = sunxi_ohci_hcd_init(hci_num);
	if (ret)
		return ret;
#endif

	return ret;
}

int hal_usb_hcd_deinit(int hci_num)
{
	int ret = 0;

#ifdef CONFIG_USB_EHCI
	ret = sunxi_ehci_hcd_deinit(hci_num);
	if (ret)
		return ret;
#endif

#ifdef CONFIG_USB_OHCI
	ret = sunxi_ohci_hcd_deinit(hci_num);
	if (ret)
		return ret;
#endif

	return ret;
}

int hal_usb_hci_init(void)
{
	/*except for usb0， other usb hci should be init in OS.
	 *usb0 hci depends on otg.
	 */
#if !defined(CONFIG_USB_MANAGER) && !defined(CONFIG_USB_DEVICE)
	hal_usb_hcd_init(0);
#endif

	for (int hci_num = 1; hci_num < USB_MAX_CONTROLLER_COUNT; hci_num++)
		hal_usb_hcd_init(hci_num);
	return 0;
}

int hal_usb_hci_deinit(void)
{
	/*except for usb0， other usb hci should be init in OS.
	 *usb0 hci depends on otg.
	 */
#if !defined(CONFIG_USB_MANAGER) && !defined(CONFIG_USB_DEVICE)
	hal_usb_hcd_deinit(0);
#endif

	for (int hci_num = 1; hci_num < USB_MAX_CONTROLLER_COUNT; hci_num++)
		hal_usb_hcd_deinit(hci_num);
	return 0;
}

void hal_usb_hcd_debug_set(int value)
{
	g_sunxi_usb_debug = value;
}

int hal_usb_hcd_debug_get(void)
{
	return g_sunxi_usb_debug;
}

/**********************
* hci hal APIs
* only set ehci,ignore ohci,because they share smae phy!!
* if there are special design someday ,should add ohci setting!
**********************/

void hal_hci_ed_test(int hci_num, const char *buf, unsigned int count)
{
	__u32 test_mode = 0;

	DMSG_INFO("ehci%d_ed_test : %s\n", hci_num, buf);
	if (!strncmp(buf, "test_not_operating", 18)) {
		test_mode = EHCI_PORTSC_PTC_DIS;
		DMSG_INFO("test_mode:0x%x\n", test_mode);
	} else if (!strncmp(buf, "test_j_state", 12)) {
		test_mode = EHCI_PORTSC_PTC_J;
		DMSG_INFO("test_mode:0x%x\n", test_mode);
	} else if (!strncmp(buf, "test_k_state", 12)) {
		test_mode = EHCI_PORTSC_PTC_K;
		DMSG_INFO("test_mode:0x%x\n", test_mode);
	} else if (!strncmp(buf, "test_se0_nak", 12)) {
		test_mode = EHCI_PORTSC_PTC_SE0NAK;
		DMSG_INFO("test_mode:0x%x\n", test_mode);
	} else if (!strncmp(buf, "test_pack", 9)) {
		test_mode = EHCI_PORTSC_PTC_PACKET;
		DMSG_INFO("test_mode:0x%x\n", test_mode);
	} else if (!strncmp(buf, "test_force_enable", 17)) {
		test_mode = EHCI_PORTSC_PTC_FORCE;
		DMSG_INFO("test_mode:0x%x\n", test_mode);
	} else if (!strncmp(buf, "test_mask", 9)) {
		test_mode = EHCI_PORTSC_PTC_MASK;
		DMSG_INFO("test_mode:0x%x\n", test_mode);
	} else {
		DMSG_PANIC("ERR: test_mode Argment is invalid\n");
	}

	ehci_ed_test(hci_num, test_mode);
}

void hal_hci_driverlevel_show(int hci_num)
{
	unsigned int phy_addr = sunxi_ehci_phy_addr_get(hci_num);
	usb_new_phy_driverlevel_show(phy_addr);
}

void hal_hci_driverlevel_adjust(int hci_num, int driverlevel)
{
	unsigned int phy_addr = sunxi_ehci_phy_addr_get(hci_num);
	usb_new_phy_driverlevel_adjust(phy_addr, driverlevel);
}

void hal_hci_phy_range_show(int hci_num)
{
	unsigned int phy_addr = sunxi_ehci_phy_addr_get(hci_num);
	DMSG_INFO("rate val:0x%x, usbc_no:%d\n",
		  usb_phy_tp_read(phy_addr, 0x20, 5), hci_num);
}

void hal_hci_phy_range_set(int hci_num, int val)
{
	unsigned int phy_addr = sunxi_ehci_phy_addr_get(hci_num);

	if ((val >= 0) && (val <= 0x1f)) {
		usb_phy_tp_write(phy_addr, 0x20, val, 5);
		DMSG_INFO("adjust success, rate val:0x%x, usbc_no:%d\n",
		  usb_phy_tp_read(phy_addr, 0x20, 5), hci_num);
	} else
		DMSG_PANIC("adjust PHY's rate and range fail : 0x%x, value range : 0x0~0x1f\n", val);

}