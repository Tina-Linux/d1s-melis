#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern int usb_gadget_ncm_init(void);
extern int usb_gadget_ncm_deinit(void);
extern int usb_gadget_function_disable(const char *name);
extern int usb_gadget_function_enable(const char *name);

int ncm_main(void)
{
    printf("%s %d ncm service init successful\n", __FILE__, __LINE__);
	usb_gadget_ncm_init();
	usb_gadget_function_enable("ncm");
    return 0;
}

int ncm_exit(void)
{
    printf("ncm service init successful\n");
	usb_gadget_function_disable("ncm");
	usb_gadget_ncm_deinit();
	return 0;
}

