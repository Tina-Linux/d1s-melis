/*
 * include/include.h
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef _INCLUDE_H
#define _INCLUDE_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <sunxi_hal_spi.h>
#include <sunxi_hal_dbi.h>
#include <hal_lcd_fb.h>
#include <hal_gpio.h>
#include <sunxi_hal_pwm.h>
#include <hal_workqueue.h>
#include <hal_interrupt.h>
#include <hal_time.h>
#include <hal_timer.h>
#include <hal_mem.h>
#include <aw_list.h>
#include <hal_log.h>
#include "lcd_fb_feature.h"

#define LCD_GAMMA_TABLE_SIZE (256 * sizeof(unsigned int))
#define LCD_FB_DEBUG_LEVEL 0
#define LCD_FB_MAX 2 /*TODO:auto detect*/

#if LCD_FB_DEBUG_LEVEL == 1
#define lcd_fb_inf(msg...) do { \
			hal_log_info("[LCD_FB] %s,line:%d:", __func__, __LINE__); \
			hal_log_info(msg); \
	} while (0)
#define lcd_fb_msg(msg...) do { \
			hal_log_info("[LCD_FB] %s,line:%d:", __func__, __LINE__); \
			hal_log_info(msg); \
	} while (0)
#define lcd_fb_here
#define lcd_fb_dbg(msg...)
#elif LCD_FB_DEBUG_LEVEL == 2
#define lcd_fb_inf(msg...) do { \
			hal_log_info("[LCD_FB] %s,line:%d:", __func__, __LINE__); \
			hal_log_info(msg); \
	} while (0)
#define lcd_fb_here do { \
			hal_log_info("[LCD_FB] %s,line:%d\n", __func__, __LINE__);\
	} while (0)
#define lcd_fb_dbg(msg...)  do { \
			hal_log_debug("[LCD_FB] %s,line:%d:", __func__, __LINE__); \
			hal_log_debug(msg); \
	} while (0)
#else
#define lcd_fb_inf(msg...)
#define lcd_fb_msg(msg...)
#define lcd_fb_here
#define lcd_fb_dbg(msg...)
#endif

#define lcd_fb_wrn(msg...) do { \
			hal_log_warn("[LCD_FB] %s,line:%d:", __func__, __LINE__); \
			hal_log_warn(msg); \
		} while (0)


typedef void (*LCD_FUNC) (unsigned int sel);
struct disp_lcd_function {
	LCD_FUNC func;
	unsigned int delay;
};

#define LCD_MAX_SEQUENCES 7
struct disp_lcd_flow {
	struct disp_lcd_function func[LCD_MAX_SEQUENCES];
	unsigned int func_num;
	unsigned int cur_step;
};

struct panel_extend_para {
	unsigned int lcd_gamma_en;
	unsigned int lcd_gamma_tbl[256];
	unsigned int lcd_cmap_en;
	unsigned int lcd_cmap_tbl[2][3][4];
	unsigned int lcd_bright_curve_tbl[256];
};

struct disp_rect {
	int x;
	int y;
	unsigned int width;
	unsigned int height;
};


struct disp_lcd_panel_fun {
	void (*cfg_panel_info)(struct panel_extend_para *info);
	int (*cfg_open_flow)(unsigned int sel);
	int (*cfg_close_flow)(unsigned int sel);
	int (*lcd_user_defined_func)(unsigned int sel, unsigned int para1,
				     unsigned int para2, unsigned int para3);
	int (*set_bright)(unsigned int sel, unsigned int bright);
	int (*blank)(unsigned int sel, unsigned int en);
	int (*set_var)(unsigned int sel, struct fb_info *info);
	int (*set_addr_win)(unsigned int sel, int x, int y, int width, int height);
};


struct disp_video_timings {
	unsigned int vic;	/* video information code */
	unsigned int tv_mode;
	unsigned int pixel_clk;
	unsigned int pixel_repeat; /* pixel repeat (pixel_repeat+1) times */
	unsigned int x_res;
	unsigned int y_res;
	unsigned int hor_total_time;
	unsigned int hor_back_porch;
	unsigned int hor_front_porch;
	unsigned int hor_sync_time;
	unsigned int ver_total_time;
	unsigned int ver_back_porch;
	unsigned int ver_front_porch;
	unsigned int ver_sync_time;
	unsigned int hor_sync_polarity;	/* 0: negative, 1: positive */
	unsigned int ver_sync_polarity;	/* 0: negative, 1: positive */
	bool b_interlace;
	unsigned int vactive_space;
	unsigned int trd_mode;
	unsigned long      dclk_rate_set; /*unit: hz */
	unsigned long long frame_period; /* unit: ns */
	int                start_delay; /* unit: line */
};

enum disp_fb_mode {
	FB_MODE_SCREEN0 = 0,
	FB_MODE_SCREEN1 = 1,
	FB_MODE_SCREEN2 = 2,
	FB_MODE_DUAL_SAME_SCREEN_TB = 3,/* two screen, top buffer for screen0, bottom buffer for screen1 */
	FB_MODE_DUAL_DIFF_SCREEN_SAME_CONTENTS = 4,/* two screen, they have same contents; */
};

struct disp_fb_create_info {
	enum disp_fb_mode fb_mode;
	unsigned int buffer_num;
	unsigned int width;
	unsigned int height;

	unsigned int output_width;  /* used when scaler mode */
	unsigned int output_height; /* used when scaler mode */
};


struct lcd_spi_master {
    hal_spi_master_port_t port;
    hal_spi_master_config_t cfg;
};

struct lcd_dbi_master {
    hal_dbi_master_port_t port;
    hal_dbi_config_t cfg;
};

enum disp_lcd_if {
	LCD_FB_IF_SPI = 0,
	LCD_FB_IF_DBI = 1,
};

enum lcd_fb_dbi_if {
	LCD_FB_L3I1 = 0x0,
	LCD_FB_L3I2 = 0x1,
	LCD_FB_L4I1 = 0x2,
	LCD_FB_L4I2 = 0x3,
	LCD_FB_D2LI = 0x4,
};

enum lcdfb_dbi_fmt {
	LCDFB_DBI_RGB111 = 0x0,
	LCDFB_DBI_RGB444 = 0x1,
	LCDFB_DBI_RGB565 = 0x2,
	LCDFB_DBI_RGB666 = 0x3,
	LCDFB_DBI_RGB888 = 0x4,
};

struct disp_panel_para {
	enum disp_lcd_if lcd_if;
	enum lcd_fb_dbi_if dbi_if;
	unsigned int lcd_spi_bus_num;

	unsigned int lcd_data_speed;
	unsigned int lcd_x;	/* horizontal resolution */
	unsigned int lcd_y;	/* vertical resolution */
	unsigned int lcd_width;	/* width of lcd in mm */
	unsigned int lcd_height;	/* height of lcd in mm */

	unsigned int lcd_pwm_used;
	unsigned int lcd_pwm_ch;
	unsigned int lcd_pwm_freq;
	unsigned int lcd_pwm_pol;
	enum lcdfb_pixel_format lcd_pixel_fmt;
	enum lcdfb_dbi_fmt lcd_dbi_fmt;
	unsigned int lcd_dbi_clk_mode;
	unsigned int lcd_dbi_te;
	unsigned int fb_buffer_num;

	dbi_source_format_t lcd_rgb_order;
	unsigned int lcd_fps;

	unsigned int lcd_frm;
	unsigned int lcd_gamma_en;
	unsigned int lcd_bright_curve_en;
	unsigned int lines_per_transfer;

	char lcd_size[8];	/* e.g. 7.9, 9.7 */
	char lcd_model_name[32];

};



struct lcd_fb_device {
	struct list_head list;
	struct device *dev;
	char name[32];
	u32 disp;

	void *priv_data;

	struct disp_video_timings timings;
	s32 (*init)(struct lcd_fb_device *p_lcd);
	s32 (*exit)(struct lcd_fb_device *p_lcd);
	s32 (*enable)(struct lcd_fb_device *p_lcd);
	s32 (*fake_enable)(struct lcd_fb_device *p_lcd);
	s32 (*disable)(struct lcd_fb_device *p_lcd);
	s32 (*is_enabled)(struct lcd_fb_device *p_lcd);
	s32 (*get_resolution)(struct lcd_fb_device *p_lcd, u32 *xres,
			      u32 *yres);
	s32 (*get_dimensions)(struct lcd_fb_device *dispdev, u32 *width,
			      u32 *height);
	s32 (*set_color_temperature)(struct lcd_fb_device *dispdev, s32 color_temperature);
	s32 (*get_color_temperature)(struct lcd_fb_device *dispdev);
	s32 (*suspend)(struct lcd_fb_device *p_lcd);
	s32 (*resume)(struct lcd_fb_device *p_lcd);
	s32 (*set_bright)(struct lcd_fb_device *p_lcd, u32 bright);
	s32 (*get_bright)(struct lcd_fb_device *p_lcd);
	s32 (*backlight_enable)(struct lcd_fb_device *p_lcd);
	s32 (*backlight_disable)(struct lcd_fb_device *p_lcd);
	s32 (*pwm_enable)(struct lcd_fb_device *p_lcd);
	s32 (*pwm_disable)(struct lcd_fb_device *p_lcd);
	s32 (*power_enable)(struct lcd_fb_device *p_lcd, u32 power_id);
	s32 (*power_disable)(struct lcd_fb_device *p_lcd, u32 power_id);
	s32 (*pin_cfg)(struct lcd_fb_device *p_lcd, u32 bon);
	s32 (*set_gamma_tbl)(struct lcd_fb_device *p_lcd, u32 *tbl,
			      u32 size);
	s32 (*set_bright_dimming)(struct lcd_fb_device *dispdev, u32 dimming);
	s32 (*enable_gamma)(struct lcd_fb_device *p_lcd);
	s32 (*disable_gamma)(struct lcd_fb_device *p_lcd);
	s32 (*set_panel_func)(struct lcd_fb_device *lcd, char *name,
			      struct disp_lcd_panel_fun *lcd_cfg);
	s32 (*set_open_func)(struct lcd_fb_device *lcd, LCD_FUNC func,
			     u32 delay);
	s32 (*set_close_func)(struct lcd_fb_device *lcd, LCD_FUNC func,
			      u32 delay);
	int (*gpio_set_value)(struct lcd_fb_device *p_lcd,
			      unsigned int io_index, u32 value);
	int (*gpio_set_direction)(struct lcd_fb_device *p_lcd,
				  unsigned int io_index, u32 direction);
	int (*get_panel_info)(struct lcd_fb_device *p_lcd,
			       struct disp_panel_para *info);
	int (*set_layer)(struct lcd_fb_device *p_lcd, struct fb_info *p_info);
	int (*set_var)(struct lcd_fb_device *p_lcd, struct fb_info *p_info);
	int (*blank)(struct lcd_fb_device *p_lcd, unsigned int en);
	int (*cmd_write)(struct lcd_fb_device *p_lcd, unsigned char cmd);
	int (*para_write)(struct lcd_fb_device *p_lcd, unsigned char para);
	int (*cmd_read)(struct lcd_fb_device *p_lcd, unsigned char cmd,
			unsigned char *rx_buf, unsigned char len);
	int (*wait_for_vsync)(struct lcd_fb_device *p_lcd);
};

struct sunxi_disp_source_ops {
	int (*sunxi_lcd_delay_ms)(unsigned int ms);
	int (*sunxi_lcd_delay_us)(unsigned int us);
	int (*sunxi_lcd_backlight_enable)(unsigned int screen_id);
	int (*sunxi_lcd_backlight_disable)(unsigned int screen_id);
	int (*sunxi_lcd_pwm_enable)(unsigned int screen_id);
	int (*sunxi_lcd_pwm_disable)(unsigned int screen_id);
	int (*sunxi_lcd_power_enable)(unsigned int screen_id,
				       unsigned int pwr_id);
	int (*sunxi_lcd_power_disable)(unsigned int screen_id,
					unsigned int pwr_id);
	int (*sunxi_lcd_set_panel_funs)(char *drv_name,
					 struct disp_lcd_panel_fun *lcd_cfg);
	int (*sunxi_lcd_pin_cfg)(unsigned int screen_id, unsigned int bon);
	int (*sunxi_lcd_gpio_set_value)(unsigned int screen_id,
					 unsigned int io_index, u32 value);
	int (*sunxi_lcd_gpio_set_direction)(unsigned int screen_id,
					     unsigned int io_index,
					     u32 direction);
	int (*sunxi_lcd_cmd_write)(unsigned int screen_id,
					 unsigned char cmd);
	int (*sunxi_lcd_para_write)(unsigned int screen_id,
					 unsigned char para);
	int (*sunxi_lcd_cmd_read)(unsigned int screen_id,
					 unsigned char cmd, unsigned char *rx_buf, unsigned char len);
};



#endif /*End of file*/
