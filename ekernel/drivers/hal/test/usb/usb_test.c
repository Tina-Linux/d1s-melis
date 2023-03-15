#include "usb_test.h"

void usb_test_show_help(void)
{
	printf("\nUsage:\n"\
		"\tusb hci {-i|-r} [<port>]\n"\
		"\tusb udc {-i|-r} [<port>]\n"\
		"\tusb phy_range {-s|-g} {<port>} [<phyrange>]\n"\
		"\tusb dl_adjust {-s|-g} {<port>} [<dlrange>]\n"\
		"\tusb ed_test {<port>} {<type>}\n"\
		"\tusb debug {<status>}\n"\
		"\tusb uvc_test\n"\
		"\n\t- - - - - - - - - - - - - - - - - - - - -\n"\
		"Meaning:\n"\
		"\t-i:insmod, -r:rmmod, -s:set, -g:get\n"\
		"\n"\
		"\tport     : [0-%d],port number\n"\
		"\tphyrange : [0x0-0x1f],phy range\n"\
		"\tdlrange  : [0x0-0xf],driver level range\n"\
		"\tstatus   : [0-disable,1-enable],hci debug status\n"\
		"\ttype     : [test_j_state/test_k_state/test_se0_nak/test_pack]--hci & otg\n"\
		"\t           [test_not_operating/test_force_enable/test_mask]--hci only\n"\
		"\n\t==>> More information refer to spec <<==\n",
		USB_MAX_CONTROLLER_COUNT - 1);
}

int usb_test_is_otg(int port)
{
#if defined(CONFIG_HAL_TEST_HCI)
	if (port == 0 && !(sunxi_ehci_status_get() & 0x1)) /*otg mode*/
#else
	if (port == 0)
#endif
		return 1;
	else
		return 0;
}

int usb_test_get_port(const char *buf, int *port)
{
	*port = atoi(buf);
	if (*port > USB_MAX_CONTROLLER_COUNT - 1) {
		printf("ERR: port(%d) choose error! Port range [0-%d]\n", *port,
			USB_MAX_CONTROLLER_COUNT - 1);
		return -1;
	}

	return 0;
}

static int usb_test_command_hci(int argc, const char **argv)
{
#if defined(CONFIG_HAL_TEST_HCI)
	return usb_test_cmd_hci(argc, argv);
#else
	printf("ERR: Can't find command config!\n");
	return -1;
#endif
}

static int usb_test_command_udc(int argc, const char **argv)
{
#if defined(CONFIG_HAL_TEST_UDC)
	return usb_test_cmd_udc(argc, argv);
#else
	printf("ERR: Can't find command config!\n");
	return -1;
#endif
}

static int usb_test_command_phy_range(int argc, const char **argv)
{
	return usb_test_cmd_phy_range(argc, argv);
}

static int usb_test_command_dl_adjust(int argc, const char **argv)
{
	return usb_test_cmd_dl_adjust(argc, argv);
}

static int usb_test_command_ed_test(int argc, const char **argv)
{
	return usb_test_cmd_ed_test(argc, argv);
}

static int usb_test_command_debug(int argc, const char **argv)
{
#if defined(CONFIG_HAL_TEST_HCI)
	return usb_test_cmd_debug(argc, argv);
#else
	printf("ERR: Can't find command config!\n");
	return -1;
#endif
}

static int usb_test_command_uvc(int argc, const char **argv)
{
#if defined(CONFIG_HAL_TEST_UVC)
	// return usb_test_cmd_uvc(argc, argv);
	usb_test_cmd_uvc(argc, argv);/* -1 has other meaning in this case*/
	return 0;
#else
	printf("ERR: Can't find command config!\n");
	return -1;
#endif
}

static int usb_test_command(int argc, const char **argv)
{
	int ret = -1;
	if (argc < 2) {
		printf("ERR: command error\n");
		usb_test_show_help();
		return -1;
	}

	if (!strcmp(argv[1], "hci"))
		ret = usb_test_command_hci(argc, argv);
	else if (!strcmp(argv[1], "udc"))
		ret = usb_test_command_udc(argc, argv);
	else if (!strcmp(argv[1], "phy_range"))
		ret = usb_test_command_phy_range(argc, argv);
	else if (!strcmp(argv[1], "dl_adjust"))
		ret = usb_test_command_dl_adjust(argc, argv);
	else if (!strcmp(argv[1], "ed_test"))
		ret = usb_test_command_ed_test(argc, argv);
	else if (!strcmp(argv[1], "debug"))
		ret = usb_test_command_debug(argc, argv);
	else if (!strcmp(argv[1], "uvc_test"))
		ret = usb_test_command_uvc(argc, argv);


	if (ret == 0)
		return 0;

	usb_test_show_help();
	return -1;
}

FINSH_FUNCTION_EXPORT_CMD(usb_test_command, usb, usb tests)