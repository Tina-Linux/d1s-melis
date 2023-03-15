/*
 * Allwinner SoCs display driver.
 *
 * Copyright (C) 2016 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <hal_cache.h>
#include "disp_lcd.h"
#include "disp_display.h"

static struct lcd_spi_master spim;
static struct lcd_dbi_master dbim;

struct disp_lcd_private_data {
	struct disp_lcd_flow open_flow;
	struct disp_lcd_flow close_flow;
	struct disp_panel_para panel_info;
	struct panel_extend_para panel_extend_info;
	struct panel_extend_para panel_extend_info_set;
	u32    panel_extend_dirty;
	struct disp_lcd_cfg lcd_cfg;
	struct disp_lcd_panel_fun lcd_panel_fun;
	bool enabling;
	bool disabling;
	bool bl_enabled;
	u32 enabled;
	u32 power_enabled;
	u32 bl_need_enabled;
	s32 color_temperature;
	u32 color_inverse;
	struct {
		uintptr_t dev;
		u32 channel;
		u32 polarity;
		u32 period_ns;
		u32 duty_ns;
		u32 enabled;
	} pwm_info;
	hal_sem_t layer_mlock;
	hal_sem_t wait;
	unsigned long wait_count;
	osal_timer_t timer;
};

static hal_spinlock_t disp_lock;
static u32 lcd_data_lock;

static struct lcd_fb_device *lcds;
static struct disp_lcd_private_data *lcd_private;

struct lcd_fb_device *disp_get_lcd(u32 disp)
{
	if (disp > SUPPORT_MAX_LCD - 1)
		return NULL;
	return &lcds[disp];
}
static struct disp_lcd_private_data *disp_lcd_get_priv(struct lcd_fb_device *lcd)
{
	if (lcd == NULL) {
		lcd_fb_wrn("param is NULL!\n");
		return NULL;
	}

	return (struct disp_lcd_private_data *)lcd->priv_data;
}

static s32 disp_lcd_is_used(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	s32 ret = 0;

	if ((lcd == NULL) || (lcdp == NULL))
		ret = 0;
	else
		ret = (s32)lcdp->lcd_cfg.lcd_used;

	return ret;
}

static s32 lcd_parse_panel_para(u32 disp, struct disp_panel_para *info)
{
	s32 ret = 0;
	char primary_key[25];
	s32 value = 0;
	u32 Bpp = 0, lines = 0;

	sprintf(primary_key, "lcd_fb%d", disp);
	memset(info, 0, sizeof(struct disp_panel_para));

	ret = lcd_fb_script_get_item(primary_key, "lcd_x", &value, 1);
	if (ret == 1)
		info->lcd_x = value;

	ret = lcd_fb_script_get_item(primary_key, "lcd_y", &value, 1);
	if (ret == 1)
		info->lcd_y = value;

	ret = lcd_fb_script_get_item(primary_key, "lcd_width", &value, 1);
	if (ret == 1)
		info->lcd_width = value;

	ret = lcd_fb_script_get_item(primary_key, "lcd_height", &value, 1);
	if (ret == 1)
		info->lcd_height = value;

	/*unit:Mhz, speed of rgb data transfer*/
	ret = lcd_fb_script_get_item(primary_key, "lcd_data_speed", &value, 1);
	if (ret == 1)
		info->lcd_data_speed = value;

	ret = lcd_fb_script_get_item(primary_key, "lcd_pwm_used", &value, 1);
	if (ret == 1)
		info->lcd_pwm_used = value;

	if (info->lcd_pwm_used) {
		ret = lcd_fb_script_get_item(primary_key, "lcd_pwm_ch", &value, 1);
		if (ret == 1)
			info->lcd_pwm_ch = value;

		ret = lcd_fb_script_get_item(primary_key, "lcd_pwm_freq", &value, 1);
		if (ret == 1)
			info->lcd_pwm_freq = value;

		ret = lcd_fb_script_get_item(primary_key, "lcd_pwm_pol", &value, 1);
		if (ret == 1)
			info->lcd_pwm_pol = value;
	}

	ret = lcd_fb_script_get_item(primary_key, "lcd_if", &value, 1);
	if (ret == 1)
		info->lcd_if = value;

	ret = lcd_fb_script_get_item(primary_key, "lcd_pixel_fmt", &value, 1);
	if (ret == 1)
		info->lcd_pixel_fmt = value;

	ret = lcd_fb_script_get_item(primary_key, "lcd_dbi_fmt", &value, 1);
	if (ret == 1)
		info->lcd_dbi_fmt = value;

	ret = lcd_fb_script_get_item(primary_key, "lcd_dbi_clk_mode", &value, 1);
	if (ret == 1)
		info->lcd_dbi_clk_mode = value;

	ret = lcd_fb_script_get_item(primary_key, "lcd_dbi_te", &value, 1);
	if (ret == 1)
		info->lcd_dbi_te = value;

	ret = lcd_fb_script_get_item(primary_key, "fb_buffer_num", &value, 1);
	if (ret == 1)
		info->fb_buffer_num = value;
	else
		info->fb_buffer_num = 2;

	ret = lcd_fb_script_get_item(primary_key, "lcd_dbi_if", &value, 1);
	if (ret == 1)
		info->dbi_if = value;


	ret = lcd_fb_script_get_item(primary_key, "lcd_rgb_order", &value, 1);
	if (ret == 1)
		info->lcd_rgb_order = value;

	ret = lcd_fb_script_get_item(primary_key, "lcd_fps", &value, 1);
	if (ret == 1)
		info->lcd_fps = value;
	else
		info->lcd_fps = 25;

	ret = lcd_fb_script_get_item(primary_key, "lcd_spi_bus_num", &value, 1);
	if (ret == 1)
		info->lcd_spi_bus_num = value;

	ret = lcd_fb_script_get_item(primary_key, "lcd_frm", &value, 1);
	if (ret == 1)
		info->lcd_frm = value;


	ret = lcd_fb_script_get_item(primary_key, "lcd_gamma_en", &value, 1);
	if (ret == 1)
		info->lcd_gamma_en = value;

	ret =
	    lcd_fb_script_get_item(primary_key, "lcd_model_name",
				     (int *)info->lcd_model_name, 2);

	return 0;
}

static void lcd_get_sys_config(u32 disp, struct disp_lcd_cfg *lcd_cfg)
{
	struct disp_gpio_set_t *gpio_info;
	int value = 1;
	char primary_key[20], sub_name[25];
	int i = 0;
	int ret;
	int gpio_regu_num = 0, power_num = 0;

	sprintf(primary_key, "lcd_fb%d", disp);
	/* lcd_used */
	ret = lcd_fb_script_get_item(primary_key, "lcd_used", &value, 1);
	if (ret == 1)
		lcd_cfg->lcd_used = value;

	if (lcd_cfg->lcd_used == 0)
		return;

	/* lcd_bl_en */
	lcd_cfg->lcd_bl_en_used = 0;
	gpio_info = &(lcd_cfg->lcd_bl_en);
	ret =
	    lcd_fb_script_get_item(primary_key, "lcd_bl_en", (int *)gpio_info,
				     3);
	if (ret == 3)
		lcd_cfg->lcd_bl_en_used = 1;

	sprintf(sub_name, "lcd_bl_en_power");
	ret =
	    lcd_fb_script_get_item(primary_key, sub_name,
				     (int *)lcd_cfg->lcd_bl_en_power, 2);

	ret = lcd_fb_script_get_item(primary_key, "lcd_power_num", &value, 1);
	if (ret == 1)
		power_num = value;

	/* lcd fix power */
	for (i = 0; i < power_num; i++) {
		if (i == 0)
			sprintf(sub_name, "lcd_fix_power");
		else
			sprintf(sub_name, "lcd_fix_power%d", i);
		lcd_cfg->lcd_power_used[i] = 0;
		ret =
		    lcd_fb_script_get_item(primary_key, sub_name,
					     (int *)(lcd_cfg->lcd_fix_power[i]),
					     2);
		if (ret == 2)
			/* str */
			lcd_cfg->lcd_fix_power_used[i] = 1;
	}

	/* lcd_power */
	for (i = 0; i < power_num; i++) {
		if (i == 0)
			sprintf(sub_name, "lcd_power");
		else
			sprintf(sub_name, "lcd_power%d", i);
		lcd_cfg->lcd_power_used[i] = 0;
		ret =
		    lcd_fb_script_get_item(primary_key, sub_name,
					     (int *)(lcd_cfg->lcd_power[i]), 2);
		if (ret == 2)
			/* str */
			lcd_cfg->lcd_power_used[i] = 1;
	}

	/* lcd_gpio */
	for (i = 0; i < 6; i++) {
		sprintf(sub_name, "lcd_gpio_%d", i);

		gpio_info = &(lcd_cfg->lcd_gpio[i]);
		ret =
		    lcd_fb_script_get_item(primary_key, sub_name,
					     (int *)gpio_info, 3);
		if (ret == 3)
			lcd_cfg->lcd_gpio_used[i] = 1;
	}

	sprintf(sub_name, "lcd_spi_dc_pin");
	gpio_info = &(lcd_cfg->lcd_spi_dc_pin);
	ret =
		lcd_fb_script_get_item(primary_key, sub_name,
				       (int *)gpio_info, 3);


	ret = lcd_fb_script_get_item(primary_key, "lcd_gpio_regu_num", &value, 1);
	if (ret == 1 ) {
		gpio_regu_num = value;
	}

	for (i = 0; i < gpio_regu_num; i++) {
		sprintf(sub_name, "lcd_gpio_power%d", i);

		ret =
		    lcd_fb_script_get_item(primary_key, sub_name,
					     (int *)lcd_cfg->lcd_gpio_power[i],
					     2);
	}

	for (i = 0; i < gpio_regu_num; i++) {
		if (i == 0)
			sprintf(sub_name, "lcd_pin_power");
		else
			sprintf(sub_name, "lcd_pin_power%d", i);
		ret =
		    lcd_fb_script_get_item(primary_key, sub_name,
					     (int *)lcd_cfg->lcd_pin_power[i],
					     2);
	}

	/* backlight adjust */
	value = 0;
	lcd_fb_script_get_item(primary_key, "lcd_pwm_used", &value, 1);
	if (value == 1) {

		ret = lcd_fb_script_get_item(primary_key, "lcd_bl_percent_num", &value, 1);
		if (ret == 1 && value > 0) {
			for (i = 0; i < 101; i++) {
				sprintf(sub_name, "lcd_bl_%d_percent", i);
				lcd_cfg->backlight_curve_adjust[i] = 0;

				if (i == 100)
					lcd_cfg->backlight_curve_adjust[i] = 255;

				ret =
					lcd_fb_script_get_item(primary_key, sub_name, &value, 1);
				if (ret == 1) {
					value = (value > 100) ? 100 : value;
					value = value * 255 / 100;
					lcd_cfg->backlight_curve_adjust[i] = value;
				}
			}
		}
		sprintf(sub_name, "lcd_backlight");
		ret = lcd_fb_script_get_item(primary_key, sub_name, &value, 1);
		if (ret == 1) {
			value = (value > 256) ? 256 : value;
			lcd_cfg->backlight_bright = value;
		} else {
			lcd_cfg->backlight_bright = 197;
		}
	}


}

static s32 disp_lcd_pin_cfg(struct lcd_fb_device *lcd, u32 bon)
{
	int i;
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	char dev_name[25];

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}
	lcd_fb_inf("lcd %d pin config, state %s, %d\n", lcd->disp,
	       (bon) ? "on" : "off", bon);

	/* io-pad */
	if (bon == 1) {
		for (i = 0; i < LCD_GPIO_REGU_NUM; i++) {
			if (!
			    ((!strcmp(lcdp->lcd_cfg.lcd_pin_power[i], ""))
			     ||
			     (!strcmp(lcdp->lcd_cfg.lcd_pin_power[i], "none"))))
				lcd_fb_power_enable(lcdp->lcd_cfg.
						      lcd_pin_power[i]);
		}
	}

	sprintf(dev_name, "lcd_fb%d", lcd->disp);
	lcd_fb_pin_set_state(dev_name,
			       (bon == 1) ?
			       DISP_PIN_STATE_ACTIVE : DISP_PIN_STATE_SLEEP);

	if (bon == 0) {
		for (i = LCD_GPIO_REGU_NUM - 1; i >= 0; i--) {
			if (!
			    ((!strcmp(lcdp->lcd_cfg.lcd_pin_power[i], ""))
			     ||
			     (!strcmp(lcdp->lcd_cfg.lcd_pin_power[i], "none"))))
				lcd_fb_power_disable(lcdp->lcd_cfg.
						       lcd_pin_power[i]);
		}
	}

	return 0;
}

static s32 disp_lcd_pwm_enable(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}
	if (lcdp->panel_info.lcd_pwm_used && !lcdp->pwm_info.dev) {
		lcdp->pwm_info.dev =
		    lcd_fb_pwm_request(lcdp->panel_info.lcd_pwm_ch);
	}

	if (disp_lcd_is_used(lcd) && lcdp->pwm_info.dev) {
		lcd_fb_pwm_set_polarity(lcdp->pwm_info.dev, lcdp->pwm_info.polarity);
		return lcd_fb_pwm_enable(lcdp->pwm_info.dev);
	}
	lcd_fb_wrn("pwm device hdl is NULL\n");

	return -1;
}

static s32 disp_lcd_pwm_disable(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	s32 ret = -1;

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	if (disp_lcd_is_used(lcd) && lcdp->pwm_info.dev) {
		ret = lcd_fb_pwm_disable(lcdp->pwm_info.dev);
		lcd_fb_pwm_free(lcdp->pwm_info.dev);
		lcdp->pwm_info.dev = 0;

		return ret;
	}
	lcd_fb_wrn("pwm device hdl is NULL\n");

	return -1;
}

static s32 disp_lcd_backlight_enable(struct lcd_fb_device *lcd)
{
	struct disp_gpio_set_t gpio_info[1];
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	lcd_data_lock = hal_spin_lock_irqsave(&disp_lock);
	if (lcdp->bl_enabled) {
		hal_spin_unlock_irqrestore(&disp_lock, lcd_data_lock);
		return -1;
	}

	lcdp->bl_need_enabled = 1;
	lcdp->bl_enabled = true;
	hal_spin_unlock_irqrestore(&disp_lock, lcd_data_lock);

	if (disp_lcd_is_used(lcd)) {
		unsigned bl;

		if (lcdp->lcd_cfg.lcd_bl_en_used) {
			/* io-pad */
			if (!
			    ((!strcmp(lcdp->lcd_cfg.lcd_bl_en_power, ""))
			     ||
			     (!strcmp(lcdp->lcd_cfg.lcd_bl_en_power, "none"))))
				lcd_fb_power_enable(lcdp->lcd_cfg.
						      lcd_bl_en_power);

			memcpy(gpio_info, &(lcdp->lcd_cfg.lcd_bl_en),
			       sizeof(struct disp_gpio_set_t));

			lcdp->lcd_cfg.lcd_bl_gpio_hdl =
			    lcd_fb_gpio_request(gpio_info, 1);
		}
		bl = disp_lcd_get_bright(lcd);
		disp_lcd_set_bright(lcd, bl);
	}

	return 0;
}

static s32 disp_lcd_backlight_disable(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	lcd_data_lock = hal_spin_lock_irqsave(&disp_lock);
	if (!lcdp->bl_enabled) {
		hal_spin_unlock_irqrestore(&disp_lock, lcd_data_lock);
		return -1;
	}

	lcdp->bl_enabled = false;
	hal_spin_unlock_irqrestore(&disp_lock, lcd_data_lock);

	if (disp_lcd_is_used(lcd)) {
		if (lcdp->lcd_cfg.lcd_bl_en_used) {
			lcd_fb_gpio_release(lcdp->lcd_cfg.lcd_bl_gpio_hdl, 2);

			/* io-pad */
			if (!
			    ((!strcmp(lcdp->lcd_cfg.lcd_bl_en_power, ""))
			     ||
			     (!strcmp(lcdp->lcd_cfg.lcd_bl_en_power, "none"))))
				lcd_fb_power_disable(lcdp->lcd_cfg.
						       lcd_bl_en_power);
		}
	}

	return 0;
}

static s32 disp_lcd_power_enable(struct lcd_fb_device *lcd, u32 power_id)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	if (disp_lcd_is_used(lcd)) {
		if (lcdp->lcd_cfg.lcd_power_used[power_id] == 1) {
			/* regulator type */
			lcd_fb_power_enable(lcdp->lcd_cfg.
					      lcd_power[power_id]);
		}
	}

	return 0;
}

static s32 disp_lcd_power_disable(struct lcd_fb_device *lcd, u32 power_id)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	if (disp_lcd_is_used(lcd)) {
		if (lcdp->lcd_cfg.lcd_power_used[power_id] == 1) {
			/* regulator type */
			lcd_fb_power_disable(lcdp->lcd_cfg.
					       lcd_power[power_id]);
		}
	}

	return 0;
}

static s32 disp_lcd_bright_get_adjust_value(struct lcd_fb_device *lcd, u32 bright)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}
	bright = (bright > 255) ? 255 : bright;
	return lcdp->panel_extend_info.lcd_bright_curve_tbl[bright];
}

static s32 disp_lcd_bright_curve_init(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	u32 i = 0, j = 0;
	u32 items = 0;
	u32 lcd_bright_curve_tbl[101][2];

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	for (i = 0; i < 101; i++) {
		if (lcdp->lcd_cfg.backlight_curve_adjust[i] == 0) {
			if (i == 0) {
				lcd_bright_curve_tbl[items][0] = 0;
				lcd_bright_curve_tbl[items][1] = 0;
				items++;
			}
		} else {
			lcd_bright_curve_tbl[items][0] = 255 * i / 100;
			lcd_bright_curve_tbl[items][1] =
			    lcdp->lcd_cfg.backlight_curve_adjust[i];
			items++;
		}
	}

	for (i = 0; i < items - 1; i++) {
		u32 num =
		    lcd_bright_curve_tbl[i + 1][0] - lcd_bright_curve_tbl[i][0];

		for (j = 0; j < num; j++) {
			u32 value = 0;

			value =
			    lcd_bright_curve_tbl[i][1] +
			    ((lcd_bright_curve_tbl[i + 1][1] -
			      lcd_bright_curve_tbl[i][1]) * j) / num;
			lcdp->panel_extend_info.
			    lcd_bright_curve_tbl[lcd_bright_curve_tbl[i][0] +
						 j] = value;
		}
	}
	lcdp->panel_extend_info.lcd_bright_curve_tbl[255] =
	    lcd_bright_curve_tbl[items - 1][1];

	return 0;
}

s32 disp_lcd_set_bright(struct lcd_fb_device *lcd, u32 bright)
{
	u32 duty_ns;
	unsigned long long backlight_bright = bright;
	unsigned long long backlight_dimming;
	unsigned long long period_ns;
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	bool bright_update = false;

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	lcd_data_lock = hal_spin_lock_irqsave(&disp_lock);
	backlight_bright = (backlight_bright > 255) ? 255 : backlight_bright;
	if (lcdp->lcd_cfg.backlight_bright != backlight_bright) {
		bright_update = true;
		lcdp->lcd_cfg.backlight_bright = backlight_bright;
	}
	hal_spin_unlock_irqrestore(&disp_lock, lcd_data_lock);

	if (lcdp->pwm_info.dev) {
		if (backlight_bright != 0)
			backlight_bright += 1;
		backlight_bright =
		    disp_lcd_bright_get_adjust_value(lcd, backlight_bright);

		lcdp->lcd_cfg.backlight_dimming =
		    (lcdp->lcd_cfg.backlight_dimming ==
		     0) ? 256 : lcdp->lcd_cfg.backlight_dimming;
		backlight_dimming = lcdp->lcd_cfg.backlight_dimming;
		period_ns = lcdp->pwm_info.period_ns;
		duty_ns =
		    (backlight_bright * backlight_dimming * period_ns / 256 +
		     128) / 256;
		lcdp->pwm_info.duty_ns = duty_ns;
		lcd_fb_pwm_config(lcdp->pwm_info.dev, duty_ns, period_ns);
	}

	if (lcdp->lcd_panel_fun.set_bright && lcdp->enabled) {
		lcdp->lcd_panel_fun.set_bright(lcd->disp,
					       disp_lcd_bright_get_adjust_value
					       (lcd, bright));
	}

	return 0;
}

s32 disp_lcd_get_bright(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	return lcdp->lcd_cfg.backlight_bright;
}

static s32 disp_lcd_set_bright_dimming(struct lcd_fb_device *lcd, u32 dimming)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	u32 bl = 0;

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	dimming = dimming > 256 ? 256 : dimming;
	lcdp->lcd_cfg.backlight_dimming = dimming;
	bl = disp_lcd_get_bright(lcd);
	disp_lcd_set_bright(lcd, bl);

	return 0;
}

static s32 disp_lcd_get_panel_info(struct lcd_fb_device *lcd,
				   struct disp_panel_para *info)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	memcpy(info, (struct disp_panel_para *) (&(lcdp->panel_info)),
	       sizeof(struct disp_panel_para));
	return 0;
}



/* lcd enable except for backlight */
static s32 disp_lcd_fake_enable(struct lcd_fb_device *lcd)
{
	lcd_fb_wrn("To be implement!\n");
	return 0;
}



void disp_lcd_vsync_handle(struct lcd_fb_device *p_lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(p_lcd);
	hal_sem_post(lcdp->wait);
}

void disp_lcd_timer_handle(void *timer_para_lcd)
{
	disp_lcd_vsync_handle((struct lcd_fb_device *) timer_para_lcd);
}


#if defined(SUPPORT_DBI_IF)
static s32 disp_lcd_dbi_init(struct lcd_fb_device *lcd)
{
	int ret = -1;
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	lcd_fb_inf("DBI MODE");
	if (!lcd || !lcdp) {
		lcd_fb_wrn("Null pointer!\n");
		return -1;
	}

	memset(&dbim, 0, sizeof(struct lcd_dbi_master));

	dbim.port = lcdp->panel_info.lcd_spi_bus_num;

	dbim.cfg.clock_frequency =
		lcdp->panel_info.lcd_data_speed * 1000 * 1000;
	dbim.cfg.dbi_interface = lcdp->panel_info.dbi_if;
	dbim.cfg.v_size = lcdp->panel_info.lcd_x;
	dbim.cfg.h_size = lcdp->panel_info.lcd_y;

	dbim.cfg.ahb_ready_bypass = 1;
	if (lcdp->panel_info.lcd_pixel_fmt == LCDFB_FORMAT_RGB_565 ||
	    lcdp->panel_info.lcd_pixel_fmt == LCDFB_FORMAT_BGR_565) {
		dbim.cfg.rgb_src_typ = 1;//RGB Source type :0 RGB 32,1 RGB16
	} else
		dbim.cfg.rgb_src_typ = 0;

	dbim.cfg.dbi_source_format = lcdp->panel_info.lcd_rgb_order;
	dbim.cfg.dbi_output_data_format = 0;
	dbim.cfg.te_en = 0;
	dbim.cfg.dbi_output_clk_mode = lcdp->panel_info.lcd_dbi_clk_mode;
	dbim.cfg.dbi_en_mode = DBI_MODE_ALLWAYS_ON;


	ret = hal_dbi_init(dbim.port, &dbim.cfg);
	if (ret != DBI_MASTER_OK)
		lcd_fb_wrn("init dbi master failed - %d\n", ret);

	lcd_fb_inf("dbi interface = %d [0-1:L3, 2-3:L4, 4:D2]", dbim.cfg.dbi_interface);

	return ret;
}
#endif

static s32 disp_lcd_spi_init(struct lcd_fb_device *lcd)
{
	int ret = -1;
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	lcd_fb_inf("SPI MODE");
	if (!lcd || !lcdp) {
		lcd_fb_wrn("Null pointer!\n");
		return -1;
	}

	spim.port = lcdp->panel_info.lcd_spi_bus_num;
	spim.cfg.clock_frequency = 
		lcdp->panel_info.lcd_data_speed * 1000 * 1000;
	spim.cfg.slave_port = HAL_SPI_MASTER_SLAVE_0;
	spim.cfg.cpha = HAL_SPI_MASTER_CLOCK_PHASE0;
	spim.cfg.cpol = HAL_SPI_MASTER_CLOCK_POLARITY0;
	spim.cfg.bit_order = HAL_SPI_MASTER_LSB_FIRST;

	ret = hal_spi_init(spim.port, &spim.cfg);
	if (ret != SPI_MASTER_OK)
		lcd_fb_wrn("init spi master failed - %d\n", ret);

	return ret;

}

static s32 disp_lcd_enable(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	int i;
	unsigned bl;

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}
	lcd_fb_inf("lcd %d\n", lcd->disp);

	if (disp_lcd_is_enabled(lcd) == 1)
		return 0;

	/* init fix power */
	for (i = 0; i < LCD_POWER_NUM; i++) {
		if (lcdp->lcd_cfg.lcd_fix_power_used[i] == 1)
			lcd_fb_power_enable(lcdp->lcd_cfg.lcd_fix_power[i]);
	}

	lcd_data_lock = hal_spin_lock_irqsave(&disp_lock);
	lcdp->enabling = 1;
	lcdp->bl_need_enabled = 0;
	hal_spin_unlock_irqrestore(&disp_lock, lcd_data_lock);

	lcdp->panel_extend_info.lcd_gamma_en = lcdp->panel_info.lcd_gamma_en;
	disp_lcd_gpio_init(lcd);

	lcd_fb_pwm_config(lcdp->pwm_info.dev, lcdp->pwm_info.duty_ns,
			    lcdp->pwm_info.period_ns);
	lcd_fb_pwm_set_polarity(lcdp->pwm_info.dev, lcdp->pwm_info.polarity);

	if (lcdp->panel_info.lcd_if == LCD_FB_IF_SPI) {
		if (disp_lcd_spi_init(lcd)) {
			lcd_fb_wrn("Init spi fail!\n");
			return -1;
		}
	}
	else {
#if defined(SUPPORT_DBI_IF)
		if (disp_lcd_dbi_init(lcd)) {
			lcd_fb_wrn("Init dbi fail!\n");
			return -1;
		}
#else
		lcd_fb_wrn("Please define macro:SUPPORT_DBI_IF in file:lcd_fb_feature.h!\n");
		return -1;
#endif
	}

	lcdp->open_flow.func_num = 0;

	if (lcdp->lcd_panel_fun.cfg_open_flow)
		lcdp->lcd_panel_fun.cfg_open_flow(lcd->disp);
	else
		lcd_fb_wrn("lcd_panel_fun[%d].cfg_open_flow is NULL\n", lcd->disp);

	for (i = 0; i < lcdp->open_flow.func_num; i++) {
		if (lcdp->open_flow.func[i].func) {
			lcdp->open_flow.func[i].func(lcd->disp);
			lcd_fb_inf("open flow:step %d finish, to delay %d\n", i,
			       lcdp->open_flow.func[i].delay);
			if (lcdp->open_flow.func[i].delay != 0)
				disp_delay_ms(lcdp->open_flow.func[i].delay);
		}
	}

	if (lcdp->panel_info.lcd_fps && !lcdp->panel_info.lcd_dbi_te) {
		lcdp->timer = osal_timer_create("disp_vsync", disp_lcd_timer_handle, (void*)lcd,
				1000 / lcdp->panel_info.lcd_fps, OSAL_TIMER_FLAG_PERIODIC);
		osal_timer_start(lcdp->timer);
	}

	lcd_data_lock = hal_spin_lock_irqsave(&disp_lock);
	lcdp->enabled = 1;
	lcdp->enabling = 0;
	hal_spin_unlock_irqrestore(&disp_lock, lcd_data_lock);
	bl = disp_lcd_get_bright(lcd);
	disp_lcd_set_bright(lcd, bl);

	return 0;
}

static s32 disp_lcd_disable(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	int i;

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	lcd_fb_inf("lcd %d\n", lcd->disp);
	if (disp_lcd_is_enabled(lcd) == 0)
		return 0;

	lcd_data_lock = hal_spin_lock_irqsave(&disp_lock);
	lcdp->enabled = 0;
	hal_spin_unlock_irqrestore(&disp_lock, lcd_data_lock);

	if (lcdp->panel_info.lcd_fps && !lcdp->panel_info.lcd_dbi_te)
		osal_timer_delete(lcdp->timer);

	lcdp->bl_need_enabled = 0;
	lcdp->close_flow.func_num = 0;
	if (lcdp->lcd_panel_fun.cfg_close_flow)
		lcdp->lcd_panel_fun.cfg_close_flow(lcd->disp);
	else
		lcd_fb_wrn("lcd_panel_fun[%d].cfg_close_flow is NULL\n", lcd->disp);

	for (i = 0; i < lcdp->close_flow.func_num; i++) {
		if (lcdp->close_flow.func[i].func) {
			lcdp->close_flow.func[i].func(lcd->disp);
			lcd_fb_inf("close flow:step %d finish, to delay %d\n", i,
			       lcdp->close_flow.func[i].delay);
			if (lcdp->close_flow.func[i].delay != 0)
				disp_delay_ms(lcdp->close_flow.func[i].delay);
		}
	}

	disp_lcd_gpio_exit(lcd);

	for (i = LCD_POWER_NUM - 1; i >= 0; i--) {
		if (lcdp->lcd_cfg.lcd_fix_power_used[i] == 1)
			lcd_fb_power_disable(lcdp->lcd_cfg.lcd_fix_power[i]);
	}

#if defined(SUPPORT_DBI_IF)
	if (lcdp->panel_info.lcd_if == LCD_FB_IF_DBI) {
		hal_dbi_deinit(dbim.port);
	} else
#else
	{
		hal_spi_deinit(spim.port);
	}
#endif

	return 0;
}

s32 disp_lcd_is_enabled(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	return (s32)lcdp->enabled;
}

/**
 * disp_lcd_check_if_enabled - check lcd if be enabled status
 *
 * this function only be used by bsp_disp_sync_with_hw to check
 * the device enabled status when driver init
 */
s32 disp_lcd_check_if_enabled(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	int ret = 1;

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	return ret;
}

static s32 disp_lcd_set_open_func(struct lcd_fb_device *lcd, LCD_FUNC func,
				  u32 delay)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	if (func) {
		lcdp->open_flow.func[lcdp->open_flow.func_num].func = func;
		lcdp->open_flow.func[lcdp->open_flow.func_num].delay = delay;
		lcdp->open_flow.func_num++;
	}

	return 0;
}

static s32 disp_lcd_set_close_func(struct lcd_fb_device *lcd, LCD_FUNC func,
				   u32 delay)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	if (func) {
		lcdp->close_flow.func[lcdp->close_flow.func_num].func = func;
		lcdp->close_flow.func[lcdp->close_flow.func_num].delay = delay;
		lcdp->close_flow.func_num++;
	}

	return 0;
}

static s32 disp_lcd_set_panel_funs(struct lcd_fb_device *lcd, char *name,
				   struct disp_lcd_panel_fun *lcd_cfg)
{
	char primary_key[20], drv_name[32];
	s32 ret;
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	sprintf(primary_key, "lcd_fb%d", lcd->disp);

	ret =
	    lcd_fb_script_get_item(primary_key, "lcd_driver_name",
				     (int *)drv_name, 2);
	lcd_fb_inf("lcd %d, driver_name %s,  panel_name %s\n", lcd->disp, drv_name,
	       name);
	if ((ret == 2) && !strcmp(drv_name, name)) {
		memset(&lcdp->lcd_panel_fun,
		       0,
		       sizeof(struct disp_lcd_panel_fun));
		lcdp->lcd_panel_fun.cfg_panel_info = lcd_cfg->cfg_panel_info;
		lcdp->lcd_panel_fun.cfg_open_flow = lcd_cfg->cfg_open_flow;
		lcdp->lcd_panel_fun.cfg_close_flow = lcd_cfg->cfg_close_flow;
		lcdp->lcd_panel_fun.lcd_user_defined_func =
		    lcd_cfg->lcd_user_defined_func;
		lcdp->lcd_panel_fun.set_bright = lcd_cfg->set_bright;
		lcdp->lcd_panel_fun.blank = lcd_cfg->blank;
		lcdp->lcd_panel_fun.set_var = lcd_cfg->set_var;
		lcdp->lcd_panel_fun.set_addr_win = lcd_cfg->set_addr_win;
		if (lcdp->lcd_panel_fun.cfg_panel_info) {
			lcdp->lcd_panel_fun.cfg_panel_info(&lcdp->panel_extend_info);
			memcpy(&lcdp->panel_extend_info_set,
				&lcdp->panel_extend_info, sizeof(struct panel_extend_para));
		} else {
			lcd_fb_inf("lcd_panel_fun[%d].cfg_panel_info is NULL\n", lcd->disp);
		}

		return 0;
	}

	return -1;
}

s32 disp_lcd_gpio_init(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	int i = 0;
	struct disp_gpio_set_t gpio_info[1];

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	/* io-pad */
	for (i = 0; i < LCD_GPIO_REGU_NUM; i++) {
		if (!
		    ((!strcmp(lcdp->lcd_cfg.lcd_gpio_power[i], ""))
		     || (!strcmp(lcdp->lcd_cfg.lcd_gpio_power[i], "none"))))
			lcd_fb_power_enable(lcdp->lcd_cfg.lcd_gpio_power[i]);
	}

	for (i = 0; i < LCD_GPIO_NUM; i++) {
		lcdp->lcd_cfg.gpio_hdl[i] = 0;

		if (lcdp->lcd_cfg.lcd_gpio_used[i]) {

			memcpy(gpio_info, &(lcdp->lcd_cfg.lcd_gpio[i]),
			       sizeof(struct disp_gpio_set_t));
			lcdp->lcd_cfg.gpio_hdl[i] =
			    lcd_fb_gpio_request(gpio_info, 1);
		}
	}

	if (strlen(lcdp->lcd_cfg.lcd_spi_dc_pin.gpio_name)) {
		memcpy(gpio_info, &(lcdp->lcd_cfg.lcd_spi_dc_pin),
		       sizeof(struct disp_gpio_set_t));
		lcdp->lcd_cfg.spi_dc_pin_hdl =
		    lcd_fb_gpio_request(gpio_info, 1);
	}

	return 0;
}

s32 disp_lcd_gpio_exit(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	int i = 0;
	struct disp_gpio_set_t gpio_info[1];

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	for (i = 0; i < LCD_GPIO_NUM; i++) {
		if (lcdp->lcd_cfg.gpio_hdl[i]) {

			lcd_fb_gpio_release(lcdp->lcd_cfg.gpio_hdl[i], 2);

			memcpy(gpio_info, &(lcdp->lcd_cfg.lcd_gpio[i]),
			       sizeof(struct disp_gpio_set_t));
			gpio_info->mul_sel = 7;
			lcdp->lcd_cfg.gpio_hdl[i] =
			    lcd_fb_gpio_request(gpio_info, 1);
			lcd_fb_gpio_release(lcdp->lcd_cfg.gpio_hdl[i], 2);
			lcdp->lcd_cfg.gpio_hdl[i] = 0;
		}
	}

	/* io-pad */
	for (i = LCD_GPIO_REGU_NUM - 1; i >= 0; i--) {
		if (!
		    ((!strcmp(lcdp->lcd_cfg.lcd_gpio_power[i], ""))
		     || (!strcmp(lcdp->lcd_cfg.lcd_gpio_power[i], "none"))))
			lcd_fb_power_disable(lcdp->lcd_cfg.lcd_gpio_power[i]);
	}

	if (strlen(lcdp->lcd_cfg.lcd_spi_dc_pin.gpio_name)) {
		lcd_fb_gpio_release(lcdp->lcd_cfg.spi_dc_pin_hdl, 2);

		memcpy(gpio_info, &(lcdp->lcd_cfg.lcd_spi_dc_pin),
		       sizeof(struct disp_gpio_set_t));
		gpio_info->mul_sel = 7;
		lcdp->lcd_cfg.spi_dc_pin_hdl =
		    lcd_fb_gpio_request(gpio_info, 1);
		lcd_fb_gpio_release(lcdp->lcd_cfg.spi_dc_pin_hdl, 2);
	}

	return 0;
}

/* direction: input(0), output(1) */
s32 disp_lcd_gpio_set_direction(struct lcd_fb_device *lcd, u32 io_index,
				u32 direction)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	char gpio_name[20];

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	sprintf(gpio_name, "lcd_gpio_%d", io_index);
	return lcd_fb_gpio_set_direction(lcdp->lcd_cfg.gpio_hdl[io_index],
					   direction, gpio_name);
}

s32 disp_lcd_gpio_get_value(struct lcd_fb_device *lcd, u32 io_index)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	char gpio_name[20];

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	sprintf(gpio_name, "lcd_gpio_%d", io_index);
	return lcd_fb_gpio_get_value(lcdp->lcd_cfg.gpio_hdl[io_index],
				       gpio_name);
}

s32 disp_lcd_gpio_set_value(struct lcd_fb_device *lcd, u32 io_index, u32 data)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	char gpio_name[20];

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	if (io_index >= LCD_GPIO_NUM) {
		lcd_fb_wrn("gpio num out of range\n");
		return -1;
	}
	sprintf(gpio_name, "lcd_gpio_%d", io_index);
	return lcd_fb_gpio_set_value(lcdp->lcd_cfg.gpio_hdl[io_index], data,
				       gpio_name);
}

static s32 disp_lcd_get_dimensions(struct lcd_fb_device *lcd, u32 *width,
				   u32 *height)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	if (width)
		*width = lcdp->panel_info.lcd_width;

	if (height)
		*height = lcdp->panel_info.lcd_height;

	return 0;
}


static s32 disp_lcd_update_gamma_tbl_set(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	int i;
	unsigned int *gamma, *gamma_set;
	unsigned int r, g, b;
	s32 color_temperature;
	u32 color_inverse;

	color_temperature = lcdp->color_temperature;
	color_inverse = lcdp->color_inverse;
	memcpy(&lcdp->panel_extend_info_set, &lcdp->panel_extend_info,
		sizeof(struct panel_extend_para));
	gamma = lcdp->panel_extend_info.lcd_gamma_tbl;
	gamma_set = lcdp->panel_extend_info_set.lcd_gamma_tbl;
	if (color_temperature > 0) {
		/* warm color */
		for (i = 0; i < 256; i++) {
			r = (gamma[i] >> 16) & 0xff;
			g = (gamma[i] >> 8) & 0xff;
			b = gamma[i] & 0xff;

			g = g * (512 - color_temperature) / 512;
			b = b * (256 - color_temperature) / 256;
			r = r << 16;

			g = g << 8;
			gamma_set[i] = r | g | b;
		}
	} else if (color_temperature < 0) {
		/* cool color */
		for (i = 0; i < 256; i++) {
			r = (gamma[i] >> 16) & 0xff;
			g = (gamma[i] >> 8) & 0xff;
			b = gamma[i] & 0xff;

			r = r * (256 + color_temperature) / 256;
			g = g * (512 + color_temperature) / 512;

			r = r << 16;
			g = g << 8;

			gamma_set[i] = r | g | b;
		}
	}
	if (color_inverse == 1) {
		for (i = 0; i < 256; i++)
			gamma_set[i] = 0xffffffff -  gamma_set[i];
	}
	if (color_inverse != 0)
		lcdp->panel_extend_info_set.lcd_gamma_en = 1;
	if (color_temperature != 0)
		lcdp->panel_extend_info_set.lcd_gamma_en = 1;

	return 0;
}


static s32 disp_lcd_set_gamma_tbl(struct lcd_fb_device *lcd,
			unsigned int *gamma_table, unsigned int size)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if ((lcd == NULL) || (lcdp == NULL)
	    || (gamma_table == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return 0;
	}

	size = (size > LCD_GAMMA_TABLE_SIZE) ?
	    LCD_GAMMA_TABLE_SIZE : size;
	lcd_data_lock = hal_spin_lock_irqsave(&disp_lock);
	memcpy(lcdp->panel_extend_info.lcd_gamma_tbl, gamma_table, size);
	disp_lcd_update_gamma_tbl_set(lcd);
	lcdp->panel_extend_dirty = 1;
	hal_spin_unlock_irqrestore(&disp_lock, lcd_data_lock);

	return 0;
}

static s32 disp_lcd_enable_gamma(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return 0;
	}

	lcd_data_lock = hal_spin_lock_irqsave(&disp_lock);
	if (lcdp->panel_extend_info.lcd_gamma_en == 0) {
		lcdp->panel_extend_info.lcd_gamma_en = 1;
		disp_lcd_update_gamma_tbl_set(lcd);
		lcdp->panel_extend_dirty = 1;
	}
	hal_spin_unlock_irqrestore(&disp_lock, lcd_data_lock);

	return 0;
}

static s32 disp_lcd_disable_gamma(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	int ret = -1;

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return 0;
	}

	/*if (lcdp->panel_extend_info.lcd_gamma_en == 1) {*/
		/*lcdp->panel_extend_info.lcd_gamma_en = 0;*/
	/*} else {*/
		/*ret = 0;*/
	/*}*/

	return ret;
}

static s32 disp_lcd_set_color_temperature(struct lcd_fb_device *lcd,
	s32 color_temperature)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if ((NULL == lcd) || (NULL == lcdp)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	lcd_data_lock = hal_spin_lock_irqsave(&disp_lock);
	lcdp->color_temperature = color_temperature;
	disp_lcd_update_gamma_tbl_set(lcd);
	lcdp->panel_extend_dirty = 1;
	hal_spin_unlock_irqrestore(&disp_lock, lcd_data_lock);

	return 0;
}

static s32 disp_lcd_get_color_temperature(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	s32 color_temperature = 0;

	if ((NULL == lcd) || (NULL == lcdp)) {
		lcd_fb_wrn("NULL hdl!\n");
		return 0;
	}

	lcd_data_lock = hal_spin_lock_irqsave(&disp_lock);
	color_temperature = lcdp->color_temperature;
	hal_spin_unlock_irqrestore(&disp_lock, lcd_data_lock);

	return color_temperature;
}


static s32 disp_lcd_init(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);
	int i;

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}
	lcd_fb_inf("lcd %d\n", lcd->disp);
	lcd_fb_mutex_init(&lcdp->layer_mlock);
	lcdp->wait = hal_sem_create(0);

	if (lcdp->wait == NULL)
		lcd_fb_wrn("creating lcdp->wait failed\n");

	lcd_get_sys_config(lcd->disp, &lcdp->lcd_cfg);
	if (disp_lcd_is_used(lcd)) {
		lcd_parse_panel_para(lcd->disp, &lcdp->panel_info);
	}
	disp_lcd_bright_curve_init(lcd);

	if (disp_lcd_is_used(lcd)) {
		unsigned long long backlight_bright;
		unsigned long long period_ns, duty_ns;

		if (lcdp->panel_info.lcd_pwm_used) {
			lcdp->pwm_info.channel = lcdp->panel_info.lcd_pwm_ch;
			lcdp->pwm_info.polarity = lcdp->panel_info.lcd_pwm_pol;
			lcdp->pwm_info.dev =
			    lcd_fb_pwm_request(lcdp->panel_info.lcd_pwm_ch);

			if (lcdp->panel_info.lcd_pwm_freq != 0) {
				period_ns =
				    1000 * 1000 * 1000 /
				    lcdp->panel_info.lcd_pwm_freq;
			} else {
				lcd_fb_wrn("lcd%d.lcd_pwm_freq is ZERO\n",
				       lcd->disp);
				/* default 1khz */
				period_ns = 1000 * 1000 * 1000 / 1000;
			}

			backlight_bright = lcdp->lcd_cfg.backlight_bright;

			duty_ns = (backlight_bright * period_ns) / 256;
			lcdp->pwm_info.duty_ns = duty_ns;
			lcdp->pwm_info.period_ns = period_ns;
		}
		for (i = 0; i < 256; i++) {
			lcdp->panel_extend_info.lcd_gamma_tbl[i] =
				(i << 24) | (i << 16) | (i << 8) | (i);
		}
	}

	return 0;
}



static s32 disp_lcd_exit(struct lcd_fb_device *lcd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if ((lcd == NULL) || (lcdp == NULL)) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	hal_sem_delete(lcdp->wait);

	if (lcdp->panel_info.lcd_pwm_used) {
		lcd_fb_pwm_free(lcdp->pwm_info.dev);
	}


	return 0;
}

s32 disp_lcd_get_resolution(struct lcd_fb_device *dispdev, u32 *xres,
			       u32 *yres)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(dispdev);

	if (dispdev == NULL) {
		lcd_fb_wrn("NULL hdl!\n");
		return -1;
	}

	if (xres)
		*xres = lcdp->panel_info.lcd_x;

	if (yres)
		*yres = lcdp->panel_info.lcd_y;

	return 0;
}

#if defined(SUPPORT_DBI_IF)
static int disp_lcd_cmd_read_by_dbi(hal_dbi_master_port_t port,
		const void *buf, uint32_t size)
{
	dbi_master_status_t ret;
	hal_dbi_master_transfer_t t;
	memset(&t, 0, sizeof(hal_dbi_master_transfer_t));

	dbim.cfg.cmd_typ = DBI_READ;//write command
	dbim.cfg.output_data_sequence = 0;//MSB first
	dbim.cfg.transmit_mode = 0;//cmd
	dbim.cfg.dcx_data = 0;
	dbim.cfg.dbi_output_data_format = DBI_RGB111;//must set rgb111
	hal_dbi_init(port, &dbim.cfg);

	t.tx_buf = NULL;
	t.tx_len = 0;
	t.rx_buf = buf;
	t.rx_len = size;

	ret = hal_dbi_xfer(port, &t);
	return ret;
}

static int disp_lcd_para_write_by_dbi(hal_dbi_master_port_t port,
		const void *buf, uint32_t size)
{
	dbi_master_status_t ret;
	hal_dbi_master_transfer_t t;
	memset(&t, 0, sizeof(hal_dbi_master_transfer_t));

	dbim.cfg.cmd_typ = DBI_WRITE;//write command
	dbim.cfg.output_data_sequence = 0;//MSB first
	dbim.cfg.transmit_mode = 0;//para
	dbim.cfg.dcx_data = 1;
	dbim.cfg.dbi_output_data_format = DBI_RGB111;//must set rgb111
	hal_dbi_init(port, &dbim.cfg);

	t.tx_buf = buf;
	t.tx_len = size;
	t.rx_buf = NULL;
	t.rx_len = 0;

	ret = hal_dbi_xfer(port, &t);
	return ret;
}

static int disp_lcd_cmd_write_by_dbi(hal_dbi_master_port_t port,
		const void *buf, uint32_t size)
{
	dbi_master_status_t ret;
	hal_dbi_master_transfer_t t;
	memset(&t, 0, sizeof(hal_dbi_master_transfer_t));

	dbim.cfg.cmd_typ = DBI_WRITE;//write command
	dbim.cfg.output_data_sequence = 0;//MSB first
	dbim.cfg.transmit_mode = 0;//cmd
	dbim.cfg.dcx_data = 0;
	dbim.cfg.dbi_output_data_format = DBI_RGB111;//must set rgb111
	hal_dbi_init(port, &dbim.cfg);

	t.tx_buf = buf;
	t.tx_len = size;
	t.rx_buf = NULL;
	t.rx_len = 0;

	ret = hal_dbi_xfer(port, &t);
	return ret;
}

static int disp_lcd_send_pixel_by_dbi(struct disp_lcd_private_data *lcdp,
		hal_dbi_master_port_t port, const void *buf, unsigned int len)
{
	dbi_master_status_t ret;
	hal_dbi_master_transfer_t t;
	static int first_flag = 0;

	memset(&t, 0, sizeof(hal_dbi_master_transfer_t));

	if (first_flag == 0) {
		dbim.cfg.cmd_typ = DBI_WRITE;//write command
		dbim.cfg.output_data_sequence = 0;//MSB first
		dbim.cfg.transmit_mode = 1;//video
		dbim.cfg.dcx_data = 1;
		dbim.cfg.dbi_output_data_format = lcdp->panel_info.lcd_dbi_fmt;
		dbim.cfg.te_en = lcdp->panel_info.lcd_dbi_te;
		if (lcdp->panel_info.lcd_dbi_te)
			dbim.cfg.dbi_en_mode = DBI_MODE_TE_TRIGGER;
		hal_dbi_init(port, &dbim.cfg);
	}
	first_flag = 1;

	t.tx_buf = buf;
	t.tx_len = len;
	t.rx_buf = NULL;
	t.rx_len = 0;

	ret = hal_dbi_xfer(port, &t);
	return ret;
}
#endif

static int disp_lcd_read_by_spi(hal_spi_master_port_t port,
		const void *buf, uint32_t size)
{
	spi_master_status_t ret;
	hal_spi_master_transfer_t t;

	memset(&t, 0, sizeof(hal_spi_master_transfer_t));

	t.rx_buf = buf;
	t.rx_len = size;
	t.tx_buf = NULL;
	t.tx_len = 0;
	t.dummy_byte = 0;
	t.tx_single_len = size;
	t.rx_nbits = SPI_NBITS_SINGLE;

	ret = hal_spi_xfer(port, &t);
	return ret;
}

static int disp_lcd_write_by_spi(hal_spi_master_port_t port,
		const void *buf, uint32_t size)
{
	spi_master_status_t ret;
	hal_spi_master_transfer_t t;

	memset(&t, 0, sizeof(hal_spi_master_transfer_t));

	t.tx_buf = buf;
	t.tx_len = size;
	t.rx_buf = NULL;
	t.rx_len = 0;
	t.dummy_byte = 0;
	t.tx_single_len = size;
	t.tx_nbits = SPI_NBITS_SINGLE;

	ret = hal_spi_xfer(port, &t);
	return ret;
}

static int disp_lcd_send_pixel_by_spi(hal_spi_master_port_t port,
		const void *buf, unsigned int len)
{
	spi_master_status_t ret;
	hal_spi_master_transfer_t t;

	memset(&t, 0, sizeof(hal_spi_master_transfer_t));

	t.tx_buf = (void *)buf;
	t.tx_len	= len;
	t.tx_single_len	= len;
	t.bits_per_word	= 8;
	t.dummy_byte = 0;
	t.tx_nbits = SPI_NBITS_SINGLE;

	ret =  hal_spi_xfer(port, &t);
	return ret;
}

static int disp_lcd_panel_dma_transfer(struct lcd_fb_device *p_lcd, void *buf,
				       unsigned int len)
{
	int ret = 0;
	struct disp_lcd_private_data *p_lcdp = disp_lcd_get_priv(p_lcd);
	if (!p_lcd || !p_lcdp) {
		lcd_fb_wrn("Null pointer!\n");
		return -1;
	}

#if defined(SUPPORT_DBI_IF)
	if (p_lcdp->panel_info.lcd_if == LCD_FB_IF_DBI) {
		ret = disp_lcd_send_pixel_by_dbi(p_lcdp, dbim.port, buf, len);
		if (ret)
			lcd_fb_wrn("dbi transfer pixel fail!\n");
	} else
#endif
	{
		ret =  disp_lcd_send_pixel_by_spi(spim.port, buf, len);
		if (ret)
			lcd_fb_wrn("spi transfer pixel fail!\n");
	}
	return ret;
}

static s32 _spi_dc_pin_set(struct disp_lcd_private_data *lcdp, u32 data)
{
	if (!lcdp)
		goto OUT;

	if (strlen(lcdp->lcd_cfg.lcd_spi_dc_pin.gpio_name))
		return lcd_fb_gpio_set_value(lcdp->lcd_cfg.spi_dc_pin_hdl, data,
					     "lcd_spi_dc_pin");
OUT:
	return -1;
}

int disp_lcd_wait_for_vsync(struct lcd_fb_device *p_lcd)
{
	unsigned long count;
	int ret = -1;
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(p_lcd);

	if (lcdp->panel_info.lcd_dbi_te)
		return 0;

	ret = hal_sem_timedwait(lcdp->wait, MS_TO_OSTICK(50));
	if (ret != 0)
		lcd_fb_wrn("spilcd wait for vsync failed!\n");

	return ret;
}


int disp_lcd_set_layer(struct lcd_fb_device *lcd, struct fb_info *p_info)
{
	int ret = -1;
	unsigned int i = 0, len = 0, last_len = 0;
	unsigned char *addr = NULL, *end_addr = NULL;
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	lcd_fb_mutex_lock(&lcdp->layer_mlock);

	len = p_info->fix.line_length;
	len *= lcdp->panel_info.lcd_y;

	if (p_info) {
		addr = (unsigned char *)p_info->screen_base +
			p_info->var.yoffset * p_info->fix.line_length;
		ret = disp_lcd_panel_dma_transfer(lcd, (void *)addr, len);
	}

	lcd_fb_mutex_unlock(&lcdp->layer_mlock);
	return ret;
}

int disp_lcd_set_var(struct lcd_fb_device *lcd, struct fb_info *p_info)
{
	int ret = -1;
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	lcd_fb_mutex_lock(&lcdp->layer_mlock);
	if (lcdp->lcd_panel_fun.set_var && p_info)
		ret = lcdp->lcd_panel_fun.set_var(lcd->disp, p_info);
	lcd_fb_mutex_unlock(&lcdp->layer_mlock);
	return ret;
}

int disp_lcd_blank(struct lcd_fb_device *lcd, unsigned int en)
{
	int ret = -1;
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(lcd);

	if (lcdp->lcd_panel_fun.blank)
		ret = lcdp->lcd_panel_fun.blank(lcd->disp, en);
	return ret;
}

static int disp_lcd_cmd_read(struct lcd_fb_device *p_lcd, unsigned char cmd,
		unsigned char *rx_buf, unsigned char len)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(p_lcd);
	int ret = 0;

#if defined(SUPPORT_DBI_IF)
	if (lcdp->panel_info.lcd_if == LCD_FB_IF_DBI) {
		ret = disp_lcd_cmd_read_by_dbi(dbim.port, &cmd, 1);
		if (ret)
			lcd_fb_wrn("dbi read cmd fail!\n");
	} else
#endif
	{
		ret = disp_lcd_read_by_spi(spim.port, &cmd, 1);
		if (ret)
			lcd_fb_wrn("spi read cmd fail!\n");
	}
	return ret;
}

static int disp_lcd_para_write(struct lcd_fb_device *p_lcd, unsigned char para)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(p_lcd);
	int ret = 0;

#if defined(SUPPORT_DBI_IF)
	if (lcdp->panel_info.lcd_if == LCD_FB_IF_DBI) {
		ret = disp_lcd_para_write_by_dbi(dbim.port, &para, 1);
		if (ret)
			lcd_fb_wrn("dbi transfer para fail!\n");
	} else
#endif
	{
		_spi_dc_pin_set(lcdp, 1);

		ret = disp_lcd_write_by_spi(spim.port, &para, 1);

		if (ret)
			lcd_fb_wrn("spi transfer para fail!\n");
	}
	return ret;
}

static int disp_lcd_cmd_write(struct lcd_fb_device *p_lcd, unsigned char cmd)
{
	struct disp_lcd_private_data *lcdp = disp_lcd_get_priv(p_lcd);
	int ret = 0;

#if defined(SUPPORT_DBI_IF)
	if (lcdp->panel_info.lcd_if == LCD_FB_IF_DBI) {
		ret = disp_lcd_cmd_write_by_dbi(dbim.port, &cmd, 1);
		if (ret)
			lcd_fb_wrn("dbi transfer cmd fail!\n");
	} else
#endif
	{
		_spi_dc_pin_set(lcdp, 0);

		ret = disp_lcd_write_by_spi(spim.port, &cmd, 1);
		if (ret)
			lcd_fb_wrn("spi transfer cmd fail!\n");

		/*restore to transfer data*/
		_spi_dc_pin_set(lcdp, 1);
	}
	return ret;
}

s32 disp_init_lcd(struct dev_lcd_fb_t *p_info)
{
	u32 disp = 0;
	struct lcd_fb_device *lcd;
	struct disp_lcd_private_data *lcdp;
	u32 hwdev_index = 0;
	u32 num_devices_support_lcd = SUPPORT_MAX_LCD;
	char primary_key[20];
	int ret = 0, value = 1;

	lcd_fb_inf("disp_init_lcd\n");

	lcd_data_lock = 0;
	lcds =
	    lcd_fb_malloc(num_devices_support_lcd * sizeof(struct lcd_fb_device));
	if (lcds == NULL) {
		lcd_fb_wrn("malloc memory(%d bytes) fail!\n",
		       (unsigned int)sizeof(struct lcd_fb_device) *
		       num_devices_support_lcd);
		goto malloc_err;
	}
	lcd_private =
	    (struct disp_lcd_private_data *)
	    lcd_fb_malloc(sizeof(struct disp_lcd_private_data)
		    * num_devices_support_lcd);
	if (lcd_private == NULL) {
		lcd_fb_wrn("malloc memory(%d bytes) fail!\n",
		       (unsigned int)sizeof(struct disp_lcd_private_data) *
		       num_devices_support_lcd);
		goto malloc_err;
	}

	disp = 0;
	for (hwdev_index = 0; hwdev_index < num_devices_support_lcd; hwdev_index++) {

		sprintf(primary_key, "lcd_fb%d", disp);
		ret = lcd_fb_script_get_item(primary_key, "lcd_used", &value,
					       1);
		if (ret != 1 || value != 1)
			continue;
		lcd = &lcds[disp];
		lcdp = &lcd_private[disp];
		lcd->priv_data = (void *)lcdp;
		++p_info->lcd_fb_num;

		sprintf(lcd->name, "lcd%d", disp);
		lcd->disp = disp;
		lcd->dev = p_info->device;

		lcd->get_resolution = disp_lcd_get_resolution;
		lcd->enable = disp_lcd_enable;
		lcd->fake_enable = disp_lcd_fake_enable;
		lcd->disable = disp_lcd_disable;
		lcd->is_enabled = disp_lcd_is_enabled;
		lcd->set_bright = disp_lcd_set_bright;
		lcd->get_bright = disp_lcd_get_bright;
		lcd->set_bright_dimming = disp_lcd_set_bright_dimming;
		lcd->get_panel_info = disp_lcd_get_panel_info;

		lcd->set_panel_func = disp_lcd_set_panel_funs;
		lcd->set_open_func = disp_lcd_set_open_func;
		lcd->set_close_func = disp_lcd_set_close_func;
		lcd->backlight_enable = disp_lcd_backlight_enable;
		lcd->backlight_disable = disp_lcd_backlight_disable;
		lcd->pwm_enable = disp_lcd_pwm_enable;
		lcd->pwm_disable = disp_lcd_pwm_disable;
		lcd->power_enable = disp_lcd_power_enable;
		lcd->power_disable = disp_lcd_power_disable;
		lcd->pin_cfg = disp_lcd_pin_cfg;
		lcd->gpio_set_value = disp_lcd_gpio_set_value;
		lcd->gpio_set_direction = disp_lcd_gpio_set_direction;
		lcd->get_dimensions = disp_lcd_get_dimensions;
		lcd->set_gamma_tbl = disp_lcd_set_gamma_tbl;
		lcd->enable_gamma = disp_lcd_enable_gamma;
		lcd->disable_gamma = disp_lcd_disable_gamma;
		lcd->set_color_temperature = disp_lcd_set_color_temperature;
		lcd->get_color_temperature = disp_lcd_get_color_temperature;
		lcd->set_layer = disp_lcd_set_layer;
		lcd->blank = disp_lcd_blank;
		lcd->set_var = disp_lcd_set_var;
		lcd->para_write = disp_lcd_para_write;
		lcd->cmd_write = disp_lcd_cmd_write;
		lcd->cmd_read = disp_lcd_cmd_read;
		lcd->wait_for_vsync = disp_lcd_wait_for_vsync;


		lcd->init = disp_lcd_init;
		lcd->exit = disp_lcd_exit;

		lcd->init(lcd);
		lcd_fb_device_register(lcd);

		disp++;
	}

	return 0;

malloc_err:
	lcd_fb_free(lcds);
	lcd_fb_free(lcd_private);
	lcds = NULL;
	lcd_private = NULL;

	return -1;
}

s32 disp_exit_lcd(void)
{
	u32 hwdev_index = 0, disp = 0;
	char primary_key[20];
	int ret = 0, value = 1;
	struct lcd_fb_device *lcd;

	if (!lcds)
		return 0;

	for (hwdev_index = 0; hwdev_index < SUPPORT_MAX_LCD; hwdev_index++) {
		sprintf(primary_key, "lcd_fb%d", disp);
		ret =
		    lcd_fb_script_get_item(primary_key, "lcd_used", &value, 1);
		if (ret != 1 || value != 1)
			continue;

		lcd = &lcds[disp];
		lcd_fb_device_unregister(lcd);
		lcd->exit(lcd);
		disp++;
	}

	lcd_fb_free(lcds);
	lcd_fb_free(lcd_private);
	lcds = NULL;
	lcd_private = NULL;
	return 0;
}

void lcd_fb_black_screen(unsigned int sel)
{
	int i = 0, ret = -1;
	unsigned char *pixel = NULL;
	unsigned int Bpp = 0;
	unsigned long int screensize = 0;
	struct disp_panel_para info[LCD_FB_MAX];
	struct lcd_fb_device *lcd;

	lcd = disp_get_lcd(sel);

	ret = bsp_disp_get_panel_info(sel, &info[sel]);
	if (ret) {
		lcd_fb_wrn("get panel info fail!\n");
	}
	if (info->lcd_pixel_fmt <= LCDFB_FORMAT_BGR_888) {
		Bpp = 4;
	} else if (info->lcd_pixel_fmt == LCDFB_FORMAT_BGR_888 ||
		   info->lcd_pixel_fmt == LCDFB_FORMAT_RGB_888) {
		Bpp = 3;
	} else if (info->lcd_pixel_fmt >= LCDFB_FORMAT_RGB_565) {
		Bpp = 2;
	}

	screensize = info[sel].lcd_x * Bpp * info[sel].lcd_y;
	pixel = lcd_fb_dma_malloc(screensize);
	if (!pixel)
		lcd_fb_wrn("malloc pixel fail!\n");
	lcd_fb_inf("pixel_addr = %x, size = %d\n", pixel, screensize);

	memset(pixel, 0, screensize);
	hal_dcache_clean((unsigned long)pixel, screensize);
	disp_lcd_panel_dma_transfer(lcd, pixel, screensize);
	lcd_fb_dma_free(pixel);
}
