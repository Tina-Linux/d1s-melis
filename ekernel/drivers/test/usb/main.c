#include <rtthread.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <usb/sunxi_hal_udc.h>

#include "usb_msg.h"

static int gadget_test(int argc, const char **argv)
{
        printf("usb msg desc init\n");
	usb_msg_desc_init();
        printf("hal udc init\n");
	hal_udc_init();
	hal_udc_register_callback(usb_msg_callback);
	return 0;

}
FINSH_FUNCTION_EXPORT_ALIAS(gadget_test, __cmd_gadget_test, gadget_test);

