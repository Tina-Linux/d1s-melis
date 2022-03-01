#ifndef __SUNXI_DRV_LEDC_H
#define __SUNXI_DRV_LEDC_H

#include <hal_sem.h>
#include "sunxi_hal_ledc.h"
#include <rtdef.h>
#include <hal_atomic.h>

#define RESULT_COMPLETE 1
#define RESULT_ERR      2
typedef unsigned char u8;
struct sunxi_led {
	u8 result;
	struct ledc_config config;
	rt_sem_t hal_sem;
};

int sunxi_led_init(void);
int sunxi_set_led_brightness(int led_num, unsigned int brightness);

#endif
