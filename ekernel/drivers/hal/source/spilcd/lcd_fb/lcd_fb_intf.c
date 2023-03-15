/*
 * Allwinner SoCs display driver.
 *
 * Copyright (C) 2016 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include "lcd_fb_intf.h"
#include "dev_lcd_fb.h"
#include "../soc/disp_board_config.h"

void *lcd_fb_malloc(u32 size)
{
	void *ptr = hal_malloc(size);
	if(ptr != NULL)
		memset(ptr, 0, size);
	return ptr;
}

void lcd_fb_free(void *ptr)
{
	hal_free(ptr);
}

int lcd_fb_mutex_lock(hal_sem_t *sem)
{
    return hal_sem_wait(*sem);
}

int lcd_fb_mutex_unlock(hal_sem_t *sem)
{
    return hal_sem_post(*sem);
}

int lcd_fb_mutex_init(hal_sem_t *lock)
{
	*lock = hal_sem_create(1);

	if (*lock == NULL)
		return -1;

	return 0;
}

/*
*******************************************************************************
*                     lcd_fb_register_irq
*
* Description:
*    irq register
*
* Parameters:
*    irqno	    ��input.  irq no
*    flags	    ��input.
*    Handler	    ��input.  isr handler
*    pArg	        ��input.  para
*    DataSize	    ��input.  len of para
*    prio	        ��input.    priority

*
* Return value:
*
*
* note:
*    typedef s32 (*ISRCallback)( void *pArg)��
*
*******************************************************************************
*/
int lcd_fb_register_irq(u32 IrqNo, u32 Flags, void *Handler, void *pArg,
			  u32 DataSize, u32 Prio)
{
	lcd_fb_inf("%s, irqNo=%d, Handler=0x%p, pArg=0x%p\n", __func__, IrqNo,
	      Handler, pArg);
	return hal_request_irq(IrqNo, (hal_irq_handler_t) Handler, "display", pArg);
}

/*
*******************************************************************************
*                     lcd_fb_unregister_irq
*
* Description:
*    irq unregister
*
* Parameters:
*    irqno	��input.  irq no
*    handler	��input.  isr handler
*    Argment	��input.    para
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
void lcd_fb_unregister_irq(u32 IrqNo, void *Handler, void *pArg)
{
	hal_free_irq(IrqNo);
}

/*
*******************************************************************************
*                     lcd_fb_enable_irq
*
* Description:
*    enable irq
*
* Parameters:
*    irqno ��input.  irq no
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
void lcd_fb_enable_irq(u32 IrqNo)
{
	/* hal_enable_irq(IrqNo); */
}

/*
*******************************************************************************
*                     lcd_fb_disable_irq
*
* Description:
*    disable irq
*
* Parameters:
*     irqno ��input.  irq no
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
void lcd_fb_disable_irq(u32 IrqNo)
{
	/* hal_disable_irq(IrqNo); */
}

/* type: 0:invalid, 1: int; 2:str, 3: gpio */
int lcd_fb_script_get_item(char *main_name, char *sub_name, int value[],
			     int type)
{
	return disp_get_property_value(main_name, sub_name, value, type);

}

int lcd_fb_get_ic_ver(void)
{
	return 0;
}

int lcd_fb_gpio_request(struct disp_gpio_set_t *gpio_list,
			  u32 group_count_max)
{
	int ret = 0;
	u32 gpio, mul_sel, pull, drv_level, data;
	char pin_name[32];
	u32 config;

	if (gpio_list == NULL) {
		lcd_fb_wrn("%s: gpio list is null\n", __func__);
		return 0;
	}

	gpio = gpio_list->gpio;
	mul_sel = gpio_list->mul_sel;
	pull = gpio_list->pull;
	drv_level = gpio_list->drv_level;
	data = gpio_list->data;
	hal_gpio_pinmux_set_function(gpio, mul_sel);
	//hal_gpio_set_direction(gpio, GPIO_DIRECTION_OUTPUT);
	hal_gpio_set_pull(gpio, pull);
	hal_gpio_set_driving_level(gpio, drv_level);
	hal_gpio_set_data(gpio, data);

	return gpio;
}

int lcd_fb_gpio_release(int p_handler, s32 if_release_to_default_status)
{
	if (p_handler)
		hal_gpio_pinmux_set_function(p_handler, 7);
	else
		lcd_fb_wrn("OSAL_GPIO_Release, hdl is NULL\n");

	return 0;
}

/* direction: 0:input, 1:output */
int lcd_fb_gpio_set_direction(u32 p_handler, u32 direction,
				const char *gpio_name)
{
	int ret = -1;

	if (p_handler) {
		ret = hal_gpio_set_direction(p_handler, direction);
	} else {
		lcd_fb_wrn("OSAL_GPIO_DevSetONEPIN_IO_STATUS, hdl is NULL\n");
		ret = -1;
	}
	return ret;
}

int lcd_fb_gpio_get_value(u32 p_handler, const char *gpio_name)
{
	gpio_data_t data = 0;
	if (p_handler) {
		if(!hal_gpio_get_data(p_handler, &data))
			return data;
		else {
			lcd_fb_wrn("gpio_get_data fail!\n");
			return -1;
		}
	}
	lcd_fb_wrn("OSAL_GPIO_DevREAD_ONEPIN_DATA, hdl is NULL\n");

	return -1;
}

int lcd_fb_gpio_set_value(u32 p_handler, u32 value_to_gpio,
			    const char *gpio_name)
{
	if (p_handler)
		hal_gpio_set_data(p_handler, value_to_gpio);
	else
		lcd_fb_wrn("OSAL_GPIO_DevWRITE_ONEPIN_DATA, hdl is NULL\n");

	return 0;
}

int lcd_fb_pin_set_state(char *dev_name, char *name)
{
	u32 len  = 0, i = 0;
	struct disp_gpio_set_t *p_list = NULL;
	p_list = disp_get_all_pin_property(dev_name, &len);

	if (!p_list || !len) {
		/* not every lcd need pin config */
		lcd_fb_inf("No pin to be configureed!\n");
		return 0;
	}
	for (i = 0; i < len; ++i) {
		if (!strcmp(name, DISP_PIN_STATE_ACTIVE)) {
			hal_gpio_pinmux_set_function(p_list[i].gpio,
						     p_list[i].mul_sel);
			hal_gpio_set_pull(p_list[i].gpio, p_list[i].pull);
			hal_gpio_set_driving_level(p_list[i].gpio,
						   p_list[i].drv_level);
			hal_gpio_set_data(p_list[i].gpio, p_list[i].data);
		} else if (!strcmp(name, DISP_PIN_STATE_SLEEP)) {
			hal_gpio_pinmux_set_function(p_list[i].gpio,
						     GPIO_MUXSEL_DISABLED);
			hal_gpio_set_pull(p_list[i].gpio, p_list[i].pull);
			hal_gpio_set_driving_level(p_list[i].gpio,
						   p_list[i].drv_level);
			hal_gpio_set_data(p_list[i].gpio, p_list[i].data);
		} else {
			lcd_fb_wrn("Wrong pin state name:%s\n", name);
			return -1;
		}
	}
	return 0;
}

int lcd_fb_power_enable(char *name)
{
	int ret = 0;
#ifdef CONFIG_DRIVERS_REGULATOR
	struct disp_power_t *tmp = (struct disp_power_t *)p_power;
	struct regulator_dev regulator;

	if (!tmp) {
		ret = -1;
		lcd_fb_wrn("NUll pointer!\n");
		goto OUT;
	}
	lcd_fb_inf("Enabling power :%s %lu\n", tmp->power_name, tmp->power_vol);
	hal_regulator_get(REGULATOR_GET(tmp->power_type, tmp->power_id), &regulator);
	ret = hal_regulator_set_voltage(&regulator, tmp->power_vol);
	if (ret)
		lcd_fb_wrn("set %s power vol %d fail!\n", tmp->power_name, tmp->power_vol);

	ret = hal_regulator_enable(&regulator);
	if (ret)
		lcd_fb_wrn("Enable %s power fail!\n", tmp->power_name);

OUT:
#endif
	return ret;
}

int lcd_fb_power_disable(char *name)
{
	int ret = 0;
#ifdef CONFIG_DRIVERS_REGULATOR
	struct disp_power_t *tmp = (struct disp_power_t *)p_power;
	struct regulator_dev regulator;

	if (!tmp) {
		ret = -1;
		lcd_fb_wrn("NUll pointer!\n");
		goto OUT;
	}
	ret = hal_regulator_get(REGULATOR_GET(tmp->power_type, tmp->power_id), &regulator);

	if (tmp->always_on == false) {
		lcd_fb_inf("Disabling power :%s\n", tmp->power_name);
		ret = hal_regulator_disable(&regulator);
		if (ret)
			lcd_fb_wrn("Disable %s power fail!\n", tmp->power_name);
	}

OUT:
#endif
	return ret;
}

s32 disp_delay_ms(u32 ms)
{
	return hal_msleep(ms);
}

s32 disp_delay_us(u32 us)
{
	return hal_usleep(us);
}

uintptr_t lcd_fb_pwm_request(u32 pwm_id)
{
	uintptr_t ret = 0;
#if !defined(CONFIG_FPGA_V4_PLATFORM) \
	&& !defined(CONFIG_FPGA_V7_PLATFORM)
	struct disp_pwm_dev *dev = NULL;

	hal_pwm_init();
	dev = lcd_fb_malloc(sizeof(struct disp_pwm_dev));
	dev->pwm_channel_id = pwm_id;

	ret = (uintptr_t) dev;

#endif
	return ret;
}

int lcd_fb_pwm_free(uintptr_t p_handler)
{
	int ret = 0;
#if !defined(CONFIG_FPGA_V4_PLATFORM) \
	&& !defined(CONFIG_FPGA_V7_PLATFORM)
	struct disp_pwm_dev *dev = (struct disp_pwm_dev *)p_handler;

	if (dev)
		lcd_fb_free(dev);

#endif
	return ret;
}

int lcd_fb_pwm_enable(uintptr_t p_handler)
{
	int ret = 0;
#if !defined(CONFIG_FPGA_V4_PLATFORM) \
	&& !defined(CONFIG_FPGA_V7_PLATFORM)
	struct disp_pwm_dev *pwm_dev;

	pwm_dev = (struct disp_pwm_dev *)p_handler;
	if (pwm_dev) {
		lcd_fb_inf("period_ns = %d \n", pwm_dev->cfg.period_ns);
		lcd_fb_inf("duty_ns = %d \n",  pwm_dev->cfg.duty_ns);
		lcd_fb_inf("polarity = %d \n", pwm_dev->cfg.polarity);
		lcd_fb_inf("channel = %d \n", pwm_dev->pwm_channel_id);
		if (pwm_dev->cfg.period_ns != 0 && pwm_dev->cfg.duty_ns !=0)
			ret = hal_pwm_control(pwm_dev->pwm_channel_id, &pwm_dev->cfg);
		pwm_dev->enable = true;
	}
#endif
	return ret;
}

int lcd_fb_pwm_disable(uintptr_t p_handler)
{
	int ret = 0;
#if !defined(CONFIG_FPGA_V4_PLATFORM) \
	&& !defined(CONFIG_FPGA_V7_PLATFORM)
	struct disp_pwm_dev *pwm_dev;

	pwm_dev = (struct disp_pwm_dev *)p_handler;
	pwm_dev->enable = false;

	if (pwm_dev->cfg.polarity)
		pwm_dev->cfg.duty_ns = 0;
	else
		pwm_dev->cfg.duty_ns = pwm_dev->cfg.period_ns;
	if (pwm_dev)
		ret = hal_pwm_control(pwm_dev->pwm_channel_id, &pwm_dev->cfg);

	hal_pwm_disable_controller(pwm_dev->pwm_channel_id);
#endif
	return ret;
}

int lcd_fb_pwm_config(uintptr_t p_handler, int duty_ns, int period_ns)
{
	int ret = 0;
#if !defined(CONFIG_FPGA_V4_PLATFORM) \
	&& !defined(CONFIG_FPGA_V7_PLATFORM)
	struct disp_pwm_dev *pwm_dev;

	pwm_dev = (struct disp_pwm_dev *)p_handler;
	if (!pwm_dev) {
		lcd_fb_wrn("disp_sys_pwm_Config, handle is NULL!\n");
		ret = -1;
	} else {
		pwm_dev->cfg.duty_ns = duty_ns;
		pwm_dev->cfg.period_ns = period_ns;
		if (pwm_dev->enable == true)
			ret = hal_pwm_control(pwm_dev->pwm_channel_id, &pwm_dev->cfg);
	}
#endif
	return ret;
}

int lcd_fb_pwm_set_polarity(uintptr_t p_handler, int polarity)
{
	int ret = 0;
#if !defined(CONFIG_FPGA_V4_PLATFORM) \
	&& !defined(CONFIG_FPGA_V7_PLATFORM)
	struct disp_pwm_dev *pwm_dev;

	pwm_dev = (struct disp_pwm_dev *)p_handler;
	if (!pwm_dev) {
		lcd_fb_wrn("disp_sys_pwm_Config, handle is NULL!\n");
		ret = -1;
	} else {
		pwm_dev->cfg.polarity = polarity;
	}
#endif
	return ret;
}
