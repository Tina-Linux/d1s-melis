#include "usb_test.h"

int usb_test_cmd_hci(int argc, const char **argv)
{
	int c;
	int hci_num = 0;
	unsigned int port = 0;

	if (argc == 4) {
		// insmod/rmmod indicate host driver
		if (usb_test_get_port(argv[3], &port))
			return 0;
	} else if (argc == 3) {
		// insmod/rmmod all host driver
		port = 0xffff;
	} else
		return -1;

	while ((c = getopt(argc, (char *const *)argv, "ir")) != -1) {
		switch (c) {
		case 'i':
#ifdef CONFIG_HAL_TEST_UDC
			/*otg mode rmmod device driver before insmod hci*/
			if (usb_test_is_otg(port)) {
				printf("[usb0] rmmod device driver!\n");
				hal_gadget_exit();
			}
#endif
			if (!usb_core_is_enabled())
				hal_usb_core_init();

			if (port == 0xffff)
				for (hci_num = 0; hci_num < USB_MAX_CONTROLLER_COUNT; hci_num++)
					hal_usb_hcd_init(hci_num);
			else
				hal_usb_hcd_init(port);
			break;
		case 'r':
			if (port == 0xffff)
				for (hci_num = 0; hci_num < USB_MAX_CONTROLLER_COUNT; hci_num++)
					hal_usb_hcd_deinit(hci_num);
			else
				hal_usb_hcd_deinit(port);

			if (usb_core_is_enabled() && !sunxi_ehci_status_get())
				hal_usb_core_exit();
			break;
		default:
			printf("ERR: insmod/rmmod error!\n");
			usb_test_show_help();
			break;
		}
	}

	return 0;
}

int usb_test_cmd_debug(int argc, const char **argv)
{
	int enable = 0;

	if (argc != 3)
		return -1;

	enable = atoi(argv[2]);
	if (enable == 1 || enable == 0) {
		hal_usb_hcd_debug_set(enable);
		printf("USB debug %s!\n", hal_usb_hcd_debug_get() ? "open" : "close");
		return 0;
	}
	
	return -1;
}
