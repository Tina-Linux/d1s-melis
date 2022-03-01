#include <stdio.h>
#include <stdlib.h>
#include <hal_sem.h>
#include <usb/usb_gadget.h>
int usb_gadget_mtp_init(void);
int usb_gadget_mtp_deinit(void);
int mtp_usb_init(void)
{
    int ret;
	void SendPlugIn2App(void);
	extern int mtp_exit;

	mtp_exit = 0;
    ret = usb_gadget_mtp_init();
    if (ret < 0)
    {
        printf("usb_gadget_adb_init failed");
        goto err;
    }
    ret = usb_gadget_function_enable("mtp");
    if (ret < 0)
    {
        printf("enable adb function failed");
        goto err;
    }
	SendPlugIn2App();
    return ret;
err:
    return ret;
}

int mtp_usb_exit(void)
{
	void SendPlugOut2App(void);
	extern int mtp_exit;
	hal_sem_t mtp_sem = NULL;
		
	SendPlugOut2App();
	mtp_sem = hal_sem_create(0);
	while (mtp_exit == 0) {
		hal_sem_timedwait(mtp_sem, 100 / (1000 / CONFIG_HZ));
	}
	usb_gadget_function_disable("mtp");
	usb_gadget_mtp_deinit();
	hal_sem_delete(mtp_sem);
	return 0;
}

