/*****************************************************************************
**
**  Name:           host_gpio.c
**
**  Description:    This is the wrapper file for Hardware Abstraction
**                  layer (HAL) and OS Abstraction Layer (OSAL) of MHD,
**                  Broadcom's WiFi host driver.
**
**  Copyright (c) 2018, Broadcom Inc., All Rights Reserved.
**  Broadcom WCC. Proprietary and confidential.
**
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <rtthread.h>
#include <rtdevice.h>

#include <sunxi_drv_gpio.h>
#include "host_gpio.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

//EVB2.0
//#define WIFI_POWER_PIN_NUM           (GPIO_PG7)
#define WIFI_OOB_PIN_NUM             (GPIO_PG6)

//D201
#define WIFI_POWER_PIN_NUM           (GPIO_PL2)
//#define WIFI_OOB_PIN_NUM             (GPIO_PE16)

#define WIFI_SDC_INDEX                1

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/
static uint32_t wifi_oob_irq;

/******************************************************
 *             Function definitions
 ******************************************************/

extern void mhd_thread_notify_irq( void );

/******************************************************
 *               Functions Code
 ******************************************************/

static irqreturn_t sdio_oob_irq_handler(int unused, void *data)		
{
    mhd_thread_notify_irq( );

    return 0;
}

/* config oob pin as interrupt source, rising edge trigger */
void host_oob_interrupt_config( void )
{
    /* Init input switch GPIO. */
    gpio_status_t ret;

    GPIO_INFO( "[WiFi] Interrupt mode Enabled. PIN[%d]\n", WIFI_OOB_PIN_NUM );

    ret = drv_gpio_to_irq(WIFI_OOB_PIN_NUM, &wifi_oob_irq);
    if (ret < 0)
    {
        GPIO_ERR("gpio to irq error, error num: %d\n", ret);
        return;
    }

    //ret = drv_gpio_irq_request(wifi_oob_irq, sdio_oob_irq_handler, IRQ_TYPE_EDGE_RISING, NULL);
    ret = drv_gpio_irq_request(wifi_oob_irq, sdio_oob_irq_handler, IRQ_TYPE_LEVEL_HIGH, NULL);
    if (ret < 0)
    {
        GPIO_ERR("request irq error, irq num:%lu error num: %d\n", wifi_oob_irq, ret);
        return;
    }
	
    RT_ASSERT(ret == DRV_GPIO_STATUS_OK);
}

/* enable oob interrupt */
void host_oob_interrupt_enable( void )
{
    gpio_status_t ret;

	ret = drv_gpio_irq_enable(wifi_oob_irq);
    RT_ASSERT(ret == DRV_GPIO_STATUS_OK);
}

/* enable oob interrupt */
void host_oob_interrupt_disable( void )
{
    gpio_status_t ret;

	ret = drv_gpio_irq_disable(wifi_oob_irq);
    RT_ASSERT(ret == DRV_GPIO_STATUS_OK);
}

/* clear the interrupt status */
void host_oob_interrupt_clear( void )
{
	gpio_status_t ret;

    ret = drv_gpio_irq_free(wifi_oob_irq);
    if (ret < 0)
    {
        GPIO_ERR("free irq error, error num: %d\n", ret);
        return;
    }

    RT_ASSERT(ret == DRV_GPIO_STATUS_OK);	
}

/* check oob interrupt if valid */
unsigned int host_oob_interrupt_is_valid( void )
{
    return 0;
}

static int is_wifi_power_up = 0;
void host_wl_regon_config( int is_init )
{
    if (is_init)
    {
        if (is_wifi_power_up) /* If already power up, ignore init again. */
            return;

		drv_gpio_set_pull_status(WIFI_POWER_PIN_NUM, GPIO_PULL_UP);
		drv_gpio_set_direction(WIFI_POWER_PIN_NUM, GPIO_DIRECTION_OUTPUT);
    }
    else
    {
        if (is_wifi_power_up != 0) /* Make sure power down before deinit wl_reg_on pin */
            return;

		drv_gpio_set_pull_status(WIFI_POWER_PIN_NUM, GPIO_PULL_DOWN);
		drv_gpio_set_direction(WIFI_POWER_PIN_NUM, GPIO_DIRECTION_INPUT);
    }
}

void host_wl_regon_up( void )
{
	drv_gpio_set_output(WIFI_POWER_PIN_NUM, GPIO_DATA_HIGH);
    is_wifi_power_up = 1;
    GPIO_INFO("[WiFi] WL_REG_ON Up\n");
}

void host_wl_regon_down( void )
{
	drv_gpio_set_output(WIFI_POWER_PIN_NUM, GPIO_DATA_LOW);
    is_wifi_power_up = 0;
    GPIO_INFO("[WiFi] WL_REG_ON Down\n");
}

uint32_t host_sdc_index_config( void )
{
	uint32_t sdc_index = WIFI_SDC_INDEX;
    GPIO_INFO("[WiFi] host_sdc_index_config %d\n", sdc_index);
	return sdc_index;
}
