#include "gpio.h"
#include "delay.h"

#ifdef __MELIS__

extern void delay1ms(void);

void xr_drv_msleep(unsigned int ms)
{
	if (ms < 5) {
		int i;
		//delay1ms(ms);
		for (i = 0; i < ms; i++) {
			delay1ms();
		}
	} else {
        XR_OS_MSleep(ms);
	}
}

#endif /* __MELIS__ */
