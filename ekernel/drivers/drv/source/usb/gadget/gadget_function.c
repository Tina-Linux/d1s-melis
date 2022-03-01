#include <init.h>
int sunxi_gadget_init(void)
{
#if	defined (CONFIG_DRIVERS_USB_GADGET_ADB)
	extern int adbd_main(void);
	adbd_main();
#elif defined (CONFIG_DRIVERS_USB_GADGET_MTP)
	extern int mtpd_main(void);
	mtpd_main();
#endif
    return 0;
}
void sunxi_gadget_exit(void)
{
#if	defined (CONFIG_DRIVERS_USB_GADGET_ADB)
	extern int adbd_exit(void);
	adbd_exit();
#elif defined (CONFIG_DRIVERS_USB_GADGET_MTP)
	extern int mtpd_exit(void);
	mtpd_exit();
#endif
}

#ifndef CONFIG_DRIVER_USB_MANAGER 
late_initcall(sunxi_gadget_init);
#endif
