#include "usb_test.h"

int usb_test_cmd_udc(int argc, const char **argv)
{
	int c;
	if ((argc != 3) && (argc != 4))
		return -1;

	while ((c = getopt(argc, (char *const *)argv, "ir")) != -1) {
		switch (c) {
		case 'i':
#ifdef CONFIG_HAL_TEST_HCI
			// rmmod host driver before insmod otg
			if (usb_test_is_otg(0) == 0) /*hci mode*/
				hal_usb_hcd_deinit(0);
#endif
			printf("[usb0] insmod device driver!\n");
			hal_gadget_init();
			break;
		case 'r':
			printf("[usb0] rmmod device driver!\n");
			hal_gadget_exit();
			break;
		default:
			printf("err: insmod/rmmod error!\n");
			usb_test_show_help();
			break;
		}
	}

	return 0;
}
