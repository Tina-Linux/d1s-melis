#include "usb_test.h"

static void __usb_ed_test(int port, const char *buf)
{
	if (usb_test_is_otg(port)) { /*otg mode*/
#ifdef CONFIG_HAL_TEST_UDC
		hal_udc_ed_test(buf, strlen(buf));
#else
		printf("ERR: udc config not find!\n");
#endif

	} else {
#ifdef CONFIG_HAL_TEST_HCI
		hal_hci_ed_test(port, buf, strlen(buf));
#else
		printf("ERR: hci config not find!\n");
#endif
	}
}

static void __driver_level_adjust(int port, int driver_level)
{
	if (usb_test_is_otg(port)) { /*otg mode*/
#ifdef CONFIG_HAL_TEST_UDC
		hal_udc_driverlevel_adjust(driver_level);
#else
		printf("ERR: udc config not find!\n");
#endif
	} else {
#ifdef CONFIG_HAL_TEST_HCI
		hal_hci_driverlevel_adjust(port, driver_level);
#else
		printf("ERR: hci config not find!\n");
#endif
	}
}

static void __driver_level_show(int port)
{
	if (usb_test_is_otg(port)) { /*otg mode*/
#ifdef CONFIG_HAL_TEST_UDC
		hal_udc_driverlevel_show();
#else
		printf("ERR: udc config not find!\n");
#endif
	} else {
#ifdef CONFIG_HAL_TEST_HCI
		hal_hci_driverlevel_show(port);
#else
		printf("ERR: hci config not find!\n");
#endif
	}
}

static void __phy_range_set(int port, int val)
{
	if (usb_test_is_otg(port)) { /*otg mode*/
#ifdef CONFIG_HAL_TEST_UDC
		hal_udc_phy_range_set(port, val);
#else
		printf("ERR: udc config not find!\n");
#endif
	} else {
#ifdef CONFIG_HAL_TEST_HCI
		hal_hci_phy_range_set(port, val);
#else
		printf("ERR: hci config not find!\n");
#endif
	}
}

static void __phy_range_show(int port)
{
	if (usb_test_is_otg(port)) { /*otg mode*/
#ifdef CONFIG_HAL_TEST_UDC
		hal_udc_phy_range_show(port);
#else
		printf("ERR: udc config not find!\n");
#endif
	} else {
#ifdef CONFIG_HAL_TEST_HCI
		hal_hci_phy_range_show(port);
#else
		printf("ERR: hci config not find!\n");
#endif
	}
}


int usb_test_cmd_ed_test(int argc, const char **argv)
{
	int port = 0;

	if (argc != 4)
		return -1;

	if (usb_test_get_port(argv[2], &port))
		return 0;

	__usb_ed_test(port, argv[3]);
	return 0;
}

int usb_test_cmd_dl_adjust(int argc, const char **argv)
{
	int c;
	int port;
	int driver_level = 0;

	if ((argc != 4) && (argc != 5))
		return -1;

	if (usb_test_get_port(argv[3], &port))
		return 0;

	if (usb_test_is_otg(port)) {
		printf("OTG%d driver level adjust\n", port);
	} else
		printf("EHCI%d driver level adjust\n", port);

	while ((c = getopt(argc, (char *const *)argv, "sg")) != -1) {
		switch (c) {
		case 's':
			if (argc == 5)
				driver_level = strtol(argv[4], NULL, 10);
			else
				return -1;

			__driver_level_adjust(port, driver_level);
			break;
		case 'g':
			__driver_level_show(port);
			break;
		default:
			printf("ERR: phy_range cmd error!\n");
			usb_test_show_help();
			break;
		}
	}
	return 0;
}

int usb_test_cmd_phy_range(int argc, const char **argv)
{
	int c;
	int val;
	int port;

	if ((argc != 4) && (argc != 5))
		return -1;

	if (usb_test_get_port(argv[3], &port))
		return 0;

	if (usb_test_is_otg(port)) {
		printf("OTG%d phy range\n", port);
	} else
		printf("EHCI%d phy range\n", port);

	while ((c = getopt(argc, (char *const *)argv, "sg")) != -1) {
		switch (c) {
		case 's':
			if(argc == 5)
				val = strtol(argv[4], NULL, 16);
			else
				return -1;

			__phy_range_set(port, val);
			break;
		case 'g':
			__phy_range_show(port);
			break;
		default:
			printf("ERR: phy_range cmd error!\n");
			usb_test_show_help();
			break;
		}
	}
	return 0;
}
