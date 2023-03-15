#ifndef USB_TEST_H
#define USB_TEST_H

#include <sunxi_hal_usb.h>
#include <usb_os_platform.h>

#ifdef __cplusplus
extern "C" {
#endif

int usb_test_cmd_hci(int argc, const char **argv);
int usb_test_cmd_udc(int argc, const char **argv);
int usb_test_cmd_phy_range(int argc, const char **argv);
int usb_test_cmd_dl_adjust(int argc, const char **argv);
int usb_test_cmd_ed_test(int argc, const char **argv);
int usb_test_cmd_debug(int argc, const char **argv);
int usb_test_cmd_uvc(int argc, const char **argv);


int usb_test_is_otg(int port);
int usb_test_get_port(const char *buf, int *port);
void usb_test_show_help(void);

unsigned char usb_core_is_enabled(void);
unsigned char sunxi_ehci_status_get(void);

#ifdef __cplusplus
}
#endif

#endif // USB_TEST_H
