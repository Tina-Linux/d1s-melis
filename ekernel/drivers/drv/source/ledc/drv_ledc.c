/*
 * ===========================================================================================
 *
 *       Filename:  drv_leds.c
 *
 *    Description:  implemtaton of twi driver core based on hal.
 *
 *        Version:  Melis3.0
 *         Create:  2021-1-22
 *       Revision:  none
 *       Compiler:  GCC:version 9.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  liuyu@allwinnertech.com
 *   Organization:  SWC-BPD
 *  Last Modified:  2021-2-1
 *
 * ===========================================================================================
 */
#include <sunxi_drv_ledc.h>
#include <sunxi_hal_ledc.h>
#include <rtthread.h>
#include <log.h>
#include <init.h>
#include <hal_atomic.h>
#include <stdlib.h>
#include <interrupt.h>

#ifdef LED_DEBUG
#define led_info(fmt, args...)  printf("%s()%d - "fmt, __func__, __LINE__, ##args)
#else
#define led_info(fmt, args...)
#endif

#define led_err(fmt, args...)  printf("%s()%d - "fmt, __func__, __LINE__, ##args)

struct ledc_config ledc_config = {
	.led_count = 3,
	.reset_ns = 84,
	.t1h_ns = 800,
	.t1l_ns = 450,
	.t0h_ns = 400,
	.t0l_ns = 850,
	.wait_time0_ns = 84,
	.wait_time1_ns = 84,
	.wait_data_time_ns = 600000,
	.output_mode = "GRB",
};

struct sunxi_led *led;

static int sunxi_ledc_complete(struct sunxi_led *led)
{
	unsigned long flags = 0;
	unsigned long timeout = 0;
	u32 irq_regval;
	u32 hal_sem_ret;

	/* sleep and wait set data complete, timeout = 5*HZ */
	hal_sem_ret = hal_sem_timedwait(led->hal_sem, 500);
	if (hal_sem_ret < 0) {
		irq_regval = hal_ledc_get_irq_status();
		led_info("LEDC INTERRUPT STATUS REG IS %x", irq_regval);
		led_info("led xfer timeout\n");
		return -1;
	} else if (led->result == RESULT_ERR) {
		return -2;
	}

	led_info( "xfer complete\n");

	//__cspr =  hal_spin_lock_irqsave(&led->lock);
	led->result = 0;
	//hal_spin_unlock_irqrestore(&led->lock, __cspr);

	return 0;
}

static irqreturn_t sunxi_ledc_irq_handler(int irq, void *dummy)
{
	led_info("=======enter irq_handler=====\n");
	struct sunxi_led *led = (struct sunxi_led *)dummy;
	unsigned int irq_status;
	u32 hal_sem_ret;

	irq_status = hal_ledc_get_irq_status();
	hal_ledc_clear_all_irq();

	if (irq_status & LEDC_TRANS_FINISH_INT)
		led->result = RESULT_COMPLETE;

	if (irq_status & LEDC_WAITDATA_TIMEOUT_INT)
		led->result = RESULT_ERR;

	if (irq_status & LEDC_FIFO_OVERFLOW_INT)
		led->result = RESULT_ERR;

	hal_sem_ret = hal_sem_post(led->hal_sem);
	led->config.length = 0;
	hal_ledc_reset();
}


int sunxi_led_get_config(struct ledc_config *config)
{
	*config = ledc_config;
	return 0;
}

void sunxi_led_deinit(void)
{
	free_irq(SUNXI_IRQ_LEDC, led);
	hal_ledc_deinit();
	free(led->config.data);
	free(led);
	hal_sem_delete(led->hal_sem);
}


int sunxi_led_init(void)
{
	int ret = -1;
	unsigned long int size;

	led_info("sunxi_led_init\n");

	led = malloc(sizeof(struct sunxi_led));
	if (NULL == led) {
		led_err("sunxi led malloc err\n");
		return -1;
	}

	sunxi_led_get_config(&led->config);

	led->config.data = malloc(sizeof(unsigned int) * led->config.led_count);
	if (NULL == led->config.data) {
		led_err("sunxi led config data malloc err\n");
		goto err1;
	}

	led->hal_sem = hal_sem_create(0);
	if (led->hal_sem == NULL)
	{
		led_info("led creating hal semaphore failed \n");
		goto errsem;
	}

	hal_ledc_init();

	if (request_irq(SUNXI_IRQ_LEDC, sunxi_ledc_irq_handler, 0, "ledc-irq", led) < 0)
	{
		led_info("ledc request irq failed \n");
		goto errirq;
	}

	enable_irq(SUNXI_IRQ_LEDC);
	led_info("sunxi_led_init success\n");

	return 0;

errirq:
	free_irq(SUNXI_IRQ_LEDC, led);

errsem:
	hal_sem_delete(led->hal_sem);
	free(led->config.data);
err1:
	free(led);

	return -1;
}

int sunxi_set_all_led(unsigned int brightness)
{
	int i;
	u32 __scpr;

	led->config.length = led->config.led_count;
	for(i = 0;i < led->config.led_count;i++)
		led->config.data[i] = brightness;

	hal_ledc_trans_data(&led->config);
}

int sunxi_set_led_brightness(int led_num, unsigned int brightness)
{
	uint32_t flags = 0;
	if (NULL == led) {
		led_err("err : ledc is not init\n");
		return -1;
	}

	if (led_num > led->config.led_count) {
		led_err("has not the %d led\n", led_num);
		return -1;
	}

	led->config.length = 1;
	led->config.data[led_num-1] = brightness;

	hal_ledc_trans_data(&led->config);

	sunxi_ledc_complete(led);

	return 0;
}
