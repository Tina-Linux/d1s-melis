#include "gpio.h"

#include "compiler.h"

#include "board.h"
#include "drivers/pin.h"

struct rt_device xr_wlan_gpio;

#define PE5_INDEX 44 /* 44 is E5 */

void xr_wlan_irq_subscribe(void *func)
{
	//gpio_set_func(GPIO_PORT_E, GPIO_PIN_5, IO_INPUT(0));
	//gpio_set_pull_mode(GPIO_PORT_E, GPIO_PIN_5, PULL_UP);
	rt_pin_mode(PE5_INDEX, PIN_MODE_INPUT_PULLUP);
	gpio_set_drive_level(GPIO_PORT_E, GPIO_PIN_5, DRV_LEVEL_2); /* no rt pin interface, use aw's. */

	rt_pin_attach_irq(PE5_INDEX, PIN_IRQ_MODE_RISING, func, (void *)NULL);
	rt_pin_irq_enable(PE5_INDEX, PIN_IRQ_ENABLE);
}

__nonxip_text
void xr_wlan_irq_enable(void)
{
	rt_pin_irq_enable(PE5_INDEX, PIN_IRQ_ENABLE);
}

__nonxip_text
void xr_wlan_irq_disable(void)
{
	rt_pin_irq_enable(PE5_INDEX, PIN_IRQ_DISABLE);
}

void xr_wlan_irq_unsubscribe(void *func)
{
	rt_pin_detach_irq(PE5_INDEX);
}
