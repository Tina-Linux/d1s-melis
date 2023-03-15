/*
 * vin.c
 *
 * Copyright (c) 2018 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zequn Zheng <zequnzhengi@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <hal_clk.h>
#include <hal_timer.h>
#include <sunxi_hal_gpadc.h>
#include <openamp/sunxi_helper/openamp.h>
#include <hal_cfg.h>
//#include <script.h>

#include "vin.h"

struct rt_memheap isp_mempool;

unsigned int vin_log_mask = 0;// = 0xffff - VIN_LOG_ISP - VIN_LOG_STAT - VIN_LOG_STAT1;
struct isp_get_cfg_t isp_get_cfg[2] = {
	[0] = {
		.sensor_get_fps = 15,
	},
	[1] = {
		.sensor_get_fps = 15,
	},
};
extern struct csi_dev *global_csi[VIN_MAX_CSI];
extern struct vin_core *global_vinc[VIN_MAX_VIDEO];

void vin_set_from_partition(unsigned int id, unsigned char *ir_en)
{
	SENSOR_ISP_CONFIG_S *sensor_isp_cfg = NULL;
	enum ir_mode ir_mode = DAY_MODE;
	unsigned int check_sign = 0;
	unsigned int gpadc_ch = 0;
	unsigned int gpadc_value = 0;

	/*read from ddr*/
	if (id == 0) {
		sensor_isp_cfg = (SENSOR_ISP_CONFIG_S *)VIN_SENSOR0_RESERVE_ADDR;
		check_sign = 0xAA66AA66;
	} else {
		sensor_isp_cfg = (SENSOR_ISP_CONFIG_S *)VIN_SENSOR1_RESERVE_ADDR;
		check_sign = 0xBB66BB66;
	}
	if (sensor_isp_cfg->sign != check_sign) {
		vin_warn("sensor%d:sign is 0x%x\n", id, sensor_isp_cfg->sign);
		return;
	}

	isp_get_cfg[id].sensor_wdr_on = sensor_isp_cfg->wdr_mode;
	if (sensor_isp_cfg->fps)
		isp_get_cfg[id].sensor_get_fps = sensor_isp_cfg->fps;
	isp_get_cfg[id].sensor_deinit = sensor_isp_cfg->sensor_deinit;
	isp_get_cfg[id].get_yuv_en = sensor_isp_cfg->get_yuv_en;
	vin_print("sensor%d:get wdr mode is %d, fps is %d\n", id, isp_get_cfg[id].sensor_wdr_on, isp_get_cfg[id].sensor_get_fps);

#ifdef CONFIG_ISP_HARD_LIGHTADC
	hal_gpadc_init();
	hal_gpadc_channel_init(gpadc_ch);
	gpadc_value = gpadc_read_channel_data(gpadc_ch);
	/* boot0 set ir, melis set isp ir */
	if (sensor_isp_cfg->ircut_state) {
		if (sensor_isp_cfg->ircut_state == 1)
			ir_mode = DAY_MODE;
		else if (sensor_isp_cfg->ircut_state == 2)
			ir_mode = NIGHT_MODE;
		vin_print("boot0 set ir, ircut_state is %s, light_def is %d, gpadc_value is %d\n",
				(sensor_isp_cfg->ircut_state == 1 ? "day_state" : "night_state"),
				sensor_isp_cfg->light_def, gpadc_value);
	} else {
		/* force day or night*/
		if (sensor_isp_cfg->ir_mode == DAY_MODE || sensor_isp_cfg->ir_mode == NIGHT_MODE) {
			ir_mode = sensor_isp_cfg->ir_mode;
		} else { /* auto mode */
			if (sensor_isp_cfg->light_def == 0)
				sensor_isp_cfg->light_def = 1000;
			if (sensor_isp_cfg->adc_mode == 0) {
				if (sensor_isp_cfg->light_def > gpadc_value)
					ir_mode = NIGHT_MODE;
				else
					ir_mode = DAY_MODE;
			} else {
				if (sensor_isp_cfg->light_def > gpadc_value)
					ir_mode = DAY_MODE;
				else
					ir_mode = NIGHT_MODE;
			}
		}

		if (ir_mode == DAY_MODE) {
			sensor_isp_cfg->ircut_state = 1;
			*ir_en = 1;
		} else {
			sensor_isp_cfg->ircut_state = 2;
			*ir_en = 2;
		}
		vin_print("boot0 not set ir, ir_mode is %s, ircut_state is %s, light_def is %d, gpadc_value is %d\n",
				(sensor_isp_cfg->ir_mode == DAY_MODE ? "day_mode" : (sensor_isp_cfg->ir_mode == NIGHT_MODE ? "night_mode" : "auto_mode")),
				(sensor_isp_cfg->ircut_state == 1 ? "day_state" : "night_state"),
				sensor_isp_cfg->light_def, gpadc_value);
	}
	//hal_gpadc_channel_exit(gpadc_ch);
	//hal_gpadc_deinit();
#endif
}

void vin_set_to_partition(unsigned int id)
{
	SENSOR_ISP_CONFIG_S *sensor_isp_cfg = NULL;
	unsigned int check_sign = 0;

	/*read from ddr*/
	if (id == 0) {
		sensor_isp_cfg = (SENSOR_ISP_CONFIG_S *)VIN_SENSOR0_RESERVE_ADDR;
		check_sign = 0xAA66AA66;
	} else {
		sensor_isp_cfg = (SENSOR_ISP_CONFIG_S *)VIN_SENSOR1_RESERVE_ADDR;
		check_sign = 0xBB66BB66;
	}
	if (sensor_isp_cfg->sign != check_sign) {
		vin_warn("sensor%d:sign is 0x%x\n", id, sensor_isp_cfg->sign);
		return;
	}

	sensor_isp_cfg->wdr_mode = isp_get_cfg[id].sensor_wdr_on;
	sensor_isp_cfg->fps = isp_get_cfg[id].sensor_get_fps;
	vin_print("sensor%d:set wdr mode is %d, fps is %d\n", id, isp_get_cfg[id].sensor_wdr_on, isp_get_cfg[id].sensor_get_fps);
}

static int vin_md_clk_en(unsigned int en)
{
	struct vin_clk_info top_clk_src;

	hal_writel(0x00000001, 0x02001c2c);
	hal_writel(0x00010001, 0x02001c2c);

	if (vind_default_clk[VIN_TOP_CLK].frequency > 300000000) {
		top_clk_src.clock = vind_default_clk[VIN_TOP_CLK_SRC].clock;
		top_clk_src.frequency = VIN_PLL_CSI_RATE;
	} else {
		top_clk_src.clock = vind_default_clk[VIN_TOP_CLK_SRC1].clock;
		top_clk_src.frequency = vind_default_clk[VIN_TOP_CLK_SRC1].frequency;
	}

	if (en) {
		if (hal_clk_set_parent(vind_default_clk[VIN_TOP_CLK].clock, top_clk_src.clock)) {
			vin_err("set top_clk source failed!\n");
			return -1;
		}

		if (hal_clk_set_rate(top_clk_src.clock, top_clk_src.frequency)) {
			vin_warn("set top_clk src clock error\n");
			//return -1;
		}

		if (hal_clk_set_rate(vind_default_clk[VIN_TOP_CLK].clock, vind_default_clk[VIN_TOP_CLK].frequency)) {
			vin_err("set top_clk clock error\n");
			return -1;
		}
	}

	if (en) {
		if (hal_clock_enable(top_clk_src.clock)) {
			vin_err("top_clk_src clock enable error\n");
			return -1;
		}

		if (hal_clock_enable(vind_default_clk[VIN_TOP_CLK].clock)) {
			vin_err("top_clk clock enable error\n");
			return -1;
		}

		if (hal_clock_enable(vind_default_isp_clk[VIN_ISP_CLK].clock)) {
			vin_err("isp clock enable error\n");
			return -1;
		}

		/*if (vind_default_clk[VIN_TOP_CLK].frequency > 300000000) {
			hal_writel(0xc9006201, 0x02001048); //set pll_csi 2376M
			hal_writel(0xd1303333, 0x02001148);
			hal_writel(0x83000006, 0x02001c04); //set top_csi 340M
		}*/
	} else {
		if (hal_clock_disable(vind_default_clk[VIN_TOP_CLK].clock)) {
			vin_err("top_clk clock disable error\n");
			return -1;
		}

		if (hal_clock_disable(vind_default_isp_clk[VIN_ISP_CLK].clock)) {
			vin_err("isp clock disable error\n");
			return -1;
		}
	}

	hal_writel(0x00000001, 0x02001c2c);
	hal_writel(0x00010001, 0x02001c2c);

	vin_print("set clk end\n");

	return 0;
}

static void vin_ccu_clk_gating_en(unsigned int en)
{
	/*if (en) {
		csic_ccu_clk_gating_enable();
		csic_ccu_mcsi_clk_mode(1);
		csic_ccu_mcsi_post_clk_enable(0);
		csic_ccu_mcsi_post_clk_enable(1);
	} else {
		csic_ccu_mcsi_post_clk_disable(1);
		csic_ccu_mcsi_post_clk_disable(0);
		csic_ccu_mcsi_clk_mode(0);
		csic_ccu_clk_gating_disable();
	}*/

	csic_ccu_clk_gating_disable();
}

static void vin_md_set_power(int on)
{
	if (on) {
		vin_md_clk_en(on);
		vin_ccu_clk_gating_en(on);
		hal_usleep(120);
		csic_top_enable();
		csic_mbus_req_mex_set(0xf);
	} else {
		csic_top_disable();
		vin_ccu_clk_gating_en(on);
		vin_md_clk_en(on);
	}
}

static void vin_subdev_ccu_en(unsigned int id, unsigned int en)
{

}

static int vin_pipeline_set_mbus_config(unsigned int id)
{
	struct vin_core *vinc = global_vinc[id];
	struct v4l2_mbus_config mcfg;
	struct mbus_framefmt_res res;
	int sensor_id = vinc->mipi_sel;

	memset(&res, 0, sizeof(struct mbus_framefmt_res));
	if (global_sensors[sensor_id].sensor_core->g_mbus_config)
		global_sensors[sensor_id].sensor_core->g_mbus_config(id, &mcfg, &res);

	/* s_mbus_config on all mipi and csi */
	if (vinc->mipi_sel != 0xff)
		sunxi_mipi_s_mbus_config(vinc->mipi_sel, &mcfg, &res);

	sunxi_csi_s_mbus_config(vinc->csi_sel, &mcfg);
	vinc->total_rx_ch = global_csi[vinc->csi_sel]->bus_info.ch_total_num;

	if (vinc->tdm_rx_sel != 0xff)
		sunxi_tdm_s_mbus_config(vinc->tdm_rx_sel, &res);
	sunxi_isp_s_mbus_config(vinc->isp_sel, &res);

	return 0;
}

static int vin_s_stream(unsigned int id, int enable)
{
	struct vin_core *vinc = global_vinc[id];
	int sensor_id = vinc->mipi_sel;
	struct vin_core *logic_vinc = global_vinc[dma_virtual_find_logic[vinc->id]];
	int ret;

	vin_log(VIN_LOG_MD, "vinc%d:tdm_rx_sel = %d,  mipi_sel = %d, isp_sel = %d, vincid = %d, csi_sel = %d\n",
			id, vinc->tdm_rx_sel, vinc->mipi_sel, vinc->isp_sel, vinc->id, vinc->csi_sel);

	if (enable) {
		if (vinc->tdm_rx_sel != 0xff)
			sunxi_tdm_subdev_s_stream(vinc->tdm_rx_sel, vinc->id, enable);
		hal_usleep(120);
		if (vinc->mipi_sel != 0xff)
			sunxi_mipi_subdev_s_stream(vinc->mipi_sel, enable);
		hal_usleep(120);
		sunxi_isp_subdev_s_stream(vinc->isp_sel, vinc->id, enable);
		hal_usleep(120);
		sunxi_scaler_subdev_s_stream(vinc->id, enable);
		hal_usleep(120);
		vin_subdev_s_stream(vinc->id, enable);
		hal_usleep(120);
		sunxi_csi_subdev_s_stream(vinc->csi_sel, enable);
		hal_usleep(120);
		if (global_sensors[sensor_id].sensor_core->s_stream) {
			ret = global_sensors[sensor_id].sensor_core->s_stream(sensor_id, enable);
			if (ret)
				return ret;
		}
	} else {
		vin_subdev_s_stream(vinc->id, enable);
		hal_usleep(120);
		if (logic_vinc->work_mode == BK_ONLINE) {
			sunxi_scaler_subdev_s_stream(vinc->id, enable);
			hal_usleep(120);
			sunxi_isp_subdev_s_stream(vinc->isp_sel, vinc->id, enable);
			hal_usleep(120);
			if (vinc->tdm_rx_sel != 0xff)
				sunxi_tdm_subdev_s_stream(vinc->tdm_rx_sel, vinc->id, enable);
		} else {
			if (vinc->tdm_rx_sel != 0xff)
				sunxi_tdm_subdev_s_stream(vinc->tdm_rx_sel, vinc->id, enable);
			hal_usleep(120);
			sunxi_isp_subdev_s_stream(vinc->isp_sel, vinc->id, enable);
			hal_usleep(120);
			sunxi_scaler_subdev_s_stream(vinc->id, enable);
		}
		hal_usleep(120);
		sunxi_csi_subdev_s_stream(vinc->csi_sel, enable);
#ifndef ISP_SERVER_FASTINIT
		hal_usleep(120);
		if (vinc->mipi_sel != 0xff)
			sunxi_mipi_subdev_s_stream(vinc->mipi_sel, enable);
		hal_usleep(120);
		if (global_sensors[sensor_id].sensor_core->s_stream) {
			ret = global_sensors[sensor_id].sensor_core->s_stream(sensor_id, enable);
			if (ret)
				return ret;
		}
#endif
	}
	return 0;
}

static void vin_probe(unsigned int id)
{
	struct vin_core *vinc = &global_video[id];

	if (vinc->mipi_sel != 0xff)
		mipi_probe(vinc->mipi_sel);
	csi_probe(vinc->csi_sel);
	if (vinc->tdm_rx_sel != 0xff)
		tdm_probe(vinc->tdm_rx_sel);
	isp_probe(vinc->isp_sel); /* isp0 must first probe */
	scaler_probe(vinc->id);
	vin_core_probe(vinc->id);
}

static void vin_free(unsigned int id)
{
	struct vin_core *vinc = &global_video[id];

	if (vinc->mipi_sel != 0xff)
		mipi_remove(vinc->mipi_sel);
	csi_remove(vinc->csi_sel);
	if (vinc->tdm_rx_sel != 0xff)
		tdm_remove(vinc->tdm_rx_sel);
	isp_remove(vinc->isp_sel);
	scaler_remove(vinc->id);
	vin_core_remove(vinc->id);
}

static int vin_g_config(void)
{
	int i, ret = -1;
	user_gpio_set_t gpio_cfg;
	char main_name[16];
	char sub_name[16];
	int ivalue = 0;
	for (i = 0; i< VIN_MAX_VIDEO; i++)
	{
		ivalue = 0;
		sprintf(main_name, "sensor%d", i);
		sprintf(sub_name, "used%d", i);
		ret = Hal_Cfg_GetSubKeyValue(main_name, sub_name, (void*)&ivalue, 1);
		if(0 != ret) {
			vin_err("%s is %d\n", sub_name, ivalue);
			break;
		}
		//vin_err("%s is %d\n", sub_name, ivalue);
		global_video[i].used = ivalue;
		if(1 != ivalue)
			continue;

		sprintf(sub_name, "reset%d", i);
		ret = Hal_Cfg_GetSubKeyValue(main_name, sub_name, (void*)&gpio_cfg, 4);
		if(0 != ret) {
			vin_err("%s is port %d, num %d\n", main_name, gpio_cfg.port, gpio_cfg.port_num);
			break;
		}
		global_sensors[i].reset_gpio = (gpio_cfg.port - 1) * 32 + gpio_cfg.port_num;
		sprintf(sub_name, "pwdn%d", i);
		ret = Hal_Cfg_GetSubKeyValue(main_name, sub_name, (void*)&gpio_cfg, 4);
		if(0 != ret) {
			vin_err("%s is port %d, num %d\n", main_name, gpio_cfg.port, gpio_cfg.port_num);
			break;
		}
		global_sensors[i].pwdn_gpio = (gpio_cfg.port - 1) * 32 + gpio_cfg.port_num;
		sprintf(sub_name, "mclk%d", i);
		ret = Hal_Cfg_GetSubKeyValue(main_name, sub_name, (void*)&gpio_cfg, 4);
		if(0 != ret) {
			vin_err("%s is port %d, num %d\n", main_name, gpio_cfg.port, gpio_cfg.port_num);
			break;
		}
		vind_default_mclk[i].pin = (gpio_cfg.port - 1) * 32 + gpio_cfg.port_num;
		vind_default_mclk[i].pin_func[0] = gpio_cfg.mul_sel;
		vind_default_mclk[i].pin_func[1] = 0xf;
	}
	return 0;
}

static int vin_g_status(void)
{
	int i, ret = -1;

	for (i = 0; i < VIN_MAX_VIDEO; i++) {
		if (global_video[i].used == 1)
			ret = 0;
	}

	return ret;
}

static void vin_detect_sensor_list(int sensor_id)
{
	int i, save_detect_id;
	struct isp_autoflash_config_s *isp_autoflash_cfg = NULL;
	unsigned int sign;
	int ret = -1;

	if (sensor_id == 0) {
		isp_autoflash_cfg = (struct isp_autoflash_config_s *)ISP0_NORFLASH_SAVE;
		sign = 0xAA22AA22;
	} else {
		isp_autoflash_cfg = (struct isp_autoflash_config_s *)ISP1_NORFLASH_SAVE;
		sign = 0xBB22BB22;
	}

	save_detect_id = min(isp_autoflash_cfg->sensor_detect_id, MAX_DETECT_SENSOR - 1);
	vin_print("sensor%d save_detect_id is %d\n", sensor_id, save_detect_id);
	memcpy(&global_sensors[sensor_id], &global_sensors_list[sensor_id][save_detect_id], sizeof(struct sensor_list));
	global_sensors[sensor_id].sensor_core = find_sensor_func(global_sensors[sensor_id].sensor_name);
	if (global_sensors[sensor_id].sensor_core->sensor_test_i2c) {
		vin_print("sensor %s to save detect\n", global_sensors[sensor_id].sensor_name);
		ret = global_sensors[sensor_id].sensor_core->sensor_test_i2c(sensor_id);
	}
	if (ret) {
		for (i = 0; i < MAX_DETECT_SENSOR; i++) {
			if (i == save_detect_id)
				continue;
			memcpy(&global_sensors[sensor_id], &global_sensors_list[sensor_id][i], sizeof(struct sensor_list));
			global_sensors[sensor_id].sensor_core = find_sensor_func(global_sensors[sensor_id].sensor_name);
			if (global_sensors[sensor_id].sensor_core->sensor_test_i2c) {
				vin_print("sensor %s to detect\n", global_sensors[sensor_id].sensor_name);
				ret = global_sensors[sensor_id].sensor_core->sensor_test_i2c(sensor_id);
				if (!ret) {
					vin_print("find sensor %s\n", global_sensors[sensor_id].sensor_name);
					isp_autoflash_cfg->sensorlist_sign_id = sign;
					strcpy(isp_autoflash_cfg->sensor_name, global_sensors[sensor_id].sensor_name);
					isp_autoflash_cfg->sensor_twi_addr = global_sensors[sensor_id].sensor_twi_addr;
					isp_autoflash_cfg->sensor_detect_id = i;
					break;
				}
			}
		}
		if (ret)
			isp_autoflash_cfg->sensorlist_sign_id = 0xFFFFFFFF;
	}
}

static void __csic_dump_regs(unsigned long addr, unsigned long size)
{
	unsigned int val;
	int cnt = 0;

	do {
		if (cnt % 4 == 0)
			printk("0x%08x:", addr + cnt * 4);
		val = hal_readl(addr + cnt * 4);
		printk(" 0x%08x ", val);
		cnt++;
		if (cnt % 4 == 0 && cnt != 0)
			printk("\n");
	} while (size > cnt * 4);
}

int csi_init(int argc, const char **argv)
{
	struct vin_core *vinc = NULL;
	unsigned long reg_base;
	unsigned long ccu_base;
	int ret = 0;
	int i, j;
	int sensor_id;
	int select_video_num = 2; //VIN_MAX_VIDEO
	unsigned char ir_en = 0;

	vin_log(VIN_LOG_MD, "CSI start!\n");

	ret = vin_g_config();
	if (ret != 0) {
		vin_err("vin get config error\n");
		return -1;
	}

	ret = vin_g_status();
	if (ret != 0) {
		vin_err("There is no open CSI\n");
		return -1;
	}

	rt_memheap_init(&isp_mempool, "isp-mempool", (void *)MEMRESERVE, MEMRESERVE_SIZE);

	reg_base = sunxi_vin_get_top_base();
	csic_top_set_base_addr(reg_base);
	vin_log(VIN_LOG_MD, "reg is 0x%lx\n", reg_base);

	ccu_base = sunxi_vin_get_ccu_base();
	csic_ccu_set_base_addr(ccu_base);
	vin_log(VIN_LOG_MD, "reg is 0x%lx\n", ccu_base);

	vin_md_set_power(PWR_ON);

	sunxi_twi_init(0);
	sunxi_twi_init(1);

#if 0
	i = 0;
	if (global_sensors[sensor_id].sensor_core->sensor_test_i2c) {
		ret = global_sensors[i].sensor_core->sensor_test_i2c(i);
		if (ret)
			return -1;
	}
#endif
	//vin_probe(0);
	for (i = 0; i < select_video_num; i++) {
		if (global_video[i].used == 1) {

			sensor_id = global_video[i].mipi_sel;

			global_sensors[sensor_id].sensor_core = find_sensor_func(global_sensors[sensor_id].sensor_name);

			if (global_video[i].use_sensor_list)
				vin_detect_sensor_list(sensor_id);

			vin_set_from_partition(sensor_id, &ir_en);
			vin_probe(i);
			vinc = global_vinc[i];
			vinc->get_yuv_en = isp_get_cfg[sensor_id].get_yuv_en;

			vin_subdev_ccu_en(i, PWR_ON);

			vin_pipeline_set_mbus_config(i);
			for (j = 0; j < vinc->total_rx_ch; j++) {
				csic_isp_input_select(vinc->isp_sel/ISP_VIRT_NUM, vinc->isp_sel%ISP_VIRT_NUM + j, vinc->csi_sel, j);
			}
			csic_vipp_input_select(vinc->vipp_sel/VIPP_VIRT_NUM, vinc->isp_sel/ISP_VIRT_NUM, vinc->isp_tx_ch);

#ifdef CONFIG_ISP_FAST_CONVERGENCE
			if (global_sensors[sensor_id].sensor_core->s_ir_status)
				global_sensors[sensor_id].sensor_core->s_ir_status(sensor_id, IR_DAY);
#elif defined CONFIG_ISP_HARD_LIGHTADC
			if (global_sensors[sensor_id].sensor_core->s_ir_status) {
				if (ir_en == 1)
					global_sensors[sensor_id].sensor_core->s_ir_status(sensor_id, IR_DAY);
				else if (ir_en == 2)
					global_sensors[sensor_id].sensor_core->s_ir_status(sensor_id, IR_NIGHT);
			}
#endif
			if (global_sensors[sensor_id].sensor_core->sensor_power)
				global_sensors[sensor_id].sensor_core->sensor_power(sensor_id, PWR_ON);
			ret = vin_s_stream(i, PWR_ON);
			if (ret)
				vin_err("find not sensor\n");
		}
	}

	//__csic_dump_regs(0x05830000, 0x300);

	for (i = 0; i < select_video_num; i++) {
		if (global_video[i].used == 1) {
			vinc = global_vinc[i];

			if (vinc->get_yuv_en) {
				while (vinc->frame_cnt != 2)
					hal_usleep(1000);
			}

			while (sunxi_isp_ae_done(vinc->isp_sel, 8) != 0)
				hal_usleep(1000);
			vin_print("close video%d\n", vinc->id);

			vin_s_stream(i, PWR_OFF);
		}
	}

	for (i = 0; i < select_video_num; i++) {
		if (global_video[i].used == 1) {
			vinc = global_vinc[i];

			vin_free(i);
		}
	}
	//vin_free(0);

	openamp_init(); //wait rpmsg init

	for (i = 0; i < select_video_num; i++) {
		if (global_video[i].used == 1) {
			vinc = &global_video[i]; //global_vinc is free

			sensor_id = vinc->mipi_sel;
#if defined CONFIG_ISP_FAST_CONVERGENCE || defined CONFIG_ISP_HARD_LIGHTADC
			sunxi_isp_update_server(vinc->isp_sel);
			if (global_sensors[sensor_id].sensor_core->s_switch)
				global_sensors[sensor_id].sensor_core->s_switch(sensor_id);
#endif
			sunxi_isp_reset_server(vinc->isp_sel);

			vin_set_to_partition(sensor_id);

			if (isp_get_cfg[sensor_id].sensor_deinit) {
				if (vinc->mipi_sel != 0xff)
					sunxi_mipi_subdev_s_stream(vinc->mipi_sel, 0);
				hal_usleep(120);
				if (global_sensors[sensor_id].sensor_core->s_stream)
					global_sensors[sensor_id].sensor_core->s_stream(sensor_id, 0);
				if (global_sensors[sensor_id].sensor_core->sensor_power)
					global_sensors[sensor_id].sensor_core->sensor_power(sensor_id, PWR_OFF);
				vin_print("sensor%d is close\n", sensor_id);
			}
		}
	}

	sunxi_twi_exit(0);
	sunxi_twi_exit(1);

	rt_memheap_detach(&isp_mempool);

	vin_log(VIN_LOG_MD, "GoodBye CSI!\n");
	return ret;
}
FINSH_FUNCTION_EXPORT_ALIAS(csi_init, csi_init, rtthread vin run code);
