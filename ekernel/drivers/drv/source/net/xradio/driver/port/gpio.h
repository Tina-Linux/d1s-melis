#ifndef _XR_GPIO_H_
#define _XR_GPIO_H_

#include "../port/xr_types.h"

#if defined(__MELIS__)

#include "epdk.h"
int gpio_direction_output(unsigned gpio, int value);

void xr_wlan_irq_enable(void);
void xr_wlan_irq_subscribe(void *func);
void xr_wlan_irq_unsubscribe(void *func);
void xr_wlan_irq_clean(void);

#elif (defined(__CONFIG_ARCH_STM32))

#include "gpio_api.h"
#define gpio_direction_output gpio_write
static XR_INLINE void xr_wlan_irq_disable(void) { }
//extern void xradio_gpio_irq(void);
static XR_INLINE void xr_wlan_irq_enable_clr(void) { }

void xr_wlan_irq_enable(void);
void xr_wlan_irq_subscribe(void *func);
void xr_wlan_irq_unsubscribe(void *func);
void xr_wlan_irq_clean(void);

#elif (defined(__CONFIG_CHIP_XRADIO))

#include "driver/chip/chip.h"
#include "driver/chip/drivers/hal_nvic.h"
#include <stdio.h>

static XR_INLINE void xr_wlan_irq_subscribe(void *func)
{
	HAL_NVIC_SetIRQHandler(WIFIC_IRQn, (NVIC_IRQHandler)func);
	HAL_NVIC_SetPriority(WIFIC_IRQn, NVIC_PERIPH_PRIO_DEFAULT);
	HAL_NVIC_EnableIRQ(WIFIC_IRQn);
}

static XR_INLINE void xr_wlan_irq_clean(void)
{
	HAL_NVIC_ClearPendingIRQ(WIFIC_IRQn);
}

static XR_INLINE void xr_wlan_irq_enable(void)
{
	HAL_NVIC_EnableIRQ(WIFIC_IRQn);
}

static XR_INLINE void xr_wlan_irq_enable_clr(void)
{
	HAL_NVIC_ClearPendingIRQ(WIFIC_IRQn);
	HAL_NVIC_EnableIRQ(WIFIC_IRQn);
}

static __always_inline void xr_wlan_irq_disable(void)
{
	HAL_NVIC_DisableIRQ(WIFIC_IRQn);
}

static XR_INLINE void xr_wlan_irq_unsubscribe(void *func)
{
	HAL_NVIC_DisableIRQ(WIFIC_IRQn);
}

#elif (defined(CONFIG_OS_RTTHREAD))

void xr_wlan_irq_disable(void);

void xr_wlan_irq_enable(void);
void xr_wlan_irq_subscribe(void *func);
void xr_wlan_irq_unsubscribe(void *func);


#elif (defined(CONFIG_OS_YUNOS) || defined(CONFIG_OS_NUTTX))

typedef enum {
	HAL_IRQ_TYPE_NONE			= 0x00000000,
	HAL_IRQ_TYPE_EDGE_RISING	= 0x00000001,
	HAL_IRQ_TYPE_EDGE_FALLING	= 0x00000002,
	HAL_IRQ_TYPE_EDGE_BOTH		= (HAL_IRQ_TYPE_EDGE_FALLING | HAL_IRQ_TYPE_EDGE_RISING),
	HAL_IRQ_TYPE_LEVEL_HIGH 	= 0x00000004,
	HAL_IRQ_TYPE_LEVEL_LOW		= 0x00000008,
}hal_gpio_interrupt_mode_t;

void xr_wlan_irq_disable(void);
void xr_wlan_irq_enable(void);
void xr_wlan_irq_enable_clr(void);
void xr_wlan_irq_subscribe(void *func);
void xr_wlan_irq_unsubscribe(void *func);
int  xradio_wlan_gpio_power(int on);
#endif

extern void xradio_gpio_irq(void);

#endif /* _XR_GPIO_H_ */
