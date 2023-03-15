/*
 * mipi subdev driver module
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <hal_mem.h>

#include "../vin.h"
#include "sunxi_mipi.h"

struct mipi_dev *global_mipi[VIN_MAX_MIPI];
bool mipi_probe_flag[VIN_MAX_MIPI];

#define IS_FLAG(x, y) (((x)&(y)) == y)

#if 0
static struct combo_format sunxi_mipi_formats[] = {
	{
		.code = MEDIA_BUS_FMT_UYVY8_2X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_YUYV8_2X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_SBGGR8_1X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_SGBRG8_1X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_SGRBG8_1X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_SRGGB8_1X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_SBGGR10_1X10,
		.bit_width = RAW10,
	}, {
		.code = MEDIA_BUS_FMT_SGBRG10_1X10,
		.bit_width = RAW10,
	}, {
		.code = MEDIA_BUS_FMT_SGRBG10_1X10,
		.bit_width = RAW10,
	}, {
		.code = MEDIA_BUS_FMT_SRGGB10_1X10,
		.bit_width = RAW10,
	}, {
		.code = MEDIA_BUS_FMT_SBGGR12_1X12,
		.bit_width = RAW12,
	}, {
		.code = MEDIA_BUS_FMT_SGBRG12_1X12,
		.bit_width = RAW12,
	}, {
		.code = MEDIA_BUS_FMT_SGRBG12_1X12,
		.bit_width = RAW12,
	}, {
		.code = MEDIA_BUS_FMT_SRGGB12_1X12,
		.bit_width = RAW12,
	}, {
		.code = MEDIA_BUS_FMT_UYVY8_2X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_VYUY8_2X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_YUYV8_2X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_YVYU8_2X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_UYVY10_2X10,
		.bit_width = RAW10,
	}, {
		.code = MEDIA_BUS_FMT_VYUY10_2X10,
		.bit_width = RAW10,
	}, {
		.code = MEDIA_BUS_FMT_YUYV10_2X10,
		.bit_width = RAW10,
	}, {
		.code = MEDIA_BUS_FMT_YVYU10_2X10,
		.bit_width = RAW10,
	}
};

static unsigned int data_formats_type(u32 code)
{
	switch (code) {
	case MIPI_RAW8:
	case MIPI_RAW12:
		return RAW;
	case MIPI_YUV422:
	case MIPI_YUV422_10:
		return YUV;
	case MIPI_RGB565:
		return RGB;
	default:
		return RAW;
	}
}
#endif

static enum pkt_fmt get_pkt_fmt(u32 code)
{
	switch (code) {
	case MEDIA_BUS_FMT_RGB565_2X8_BE:
		return MIPI_RGB565;
	case MEDIA_BUS_FMT_UYVY8_1X16:
	case MEDIA_BUS_FMT_UYVY8_2X8:
	case MEDIA_BUS_FMT_VYUY8_2X8:
	case MEDIA_BUS_FMT_YUYV8_2X8:
	case MEDIA_BUS_FMT_YVYU8_2X8:
		return MIPI_YUV422;
	case MEDIA_BUS_FMT_UYVY10_2X10:
	case MEDIA_BUS_FMT_VYUY10_2X10:
	case MEDIA_BUS_FMT_YUYV10_2X10:
	case MEDIA_BUS_FMT_YVYU10_2X10:
		return MIPI_YUV422_10;
	case MEDIA_BUS_FMT_SBGGR8_1X8:
	case MEDIA_BUS_FMT_SGBRG8_1X8:
	case MEDIA_BUS_FMT_SGRBG8_1X8:
	case MEDIA_BUS_FMT_SRGGB8_1X8:
		return MIPI_RAW8;
	case MEDIA_BUS_FMT_SBGGR10_1X10:
	case MEDIA_BUS_FMT_SGBRG10_1X10:
	case MEDIA_BUS_FMT_SGRBG10_1X10:
	case MEDIA_BUS_FMT_SRGGB10_1X10:
		return MIPI_RAW10;
	case MEDIA_BUS_FMT_SBGGR12_1X12:
	case MEDIA_BUS_FMT_SGBRG12_1X12:
	case MEDIA_BUS_FMT_SGRBG12_1X12:
	case MEDIA_BUS_FMT_SRGGB12_1X12:
		return MIPI_RAW12;
	default:
		return MIPI_RAW8;
	}
}

static int __mcsi_pin_config(int id, int enable)
{
	struct vin_mipi_gpio_info *mipi_gpio = &vin_mipi_gpio[id];
	int i;

	if (enable) {
		for (i = 0; i < 6; i++) {
			hal_gpio_pinmux_set_function(mipi_gpio->pin[i], mipi_gpio->pin_func[0]);
		}
	} else {
		for (i = 0; i < 6; i++) {
			hal_gpio_pinmux_set_function(mipi_gpio->pin[i], mipi_gpio->pin_func[1]);
		}
	}

	return 0;
}

static void cmb_phy_init(struct mipi_dev *mipi)
{
	cmb_phy_lane_num_en(mipi->id + mipi->phy_offset, mipi->cmb_csi_cfg.phy_lane_cfg);
	cmb_phy0_work_mode(mipi->id + mipi->phy_offset, 0);
	cmb_phy0_ofscal_cfg(mipi->id + mipi->phy_offset);
	//cmb_phy_deskew_en(mipi->id + mipi->phy_offset, mipi->cmb_csi_cfg.phy_lane_cfg);
	cmb_term_ctl(mipi->id + mipi->phy_offset, mipi->cmb_csi_cfg.phy_lane_cfg);
	cmb_hs_ctl(mipi->id + mipi->phy_offset, mipi->cmb_csi_cfg.phy_lane_cfg);
	cmb_s2p_ctl(mipi->id + mipi->phy_offset, mipi->time_hs, mipi->cmb_csi_cfg.phy_lane_cfg);
	cmb_mipirx_ctl(mipi->id + mipi->phy_offset, mipi->cmb_csi_cfg.phy_lane_cfg);
	cmb_phy0_en(mipi->id + mipi->phy_offset, 1);
	cmb_phy_deskew1_cfg(mipi->id + mipi->phy_offset);
}

static void combo_csi_mipi_init(struct mipi_dev *mipi)
{
	int i;

	mipi->cmb_csi_cfg.phy_lane_cfg.phy_laneck_en = CK_1LANE;
	mipi->cmb_csi_cfg.phy_lane_cfg.phy_mipi_lpck_en = LPCK_1LANE;
	mipi->cmb_csi_cfg.phy_lane_cfg.phy_termck_en = TERMCK_CLOSE;
	mipi->cmb_csi_cfg.phy_lane_cfg.phy_termdt_en = TERMDT_CLOSE;
	mipi->cmb_csi_cfg.phy_lane_cfg.phy_s2p_en = S2PDT_CLOSE;
	mipi->cmb_csi_cfg.phy_lane_cfg.phy_hsck_en = HSCK_CLOSE;
	mipi->cmb_csi_cfg.phy_lane_cfg.phy_hsdt_en = HSDT_CLOSE;

	cmb_phy_init(mipi);

	/* V853 4lane needs cfg link mode */
	if (mipi->cmb_csi_cfg.phy_link_mode == ONE_4LANE) {
		mipi->cmb_csi_cfg.phy_lane_cfg.phy_mipi_lpck_en = LPCK_CLOSE;
		mipi->phy_offset = 1;
		cmb_phy_link_mode_set(ONE_4LANE);
		cmb_phy_init(mipi);
		mipi->phy_offset = 0;
	}

	for (i = 0; i < mipi->cmb_csi_cfg.lane_num; i++)
		mipi->cmb_csi_cfg.mipi_lane[i] = cmb_port_set_lane_map(mipi->id, i);
	for (i = 0; i < mipi->cmb_csi_cfg.total_rx_ch; i++) {
		if (global_sensors[mipi->id].sensor_core->sensor_g_format)
			mipi->cmb_csi_cfg.mipi_datatype[i] = get_pkt_fmt(global_sensors[mipi->id].sensor_core->sensor_g_format(mipi->id)->mbus_code);
		mipi->cmb_csi_cfg.vc[i] = i;
	}


	cmb_port_lane_num(mipi->id, mipi->cmb_csi_cfg.lane_num);
	cmb_port_out_num(mipi->id, mipi->cmb_csi_cfg.pix_num);
	if (mipi->cmb_mode == MIPI_DOL_WDR_MODE)
		cmb_port_out_chnum(mipi->id, 0);
	else
		cmb_port_out_chnum(mipi->id, mipi->cmb_csi_cfg.total_rx_ch - 1);
	cmb_port_lane_map(mipi->id, mipi->cmb_csi_cfg.mipi_lane);
	cmb_port_mipi_cfg(mipi->id, YUYV); //mipi->cmb_fmt->yuv_seq    //no support yuv mipi sensor
	cmb_port_set_mipi_datatype(mipi->id, &mipi->cmb_csi_cfg);
	cmb_port_mipi_ch_trigger_en(mipi->id, 1);
	if (mipi->cmb_mode == MIPI_DOL_WDR_MODE)
		cmb_port_set_mipi_wdr(mipi->id, 0, 2);
	cmb_port_enable(mipi->id);
}

void combo_csi_init(struct mipi_dev *mipi)
{
	switch (mipi->if_type) {
	case V4L2_MBUS_PARALLEL:
	case V4L2_MBUS_BT656:
		break;
	case V4L2_MBUS_CSI2:
		combo_csi_mipi_init(mipi);
		break;
	case V4L2_MBUS_SUBLVDS:
		break;
	case V4L2_MBUS_HISPI:
		break;
	default:
		break;
	}
}

static int sunxi_mipi_top_s_stream(int on)
{
	struct mipi_dev *mipi = global_mipi[0];

	if (on && (mipi->top_stream_count)++ > 0)
		return 0;
	else if (!on && (mipi->top_stream_count == 0 || --(mipi->stream_count) > 0))
		return 0;

	if (on) {
		cmb_phy_top_enable();
	} else {
		cmb_phy_top_disable();
	}

	return 0;
}


int sunxi_mipi_subdev_s_stream(unsigned int id, int enable)
{
	struct mipi_dev *mipi = global_mipi[id];
	int *stream_count;
	int i;

	stream_count = &mipi->stream_count;
	if (enable && (*stream_count)++ > 0)
		return 0;
	else if (!enable && (*stream_count == 0 || --(*stream_count) > 0))
		return 0;

	if (mipi->res->res_time_hs)
		mipi->time_hs = mipi->res->res_time_hs;
	else
		mipi->time_hs = 0x28;

	mipi->cmb_mode = mipi->res->res_wdr_mode;

	if (mipi->cmb_csi_cfg.phy_link_mode == ONE_4LANE) {
		__mcsi_pin_config(mipi->id, enable);
		__mcsi_pin_config(mipi->id + 1, enable);
	} else
		__mcsi_pin_config(mipi->id, enable);

	sunxi_mipi_top_s_stream(enable);
	if (enable) {
		combo_csi_init(mipi);
	} else {
		mipi->cmb_csi_cfg.phy_link_mode = TWO_2LANE;
		mipi->cmb_csi_cfg.pix_num = ONE_DATA;
		cmb_port_disable(mipi->id);
		cmb_phy0_en(mipi->id, 0);
	}

	vin_log(VIN_LOG_FMT, "%s%d %s, lane_num %d\n",
		mipi->if_name, mipi->id, enable ? "stream on" : "stream off",
		mipi->cmb_csi_cfg.lane_num);

	return 0;
}

int sunxi_mipi_s_mbus_config(unsigned int id, const struct v4l2_mbus_config *cfg, const struct mbus_framefmt_res *res)
{
	struct mipi_dev *mipi = global_mipi[id];

#if 0
	if (cfg->type == V4L2_MBUS_CSI2) {
		mipi->if_type = V4L2_MBUS_CSI2;
		memcpy(mipi->if_name, "mipi", sizeof("mipi"));
		if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_4_LANE)) {
			mipi->csi2_cfg.lane_num = 4;
			mipi->cmb_cfg.lane_num = 4;
			mipi->cmb_cfg.mipi_ln = MIPI_4LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_3_LANE)) {
			mipi->csi2_cfg.lane_num = 3;
			mipi->cmb_cfg.lane_num = 3;
			mipi->cmb_cfg.mipi_ln = MIPI_3LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_2_LANE)) {
			mipi->csi2_cfg.lane_num = 2;
			mipi->cmb_cfg.lane_num = 2;
			mipi->cmb_cfg.mipi_ln = MIPI_2LANE;
		} else {
			mipi->cmb_cfg.lane_num = 1;
			mipi->csi2_cfg.lane_num = 1;
			mipi->cmb_cfg.mipi_ln = MIPI_1LANE;
		}
	} else if (cfg->type == V4L2_MBUS_SUBLVDS) {
		mipi->if_type = V4L2_MBUS_SUBLVDS;
		memcpy(mipi->if_name, "sublvds", sizeof("sublvds"));
		if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_12_LANE)) {
			mipi->cmb_cfg.lane_num = 12;
			mipi->cmb_cfg.lvds_ln = LVDS_12LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_10_LANE)) {
			mipi->cmb_cfg.lane_num = 10;
			mipi->cmb_cfg.lvds_ln = LVDS_10LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_8_LANE)) {
			mipi->cmb_cfg.lane_num = 8;
			mipi->cmb_cfg.lvds_ln = LVDS_8LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_4_LANE)) {
			mipi->cmb_cfg.lane_num = 4;
			mipi->cmb_cfg.lvds_ln = LVDS_4LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_2_LANE)) {
			mipi->cmb_cfg.lane_num = 2;
			mipi->cmb_cfg.lvds_ln = LVDS_2LANE;
		} else {
			mipi->cmb_cfg.lane_num = 8;
			mipi->cmb_cfg.lvds_ln = LVDS_8LANE;
		}
	} else if (cfg->type == V4L2_MBUS_HISPI) {
		mipi->if_type = V4L2_MBUS_HISPI;
		memcpy(mipi->if_name, "hispi", sizeof("hispi"));
		if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_12_LANE)) {
			mipi->cmb_cfg.lane_num = 12;
			mipi->cmb_cfg.lvds_ln = LVDS_12LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_10_LANE)) {
			mipi->cmb_cfg.lane_num = 10;
			mipi->cmb_cfg.lvds_ln = LVDS_10LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_8_LANE)) {
			mipi->cmb_cfg.lane_num = 8;
			mipi->cmb_cfg.lvds_ln = LVDS_8LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_4_LANE)) {
			mipi->cmb_cfg.lane_num = 4;
			mipi->cmb_cfg.lvds_ln = LVDS_4LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_2_LANE)) {
			mipi->cmb_cfg.lane_num = 2;
			mipi->cmb_cfg.lvds_ln = LVDS_2LANE;
		} else {
			mipi->cmb_cfg.lane_num = 4;
			mipi->cmb_cfg.lvds_ln = LVDS_4LANE;
		}
	} else {
		memcpy(mipi->if_name, "combo_parallel", sizeof("combo_parallel"));
		mipi->if_type = V4L2_MBUS_PARALLEL;
	}

	mipi->csi2_cfg.total_rx_ch = 0;
	if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_CHANNEL_0)) {
		mipi->csi2_cfg.total_rx_ch++;
	}
	if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_CHANNEL_1)) {
		mipi->csi2_cfg.total_rx_ch++;
	}
	if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_CHANNEL_2)) {
		mipi->csi2_cfg.total_rx_ch++;
	}
	if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_CHANNEL_3)) {
		mipi->csi2_cfg.total_rx_ch++;
	}
#else
	if (cfg->type == V4L2_MBUS_CSI2) {
		mipi->if_type = V4L2_MBUS_CSI2;
		memcpy(mipi->if_name, "mipi", sizeof("mipi"));
		if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_4_LANE)) {
			mipi->cmb_csi_cfg.phy_lane_cfg.phy_lanedt_en = DT_4LANE;
			mipi->cmb_csi_cfg.phy_lane_cfg.phy_mipi_lpdt_en = LPDT_4LANE;
			mipi->cmb_csi_cfg.phy_lane_cfg.phy_deskew_en = DK_4LANE;
			mipi->cmb_csi_cfg.lane_num = 4;
#if defined CONFIG_ARCH_SUN20IW3
			mipi->cmb_csi_cfg.phy_link_mode = ONE_4LANE;
			if (mipi->id)
				vin_err("PORT1 supports a maximum of 2lane!\n");
#endif
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_3_LANE)) {
			mipi->cmb_csi_cfg.phy_lane_cfg.phy_lanedt_en = DT_3LANE;
			mipi->cmb_csi_cfg.phy_lane_cfg.phy_mipi_lpdt_en = LPDT_3LANE;
			mipi->cmb_csi_cfg.phy_lane_cfg.phy_deskew_en = DK_3LANE;
			mipi->cmb_csi_cfg.lane_num = 3;
#if defined CONFIG_ARCH_SUN20IW3
			mipi->cmb_csi_cfg.phy_link_mode = ONE_4LANE;
			if (mipi->id)
				vin_err("PORT1 supports a maximum of 2lane!\n");
#endif
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_2_LANE)) {
			mipi->cmb_csi_cfg.phy_lane_cfg.phy_lanedt_en = DT_2LANE;
			mipi->cmb_csi_cfg.phy_lane_cfg.phy_mipi_lpdt_en = LPDT_2LANE;
			mipi->cmb_csi_cfg.phy_lane_cfg.phy_deskew_en = DK_2LANE;
			mipi->cmb_csi_cfg.lane_num = 2;
		} else {
			mipi->cmb_csi_cfg.phy_lane_cfg.phy_lanedt_en = DT_1LANE;
			mipi->cmb_csi_cfg.phy_lane_cfg.phy_mipi_lpdt_en = LPDT_1LANE;
			mipi->cmb_csi_cfg.phy_lane_cfg.phy_deskew_en = DK_1LANE;
			mipi->cmb_csi_cfg.lane_num = 1;
		}
	}

	mipi->cmb_csi_cfg.total_rx_ch = 0;
	if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_CHANNEL_0)) {
		mipi->cmb_csi_cfg.total_rx_ch++;
	}
	if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_CHANNEL_1)) {
		mipi->cmb_csi_cfg.total_rx_ch++;
	}
	if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_CHANNEL_2)) {
		mipi->cmb_csi_cfg.total_rx_ch++;
	}
	if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_CHANNEL_3)) {
		mipi->cmb_csi_cfg.total_rx_ch++;
	}
#endif
	memcpy(mipi->res, res, sizeof(struct mbus_framefmt_res));

	return 0;
}

int mipi_probe(unsigned int id)
{
	struct mipi_dev *mipi = NULL;
	int ret = 0;

	if (mipi_probe_flag[id])
		return 0;
	else
		mipi_probe_flag[id] = true;

	if (id > VIN_MAX_MIPI) {
		vin_err("mipi%d is not existing, max is %d\n", id, VIN_MAX_MIPI);
		return -1;
	}

	mipi = hal_malloc(sizeof(struct mipi_dev));
	if (!mipi) {
		vin_err("Fail to init MIPI dev!\n");
		ret = -1;
		goto ekzalloc;
	}
	memset(mipi, 0, sizeof(struct mipi_dev));
	mipi->id = id;

	mipi->res = hal_malloc(sizeof(struct mbus_framefmt_res));
	if (!mipi->res) {
		vin_err("Fail to init MIPI res!\n");
		ret = -1;
		goto freedev;
	}
	memset(mipi->res, 0, sizeof(struct mbus_framefmt_res));

	mipi->base = sunxi_vin_get_mipi_base();
	cmb_csi_set_top_base_addr((unsigned long)mipi->base);
	vin_log(VIN_LOG_MD, "mipi%d reg is 0x%lx\n", mipi->id, mipi->base);
	mipi->phy_base = sunxi_vin_get_mipiphy_base(mipi->id);
	cmb_csi_set_phy_base_addr(mipi->id, (unsigned long)mipi->phy_base);
	mipi->port_base = sunxi_vin_get_mipiport_base(mipi->id);
	cmb_csi_set_port_base_addr(mipi->id, (unsigned long)mipi->port_base);

	global_mipi[id] = mipi;
	vin_log(VIN_LOG_MIPI, "mipi%d probe end!\n", mipi->id);
	return 0;

freedev:
	hal_free(mipi);
ekzalloc:
	vin_err("mipi probe err!\n");
	return ret;
}

int mipi_remove(unsigned int id)
{
	struct mipi_dev *mipi = global_mipi[id];

	if (!mipi_probe_flag[id])
		return 0;
	else
		mipi_probe_flag[id] = false;

	hal_free(mipi->res);
	hal_free(mipi);
	return 0;
}


