/*
 * linux-4.9/drivers/media/platform/sunxi-vin/vin-isp/isp_server/include/isp_server.h
 *
 * Copyright (c) 2007-2020 Allwinnertech Co., Ltd.
 *
 * Authors: Zheng ZeQun <zequnzheng@allwinnertech.com>
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

#ifndef _ISP_ALGO_H_
#define _ISP_ALGO_H_

#include <openamp/sunxi_helper/openamp.h>
#include <rpbuf.h>

#include "include/isp_type.h"
#include "include/isp_manage.h"
#include "include/isp_tuning.h"
#include "include/isp_cmd_intf.h"
#include "isp_version.h"

#if (ISP_VERSION >= 600)
#define HW_ISP_DEVICE_NUM	CONFIG_ISP_NUMBER
#else
#define HW_ISP_DEVICE_NUM	1
#endif

////////////////////////isp rp//////////////////////////////
/* need set the same with linux */
#define ISP_RPBUF_CONTROLLER_ID 0
#define ISP0_RPBUF_LOAD_NAME "isp0_load_rpbuf"
#define ISP0_RPBUF_SAVE_NAME "isp0_save_rpbuf"
#define ISP1_RPBUF_LOAD_NAME "isp1_load_rpbuf"
#define ISP1_RPBUF_SAVE_NAME "isp1_save_rpbuf"
#define ISP_RPBUF_LOAD_LEN ISP_LOAD_DRAM_SIZE
#define ISP_RPBUF_SAVE_LEN (ISP_STAT_TOTAL_SIZE + ISP_SAVE_LOAD_STATISTIC_SIZE)

#define ISP0_RPBUF_LDCI_NAME "isp0_ldci_rpbuf"
#define ISP1_RPBUF_LDCI_NAME "isp1_ldci_rpbuf"
#define ISP_RPBUF_LDCI_LEN (160 * 90)

#define ISP_RPMSG_CONTROLLER_ID 0
#define ISP0_RPMSG_NAME "sunxi,isp0_rpmsg"
#define ISP1_RPMSG_NAME "sunxi,isp1_rpmsg"

enum rpmsg_cmd {
	ISP_REQUEST_SENSOR_INFO = 0,
	ISP_SET_SENSOR_EXP_GAIN,
	ISP_SET_STAT_EN,
	ISP_SET_SAVE_AE,
	ISP_SET_ENCPP_DATA,
	ISP_SET_IR_STATUS,
	ISP_SET_ATTR_IOCTL,
	ISP_REQUEST_SENSOR_TEMP,
	ISP_SET_GTM_TPYE,

	VIN_SET_SENSOR_INFO,
	VIN_SET_FRAME_SYNC,
	VIN_SET_V4L2_IOCTL,
	VIN_SET_CLOSE_ISP,
	VIN_SET_ISP_RESET,
	VIN_SET_ISP_START,
	VIN_SET_ATTR_IOCTL,
	VIN_REQUEST_ATTR_IOCTL,
	VIN_SET_SERVER_RESET,
	VIN_SET_SENSOR_TEMP,
};
//////////////////////////end////////////////////////////

struct hw_isp_device {
	unsigned int id;
	int load_type;

	char sensor_name[16];

	unsigned int size;
	void *buffer;

	struct isp_lib_context *ctx;
	struct isp_tuning *tuning;

	struct rpbuf_buffer *load_buffer;
	struct rpbuf_buffer *save_buffer;
	struct rpbuf_buffer *ldci_buffer;
	struct rpmsg_endpoint *ept;
};

struct hw_isp_paras {
	int isp_sync_mode;
	int ir_flag;
};

/* save isp parameter to flash */
/* kener user write it to sector, boot0 read it and write to ddr */
#define VIN_SENSOR0_RESERVE_ADDR (0x43BFE000) /*104~110 sector, size is 4k - 512b, boot0 read it and write to 0x43BFE000*/
#define VIN_SENSOR1_RESERVE_ADDR (0x43BFF000) /*112~118 sector, size is 4k - 512b, boot0 read it and write to 0x43BFF000*/
#define LIGNT_SENSOR_SIZE 128

enum ir_mode {
	DAY_MODE = 0,
	AUTO_MODE,
	NIGHT_MODE,
};

#pragma pack(1)
typedef struct taglight_sensor_attr_s {
	unsigned short u16LightValue;	/*adc value*/
	unsigned short ae_table_idx;
	unsigned int   u32SnsExposure;
	unsigned int   u32SnsAgain;
} LIGHT_SENSOR_ATTR_S;

typedef struct tagsensor_isp_config_s {
	int sign;                    //id0: 0xAA66AA66, id1: 0xBB66BB66
	int crc;                     //checksum, crc32 check, if crc = 0, do check
	int ver;                     //version, 0x01

	int light_enable;            //adc en, 1:use LIGHT_SENSOR_ATTR_S
	int adc_mode;                //ADC mode, 0:the brighter the u16LightValue more, 1:the brighter the u16LightValue smaller
	unsigned short light_def;    //adc threshold: 1,adc_mode=0:smaller than it enter night mode, greater than it or equal enter day mode;
				     //2,adc_mode=1:greater than it enter night mode, smaller than it or equal enter day mode;

	unsigned char ircut_state;   //1:hold, 0:use ir_mode
	unsigned short ir_mode;	     //IR mode 0:force day mode, 1:auto mode, 2:force night mode
	unsigned short lv_liner_def; //lv threshold: smaller than it enter night mode, greater than it or equal enter day mode
	unsigned short lv_hdr_def;   //lv threshold: smaller than it enter night mode, greater than it or equal enter day mode

	unsigned short width;        //init size, 0:mean maximum size
	unsigned short height;
	unsigned short mirror;
	unsigned short filp;
	unsigned short fps;
	unsigned short wdr_mode;     //hdr or normal mode, wdr_mode = 0 mean normal,  wdr_mode = 1 mean hdr
	unsigned char flicker_mode;  //0:disable,1:50HZ,2:60HZ, default 50HZ

	unsigned char venc_format;   //1:H264 2:H265
	/* unsigned char reserv 256 */
	unsigned char sensor_deinit; //sensor not init in melis
	unsigned char get_yuv_en; //get melis yuv en
	unsigned char reserv[256 - 2];

	LIGHT_SENSOR_ATTR_S linear[LIGNT_SENSOR_SIZE]; //day linear mode parameter, auto exp
	LIGHT_SENSOR_ATTR_S linear_night;              //night linear mode parameter
	LIGHT_SENSOR_ATTR_S hdr[LIGNT_SENSOR_SIZE];    //day hdr mode paramete
	LIGHT_SENSOR_ATTR_S hdr_night;                 //night hdr mode parameter
} SENSOR_ISP_CONFIG_S; //size is 3395 < 3584(3.5k)
#pragma pack()
/* save isp parameter to flash end */

/* kernel auto save isp parameter to flash, in order to use in next time */
#define ISP0_NORFLASH_SAVE (VIN_SENSOR0_RESERVE_ADDR + 512 * 7) /*111 sector, size is 512b, kernel write to flash and boot0 read it and write to ISP0_NORFLASH_SAVE*/
#define ISP1_NORFLASH_SAVE (VIN_SENSOR1_RESERVE_ADDR + 512 * 7) /*119 sector, size is 512b, kernel write to flash and boot0 read it and write to ISP1_NORFLASH_SAVE*/

#pragma pack(1)
struct isp_autoflash_config_s {
	//ISP_SET_SAVE_AE
	unsigned int ae_tble_idx;
	unsigned int ev_analog_gain;
	unsigned int ev_sensor_exp_line;
	unsigned int ev_short_analog_gain;
	unsigned int ev_short_sensor_exp_line;
	unsigned int reserv1[11];

	unsigned int melisyuv_sign_id;//id0: 0xAA11AA11, id1: 0xBB11BB11
	unsigned int melisyuv_paddr;
	unsigned int melisyuv_size;
	unsigned int reserv2[45];

	//sensor_list, melis identify sensor and then use this to notice kernel which sensor is use
	unsigned int sensorlist_sign_id;//id0: 0xAA22AA22, id1: 0xBB22BB22
	unsigned char sensor_name[20];
	unsigned int sensor_twi_addr;
	unsigned int sensor_detect_id;
	unsigned int reserv3[56];
 };
#pragma pack()
/* kernel auto save isp parameter to flash end */

void isp_stats_process(struct hw_isp_device *isp, void *isp_stat_buf);
//void isp_ctrl_process(struct hw_isp_device *isp, struct v4l2_ctrl *ctrl);
void isp_fsync_process(struct hw_isp_device *isp, unsigned int *data);
struct hw_isp_device *isp_server_init(unsigned int id);
int isp_server_save_ctx(unsigned int id);
int isp_server_save_reg(unsigned int id, int ir);
int isp_server_set_paras(struct hw_isp_device *hw_isp, int ir, unsigned int id);
int isp_server_reset(struct hw_isp_device *hw_isp, int dev_id, int mode_flag);
void isp_set_firstframe_wb(int dev_id);
int isp_server_exit(struct hw_isp_device **hw_isp, unsigned int id);
void isp_save_exit(struct hw_isp_device *hw_isp, int id);
HW_S32 isp_stats_req(struct hw_isp_device *hw_isp, int dev_id, struct isp_stats_context *stats_ctx);
int isp_get_lv(struct hw_isp_device *hw_isp, int dev_id);
void isp_ldci_video_en(int id, int en);
int isp_set_attr_cfg(struct hw_isp_device *hw_isp, HW_U32 ctrl_id, void *value);
int isp_get_attr_cfg(struct hw_isp_device *hw_isp, HW_U32 ctrl_id, void *value);
int isp_get_encpp_cfg(struct hw_isp_device *hw_isp, HW_U32 ctrl_id, void *value);
void isp_get_ir_threshold(struct hw_isp_device *hw_isp);
void isp_get_ir_mode(struct hw_isp_device *hw_isp, int *ir_mode);

void isp_reset(struct hw_isp_device *hw_isp);

#endif

