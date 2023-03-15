/*
 * linux-4.9/drivers/media/platform/sunxi-vin/vin-isp/isp_server/isp_server.c
 *
 * Copyright (c) 2007-2022 Allwinnertech Co., Ltd.
 *
 * Authors:  Yang Feng <yangfeng@allwinnertech.com>
 *         Zheng ZeQun <zequnzheng@allwinnertech.com>
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
#include <hal_mem.h>
#include <hal_timer.h>
#include <hal_interrupt.h>
#include <math.h>

#include "isp_tuning/isp_tuning_priv.h"
#include "isp_math/isp_math_util.h"
#include "isp_server.h"
#include "../../utility/v4l2-controls.h"

extern unsigned int isp_default_reg[ISP_LOAD_REG_SIZE>>2];

struct isp_lib_context isp_ctx[HW_ISP_DEVICE_NUM] = {
	[0] = {
		.isp_ini_cfg = {
			.isp_test_settings = {
				.ae_en = 1,
				.awb_en = 1,
				.wb_en = 1,
				.hist_en = 1,
			},
			.isp_3a_settings = {
				.ae_stat_sel = 1,
				.ae_delay_frame = 0,
				.exp_delay_frame = 1,
				.gain_delay_frame = 1,

				.awb_interval = 2,
				.awb_speed = 32,
				.awb_stat_sel = 1,
				.awb_light_num = 9,
				.awb_light_info = {
					  254,	 256,	104,   256,   256,   256,    50,  1900,    32,	  80,
					  234,	 256,	108,   256,   256,   256,    50,  2500,    32,	  85,
					  217,	 256,	114,   256,   256,   256,    50,  2800,    32,	  90,
					  160,	 256,	153,   256,   256,   256,    50,  4000,    64,	  90,
					  141,	 256,	133,   256,   256,   256,    50,  4100,    64,	 100,
					  142,	 256,	174,   256,   256,   256,    50,  5000,   100,	 100,
					  118,	 256,	156,   256,   256,   256,    50,  5300,    64,	 100,
					  127,	 256,	195,   256,   256,   256,    50,  6500,    64,	 90,
					  115,	 256,	215,   256,   256,   256,    50,  8000,    64,	 80
				},
			},
		},
	},
};
struct isp_tuning *tuning[HW_ISP_DEVICE_NUM];
struct hw_isp_paras hw_isp_paras;

static void __ae_done(struct isp_lib_context *lib,
			ae_result_t *result __attribute__((__unused__)))
{
#if (HW_ISP_DEVICE_NUM > 1)
	if (hw_isp_paras.isp_sync_mode && (lib->isp_id == 0))
		*result = isp_ctx[1].ae_entity_ctx.ae_result;
#endif
	FUNCTION_LOG;
}
static void __af_done(struct isp_lib_context *lib,
			af_result_t *result __attribute__((__unused__)))
{
#if (HW_ISP_DEVICE_NUM > 1)
	if (hw_isp_paras.isp_sync_mode && (lib->isp_id == 0))
		*result = isp_ctx[1].af_entity_ctx.af_result;
#endif
	FUNCTION_LOG;
}
static void __awb_done(struct isp_lib_context *lib,
			awb_result_t *result __attribute__((__unused__)))
{
#if (HW_ISP_DEVICE_NUM > 1)
	if (hw_isp_paras.isp_sync_mode && (lib->isp_id == 0))
		*result = isp_ctx[1].awb_entity_ctx.awb_result;
#endif
	FUNCTION_LOG;
}
static void __afs_done(struct isp_lib_context *lib,
			afs_result_t *result __attribute__((__unused__)))
{
#if (HW_ISP_DEVICE_NUM > 1)
	if (hw_isp_paras.isp_sync_mode && (lib->isp_id == 0))
		*result = isp_ctx[1].afs_entity_ctx.afs_result;
#endif
	FUNCTION_LOG;
}

static void __md_done(struct isp_lib_context *lib,
			md_result_t *result __attribute__((__unused__)))
{
	FUNCTION_LOG;
}

static void __pltm_done(struct isp_lib_context *lib,
			pltm_result_t *result __attribute__((__unused__)))
{
#if (HW_ISP_DEVICE_NUM > 1)
	if (hw_isp_paras.isp_sync_mode && (lib->isp_id == 0))
		*result = isp_ctx[1].pltm_entity_ctx.pltm_result;
#endif
	FUNCTION_LOG;
}

static struct isp_ctx_operations isp_ctx_ops = {
	.ae_done = __ae_done,
	.af_done = __af_done,
	.awb_done = __awb_done,
	.afs_done = __afs_done,
	.md_done = __md_done,
	.pltm_done = __pltm_done,
};

void isp_stats_process(struct hw_isp_device *isp, void *isp_stat_buf)
{
	struct isp_lib_context *ctx;
	unsigned long flags;

	ctx = isp_dev_get_ctx(isp);
	if (ctx == NULL)
		return;

	ctx->isp_stat_buf = isp_stat_buf;

	isp_ctx_stats_prepare(ctx, ctx->isp_stat_buf);

	isp_stat_save_run(ctx);

	FUNCTION_LOG;
	flags = hal_interrupt_save();
	isp_ctx_algo_run(ctx);
	hal_interrupt_restore(flags);
	FUNCTION_LOG;

	isp_log_save_run(ctx);
}

void isp_fsync_process(struct hw_isp_device *isp, unsigned int *data)
{
	struct isp_lib_context *ctx = isp_dev_get_ctx(isp);

	if (ctx->sensor_info.color_space != data[1]) {
		ctx->sensor_info.color_space = data[1];
		ctx->isp_3a_change_flags |= ISP_SET_HUE;
	}

	isp_lib_log_param = (data[3] << 8) | data[2] | ctx->isp_ini_cfg.isp_test_settings.isp_log_param;
}

void isp_ctrl_process(struct hw_isp_device *isp, unsigned int *data)
{
	struct isp_lib_context *isp_gen = isp_dev_get_ctx(isp);
	HW_S32 iso_qmenu[] = { 100, 200, 400, 800, 1600, 3200, 6400};
	HW_S32 exp_bias_qmenu[] = { -4, -3, -2, -1, 0, 1, 2, 3, 4, };

	if (isp_gen == NULL)
		return;

	switch (data[1]) {
	case V4L2_CID_BRIGHTNESS:
		isp_s_brightness(isp_gen, data[2]);
		break;
	case V4L2_CID_CONTRAST:
		isp_s_contrast(isp_gen, data[2]);
		break;
	case V4L2_CID_SATURATION:
		isp_s_saturation(isp_gen, data[2]);
		break;
	case V4L2_CID_HUE:
		isp_s_hue(isp_gen, data[2]);
		break;
	case V4L2_CID_AUTO_WHITE_BALANCE:
		isp_s_auto_white_balance(isp_gen, data[2]);
		break;
	case V4L2_CID_EXPOSURE:
		isp_s_exposure(isp_gen, data[2]);
		break;
	case V4L2_CID_AUTOGAIN:
		isp_s_auto_gain(isp_gen, data[2]);
		break;
	case V4L2_CID_GAIN:
		isp_s_gain(isp_gen, data[2]);
		break;
	case V4L2_CID_POWER_LINE_FREQUENCY:
		isp_s_power_line_frequency(isp_gen, data[2]);
		break;
	case V4L2_CID_WHITE_BALANCE_TEMPERATURE:
		isp_s_white_balance_temperature(isp_gen, data[2]);
		break;
	case V4L2_CID_SHARPNESS:
		isp_s_sharpness(isp_gen, data[2]);
		break;
	case V4L2_CID_AUTOBRIGHTNESS:
		isp_s_auto_brightness(isp_gen, data[2]);
		break;
	case V4L2_CID_BAND_STOP_FILTER:
		isp_s_band_stop_filter(isp_gen, data[2]);
		break;
	case V4L2_CID_ILLUMINATORS_1:
		isp_s_illuminators_1(isp_gen, data[2]);
		break;
	case V4L2_CID_ILLUMINATORS_2:
		isp_s_illuminators_2(isp_gen, data[2]);
		break;
	case V4L2_CID_EXPOSURE_AUTO:
		isp_s_exposure_auto(isp_gen, data[2]);
		break;
	case V4L2_CID_EXPOSURE_ABSOLUTE:
		isp_s_exposure_absolute(isp_gen, data[2]);
		break;
	case V4L2_CID_FOCUS_ABSOLUTE:
		isp_s_focus_absolute(isp_gen, data[2]);
		break;
	case V4L2_CID_FOCUS_RELATIVE:
		isp_s_focus_relative(isp_gen, data[2]);
		break;
	case V4L2_CID_FOCUS_AUTO:
		isp_s_focus_auto(isp_gen, data[2]);
		break;
	case V4L2_CID_AUTO_EXPOSURE_BIAS:
		isp_s_auto_exposure_bias(isp_gen, exp_bias_qmenu[data[2]]);
		break;
	case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
		isp_s_auto_n_preset_white_balance(isp_gen, data[2]);
		break;
	case V4L2_CID_ISO_SENSITIVITY:
		isp_s_iso_sensitivity(isp_gen, iso_qmenu[data[2]]);
		break;
	case V4L2_CID_ISO_SENSITIVITY_AUTO:
		isp_s_iso_sensitivity_auto(isp_gen, data[2]);
		break;
	case V4L2_CID_EXPOSURE_METERING:
		isp_s_ae_metering_mode(isp_gen, data[2]);
		break;
	case V4L2_CID_SCENE_MODE:
		isp_s_scene_mode(isp_gen, data[2]);
		break;
	case V4L2_CID_3A_LOCK:
		//isp_s_3a_lock(isp_gen, data[1]);
		break;
	case V4L2_CID_AUTO_FOCUS_START:
		isp_s_auto_focus_start(isp_gen, data[2]);
		break;
	case V4L2_CID_AUTO_FOCUS_STOP:
		isp_s_auto_focus_stop(isp_gen, data[2]);
		break;
	case V4L2_CID_AUTO_FOCUS_RANGE:
		isp_s_auto_focus_range(isp_gen, data[2]);
		break;
	//case V4L2_CID_TAKE_PICTURE:
	//	isp_s_take_picture(isp_gen, data[1]);
	//	break;
	case V4L2_CID_FLASH_LED_MODE:
		isp_s_flash_mode(isp_gen, data[2]);
		break;
	default:
		ISP_ERR("Unknown ctrl.\n");
		break;
	}
}

struct hw_isp_device *isp_server_init(unsigned int id)
{
	struct hw_isp_device *hw_isp = NULL;

	if (id >= HW_ISP_DEVICE_NUM)
		return NULL;

	isp_version_info();

	hw_isp = hal_malloc(sizeof(struct hw_isp_device));
	if (!hw_isp)
		goto ekzalloc;
	memset(hw_isp, 0, sizeof(struct hw_isp_device));

	hw_isp->id = id;
	isp_ctx[id].isp_id = id;
	isp_dev_banding_ctx(hw_isp, &isp_ctx[id]);

	return hw_isp;
ekzalloc:
	ISP_ERR("isp server init err!\n");
	return NULL;
}

int isp_server_save_ctx(unsigned int id)
{
	return isp_ctx_save_init(&isp_ctx[id]);
}

int isp_server_save_reg(unsigned int id, int ir)
{
	return isp_reg_save_init(&isp_ctx[id], ir);
}

int isp_server_set_paras(struct hw_isp_device *hw_isp, int ir, unsigned int id)
{
	int ret = 0;

	if (id >= HW_ISP_DEVICE_NUM)
		return -1;

	hw_isp_paras.isp_sync_mode = 0;
	hw_isp_paras.ir_flag = ir;
	isp_ctx[id].isp_ir_flag = hw_isp_paras.ir_flag;
	ret = isp_params_parse(hw_isp, &isp_ctx[id].isp_ini_cfg, hw_isp_paras.ir_flag, hw_isp_paras.isp_sync_mode);
	if (ret < 0)
		ISP_PRINT("isp param parse fail!!\n");

	FUNCTION_LOG;
	if (isp_ctx_algo_init(&isp_ctx[id], &isp_ctx_ops))
		return -1;
	FUNCTION_LOG;

	tuning[id] = isp_tuning_init(hw_isp, &isp_ctx[id].isp_ini_cfg);
	if (tuning[id] == NULL) {
		ISP_ERR("error: unable to initialize isp tuning\n");
		return -1;
	}
	FUNCTION_LOG;

	return ret;
}

int isp_server_reset(struct hw_isp_device *hw_isp, int dev_id, int mode_flag)
{
	int ret = 0;

	if (dev_id >= HW_ISP_DEVICE_NUM)
		return -1;

	if (mode_flag & 0x02) {
		hw_isp_paras.ir_flag = 1;
		ISP_PRINT("ISP select ir config\n");
	} else
		hw_isp_paras.ir_flag = 0;

	//isp_ctx[dev_id].isp_load_flag = 0;
	isp_ctx[dev_id].isp_ir_flag = hw_isp_paras.ir_flag;  /*change hue and color, when isp reset*/
	isp_params_parse(hw_isp, &isp_ctx[dev_id].isp_ini_cfg, hw_isp_paras.ir_flag, hw_isp_paras.isp_sync_mode);
	ret = isp_tuning_reset(hw_isp, &isp_ctx[dev_id].isp_ini_cfg);
	if (ret) {
		ISP_ERR("error: unable to reset isp tuning\n");
	}

	return ret;
}

void isp_set_firstframe_wb(int dev_id)
{
	struct isp_lib_context *isp_gen = &isp_ctx[dev_id];
	isp_awb_entity_context_t *isp_awb_cxt = &isp_gen->awb_entity_ctx;
	struct isp_param_config *param = &isp_gen->isp_ini_cfg;
	struct isp_module_config *mod_cfg = &isp_gen->module_cfg;

	if (dev_id >= HW_ISP_DEVICE_NUM)
		return;

	/* when liner high frame rate run first and get now wb gain, and then run in wdr low frame rate, wb gain must be sqrt*/
	if (param->isp_test_settings.nrp_en && mod_cfg->nrp_cfg.gamma_en &&
			!mod_cfg->nrp_cfg.inv_gamma_en && mod_cfg->mode_cfg.wb_sel) {
		isp_gen->module_cfg.wb_gain_cfg.wb_gain.r_gain = (HW_U16)(pow((double)isp_awb_cxt->awb_result.wb_gain_output.r_gain / 256.0, 1/1.3) * 256.0);
		isp_gen->module_cfg.wb_gain_cfg.wb_gain.gr_gain = (HW_U16)(pow((double)isp_awb_cxt->awb_result.wb_gain_output.gr_gain / 256.0, 1/1.3) * 256.0);
		isp_gen->module_cfg.wb_gain_cfg.wb_gain.gb_gain = (HW_U16)(pow((double)isp_awb_cxt->awb_result.wb_gain_output.gb_gain / 256.0, 1/1.3) * 256.0);
		isp_gen->module_cfg.wb_gain_cfg.wb_gain.b_gain = (HW_U16)(pow((double)isp_awb_cxt->awb_result.wb_gain_output.b_gain / 256.0, 1/1.3) * 256.0);
	} else {
		isp_gen->module_cfg.wb_gain_cfg.wb_gain = isp_awb_cxt->awb_result.wb_gain_output;
	}

	if (isp_gen->otp_enable == 1) {
		isp_gen->module_cfg.wb_gain_cfg.wb_gain.r_gain = isp_gen->module_cfg.wb_gain_cfg.wb_gain.r_gain*isp_gen->wb_golden_ratio[0];
		isp_gen->module_cfg.wb_gain_cfg.wb_gain.gr_gain = isp_gen->module_cfg.wb_gain_cfg.wb_gain.gr_gain*isp_gen->wb_golden_ratio[1];
		isp_gen->module_cfg.wb_gain_cfg.wb_gain.gb_gain = isp_gen->module_cfg.wb_gain_cfg.wb_gain.gb_gain*isp_gen->wb_golden_ratio[1];
		isp_gen->module_cfg.wb_gain_cfg.wb_gain.b_gain = isp_gen->module_cfg.wb_gain_cfg.wb_gain.b_gain*isp_gen->wb_golden_ratio[2];
	}
}

int isp_server_exit(struct hw_isp_device **hw_isp, unsigned int id)
{
	if (id >= HW_ISP_DEVICE_NUM)
		return -1;

	isp_log_save_exit(&isp_ctx[id]);
	isp_stat_save_exit(&isp_ctx[id]);
	isp_reg_save_exit(&isp_ctx[id]);
	isp_ctx_save_exit(&isp_ctx[id]);
	isp_tuning_exit(*hw_isp);
	isp_ctx_algo_exit(&isp_ctx[id]);
	hal_free(*hw_isp);
	*hw_isp = NULL;

	ISP_PRINT("isp%d exit end!!!\n", id);

	return 0;
}

void isp_save_exit(struct hw_isp_device *hw_isp, int id)
{
	if (id >= HW_ISP_DEVICE_NUM)
		return;

	isp_log_save_exit(&isp_ctx[id]);
	isp_stat_save_exit(&isp_ctx[id]);
	isp_reg_save_exit(&isp_ctx[id]);
	isp_ctx_save_exit(&isp_ctx[id]);
}

HW_S32 isp_stats_req(struct hw_isp_device *hw_isp, int dev_id, struct isp_stats_context *stats_ctx)
{
	struct isp_lib_context *ctx = NULL;

	if (dev_id >= HW_ISP_DEVICE_NUM)
		return -1;

	ctx = isp_dev_get_ctx(hw_isp);
	if (ctx == NULL)
		return -1;

	return isp_ctx_stats_req(ctx, stats_ctx);
}

int isp_get_lv(struct hw_isp_device *hw_isp, int dev_id)
{
	struct isp_lib_context *ctx;

	ctx = isp_dev_get_ctx(hw_isp);
	if (ctx == NULL) {
		ISP_ERR("isp%d get isp ctx failed!\n", dev_id);
		return -1;
	}

	return ctx->ae_entity_ctx.ae_result.ev_lv_adj;
}

void isp_ldci_video_en(int id, int en)
{
	/*if (isp_ctx[id].isp_ini_cfg.isp_tunning_settings.gtm_type == 4) {
		if (en)
			enable_ldci_video(LDCI_VIDEO_CHN);
		else
			disable_ldci_video(LDCI_VIDEO_CHN);
	}*/
}

int isp_get_attr_cfg(struct hw_isp_device *hw_isp, HW_U32 ctrl_id, void *value)
{
	struct isp_lib_context *isp_gen = isp_dev_get_ctx(hw_isp);
	unsigned char *pdata_u8 = (unsigned char *)value;
	int i;

	if (isp_gen == NULL)
		return -1;

	switch (ctrl_id) {
	case ISP_CTRL_MODULE_EN:
		break;
	case ISP_CTRL_DIGITAL_GAIN:
		*(HW_S32 *)value = isp_gen->ae_entity_ctx.ae_result.sensor_set.ev_set_curr.ev_digital_gain;
		break;
	case ISP_CTRL_PLTMWDR_STR:
		*(HW_S32 *)value = isp_gen->tune.pltmwdr_level;
		break;
	case ISP_CTRL_DN_STR:
		*(HW_S32 *)value = isp_gen->tune.denoise_level;
		break;
	case ISP_CTRL_3DN_STR:
		*(HW_S32 *)value = isp_gen->tune.tdf_level;
		break;
	case ISP_CTRL_HIGH_LIGHT:
		*(HW_S32 *)value = isp_gen->tune.highlight_level;
		break;
	case ISP_CTRL_BACK_LIGHT:
		*(HW_S32 *)value = isp_gen->tune.backlight_level;
		break;
	case ISP_CTRL_WB_MGAIN:
		*(struct isp_wb_gain *)value = isp_gen->awb_entity_ctx.awb_result.wb_gain_output;
		break;
	case ISP_CTRL_AGAIN_DGAIN:
		*(struct gain_cfg *)value = isp_gen->tune.gains;
		break;
	case ISP_CTRL_COLOR_EFFECT:
		*(HW_S32 *)value = isp_gen->tune.effect;
		break;
	case ISP_CTRL_AE_ROI:
		*(struct isp_h3a_coor_win *)value = isp_gen->ae_settings.ae_coor;
		break;
	case ISP_CTRL_COLOR_TEMP:
		*(HW_S32 *)value = isp_gen->awb_entity_ctx.awb_result.color_temp_output;
		break;
	case ISP_CTRL_EV_IDX:
		*(HW_S32 *)value = isp_gen->ae_entity_ctx.ae_result.sensor_set.ev_set.ev_idx;
		break;
	case ISP_CTRL_MAX_EV_IDX:
		*(HW_S32 *)value = isp_gen->ae_entity_ctx.ae_result.sensor_set.ev_idx_max;
		break;
	case ISP_CTRL_AE_LOCK:
		*(HW_S32 *)value = isp_gen->ae_settings.exposure_lock;
		break;
	case ISP_CTRL_AE_STATS:
		for (i = 0; i < ISP_AE_ROW*ISP_AE_COL; i++) {
			pdata_u8[i] = isp_gen->ae_entity_ctx.ae_stats.ae_stats->avg[i];
		}
		break;
	case ISP_CTRL_ISO_LUM_IDX:
		*(HW_S32 *)value = isp_gen->iso_entity_ctx.iso_result.lum_idx;
		break;
	case ISP_CTRL_AE_EV_LV:
		*(HW_S32 *)value = isp_gen->ae_entity_ctx.ae_result.sensor_set.ev_set_curr.ev_lv;
		break;
	case ISP_CTRL_AE_EV_LV_ADJ:
		*(HW_S32 *)value = isp_gen->ae_entity_ctx.ae_result.ev_lv_adj;
		break;
	case ISP_CTRL_IR_STATUS:
		*(HW_S32 *)value = isp_gen->isp_ir_flag;
		break;
	case ISP_CTRL_IR_AWB_GAIN:
		{
			int *rpdata_ptr = (HW_S32 *)value;
			rpdata_ptr[0] = isp_gen->rgain_ir;
			rpdata_ptr[1] = isp_gen->bgain_ir;
			break;
		}
	default:
		ISP_ERR("Unknown ctrl.\n");
		break;
	}
	return 0;
}

int isp_set_attr_cfg(struct hw_isp_device *hw_isp, HW_U32 ctrl_id, void *value)
{
	struct isp_lib_context *isp_gen = isp_dev_get_ctx(hw_isp);
	int i, ir_status;
	unsigned char *ptr_u8 = NULL;
	if (isp_gen == NULL)
		return -1;

	switch (ctrl_id) {
	case ISP_CTRL_MODULE_EN:
		break;
	case ISP_CTRL_DIGITAL_GAIN:
		break;
	case ISP_CTRL_PLTMWDR_STR:
		isp_gen->tune.pltmwdr_level = *(HW_S32 *)value;
		break;
	case ISP_CTRL_DN_STR:
		isp_gen->tune.denoise_level = *(HW_S32 *)value;
		break;
	case ISP_CTRL_3DN_STR:
		isp_gen->tune.tdf_level = *(HW_S32 *)value;
		break;
	case ISP_CTRL_HIGH_LIGHT:
		isp_gen->tune.highlight_level = clamp(*(HW_S32 *)value, -31, 35);
		break;
	case ISP_CTRL_BACK_LIGHT:
		isp_gen->tune.backlight_level = clamp(*(HW_S32 *)value, -31, 35);
		break;
	case ISP_CTRL_WB_MGAIN:
		isp_gen->awb_settings.wb_gain_manual = *(struct isp_wb_gain *)value;
		break;
	case ISP_CTRL_AGAIN_DGAIN:
		if (memcmp(&isp_gen->tune.gains, value, sizeof(struct gain_cfg))) {
			isp_gen->tune.gains = *(struct gain_cfg *)value;
			isp_gen->isp_3a_change_flags |= ISP_SET_GAIN_STR;
		}
		break;
	case ISP_CTRL_COLOR_EFFECT:
		if (isp_gen->tune.effect != *(HW_S32 *)value) {
			isp_gen->tune.effect = *(HW_S32 *)value;
			isp_gen->isp_3a_change_flags |= ISP_SET_EFFECT;
		}
		break;
	case ISP_CTRL_AE_ROI:
		isp_s_ae_roi(isp_gen, AE_METERING_MODE_SPOT, value);
		break;
	case ISP_CTRL_AF_METERING:
		isp_s_af_metering_mode(isp_gen, value);
		break;
	case ISP_CTRL_VENC2ISP_PARAM:
		ptr_u8 = (unsigned char *)value;
		isp_gen->VencVe2IspParam.mMovingLevelInfo.is_overflow = ptr_u8[0];
		for (i = 0; i < ISP_MSC_TBL_SIZE; i++) {
			isp_gen->VencVe2IspParam.mMovingLevelInfo.moving_level_table[i] = ptr_u8[i + 1] << 4;
		}
		break;
	case ISP_CTRL_NPU_NR_PARAM:
		isp_gen->npu_nr_cfg = *(struct npu_face_nr_config *)value;
		break;
	case ISP_CTRL_EV_IDX:
		isp_set_ae_ev_idx(isp_gen, *(HW_S32 *)value);
		break;
	case ISP_CTRL_AE_LOCK:
		isp_gen->ae_settings.exposure_lock = *(HW_S32 *)value;
		break;
	case ISP_CTRL_AE_TABLE:
		isp_set_ae_tbl(isp_gen, (HW_S32 *)value);
		break;
	case ISP_CTRL_IR_STATUS:
		ir_status = *(HW_S32 *)value;
		if (ir_status) {
			isp_server_reset(hw_isp, hw_isp->id, 0x02);
		} else {
			isp_server_reset(hw_isp, hw_isp->id, 0x00);
		}
		break;
	case ISP_CTRL_READ_BIN_PARAM:
		if (isp_tuning_read_from_bin(hw_isp, value))
			ISP_ERR("ISP_CTRL_READ_BIN_PARAM ERROR!!!\n");
		break;
	case ISP_CTRL_AE_ROI_TARGET:
		isp_set_ae_roi_target(isp_gen, (HW_S32 *)value);
		break;
	default:
		ISP_ERR("Unknown ctrl.\n");
		break;
	}
	return 0;
}

int isp_rpmsg_ept_send(struct rpmsg_endpoint *ept, void *data, int len);
void isp_save_ae_gainexp(struct hw_isp_device *hw_isp)
{
#ifdef CONFIG_ISP_READ_THRESHOLD
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);
	unsigned int rpmsg_data[6];
	ae_result_t *ae_result = NULL;

	memset(rpmsg_data, 0, 6 * sizeof(unsigned int));
	ae_result = &ctx->ae_entity_ctx.ae_result;
	rpmsg_data[0] = ISP_SET_SAVE_AE;
	rpmsg_data[1] = ae_result->sensor_set.ev_set_curr.ev_idx;
	rpmsg_data[2] = ae_result->sensor_set.ev_set_curr.ev_analog_gain >> 4;
	rpmsg_data[3] = ae_result->sensor_set.ev_set_curr.ev_sensor_exp_line;
	rpmsg_data[4] = ae_result->sensor_set_short.ev_set_curr.ev_analog_gain >> 4;
	rpmsg_data[5] = ae_result->sensor_set_short.ev_set_curr.ev_sensor_exp_line;
	if (ctx->ae_settings.ae_mode == AE_WDR_AUTO || ctx->ae_settings.ae_mode == AE_WDR_AUTO_DG) {
		rpmsg_data[3] = N_ROUND(ae_result->sensor_set.ev_set_curr.ev_sensor_exp_line, 256);
		if (rpmsg_data[3] == 0)
			rpmsg_data[3] = 256;
		rpmsg_data[5] =  rpmsg_data[3] / 16;
	}
	isp_rpmsg_ept_send(hw_isp->ept, rpmsg_data, 6*4);
#else //#elif defined CONFIG_ISP_FAST_CONVERGENCE || defined CONFIG_ISP_HARD_LIGHTADC
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);
	unsigned int rpmsg_data[6];
	ae_result_t *ae_result = NULL;

	memset(rpmsg_data, 0, 6 * sizeof(unsigned int));
	ae_result = &ctx->ae_entity_ctx.ae_result;
	rpmsg_data[0] = ISP_SET_SAVE_AE;
	rpmsg_data[1] = ae_result->fast_convergence_ev_idx;
	rpmsg_data[2] = ae_result->fast_convergence_gain >> 4;
	rpmsg_data[3] = ae_result->fast_convergence_exp;
	rpmsg_data[4] = 16;
	rpmsg_data[5] = 16;
	isp_rpmsg_ept_send(hw_isp->ept, rpmsg_data, 6*4);
#endif
}

int isp_rpmsg_ir_status_send(struct hw_isp_device *hw_isp)
{
	struct isp_lib_context *isp_gen = isp_dev_get_ctx(hw_isp);
	unsigned int rpmsg_data[2];
	if (isp_gen == NULL)
		return -1;

	rpmsg_data[0] = ISP_SET_IR_STATUS;
	rpmsg_data[1] = isp_gen->isp_ir_flag;

	isp_rpmsg_ept_send(hw_isp->ept, rpmsg_data, 2*4);
	return 0;
}

int isp_rpmsg_encpp_send(struct hw_isp_device *hw_isp)
{
	struct isp_lib_context *isp_gen = isp_dev_get_ctx(hw_isp);
	unsigned short rpmsg_data[209];
	unsigned int *rpmsg_head = (unsigned int *)&rpmsg_data[0];
	unsigned short *ptr = NULL;
	int i;
	if (isp_gen == NULL)
		return -1;

	rpmsg_head[0] = ISP_SET_ENCPP_DATA;
	rpmsg_data[2] = isp_gen->isp_ini_cfg.isp_test_settings.encpp_en;
	rpmsg_data[3] = isp_gen->encpp_static_sharp_cfg.ss_shp_ratio;
	rpmsg_data[4] = isp_gen->encpp_static_sharp_cfg.ls_shp_ratio;
	rpmsg_data[5] = isp_gen->encpp_static_sharp_cfg.ss_dir_ratio;
	rpmsg_data[6] = isp_gen->encpp_static_sharp_cfg.ls_dir_ratio;
	rpmsg_data[7] = isp_gen->encpp_static_sharp_cfg.ss_crc_stren;
	rpmsg_data[8] = isp_gen->encpp_static_sharp_cfg.ss_crc_min;
	rpmsg_data[9] = isp_gen->encpp_static_sharp_cfg.wht_sat_ratio;
	rpmsg_data[10] = isp_gen->encpp_static_sharp_cfg.blk_sat_ratio;
	rpmsg_data[11] = isp_gen->encpp_static_sharp_cfg.wht_slp_bt;
	rpmsg_data[12] = isp_gen->encpp_static_sharp_cfg.blk_slp_bt;
	ptr = &rpmsg_data[13];
	for (i = 0; i < ISP_REG_TBL_LENGTH; i++, ptr++) {
		*ptr = isp_gen->encpp_static_sharp_cfg.sharp_ss_value[i];
	}
	ptr = &rpmsg_data[46];
	for (i = 0; i < ISP_REG_TBL_LENGTH; i++, ptr++) {
		*ptr = isp_gen->encpp_static_sharp_cfg.sharp_ls_value[i];
	}
	ptr = &rpmsg_data[79];
	for (i = 0; i < 46; i++, ptr++) {
		*ptr = isp_gen->encpp_static_sharp_cfg.sharp_hsv[i];
	}
	rpmsg_data[125] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.ss_ns_lw;
	rpmsg_data[126] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.ss_ns_hi;
	rpmsg_data[127] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.ls_ns_lw;
	rpmsg_data[128] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.ls_ns_hi;
	rpmsg_data[129] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.ss_lw_cor;
	rpmsg_data[130] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.ss_hi_cor;
	rpmsg_data[131] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.ls_lw_cor;
	rpmsg_data[132] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.ls_hi_cor;
	rpmsg_data[133] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.ss_blk_stren;
	rpmsg_data[134] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.ss_wht_stren;
	rpmsg_data[135] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.ls_blk_stren;
	rpmsg_data[136] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.ls_wht_stren;
	rpmsg_data[137] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.ss_avg_smth;
	rpmsg_data[138] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.ss_dir_smth;
	ptr = &rpmsg_data[139];
	for (i = 0; i < 4; i++, ptr++) {
		*ptr = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.dir_smth[i];
	}
	rpmsg_data[143] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.hfr_smth_ratio;
	rpmsg_data[144] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.hfr_hf_wht_stren;
	rpmsg_data[145] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.hfr_hf_blk_stren;
	rpmsg_data[146] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.hfr_mf_wht_stren;
	rpmsg_data[147] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.hfr_mf_blk_stren;
	rpmsg_data[148] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.hfr_hf_cor_ratio;
	rpmsg_data[149] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.hfr_mf_cor_ratio;
	rpmsg_data[150] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.hfr_hf_mix_ratio;
	rpmsg_data[151] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.hfr_mf_mix_ratio;
	rpmsg_data[152] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.hfr_hf_mix_min_ratio;
	rpmsg_data[153] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.hfr_mf_mix_min_ratio;
	rpmsg_data[154] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.hfr_hf_wht_clp;
	rpmsg_data[155] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.hfr_hf_blk_clp;
	rpmsg_data[156] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.hfr_mf_wht_clp;
	rpmsg_data[157] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.hfr_mf_blk_clp;
	rpmsg_data[158] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.wht_clp_para;
	rpmsg_data[159] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.blk_clp_para;
	rpmsg_data[160] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.wht_clp_slp;
	rpmsg_data[161] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.blk_clp_slp;
	rpmsg_data[162] = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.max_clp_ratio;
	ptr = &rpmsg_data[163];
	for (i = 0; i < ISP_REG_TBL_LENGTH; i++, ptr++) {
		*ptr = isp_gen->iso_entity_ctx.iso_result.encpp_dynamic_sharp_cfg.sharp_edge_lum[i];
	}
	rpmsg_data[196] = isp_gen->iso_entity_ctx.iso_result.encoder_3dnr_cfg.enable_3d_fliter;
	rpmsg_data[197] = isp_gen->iso_entity_ctx.iso_result.encoder_3dnr_cfg.adjust_pix_level_enable;
	rpmsg_data[198] = isp_gen->iso_entity_ctx.iso_result.encoder_3dnr_cfg.smooth_filter_enable;
	rpmsg_data[199] = isp_gen->iso_entity_ctx.iso_result.encoder_3dnr_cfg.max_pix_diff_th;
	rpmsg_data[200] = isp_gen->iso_entity_ctx.iso_result.encoder_3dnr_cfg.max_mad_th;
	rpmsg_data[201] = isp_gen->iso_entity_ctx.iso_result.encoder_3dnr_cfg.max_mv_th;
	rpmsg_data[202] = isp_gen->iso_entity_ctx.iso_result.encoder_3dnr_cfg.min_coef;
	rpmsg_data[203] = isp_gen->iso_entity_ctx.iso_result.encoder_3dnr_cfg.max_coef;
	rpmsg_data[204] = isp_gen->iso_entity_ctx.iso_result.encoder_2dnr_cfg.enable_2d_fliter;
	rpmsg_data[205] = isp_gen->iso_entity_ctx.iso_result.encoder_2dnr_cfg.filter_strength_uv;
	rpmsg_data[206] = isp_gen->iso_entity_ctx.iso_result.encoder_2dnr_cfg.filter_strength_y;
	rpmsg_data[207] = isp_gen->iso_entity_ctx.iso_result.encoder_2dnr_cfg.filter_th_uv;
	rpmsg_data[208] = isp_gen->iso_entity_ctx.iso_result.encoder_2dnr_cfg.filter_th_y;

	isp_rpmsg_ept_send(hw_isp->ept, rpmsg_data, sizeof(rpmsg_data));
	return 0;
}

void isp_get_ae_ev(struct hw_isp_device *hw_isp)
{
#ifdef CONFIG_ISP_READ_THRESHOLD
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);
	ae_result_t *ae_result = NULL;

	ae_result = &ctx->ae_entity_ctx.ae_result;
	if (hw_isp->id == 0)
		ae_result->sensor_set.ev_set_curr.ev_idx =  clamp(*((unsigned int *)ISP0_NORFLASH_SAVE), 0, ae_result->sensor_set.ev_idx_max);
	else
		ae_result->sensor_set.ev_set_curr.ev_idx =  clamp(*((unsigned int *)ISP1_NORFLASH_SAVE), 0, ae_result->sensor_set.ev_idx_max);
	ae_result->sensor_set.ev_set_last.ev_idx =  ae_result->sensor_set.ev_set_curr.ev_idx;
	ae_result->sensor_set.ev_set.ev_idx = ae_result->sensor_set.ev_set_curr.ev_idx;
	ctx->ae_frame_cnt  = 0;
	ISP_PRINT("get ae idx is %d\n", ae_result->sensor_set.ev_set_curr.ev_idx);
#endif
}

void isp_get_ir_threshold(struct hw_isp_device *hw_isp)
{
#ifdef CONFIG_ISP_FAST_CONVERGENCE
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);
	struct tagsensor_isp_config_s *sensor_isp_cfg = NULL;
	ae_result_t *ae_result = NULL;

	ae_result = &ctx->ae_entity_ctx.ae_result;
	ae_result->ir_status_output = 0;
	/*read from ddr*/
	if (hw_isp->id == 0)
		sensor_isp_cfg = (struct tagsensor_isp_config_s *)VIN_SENSOR0_RESERVE_ADDR;
	else
		sensor_isp_cfg = (struct tagsensor_isp_config_s *)VIN_SENSOR1_RESERVE_ADDR;
	if ((hw_isp->id == 0 && sensor_isp_cfg->sign == 0xAA66AA66) ||
		(hw_isp->id != 0 && sensor_isp_cfg->sign == 0xBB66BB66)) {
		//if (ctx->sensor_info.wdr_mode) {
		//	if (sensor_isp_cfg->lv_hdr_def) {
		//		ae_result->day_to_ir_night_th = sensor_isp_cfg->lv_hdr_def;
		//	} else {
		//		ISP_WARN("lv_hdr_def is fault, use default\n");
		//		ae_result->day_to_ir_night_th = 200;
		//	}
		//} else {
			if (sensor_isp_cfg->lv_liner_def) {
				ae_result->day_to_ir_night_th = sensor_isp_cfg->lv_liner_def;
			} else {
				ISP_WARN("lv_liner_def is fault, use default\n");
				ae_result->day_to_ir_night_th = 200;
			}
		//}
	} else {
		ISP_WARN("sensor%d reserve addr sign error\n", hw_isp->id);
		ae_result->day_to_ir_night_th = 200;
	}
	ISP_PRINT("high frame rate day_to_ir_th = %d\n", ae_result->day_to_ir_night_th);
	ae_result->ir_night_to_day_th = 450;
	ae_result->ir_awb_gain_th = -210;
#endif
}

void isp_get_ir_mode(struct hw_isp_device *hw_isp, int *ir_mode)
{
#ifdef CONFIG_ISP_HARD_LIGHTADC
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);
	struct tagsensor_isp_config_s *sensor_isp_cfg = NULL;
	ae_result_t *ae_result = NULL;

	/*read from ddr*/
	if (hw_isp->id == 0)
		sensor_isp_cfg = (struct tagsensor_isp_config_s *)VIN_SENSOR0_RESERVE_ADDR;
	else
		sensor_isp_cfg = (struct tagsensor_isp_config_s *)VIN_SENSOR1_RESERVE_ADDR;
	if ((hw_isp->id == 0 && sensor_isp_cfg->sign == 0xAA66AA66) ||
		(hw_isp->id != 0 && sensor_isp_cfg->sign == 0xBB66BB66)) {
		*ir_mode = sensor_isp_cfg->ircut_state == 2 ? 2 : 0;
	} else {
		ISP_WARN("sensor%d reserve addr sign error\n", hw_isp->id);
		*ir_mode = 0;
	}
	ISP_PRINT("hard light_adc set isp ir mdoe is %s\n", *ir_mode == 2 ? "night_mode" : "day_mode");
#endif
}

void isp_set_ir_hold(struct hw_isp_device *hw_isp)
{
#ifdef CONFIG_ISP_FAST_CONVERGENCE
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);
	struct tagsensor_isp_config_s *sensor_isp_cfg = NULL;

	/*write to ddr*/
	if (hw_isp->id == 0)
		sensor_isp_cfg = (struct tagsensor_isp_config_s *)VIN_SENSOR0_RESERVE_ADDR;
	else
		sensor_isp_cfg = (struct tagsensor_isp_config_s *)VIN_SENSOR1_RESERVE_ADDR;
	if ((hw_isp->id == 0 && sensor_isp_cfg->sign == 0xAA66AA66) ||
		(hw_isp->id != 0 && sensor_isp_cfg->sign == 0xBB66BB66)) {
		sensor_isp_cfg->ircut_state = ctx->isp_ir_flag + 1; /* 1 mean day mode, 2 mean night mode, 0 mean melis no ctrl ircut */
		ISP_PRINT("high frame set ir hold = %d\n", sensor_isp_cfg->ircut_state);
	}
#endif
}

static void isp_update_encpp_cfg(struct hw_isp_device *hw_isp)
{
	struct isp_lib_context *isp_gen = isp_dev_get_ctx(hw_isp);
	isp_iso_entity_context_t *isp_iso_cxt = &isp_gen->iso_entity_ctx;

	isp_gen->iso_entity_ctx.iso_param->cnr_adjust = false;
	isp_gen->iso_entity_ctx.iso_param->sharpness_adjust = false;
	isp_gen->iso_entity_ctx.iso_param->brightness_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->contrast_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->cem_adjust = false;
	isp_gen->iso_entity_ctx.iso_param->denoise_adjust = false;
	isp_gen->iso_entity_ctx.iso_param->black_level_adjust = false;
	isp_gen->iso_entity_ctx.iso_param->dpc_adjust = false;
	isp_gen->iso_entity_ctx.iso_param->defog_value_adjust = false;
	isp_gen->iso_entity_ctx.iso_param->pltm_dynamic_cfg_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->tdnr_adjust = false;
	isp_gen->iso_entity_ctx.iso_param->ae_cfg_adjust = false;
	isp_gen->iso_entity_ctx.iso_param->gtm_cfg_adjust = false;
	isp_gen->iso_entity_ctx.iso_param->lca_cfg_adjust = false;
	isp_gen->iso_entity_ctx.iso_param->wdr_cfg_adjust = false;
	isp_gen->iso_entity_ctx.iso_param->cfa_adjust = false;

	isp_gen->temp_info.temperature_param[TEMP_COMP_2D_BLACK] = 256;
	isp_gen->temp_info.temperature_param[TEMP_COMP_2D_WHITE] = 256;
	isp_gen->temp_info.temperature_param[TEMP_COMP_3D_BLACK] = 256;
	isp_gen->temp_info.temperature_param[TEMP_COMP_3D_WHITE] = 256;
	isp_gen->temp_info.temperature_param[TEMP_COMP_DTC_STREN] = 256;
	isp_gen->temp_info.temperature_param[TEMP_COMP_BLC_R] = 0;
	isp_gen->temp_info.temperature_param[TEMP_COMP_BLC_G] = 0;
	isp_gen->temp_info.temperature_param[TEMP_COMP_BLC_B] = 0;
	isp_gen->temp_info.temperature_param[TEMP_COMP_SHARP] = 256;
	isp_gen->temp_info.temperature_param[TEMP_COMP_SATU_LOW] = 256;
	isp_gen->temp_info.temperature_param[TEMP_COMP_SATU_MID] = 256;
	isp_gen->temp_info.temperature_param[TEMP_COMP_SATU_HIGH] = 256;
	isp_iso_cxt->ops->isp_iso_run(isp_iso_cxt->iso_entity, &isp_iso_cxt->iso_result);

	isp_gen->iso_entity_ctx.iso_param->cnr_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->sharpness_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->brightness_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->contrast_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->cem_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->denoise_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->black_level_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->dpc_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->defog_value_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->pltm_dynamic_cfg_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->tdnr_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->ae_cfg_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->gtm_cfg_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->lca_cfg_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->wdr_cfg_adjust = true;
	isp_gen->iso_entity_ctx.iso_param->cfa_adjust = true;
}

static void isp_set_firstdrc_tbl(struct hw_isp_device *hw_isp)
{
	struct isp_lib_context *isp_gen = isp_dev_get_ctx(hw_isp);
	int contrast = isp_gen->adjust.contrast, brightness = isp_gen->adjust.brightness;
	int i, contrast_cal, brightness_cal, fir_len = 64, ctable_0;
	int *ctable = NULL, *blend_curve = NULL;
	if (isp_gen->isp_ini_cfg.isp_test_settings.gtm_en == 0 || (!contrast && !brightness))
		return;

	ctable = hal_malloc(321*sizeof(int));//avoid stack over 4kb
	if (!ctable) {
		ISP_ERR("ctable malloc fail!\n");
		goto err0;
	}
	blend_curve = hal_malloc(256*sizeof(int));//avoid stack over 4kb
	if (!blend_curve) {
		ISP_ERR("blend_curve malloc fail!\n");
		goto err1;
	}
	memset(ctable, 0, 321 * sizeof(int));
	memset(blend_curve, 0, 256 * sizeof(int));
	/*(contrast - 15) and (brightness + 98) make contrast and brightness not change when value is 0*/
	if (contrast < 0) {
		contrast_cal = (100 + (contrast / 2 - 15));
		brightness_cal = ((brightness + contrast / 2 - 30) + 128)*100;
	} else {
		contrast_cal = (100 + (contrast - 15));
		brightness_cal = ((brightness - 30) + 128)*100;
	}
	for (i = 0; i < 321; i++)//256+64
		ctable[i] = max(1, min(25500, ((i-128) * contrast_cal + brightness_cal)));

	for (i = 0; i < 256; i++)
		blend_curve[i] = ArraySum(&ctable[i], fir_len) >> 6;

	if (contrast < 0)
		ctable_0 = blend_curve[0] / 2;
	else
		ctable_0 = blend_curve[0];

	for (i = 1; i < 256; i++)
		blend_curve[i] = blend_curve[i] - ctable_0;

	if(blend_curve[255] == 0)
		blend_curve[255] = 1;

	for (i = 0; i < 256; i++) {
		blend_curve[i] = clamp((blend_curve[i] << 12)/blend_curve[255], 0, 4096);
	}

	for (i = 0; i < 256; i++) {
		if(i == 0 || blend_curve[i] == 0) {
			isp_gen->module_cfg.drc_cfg.drc_table[i] = 512;
		} else {
			isp_gen->module_cfg.drc_cfg.drc_table[i] = blend_curve[i] * 32 / i;
			if(isp_gen->module_cfg.drc_cfg.drc_table[i] > 4095)
				isp_gen->module_cfg.drc_cfg.drc_table[i] = 4095;
		}
		isp_gen->module_cfg.drc_cfg.drc_table_last[i] = isp_gen->module_cfg.drc_cfg.drc_table[i];
	}
	hal_free(blend_curve);
err1:
	hal_free(ctable);
err0:
	return;
}

static void isp_update_fix_param(struct hw_isp_device *hw_isp)
{
	struct isp_lib_context *isp_gen = isp_dev_get_ctx(hw_isp);
	struct isp_module_config *mod_cfg = &isp_gen->module_cfg;
	struct isp_param_config *param = &isp_gen->isp_ini_cfg;
	HW_U8 *tdnf_table = (HW_U8 *)mod_cfg->tdnf_table;
	int i, j;
	HW_U8 isp_tdnf_df_bd_tbl[ISP_TDNF_TBL_SIZE] = {
		255,   255,   255,   255,   255,   255,   255,   255,
		255,   255,   255,   255,   255,   255,   255,   255,
		255,   254,   254,   254,   254,   254,   254,   253,
		253,   253,   253,   253,   252,   252,   252,   252,
		252,   251,   251,   251,   250,   250,   250,   250,
		249,   249,   249,   248,   248,   248,   247,   247,
		247,   246,   246,   245,   245,   245,   244,   244,
		243,   243,   242,   242,   241,   241,   240,   240,
		240,   239,   238,   238,   237,   237,   236,   236,
		235,   235,   234,   234,   233,   232,   232,   231,
		231,   230,   229,   229,   228,   227,   227,   226,
		225,   225,   224,   223,   222,   222,   221,   220,
		220,   219,   218,   217,   216,   216,   215,   214,
		213,   212,   212,   211,   210,   209,   208,   207,
		207,   206,   205,   204,   203,   202,   201,   200,
		199,   198,   197,   196,   195,   194,   193,   192,
		192,   190,   189,   188,   187,   186,   185,   184,
		183,   182,   181,   180,   179,   178,   177,   176,
		175,   173,   172,   171,   170,   169,   168,   166,
		165,   164,   163,   162,   160,   159,   158,   157,
		156,   154,   153,   152,   150,   149,   148,   147,
		145,   144,   143,   141,   140,   139,   137,   136,
		135,   133,   132,   130,   129,   128,   126,   125,
		123,   122,   120,   119,   117,   116,   114,   113,
		112,   110,   108,   107,   105,   104,   102,   101,
		99,    98,    96,    95,    93,    91,    90,    88,
		87,    85,    83,    82,    80,    78,    77,    75,
		73,    72,    70,    68,    66,    65,    63,    61,
		60,    58,    56,    54,    52,    51,    49,    47,
		45,    43,    42,    40,    38,    36,    34,    32,
		31,    29,    27,    25,    23,    21,    19,    17,
		15,    13,    11,     9,     7,     5,     3,     1
	};

	//denoise
	mod_cfg->bdnf_cfg.lyr_core_ratio[0] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR0_CORE_RATIO];
	mod_cfg->bdnf_cfg.lyr_core_ratio[1] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR1_CORE_RATIO];
	mod_cfg->bdnf_cfg.lyr_core_ratio[2] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR2_CORE_RATIO];
	mod_cfg->bdnf_cfg.lyr_core_ratio[3] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR3_CORE_RATIO];
	mod_cfg->bdnf_cfg.lyr_side_ratio[0] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR0_SIDE_RATIO];
	mod_cfg->bdnf_cfg.lyr_side_ratio[1] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR1_SIDE_RATIO];
	mod_cfg->bdnf_cfg.lyr_side_ratio[2] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR2_SIDE_RATIO];
	mod_cfg->bdnf_cfg.lyr_side_ratio[3] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR3_SIDE_RATIO];
	mod_cfg->bdnf_cfg.lyr_dnr_cratio[0] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR0_DNR_CRATIO];
	mod_cfg->bdnf_cfg.lyr_dnr_cratio[1] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR1_DNR_CRATIO];
	mod_cfg->bdnf_cfg.lyr_dnr_cratio[2] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR2_DNR_CRATIO];
	mod_cfg->bdnf_cfg.lyr_dnr_cratio[3] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR3_DNR_CRATIO];
	mod_cfg->bdnf_cfg.lyr_gauss_type[0] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR0_GAUSS_TYPE];
	mod_cfg->bdnf_cfg.lyr_gauss_type[1] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR1_GAUSS_TYPE];
	mod_cfg->bdnf_cfg.lyr_gauss_type[2] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR2_GAUSS_TYPE];
	mod_cfg->bdnf_cfg.lyr_gauss_type[3] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR3_GAUSS_TYPE];
	mod_cfg->bdnf_cfg.lyr_gauss_wght[0] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR0_GAUSS_WGTH];
	mod_cfg->bdnf_cfg.lyr_gauss_wght[1] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR1_GAUSS_WGTH];
	mod_cfg->bdnf_cfg.lyr_gauss_wght[2] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR2_GAUSS_WGTH];
	mod_cfg->bdnf_cfg.lyr_gauss_wght[3] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_LYR3_GAUSS_WGTH];
	mod_cfg->bdnf_cfg.cnt_ratio[0] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_CNT_RATIO0];
	mod_cfg->bdnf_cfg.cnt_ratio[1] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_CNT_RATIO1];
	mod_cfg->bdnf_cfg.cnt_ratio[2] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_CNT_RATIO2];
	mod_cfg->bdnf_cfg.cnt_ratio[3] = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_CNT_RATIO3];
	mod_cfg->bdnf_cfg.wdr_lm_hi_slp = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_WDR_LM_HI_SLP];
	mod_cfg->bdnf_cfg.wdr_ms_hi_slp = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_WDR_MS_HI_SLP];
	mod_cfg->bdnf_cfg.wdr_lm_lw_slp = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_WDR_LM_LW_SLP];
	mod_cfg->bdnf_cfg.wdr_ms_lw_slp = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_WDR_MS_LW_SLP];
	mod_cfg->bdnf_cfg.wdr_lm_max_clp = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_WDR_LM_MAX_CLP];
	mod_cfg->bdnf_cfg.wdr_ms_max_clp = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_WDR_MS_MAX_CLP];
	mod_cfg->nr_cfg.d2d_msc_cmp_ratio = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_MSC_CMP_RATIO];
	mod_cfg->bdnf_cfg.dtc_mg_mode = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_DTC_MG_MODE];
	mod_cfg->bdnf_cfg.dtc_mg_clip = param->isp_tunning_settings.denoise_comm_cfg[ISP_DENOISE_DTC_MG_CLIP];

	//tdf
	if (param->isp_test_settings.tdf_en)
		mod_cfg->tdf_cfg.bypass_mode = 0;
	else
		mod_cfg->tdf_cfg.bypass_mode = 1;
	mod_cfg->tdf_cfg.flt_out_sel = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_FILT_OUT_SEL];
	mod_cfg->tdf_cfg.kb_wnum = isp_gen->sensor_info.sensor_width / 9;
	mod_cfg->tdf_cfg.kb_hnum = isp_gen->sensor_info.sensor_height / 9;
	mod_cfg->tdf_cfg.kst_wlen = isp_gen->sensor_info.sensor_width / 32;
	mod_cfg->tdf_cfg.kst_hlen = isp_gen->sensor_info.sensor_height / 24;
	mod_cfg->tdf_cfg.kst_pnum = mod_cfg->tdf_cfg.kst_wlen * mod_cfg->tdf_cfg.kst_hlen;
	mod_cfg->tdf_cfg.kst_wphs = ((mod_cfg->tdf_cfg.kst_wlen >> 1) << 12) / mod_cfg->tdf_cfg.kst_wlen;
	mod_cfg->tdf_cfg.kst_wstp = (1 << 12) / mod_cfg->tdf_cfg.kst_wlen;
	mod_cfg->tdf_cfg.kst_hphs = ((mod_cfg->tdf_cfg.kst_hlen >> 1) << 12) / mod_cfg->tdf_cfg.kst_hlen;
	mod_cfg->tdf_cfg.kst_hstp = (1 << 12) / mod_cfg->tdf_cfg.kst_hlen;
	mod_cfg->tdf_cfg.diff_intra_amp = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_DIFF_INTRA_AMP];
	mod_cfg->tdf_cfg.diff_inter_amp = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_DIFF_INTER_AMP];
	mod_cfg->tdf_cfg.thr_intra_amp = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_THR_INTRA_AMP];
	mod_cfg->tdf_cfg.thr_inter_amp = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_THR_INTER_AMP];
	mod_cfg->tdf_cfg.diff_min_clp = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_DIFF_MIN_CLP];
	mod_cfg->tdf_cfg.wdr_lm_hi_slp = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_WDR_LM_HI_SLP];
	mod_cfg->tdf_cfg.wdr_ms_hi_slp = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_WDR_MS_HI_SLP];
	mod_cfg->tdf_cfg.wdr_lm_lw_slp = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_WDR_LM_LW_SLP];
	mod_cfg->tdf_cfg.wdr_ms_lw_slp = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_WDR_MS_LW_SLP];
	mod_cfg->tdf_cfg.wdr_lm_max_clp = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_WDR_LM_MAX_CLP];
	mod_cfg->tdf_cfg.wdr_ms_max_clp = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_WDR_MS_MAX_CLP];
	mod_cfg->nr_cfg.d3d_msc_cmp_ratio = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_MSC_CMP_RATIO];
	mod_cfg->tdf_cfg.stl_stg_cth[0] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_STL_STG_CTH_0];
	mod_cfg->tdf_cfg.stl_stg_cth[1] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_STL_STG_CTH_1];
	mod_cfg->tdf_cfg.stl_stg_cth[2] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_STL_STG_CTH_2];
	mod_cfg->tdf_cfg.stl_stg_cth[3] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_STL_STG_CTH_3];
	mod_cfg->tdf_cfg.stl_stg_cth[4] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_STL_STG_CTH_4];
	mod_cfg->tdf_cfg.stl_stg_cth[5] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_STL_STG_CTH_5];
	mod_cfg->tdf_cfg.stl_stg_cth[6] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_STL_STG_CTH_6];
	mod_cfg->tdf_cfg.stl_stg_cth[7] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_STL_STG_CTH_7];
	mod_cfg->tdf_cfg.stl_stg_kth[0] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_STL_STG_KTH_0];
	mod_cfg->tdf_cfg.stl_stg_kth[1] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_STL_STG_KTH_1];
	mod_cfg->tdf_cfg.stl_stg_kth[2] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_STL_STG_KTH_2];
	mod_cfg->tdf_cfg.stl_stg_kth[3] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_STL_STG_KTH_3];
	mod_cfg->tdf_cfg.stl_stg_kth[4] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_STL_STG_KTH_4];
	mod_cfg->tdf_cfg.stl_stg_kth[5] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_STL_STG_KTH_5];
	mod_cfg->tdf_cfg.stl_stg_kth[6] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_STL_STG_KTH_6];
	mod_cfg->tdf_cfg.stl_stg_kth[7] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_STL_STG_KTH_7];
	mod_cfg->tdf_cfg.mot_stg_cth[0] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_MOT_STG_CTH_0];
	mod_cfg->tdf_cfg.mot_stg_cth[1] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_MOT_STG_CTH_1];
	mod_cfg->tdf_cfg.mot_stg_cth[2] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_MOT_STG_CTH_2];
	mod_cfg->tdf_cfg.mot_stg_cth[3] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_MOT_STG_CTH_3];
	mod_cfg->tdf_cfg.mot_stg_cth[4] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_MOT_STG_CTH_4];
	mod_cfg->tdf_cfg.mot_stg_cth[5] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_MOT_STG_CTH_5];
	mod_cfg->tdf_cfg.mot_stg_cth[6] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_MOT_STG_CTH_6];
	mod_cfg->tdf_cfg.mot_stg_cth[7] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_MOT_STG_CTH_7];
	mod_cfg->tdf_cfg.mot_stg_kth[0] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_MOT_STG_KTH_0];
	mod_cfg->tdf_cfg.mot_stg_kth[1] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_MOT_STG_KTH_1];
	mod_cfg->tdf_cfg.mot_stg_kth[2] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_MOT_STG_KTH_2];
	mod_cfg->tdf_cfg.mot_stg_kth[3] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_MOT_STG_KTH_3];
	mod_cfg->tdf_cfg.mot_stg_kth[4] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_MOT_STG_KTH_4];
	mod_cfg->tdf_cfg.mot_stg_kth[5] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_MOT_STG_KTH_5];
	mod_cfg->tdf_cfg.mot_stg_kth[6] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_MOT_STG_KTH_6];
	mod_cfg->tdf_cfg.mot_stg_kth[7] = param->isp_tunning_settings.tdf_comm_cfg[ISP_TDF_MOT_STG_KTH_7];
	for (i = 0; i < ISP_TDNF_TBL_SIZE; i++) {
		tdnf_table[8*i + 0] = isp_tdnf_df_bd_tbl[i];
		//tdnf_table[8*i + 1] = isp_gen->isp_ini_cfg.isp_tunning_settings.isp_tdnf_kdelta[i];
	}
	memcpy(&mod_cfg->tdf_cfg.df_shp_bk[0], &isp_gen->isp_ini_cfg.isp_tunning_settings.isp_tdnf_df_shape[0], ISP_REG1_TBL_LENGTH);
	memcpy(&mod_cfg->tdf_cfg.r_amp_bk[0], &isp_gen->isp_ini_cfg.isp_tunning_settings.isp_tdnf_ratio_amp[0], ISP_REG1_TBL_LENGTH);
	memcpy(&mod_cfg->tdf_cfg.k_dlt_bk[0], &isp_gen->isp_ini_cfg.isp_tunning_settings.isp_tdnf_k_dlt_bk[0], ISP_REG1_TBL_LENGTH);
	memcpy(&mod_cfg->tdf_cfg.ct_rt_bk[0], &isp_gen->isp_ini_cfg.isp_tunning_settings.isp_tdnf_ct_rt_bk[0], ISP_REG1_TBL_LENGTH);
	memcpy(&mod_cfg->tdf_cfg.lay0_d2d0_rt_br[0], &isp_gen->isp_ini_cfg.isp_tunning_settings.isp_tdnf_lay0_d2d0_rt_br[0], ISP_REG1_TBL_LENGTH);
	memcpy(&mod_cfg->tdf_cfg.lay1_d2d0_rt_br[0], &isp_gen->isp_ini_cfg.isp_tunning_settings.isp_tdnf_lay1_d2d0_rt_br[0], ISP_REG1_TBL_LENGTH);
	//CFA
	mod_cfg->cfa_cfg.cfa_grad_th = param->isp_tunning_settings.grad_th;
	mod_cfg->cfa_cfg.cfa_dir_v_th = param->isp_tunning_settings.dir_v_th;
	mod_cfg->cfa_cfg.cfa_dir_h_th = param->isp_tunning_settings.dir_h_th;
	mod_cfg->cfa_cfg.res_smth_high = param->isp_tunning_settings.res_smth_high;
	mod_cfg->cfa_cfg.res_smth_low = param->isp_tunning_settings.res_smth_low;
	//mod_cfg->cfa_cfg.afc_cnr_stren = 0x0;
	//mod_cfg->cfa_cfg.afc_rgb_stren = 0x0;
	//mod_cfg->cfa_cfg.afc_bayer_stren = 0x0;
	mod_cfg->cfa_cfg.res_high_th = param->isp_tunning_settings.res_high_th;
	mod_cfg->cfa_cfg.res_low_th = param->isp_tunning_settings.res_low_th;
	mod_cfg->cfa_cfg.res_dir_a = param->isp_tunning_settings.res_dir_a;
	mod_cfg->cfa_cfg.res_dir_d = param->isp_tunning_settings.res_dir_d;
	mod_cfg->cfa_cfg.res_dir_v = param->isp_tunning_settings.res_dir_v;
	mod_cfg->cfa_cfg.res_dir_h = param->isp_tunning_settings.res_dir_h;

	//ctc
	mod_cfg->ctc_cfg.ctc_th_max = param->isp_tunning_settings.ctc_th_max;//316;
	mod_cfg->ctc_cfg.ctc_th_min = param->isp_tunning_settings.ctc_th_min;//60;
	mod_cfg->ctc_cfg.ctc_th_slope = param->isp_tunning_settings.ctc_th_slope;//262;
	mod_cfg->ctc_cfg.ctc_dir_wt = param->isp_tunning_settings.ctc_dir_wt;//64;
	mod_cfg->ctc_cfg.ctc_dir_th = param->isp_tunning_settings.ctc_dir_th;//80;

	//pltm
	mod_cfg->pltm_cfg.gtm_en = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_GTM_EN];
	mod_cfg->pltm_cfg.lft_en = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_LTF_EN];
	mod_cfg->pltm_cfg.dsc_en = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_DSC_EN];
	mod_cfg->pltm_cfg.dcc_en = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_DCC_EN];
	mod_cfg->pltm_cfg.dsc_shf_bit = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_DSC_SHF_BIT];
	mod_cfg->pltm_cfg.dsc_lw_th = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_DSC_LW_TH];
	mod_cfg->pltm_cfg.dsc_hi_th = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_DSC_HI_TH];
	mod_cfg->pltm_cfg.dsc_lw_ratio = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_DSC_LW_RT];
	mod_cfg->pltm_cfg.dcc_shf_bit = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_DCC_SHF_BIT];
	mod_cfg->pltm_cfg.dcc_lw_th = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_DCC_LW_TH];
	mod_cfg->pltm_cfg.dcc_hi_th = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_DCC_HI_TH];
	mod_cfg->pltm_cfg.mgc_int_smth[0] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_MGC_INT_SMTH0];
	mod_cfg->pltm_cfg.mgc_int_smth[1] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_MGC_INT_SMTH1];
	mod_cfg->pltm_cfg.mgc_int_smth[2] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_MGC_INT_SMTH2];
	mod_cfg->pltm_cfg.mgc_int_smth[3] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_MGC_INT_SMTH3];
	mod_cfg->pltm_cfg.mgc_lum_adpt[0] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_MGC_LUM_ADPT0];
	mod_cfg->pltm_cfg.mgc_lum_adpt[1] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_MGC_LUM_ADPT1];
	mod_cfg->pltm_cfg.mgc_lum_adpt[2] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_MGC_LUM_ADPT2];
	mod_cfg->pltm_cfg.mgc_lum_adpt[3] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_MGC_LUM_ADPT3];
	mod_cfg->pltm_cfg.sts_ceil_slp[0] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_STS_CEIL_SLP0];
	mod_cfg->pltm_cfg.sts_ceil_slp[1] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_STS_CEIL_SLP1];
	mod_cfg->pltm_cfg.sts_ceil_slp[2] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_STS_CEIL_SLP2];
	mod_cfg->pltm_cfg.sts_ceil_slp[3] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_STS_CEIL_SLP3];
	mod_cfg->pltm_cfg.sts_floor_slp[0] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_STS_FLOOR_SLP0];
	mod_cfg->pltm_cfg.sts_floor_slp[1] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_STS_FLOOR_SLP1];
	mod_cfg->pltm_cfg.sts_floor_slp[2] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_STS_FLOOR_SLP2];
	mod_cfg->pltm_cfg.sts_floor_slp[3] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_STS_FLOOR_SLP3];
	mod_cfg->pltm_cfg.sts_gd_ratio[0] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_STS_GD_RT0];
	mod_cfg->pltm_cfg.sts_gd_ratio[1] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_STS_GD_RT1];
	mod_cfg->pltm_cfg.sts_gd_ratio[2] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_STS_GD_RT2];
	mod_cfg->pltm_cfg.sts_gd_ratio[3] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_STS_GD_RT3];
	mod_cfg->pltm_cfg.adjk_crct_ratio[0] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_ADJK_CRCT_RT0];
	mod_cfg->pltm_cfg.adjk_crct_ratio[1] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_ADJK_CRCT_RT1];
	mod_cfg->pltm_cfg.adjk_crct_ratio[2] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_ADJK_CRCT_RT2];
	mod_cfg->pltm_cfg.adjk_crct_ratio[3] = param->isp_tunning_settings.pltm_cfg[ISP_PLTM_ADJK_CRCT_RT3];
	for (i = 0; i < PLTM_MAX_STAGE; i++) {
		mod_cfg->pltm_cfg.sts_flt_stren[i][0] = 384;//384
		mod_cfg->pltm_cfg.sts_flt_stren[i][1] = 256;//256
		mod_cfg->pltm_cfg.sts_flt_stren[i][2] = 64;//128

		mod_cfg->pltm_cfg.adj1_asym_ratio[i] = 0;
	}
	for (i = 0; i < 128; i++) {
		mod_cfg->pltm_cfg.pltm_table[ISP_PLTM_TM_TBL_SIZE + 4*i + 0] = param->isp_tunning_settings.isp_pltm_lum_map_cv[0][i];
		mod_cfg->pltm_cfg.pltm_table[ISP_PLTM_TM_TBL_SIZE + 4*i + 1] = param->isp_tunning_settings.isp_pltm_lum_map_cv[1][i];
		mod_cfg->pltm_cfg.pltm_table[ISP_PLTM_TM_TBL_SIZE + 4*i + 2] = param->isp_tunning_settings.isp_pltm_lum_map_cv[2][i];
		mod_cfg->pltm_cfg.pltm_table[ISP_PLTM_TM_TBL_SIZE + 4*i + 3] = param->isp_tunning_settings.isp_pltm_lum_map_cv[3][i];
	}
	for (i = 0; i < 15; i++) {
		for (j = 0; j < PLTM_MAX_STAGE; j++) {
			mod_cfg->pltm_cfg.stat_gd_cv[i*PLTM_MAX_STAGE+j] = param->isp_tunning_settings.isp_pltm_stat_gd_cv[j][i];
		}
	}
	for (i = 0; i < ISP_REG_TBL_LENGTH; i++) {
		for (j = 0; j < PLTM_MAX_STAGE; j++) {
			mod_cfg->pltm_cfg.adj_k_df_cv[i*PLTM_MAX_STAGE+j] = param->isp_tunning_settings.isp_pltm_df_cv[j][i];
		}
	}

	// gca
	mod_cfg->gca_cfg.gca_ct_w = param->isp_tunning_settings.gca_cfg[ISP_GCA_CT_W];
	mod_cfg->gca_cfg.gca_ct_h = param->isp_tunning_settings.gca_cfg[ISP_GCA_CT_H];

	mod_cfg->gca_cfg.gca_r_para0 = param->isp_tunning_settings.gca_cfg[ISP_GCA_R_PARA0];
	mod_cfg->gca_cfg.gca_r_para1 = param->isp_tunning_settings.gca_cfg[ISP_GCA_R_PARA1];
	mod_cfg->gca_cfg.gca_r_para2 = param->isp_tunning_settings.gca_cfg[ISP_GCA_R_PARA2];
	mod_cfg->gca_cfg.gca_b_para0 = param->isp_tunning_settings.gca_cfg[ISP_GCA_B_PARA0];
	mod_cfg->gca_cfg.gca_b_para1 = param->isp_tunning_settings.gca_cfg[ISP_GCA_B_PARA1];
	mod_cfg->gca_cfg.gca_b_para2 = param->isp_tunning_settings.gca_cfg[ISP_GCA_B_PARA2];
	mod_cfg->gca_cfg.gca_int_cns = param->isp_tunning_settings.gca_cfg[ISP_GCA_INT_CNS];

	/*sharp*/
	mod_cfg->sharp_cfg.ss_shp_ratio = param->isp_tunning_settings.sharp_comm_cfg[ISP_SHARP_SS_SHP_RATIO];
	mod_cfg->sharp_cfg.ls_shp_ratio = param->isp_tunning_settings.sharp_comm_cfg[ISP_SHARP_LS_SHP_RATIO];
	mod_cfg->sharp_cfg.ss_dir_ratio = param->isp_tunning_settings.sharp_comm_cfg[ISP_SHARP_SS_DIR_RATIO];
	mod_cfg->sharp_cfg.ls_dir_ratio = param->isp_tunning_settings.sharp_comm_cfg[ISP_SHARP_LS_DIR_RATIO];
	mod_cfg->sharp_cfg.ss_crc_stren = param->isp_tunning_settings.sharp_comm_cfg[ISP_SHARP_SS_CRC_STREN];
	mod_cfg->sharp_cfg.ss_crc_min = param->isp_tunning_settings.sharp_comm_cfg[ISP_SHARP_SS_CRC_MIN];
	mod_cfg->sharp_cfg.wht_sat_ratio = param->isp_tunning_settings.sharp_comm_cfg[ISP_SHARP_WHT_SAT_RATIO];
	mod_cfg->sharp_cfg.blk_sat_ratio = param->isp_tunning_settings.sharp_comm_cfg[ISP_SHARP_BLK_SAT_RATIO];
	mod_cfg->sharp_cfg.wht_slp_bt = param->isp_tunning_settings.sharp_comm_cfg[ISP_SHARP_WHT_SLP_BT];
	mod_cfg->sharp_cfg.blk_slp_bt = param->isp_tunning_settings.sharp_comm_cfg[ISP_SHARP_BLK_SLP_BT];
	memcpy(&mod_cfg->sharp_cfg.sharp_ss_value[0], &param->isp_tunning_settings.isp_sharp_ss_value[0],
		ISP_REG_TBL_LENGTH * sizeof(HW_U16));
	memcpy(&mod_cfg->sharp_cfg.sharp_ls_value[0], &param->isp_tunning_settings.isp_sharp_ls_value[0],
		ISP_REG_TBL_LENGTH * sizeof(HW_U16));
	memcpy(&mod_cfg->sharp_cfg.sharp_hsv[0], &param->isp_tunning_settings.isp_sharp_hsv[0],
		46 * sizeof(HW_U16));
}

static int isp_alloc_rp_reg_tbl(struct hw_isp_device *hw_isp)
{
	struct isp_lib_context *isp_gen = isp_dev_get_ctx(hw_isp);

	/* REG LOAD*/
	isp_gen->load_reg_base = rpbuf_buffer_va(hw_isp->load_buffer);
	if (isp_gen->load_reg_base == NULL) {
		ISP_ERR("load_reg_base alloc failed, no memory!");
		return -1;
	}
	memset(isp_gen->load_reg_base, 0, ISP_LOAD_REG_SIZE);
	memcpy(isp_gen->load_reg_base, &isp_default_reg[0], ISP_LOAD_REG_SIZE);

	isp_gen->module_cfg.isp_dev_id = isp_gen->isp_id;
	//isp_gen->module_cfg.isp_platform_id = ISP_PLATFORM_SUN8IW21P1;

	isp_map_addr(&isp_gen->module_cfg, (unsigned long)isp_gen->load_reg_base);
#if (ISP_VERSION >= 600)
	/*FE TABLE*/
	isp_gen->module_cfg.fe_table = isp_gen->load_reg_base + ISP_LOAD_REG_SIZE;
	memset(isp_gen->module_cfg.fe_table, 0, ISP_FE_TABLE_SIZE);

	isp_gen->module_cfg.ch0_msc_table = isp_gen->module_cfg.fe_table + ISP_CH0_MSC_FE_MEM_OFS;
	isp_gen->module_cfg.ch1_msc_table = isp_gen->module_cfg.fe_table + ISP_CH1_MSC_FE_MEM_OFS;
	isp_gen->module_cfg.ch2_msc_table = isp_gen->module_cfg.fe_table + ISP_CH2_MSC_FE_MEM_OFS;

	/*BAYER TABLE*/
	isp_gen->module_cfg.bayer_table = isp_gen->module_cfg.fe_table + ISP_FE_TABLE_SIZE;
	memset(isp_gen->module_cfg.bayer_table, 0, ISP_BAYER_TABLE_SIZE);

	isp_gen->module_cfg.lens_table = isp_gen->module_cfg.bayer_table + ISP_LSC_MEM_OFS;
	isp_gen->module_cfg.msc_table = isp_gen->module_cfg.bayer_table + ISP_MSC_MEM_OFS;
	isp_gen->module_cfg.nr_msc_table = isp_gen->module_cfg.bayer_table + ISP_MSC_NR_MEM_OFS;
	isp_gen->module_cfg.tdnf_table = isp_gen->module_cfg.bayer_table + ISP_TDNF_DK_MEM_OFS;
	isp_gen->module_cfg.pltm_table = isp_gen->module_cfg.bayer_table + ISP_PLTM_TM_MEM_OFS;

	/*RGB TABLE*/
	isp_gen->module_cfg.rgb_table = isp_gen->module_cfg.bayer_table + ISP_BAYER_TABLE_SIZE;
	memset(isp_gen->module_cfg.rgb_table, 0, ISP_RGB_TABLE_SIZE);

	isp_gen->module_cfg.drc_table = isp_gen->module_cfg.rgb_table + ISP_DRC_MEM_OFS;
	isp_gen->module_cfg.gamma_table = isp_gen->module_cfg.rgb_table + ISP_GAMMA_MEM_OFS;

	/*YUV TABLE*/
	isp_gen->module_cfg.yuv_table = isp_gen->module_cfg.rgb_table + ISP_RGB_TABLE_SIZE;
	memset(isp_gen->module_cfg.yuv_table, 0, ISP_YUV_TABLE_SIZE);

	isp_gen->module_cfg.cem_table = isp_gen->module_cfg.yuv_table + ISP_CEM_MEM_OFS;
#endif
	return 0;
}

static int isp_config_sensor_info(struct hw_isp_device *hw_isp, void *data)
{
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);
	unsigned int *sensor_data = data;
	char *sensor_name;
	int i;

	if (ctx == NULL)
		return -1;

	ISP_LIB_LOG(ISP_LOG_RP, "isp get sensor info from linux\n");

	ctx->sensor_info.sensor_width = sensor_data[1];
	ctx->sensor_info.sensor_height = sensor_data[2];
	ctx->sensor_info.fps_fixed = sensor_data[3];
	ctx->sensor_info.wdr_mode = sensor_data[4];
	// BT601_FULL_RANGE
	ctx->sensor_info.color_space = sensor_data[5];

	ctx->sensor_info.bayer_bit = sensor_data[6];
	ctx->sensor_info.input_seq = sensor_data[7];

	ctx->sensor_info.hts = sensor_data[8];
	ctx->sensor_info.vts = sensor_data[9];
	ctx->sensor_info.pclk = sensor_data[10];
	ctx->sensor_info.bin_factor = sensor_data[11];
	ctx->sensor_info.gain_min = sensor_data[12];
	ctx->sensor_info.gain_max = sensor_data[13];
	ctx->sensor_info.hoffset = sensor_data[14];
	ctx->sensor_info.voffset = sensor_data[15];

	sensor_name = (char *)&sensor_data[16];
	memcpy(hw_isp->sensor_name, sensor_name, 4 * sizeof(unsigned int));
	ctx->sensor_info.name = hw_isp->sensor_name;

	ctx->stat.pic_size.width = ctx->sensor_info.sensor_width;
	ctx->stat.pic_size.height = ctx->sensor_info.sensor_height;

	ctx->stats_ctx.pic_w = ctx->sensor_info.sensor_width;
	ctx->stats_ctx.pic_h = ctx->sensor_info.sensor_height;
#if 1
	// update otp infomation
	if (ctx->otp_enable == -1) {
		ISP_PRINT("otp disabled, msc use 1024\n");
		for (i = 0; i < 16*16*3; i++) {
			((unsigned short *)(ctx->pmsc_table))[i] = 1024;
		}
	}
#endif
	ctx->sensor_info_init = 1;
	return 0;
}

/* isp rpbuf */
extern void sunxi_isp_send_save_load(unsigned int id, void *destin_addr);
static int isp_save_load_rpbuf_send(struct hw_isp_device *hw_isp)
{
	int ret;
	const char *buf_name;
	void *buf_va;
	int buf_len, offset;
	int data_len = ISP_SAVE_LOAD_STATISTIC_SIZE;

	if (!hw_isp->load_buffer) {
		ISP_ERR("save rpbuf is null\n");
		return -ENOENT;
	}

	buf_name = rpbuf_buffer_name(hw_isp->load_buffer);
	buf_va = rpbuf_buffer_va(hw_isp->load_buffer);
	buf_len = rpbuf_buffer_len(hw_isp->load_buffer);

	if (buf_len < data_len) {
		ISP_ERR("%s: data size(0x%x) out of buffer length(0x%x)\n",
			buf_name, data_len, buf_len);
		return -EINVAL;
	}

	if (!rpbuf_buffer_is_available(hw_isp->load_buffer)) {
		ISP_ERR("%s not available\n", buf_name);
		return -EACCES;
	}

	offset = 0;
	sunxi_isp_send_save_load(hw_isp->id, buf_va);
	ret = rpbuf_transmit_buffer(hw_isp->load_buffer, offset, data_len);
	if (ret < 0) {
		ISP_ERR("%s: rpbuf_transmit_buffer failed\n", buf_name);
		return ret;
	}

	ISP_LIB_LOG(ISP_LOG_RP, "save_load buffer send to linux!\n");

	return data_len;
}

static int isp_load_rpbuf_send(struct hw_isp_device *hw_isp)
{
	int ret;
	const char *buf_name;
	void *buf_va;
	int buf_len, offset;
	int data_len = ISP_LOAD_DRAM_SIZE;

	if (!hw_isp->load_buffer) {
		ISP_ERR("save rpbuf is null\n");
		return -ENOENT;
	}

	buf_name = rpbuf_buffer_name(hw_isp->load_buffer);
	buf_va = rpbuf_buffer_va(hw_isp->load_buffer);
	buf_len = rpbuf_buffer_len(hw_isp->load_buffer);

	if (buf_len < data_len) {
		ISP_ERR("%s: data size(0x%x) out of buffer length(0x%x)\n",
			buf_name, data_len, buf_len);
		return -EINVAL;
	}

	if (!rpbuf_buffer_is_available(hw_isp->load_buffer)) {
		ISP_ERR("%s not available\n", buf_name);
		return -EACCES;
	}

	offset = 0;
	ret = rpbuf_transmit_buffer(hw_isp->load_buffer, offset, data_len);
	if (ret < 0) {
		ISP_ERR("%s: rpbuf_transmit_buffer failed\n", buf_name);
		return ret;
	}

	ISP_LIB_LOG(ISP_LOG_RP, "load buffer send to linux!\n");

	return data_len;
}

static void isp_rpbuf_buffer_available_cb(struct rpbuf_buffer *buffer, void *priv)
{
}

static int isp_rpbuf_buffer_rx_cb(struct rpbuf_buffer *buffer,
		void *data, int data_len, void *priv)
{
	struct hw_isp_device *hw_isp = priv;
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);
	unsigned int rpmsg_data[7];
	ae_result_t *ae_result = NULL;
	awb_result_t *awb_result = NULL;
	void *buf_va;

	ISP_LIB_LOG(ISP_LOG_RP, "save buffer receive from linux!\n");

	buf_va = rpbuf_buffer_va(buffer);
	rpmsg_data[0] = ISP_REQUEST_SENSOR_TEMP;
	isp_rpmsg_ept_send(hw_isp->ept, rpmsg_data, 1*4);

	isp_stats_process(hw_isp, rpbuf_buffer_va(buffer));

	ae_result = &ctx->ae_entity_ctx.ae_result;
	awb_result = &ctx->awb_entity_ctx.awb_result;
	rpmsg_data[0] = ISP_SET_SENSOR_EXP_GAIN;
	rpmsg_data[1] = ae_result->sensor_set.ev_set_curr.ev_sensor_exp_line;
	rpmsg_data[2] = ae_result->sensor_set_short.ev_set_curr.ev_sensor_exp_line;
	rpmsg_data[3] = ae_result->sensor_set.ev_set_curr.ev_analog_gain >> 4;
	rpmsg_data[4] = ae_result->sensor_set_short.ev_set_curr.ev_analog_gain >> 4;
	rpmsg_data[5] = awb_result->wb_gain_output.r_gain * 256 / awb_result->wb_gain_output.gr_gain;
	rpmsg_data[6] = awb_result->wb_gain_output.b_gain * 256 / awb_result->wb_gain_output.gb_gain;
	isp_rpmsg_ept_send(hw_isp->ept, rpmsg_data, 7*4);

	isp_rpmsg_encpp_send(hw_isp);

	isp_load_rpbuf_send(hw_isp);

	return 0;
}

static int isp_rpbuf_buffer_destroyed_cb(struct rpbuf_buffer *buffer, void *priv)
{
	return 0;
}

static const struct rpbuf_buffer_cbs isp_rpbuf_cbs = {
	.available_cb = isp_rpbuf_buffer_available_cb,
	.rx_cb = isp_rpbuf_buffer_rx_cb,
	.destroyed_cb = isp_rpbuf_buffer_destroyed_cb,
};

static int isp_ldci_rpbuf_buffer_rx_cb(struct rpbuf_buffer *buffer,
		void *data, int data_len, void *priv)
{
	struct hw_isp_device *hw_isp = priv;
	struct isp_lib_context *ctx = isp_dev_get_ctx(hw_isp);
	void *buf_va;

	ISP_LIB_LOG(ISP_LOG_RP, "ldci buffer receive from linux!\n");

	buf_va = rpbuf_buffer_va(buffer);

	hal_mutex_lock(ctx->ctx_lock);

	if (ctx->isp_ini_cfg.isp_tunning_settings.gtm_type == 4) {
		memcpy(ctx->ldci_buf, buf_va, ISP_RPBUF_LDCI_LEN);
		ctx->gtm_entity_ctx.gtm_param->ldci_frame_buf_addr = ctx->ldci_buf;
	}

	hal_mutex_unlock(ctx->ctx_lock);
	return 0;
}

static const struct rpbuf_buffer_cbs isp_ldci_rpbuf_cbs = {
	.available_cb = isp_rpbuf_buffer_available_cb,
	.rx_cb = isp_ldci_rpbuf_buffer_rx_cb,
	.destroyed_cb = isp_rpbuf_buffer_destroyed_cb,
};

static int isp_rpbuf_create(struct hw_isp_device *hw_isp, int controller_id)
{
	const char *load_name, *save_name, *ldci_name;
	int load_len = roundup(ISP_RPBUF_LOAD_LEN, 0x1000);
	int save_len = roundup(ISP_RPBUF_SAVE_LEN, 0x1000);
	int ldci_len = roundup(ISP_RPBUF_LDCI_LEN, 0x1000);
	struct rpbuf_controller *controller = NULL;

	controller = rpbuf_get_controller_by_id(controller_id);
	if (!controller) {
		ISP_ERR("Failed to get controller%d\n", controller_id);
		return -ENOENT;
	}

	if(hw_isp->id == 0){
		load_name = ISP0_RPBUF_LOAD_NAME;
		save_name = ISP0_RPBUF_SAVE_NAME;
		ldci_name = ISP0_RPBUF_LDCI_NAME;
	} else if (hw_isp->id == 1) {
		load_name = ISP1_RPBUF_LOAD_NAME;
		save_name = ISP1_RPBUF_SAVE_NAME;
		ldci_name = ISP1_RPBUF_LDCI_NAME;
	} else{
		ISP_ERR("hw_isp%d not support rpbuf\n", hw_isp->id);
		return -ENOENT;
	}

	hw_isp->load_buffer = rpbuf_alloc_buffer(controller, load_name, load_len, NULL, NULL, hw_isp);
	if (!hw_isp->load_buffer) {
		ISP_ERR("rpbuf_alloc_buffer load failed\n");
		return -ENOENT;
	}

	hw_isp->save_buffer = rpbuf_alloc_buffer(controller, save_name, save_len, NULL, &isp_rpbuf_cbs, hw_isp);
	if (!hw_isp->save_buffer) {
		ISP_ERR("rpbuf_alloc_buffer save failed\n");
		return -ENOENT;
	}

	hw_isp->ldci_buffer = rpbuf_alloc_buffer(controller, ldci_name, ldci_len, NULL, &isp_ldci_rpbuf_cbs, hw_isp);
	if (!hw_isp->ldci_buffer) {
		ISP_ERR("rpbuf_alloc_buffer ldci failed\n");
		return -ENOENT;
	}

	return 0;
}

static int isp_rpbuf_destroy(struct hw_isp_device *hw_isp)
{
	int ret;

	ret = rpbuf_free_buffer(hw_isp->load_buffer);
	if (ret < 0) {
		ISP_ERR("rpbuf_free_buffer load failed\n");
		return -ENOENT;
	}

	ret = rpbuf_free_buffer(hw_isp->save_buffer);
	if (ret < 0) {
		ISP_ERR("rpbuf_free_buffer save failed\n");
		return -ENOENT;
	}

	ret = rpbuf_free_buffer(hw_isp->ldci_buffer);
	if (ret < 0) {
		ISP_ERR("rpbuf_free_buffer ldci failed\n");
		return -ENOENT;
	}

	hw_isp->load_buffer = NULL;
	hw_isp->save_buffer = NULL;
	hw_isp->ldci_buffer = NULL;

	return 0;
}

/* isp rpmsg */
static void isp_rpmsg_destroy(struct hw_isp_device *hw_isp);
int isp_rpmsg_ept_send(struct rpmsg_endpoint *ept, void *data, int len)
{
	int ret = 0;

	if (!ept)
		return -1;

	ISP_LIB_LOG(ISP_LOG_RP, "rpmsg_ept_send, cmd is %d\n", *((unsigned int *)data + 0));

	ret = openamp_rpmsg_send(ept, data, len);
	if (ret < 0) {
		ISP_ERR("rpmsg_ept_send failed, cmd is %d\n", *((unsigned int *)data + 0));
	}

	return ret;
}

static void isp_rpmsg_unbind_callback(struct rpmsg_endpoint *ept)
{

}

static int isp_rpmsg_ept_callback(struct rpmsg_endpoint *ept, void *data,
		size_t len, uint32_t src, void *priv)
{
	struct hw_isp_device *hw_isp = priv;
	struct isp_lib_context *isp_gen = isp_dev_get_ctx(hw_isp);
	unsigned int *p = data;
	int rpdata[124] = {0};
	int ir;
	int *p_data = data;
	int mode_flag;

	ISP_LIB_LOG(ISP_LOG_RP, "isp_rpmsg_ept_callback, cmd is %d\n", p[0]);

	switch (p[0]) {
	case VIN_SET_SENSOR_INFO:
		isp_config_sensor_info(hw_isp, data);
		break;
	case VIN_SET_FRAME_SYNC:
		isp_fsync_process(hw_isp, &p[1]);
		break;
	case VIN_SET_ATTR_IOCTL:
		isp_set_attr_cfg(hw_isp, p_data[1], &p_data[2]);
		break;
	case VIN_SET_V4L2_IOCTL:
		isp_ctrl_process(hw_isp, data);
		break;
	case VIN_REQUEST_ATTR_IOCTL:
		rpdata[0] = ISP_SET_ATTR_IOCTL;
		rpdata[1] = p_data[1];//cfg_id
		isp_get_attr_cfg(hw_isp, p_data[1], &rpdata[2]);
		isp_rpmsg_ept_send(hw_isp->ept, rpdata, 124*4);
		break;
	case VIN_SET_CLOSE_ISP:
		rpdata[0] = ISP_SET_STAT_EN;
		rpdata[1] = DISABLE;
		isp_rpmsg_ept_send(hw_isp->ept, rpdata, 2*4);

		isp_save_ae_gainexp(hw_isp);

		isp_gen->sensor_info_init = 0;
		isp_reg_save_exit(isp_gen);
#if 0 //not be free,next time set VIN_SET_ISP_RESET to open isp server
		isp_rpbuf_destroy(hw_isp);
		isp_rpmsg_destroy(hw_isp);
		isp_server_exit(&hw_isp, hw_isp->id);
#endif
		break;
	case VIN_SET_ISP_RESET:
		isp_version_info();
		isp_config_sensor_info(hw_isp, data);
		while(!isp_gen->sensor_info_init) //wait sensor info init from vin
			hal_msleep(1);
		//isp_alloc_rp_reg_tbl(hw_isp);

		ir = isp_gen->isp_ir_flag;
		if (ir == 0)
			mode_flag = 0;
		else
			mode_flag = 0x2;
		isp_server_reset(hw_isp, hw_isp->id, mode_flag);

		rpdata[0] = ISP_SET_GTM_TPYE;
		rpdata[1] = isp_gen->isp_ini_cfg.isp_tunning_settings.gtm_type;
		isp_rpmsg_ept_send(hw_isp->ept, rpdata, 2*4);

		isp_reg_save_init(isp_gen, ir);
		isp_load_rpbuf_send(hw_isp);

		rpdata[0] = ISP_SET_STAT_EN;
		rpdata[1] = ENABLE;
		isp_rpmsg_ept_send(hw_isp->ept, rpdata, 2*4);
		break;
	case VIN_SET_ISP_START:
		isp_config_sensor_info(hw_isp, data);
		while(!isp_gen->sensor_info_init)//wait sensor info init from vin
			hal_msleep(1);

		isp_alloc_rp_reg_tbl(hw_isp);

		ir = 0;
		isp_server_set_paras(hw_isp, ir, hw_isp->id);

		rpdata[0] = ISP_SET_GTM_TPYE;
		rpdata[1] = isp_gen->isp_ini_cfg.isp_tunning_settings.gtm_type;
		isp_rpmsg_ept_send(hw_isp->ept, rpdata, 2*4);

		isp_reg_save_init(isp_gen, ir);
		isp_load_rpbuf_send(hw_isp);

		rpdata[0] = ISP_SET_STAT_EN;
		rpdata[1] = ENABLE;
		isp_rpmsg_ept_send(hw_isp->ept, rpdata, 2*4);
		break;
	case VIN_SET_SERVER_RESET:
		isp_server_reset(hw_isp, hw_isp->id, p[1]);
		break;
	case VIN_SET_SENSOR_TEMP:
		isp_gen->temp_info.enable = p[1];
		isp_gen->sensor_info.temperature = p[2];
		break;
	}

	return 0;
}

static int isp_rpmsg_create(struct hw_isp_device *hw_isp, int controller_id)
{
	const char *msg_name;
	uint32_t src_addr = RPMSG_ADDR_ANY;
	uint32_t dst_addr = RPMSG_ADDR_ANY;

	if (hw_isp->id == 0)
		msg_name = ISP0_RPMSG_NAME;
	else if (hw_isp->id == 1)
		msg_name = ISP1_RPMSG_NAME;
	else {
		ISP_ERR("hw_isp%d not support rpmsg\n", hw_isp->id);
		return -1;
	}
	hw_isp->ept = openamp_ept_open(msg_name, controller_id, src_addr, dst_addr,
					hw_isp, isp_rpmsg_ept_callback, isp_rpmsg_unbind_callback);
	if(hw_isp->ept == NULL) {
		ISP_ERR("Failed to Create Endpoint\r\n");
		return -1;
	}

	return 0;
}

static void isp_rpmsg_destroy(struct hw_isp_device *hw_isp)
{
	 openamp_ept_close(hw_isp->ept);
}

void isp_reset(struct hw_isp_device *hw_isp)
{
#ifdef CONFIG_ISP_READ_THRESHOLD
	unsigned int data[2];
	int ir;
	struct isp_lib_context *isp_gen = isp_dev_get_ctx(hw_isp);

	if (isp_rpmsg_create(hw_isp, ISP_RPMSG_CONTROLLER_ID))
		return;
	if (isp_rpbuf_create(hw_isp, ISP_RPBUF_CONTROLLER_ID))
		return;

	isp_gen->ldci_buf = hal_malloc(ISP_RPBUF_LDCI_LEN);

	ir = isp_gen->isp_ir_flag;
	isp_alloc_rp_reg_tbl(hw_isp);
	isp_reg_save_init(isp_gen, ir);
	isp_update_fix_param(hw_isp);
	isp_load_rpbuf_send(hw_isp);

	isp_get_ae_ev(hw_isp);

	data[0] = ISP_SET_GTM_TPYE;
	data[1] = isp_gen->isp_ini_cfg.isp_tunning_settings.gtm_type;
	isp_rpmsg_ept_send(hw_isp->ept, data, 2*4);

	data[0] = ISP_SET_STAT_EN;
	data[1] = ENABLE;
	isp_rpmsg_ept_send(hw_isp->ept, data, 2*4);
	isp_rpmsg_ir_status_send(hw_isp);

	isp_update_encpp_cfg(hw_isp);
	isp_set_firstdrc_tbl(hw_isp);
	isp_rpmsg_encpp_send(hw_isp);
#else //CONFIG_ISP_FAST_CONVERGENCE || CONFIG_ISP_HARD_LIGHTADC
	unsigned int data[2];
	int ir;
	int module_enable_flag;
	struct isp_lib_context *isp_gen = isp_dev_get_ctx(hw_isp);
	struct isp_param_config *param = &isp_gen->isp_ini_cfg;

	if (isp_rpmsg_create(hw_isp, ISP_RPMSG_CONTROLLER_ID))
		return;
	if (isp_rpbuf_create(hw_isp, ISP_RPBUF_CONTROLLER_ID))
		return;

	isp_gen->ldci_buf = hal_malloc(ISP_RPBUF_LDCI_LEN);

	//isp_save_load_rpbuf_send(hw_isp);
	//hal_msleep(30);

	ir = isp_gen->isp_ir_flag;
	isp_alloc_rp_reg_tbl(hw_isp);
	isp_update_encpp_cfg(hw_isp);
	isp_set_firstdrc_tbl(hw_isp);
	if (!isp_reg_save_init(isp_gen, ir)) {
		/* set first pltm strength */
		isp_gen->module_cfg.pltm_cfg.strength = min(isp_gen->ae_settings.pltm_dynamic_cfg[ISP_PLTM_DYNAMIC_MANUL_STREN], isp_gen->ae_settings.pltm_dynamic_cfg[ISP_PLTM_DYNAMIC_MAX_STREN_CLIP]);
		isp_update_fix_param(hw_isp);
		//isp_gen->module_cfg.module_enable_flag &= ~ISP_FEATURES_PLTM;
		isp_hardware_update(&isp_gen->module_cfg);
		//if (param->isp_test_settings.pltm_en) {
		//	isp_gen->module_cfg.module_enable_flag |= ISP_FEATURES_PLTM;
		//	*((int *)isp_gen->load_reg_base + 0x134/4) = *((int *)isp_gen->load_reg_base + 0x134/4) | PLTM_EN;
		//}
	}
	isp_load_rpbuf_send(hw_isp);

	data[0] = ISP_SET_GTM_TPYE;
	data[1] = isp_gen->isp_ini_cfg.isp_tunning_settings.gtm_type;
	isp_rpmsg_ept_send(hw_isp->ept, data, 2*4);

	data[0] = ISP_SET_STAT_EN;
	data[1] = ENABLE;
	isp_rpmsg_ept_send(hw_isp->ept, data, 2*4);
	isp_rpmsg_ir_status_send(hw_isp);
	isp_set_ir_hold(hw_isp);

	isp_rpmsg_encpp_send(hw_isp);
#endif
}

int isp_init(int argc, const char **argv)
{
	struct hw_isp_device *hw_isp;
	unsigned int isp_id = 0;
	unsigned int data[2];
	struct isp_lib_context *isp_gen;
	int ir;

	hw_isp = isp_server_init(isp_id);

	isp_gen = isp_dev_get_ctx(hw_isp);

	if (isp_rpmsg_create(hw_isp, ISP_RPMSG_CONTROLLER_ID))
		return -1;
	if (isp_rpbuf_create(hw_isp, ISP_RPBUF_CONTROLLER_ID))
		return -1;

	isp_gen->ldci_buf = hal_malloc(ISP_RPBUF_LDCI_LEN);

	data[0] = ISP_REQUEST_SENSOR_INFO;
	isp_rpmsg_ept_send(hw_isp->ept, data, 1*4);
	while(!isp_gen->sensor_info_init)//wait sensor info init from vin
		hal_msleep(1);

	isp_alloc_rp_reg_tbl(hw_isp);

	ir = 0;
	isp_server_set_paras(hw_isp, ir, isp_id);

	data[0] = ISP_SET_STAT_EN;
	data[1] = ENABLE;
	isp_rpmsg_ept_send(hw_isp->ept, data, 2*4);

	return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(isp_init, isp_init, rtthread isp run code);

//call isp_rp_init and then wait to VIN_SET_ISP_START to run isp_server
int isp_rp_init(int argc, const char **argv)
{
	struct hw_isp_device *hw_isp;
	unsigned int isp_id = 0;
	unsigned int data[2];
	struct isp_lib_context *isp_gen;

	hw_isp = isp_server_init(isp_id);

	isp_gen = isp_dev_get_ctx(hw_isp);

	if (isp_rpmsg_create(hw_isp, ISP_RPMSG_CONTROLLER_ID))
		return -1;
	if (isp_rpbuf_create(hw_isp, ISP_RPBUF_CONTROLLER_ID))
		return -1;

	isp_gen->ldci_buf = hal_malloc(ISP_RPBUF_LDCI_LEN);

	return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(isp_rp_init, isp_rp_init, rtthread isp_rp run code);
