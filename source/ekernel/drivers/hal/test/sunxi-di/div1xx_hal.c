/*
* This confidential and proprietary software should be used
* under the licensing agreement from Allwinner Technology.

* Copyright (C) 2020-2030 Allwinner Technology Limited
* All rights reserved.

* Author:zhengwanyu <zhengwanyu@allwinnertech.com>

* The entire notice above must be reproduced on all authorised
* copies and copies may only be made to the extent permitted
* by a licensing agreement from Allwinner Technology Limited.
*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <hal_mem.h>

#include "div1xx_hal.h"
#include "drm_fourcc.h"

#define DI_INF(...) \
	do {printf("[DI HAL]"); printf(__VA_ARGS__);} while(0);
#define DI_ERR(...) \
	do {printf("[DI ERROR]"); printf(__VA_ARGS__);} while(0);
#define DI_TRACE() \
	do {DI_INF("%s\n", __func__)} while(0);

#define MEMCLEAR(x) memset(x, 0, sizeof(*x))

extern int sunxi_di_control(int cmd, void *arg);
extern int sunxi_di_release(void);
extern int sunxi_di_open(void);

struct div1xx_hal {
	unsigned int diMode;

	unsigned int processCount;

	unsigned char debug;
};

static unsigned int getDrmFormat(unsigned int format)
{
	switch(format){
	case DI_FORMAT_YUV420_PLANNER:
		return DRM_FORMAT_YVU420;

	case DI_FORMAT_YUV420_NV21:
		return DRM_FORMAT_NV21;

	case DI_FORMAT_YUV420_NV12:
		return DRM_FORMAT_NV12;

	case DI_FORMAT_YUV422_PLANNER:
		return DRM_FORMAT_YVU422;

	case DI_FORMAT_YUV422_NV61:
		return DRM_FORMAT_NV61;

	case DI_FORMAT_YUV422_NV16:
		return DRM_FORMAT_NV16;
	}

	return 0;
}

static int diGetIPVersion(struct div1xx_hal *hal)
{
	int ret;
	struct di_version v;

	ret = sunxi_di_control(DI_IOC_GET_VERSION, &v);
	if (ret) {
		DI_ERR("DI_IOC_GET_VERSION failed\n");
		return ret;
	}

	DI_INF("di version[%d.%d.%d], hw_ip[%d]\n",
		v.version_major,
		v.version_minor,
		v.version_patchlevel,
		v.ip_version);

	return ret;
}

static int diSetTimeout(struct div1xx_hal *hal, unsigned int client_num)
{
	int ret = 0;
	struct di_timeout_ns t;

	t.client_number = client_num;
	t.wait4start = 30;  // msec
	t.wait4finish = 30;  // msec
	ret = sunxi_di_control(DI_IOC_SET_TIMEOUT, &t);
	if (ret) {
		DI_ERR("DI_IOC_SET_TIMEOUT failed\n");
		return ret;
	}

	return ret;
}

static void dumpInitDebugInfo(struct div1xx_hal *hal, struct init_para *para)
{
	if (!hal->debug)
		return;
}

int diInit(DIDevice *deinterlace, unsigned int mode, struct init_para *para)
{
	struct div1xx_hal *hal = hal_malloc(sizeof(struct div1xx_hal));
	int client_num = 0;

	MEMCLEAR(hal);

	hal->diMode = mode;

	client_num = sunxi_di_open();
	if (client_num < 0) {
		DI_ERR("open di failed!\n");
		return -1;
	}

	if (diGetIPVersion(hal) < 0) {
		DI_ERR("diGetIPVersion failed!\n");
		return -1;
	}

	if (diSetTimeout(hal, client_num) < 0) {
		DI_ERR("diSetTimeout failed!\n");
		return -1;
	}

	if (mode < DIV1XX_MODE_NUM)
		hal->diMode = mode;
	else {
		hal->diMode = 0;
		DI_ERR("Invalid di mode:%u\n", mode);
		return -1;
	}

	hal->debug = para->debug_en;

	dumpInitDebugInfo(hal, para);

	*deinterlace = (DIDevice)hal;
	return client_num;
}

static int setDiFb(struct di_fb *fb,
			  unsigned int width, unsigned int height,
			  unsigned long long phyAddr, unsigned int format)
{
	unsigned int pixel_num;

	MEMCLEAR(fb);

	if (!phyAddr)
		return 0;

	pixel_num = width * height;

	fb->size.width = width;
	fb->size.height = height;

	if (1) {
		fb->buf.addr.y_addr = phyAddr;
		fb->buf.addr.cb_addr = phyAddr + pixel_num;
		if (format == DI_FORMAT_YUV420_PLANNER)
			fb->buf.addr.cr_addr = phyAddr +  pixel_num
						+ pixel_num / 4;
		else if (format == DI_FORMAT_YUV422_PLANNER)
			fb->buf.addr.cr_addr = phyAddr +  pixel_num
						+ pixel_num / 2;
	} else {
		fb->buf.offset.y_offset = 0; //y offset
		fb->buf.offset.cb_offset = pixel_num; //cb offset
		if (format == DI_FORMAT_YUV420_PLANNER)
			fb->buf.offset.cr_offset
				= pixel_num + pixel_num / 4; //cr offset
		else if (format == DI_FORMAT_YUV422_PLANNER)
			fb->buf.offset.cr_offset
				= pixel_num + pixel_num / 2; //cr offset
	}

	return 0;
}

static int diFbMdOut2(struct div1xx_hal *hal, struct fb_para_md_out2 *para, unsigned int client_number)
{
	struct di_process_fb_arg arg;

	MEMCLEAR(&arg);

	arg.client_number = client_number;
	arg.is_interlace = 1;
	arg.pixel_format = getDrmFormat(para->format);
	arg.field_order = para->topFieldFirst ?
		DI_TOP_FIELD_FIRST : DI_BOTTOM_FIELD_FIRST;
	memcpy(&arg.size, &para->size, sizeof(struct di_size));

	arg.output_mode = DI_OUT_2FRAME;
	arg.di_mode = DI_INTP_MODE_MOTION;

	if (setDiFb(&arg.in_fb0,
		  para->size.width, para->size.height, para->inFb[0].phyaddr,
		  para->format) < 0) {
		DI_ERR("setDiFb failed\n");
		return -1;
	}

	if (setDiFb(&arg.in_fb1,
		  para->size.width, para->size.height, para->inFb[1].phyaddr,
		  para->format) < 0) {
		DI_ERR("setDiFb failed\n");
		return -1;
	}

	if (setDiFb(&arg.out_fb0,
		  para->size.width, para->size.height, para->outFb[0].phyaddr,
		  para->format) < 0) {
		DI_ERR("setDiFb failed\n");
		return -1;
	}

	if (setDiFb(&arg.out_fb1,
		  para->size.width, para->size.height, para->outFb[1].phyaddr,
		  para->format) < 0) {
		DI_ERR("setDiFb failed\n");
		return -1;
	}

	if (sunxi_di_control(DI_IOC_PROCESS_FB, &arg) < 0) {
		DI_ERR("DI_IOC_PROCESS_FB failed");
		return -1;
	}

	return 0;
}

static int diFbMdOut1(struct div1xx_hal *hal, struct fb_para_md_out1 *para, unsigned int client_number)
{
	struct di_process_fb_arg arg;

	MEMCLEAR(&arg);

	arg.client_number = client_number;
	arg.is_interlace = 1;
	arg.pixel_format = getDrmFormat(para->format);
	arg.field_order = para->topFieldFirst ?
		DI_TOP_FIELD_FIRST : DI_BOTTOM_FIELD_FIRST;
	memcpy(&arg.size, &para->size, sizeof(struct di_size));

	arg.output_mode = DI_OUT_1FRAME;
	arg.di_mode = DI_INTP_MODE_MOTION;

	if (setDiFb(&arg.in_fb0,
		  para->size.width, para->size.height, para->inFb[0].phyaddr,
		  para->format) < 0) {
		DI_ERR("setDiFb failed\n");
		return -1;
	}

	if (setDiFb(&arg.in_fb1,
		  para->size.width, para->size.height, para->inFb[1].phyaddr,
		  para->format) < 0) {
		DI_ERR("setDiFb failed\n");
		return -1;
	}

	if (setDiFb(&arg.out_fb0,
		  para->size.width, para->size.height, para->outFb.phyaddr,
		  para->format) < 0) {
		DI_ERR("setDiFb failed\n");
		return -1;
	}

	if (sunxi_di_control(DI_IOC_PROCESS_FB, &arg) < 0) {
		DI_ERR("DI_IOC_PROCESS_FB failed");
		return -1;
	}

	return 0;
}

static int diFbBobOut2(struct div1xx_hal *hal, struct fb_para_bob_out2 *para, unsigned int client_number)
{
	struct di_process_fb_arg arg;

	MEMCLEAR(&arg);

	arg.client_number = client_number;
	arg.is_interlace = 1;
	arg.pixel_format = getDrmFormat(para->format);
	arg.field_order = para->topFieldFirst ?
		DI_TOP_FIELD_FIRST : DI_BOTTOM_FIELD_FIRST;
	memcpy(&arg.size, &para->size, sizeof(struct di_size));

	arg.output_mode = DI_OUT_2FRAME;
	arg.di_mode = DI_INTP_MODE_BOB;

	if (((para->inFb[0].phyaddr > 0))
		&& setDiFb(&arg.in_fb0,
		  para->size.width, para->size.height, para->inFb[0].phyaddr,
		  para->format) < 0) {
		DI_ERR("setDiFb failed\n");
		return -1;
	}

	if (setDiFb(&arg.in_fb1,
		  para->size.width, para->size.height, para->inFb[1].phyaddr,
		  para->format) < 0) {
		DI_ERR("setDiFb failed\n");
		return -1;
	}

	if (setDiFb(&arg.out_fb0,
		  para->size.width, para->size.height, para->outFb[0].phyaddr,
		  para->format) < 0) {
		DI_ERR("setDiFb failed\n");
		return -1;
	}

	if (setDiFb(&arg.out_fb1,
		  para->size.width, para->size.height, para->outFb[1].phyaddr,
		  para->format) < 0) {
		DI_ERR("setDiFb failed\n");
		return -1;
	}

	if (sunxi_di_control(DI_IOC_PROCESS_FB, &arg) < 0) {
		DI_ERR("DI_IOC_PROCESS_FB failed");
		return -1;
	}

	return 0;
}

static int diFbBobOut1(struct div1xx_hal *hal, struct fb_para_bob_out1 *para, unsigned int client_number)
{
	struct di_process_fb_arg arg;

	MEMCLEAR(&arg);

	arg.client_number = client_number;
	arg.is_interlace = 1;
	arg.pixel_format = getDrmFormat(para->format);
	arg.field_order = para->topFieldFirst ?
		DI_TOP_FIELD_FIRST : DI_BOTTOM_FIELD_FIRST;
	memcpy(&arg.size, &para->size, sizeof(struct di_size));
	arg.output_mode = DI_OUT_1FRAME;
	arg.di_mode = DI_INTP_MODE_BOB;

	if (((para->inFb[0].phyaddr > 0))
		&& setDiFb(&arg.in_fb0,
		  para->size.width, para->size.height, para->inFb[0].phyaddr,
		  para->format) < 0) {
		DI_ERR("setDiFb failed\n");
		return -1;
	}

	if (setDiFb(&arg.in_fb1,
		  para->size.width, para->size.height, para->inFb[1].phyaddr,
		  para->format) < 0) {
		DI_ERR("setDiFb failed\n");
		return -1;
	}

	if (setDiFb(&arg.out_fb0,
		  para->size.width, para->size.height, para->outFb.phyaddr,
		  para->format) < 0) {
		DI_ERR("setDiFb failed\n");
		return -1;
	}

	if (sunxi_di_control(DI_IOC_PROCESS_FB, &arg) < 0) {
		DI_ERR("DI_IOC_PROCESS_FB failed");
		return -1;
	}

	return 0;
}


static void dumpDiFrambufferInfo(struct div1xx_hal *hal, void *para)
{
	if (!hal->debug)
		return;
	DI_INF("DI FRAMEBUFFER INFO:\n");
	if (hal->diMode == DIV1XX_MODE_MD_OUT2) {
		struct fb_para_md_out2 *mdout2
				= (struct fb_para_md_out2 *)para;
		DI_INF("ttf:%d\n", mdout2->topFieldFirst);
		DI_INF("fmt:(%s)\n",
			getDiPixelFormatName(mdout2->format));
		DI_INF("width:%u height:%d\n", mdout2->size.width,
					       mdout2->size.height);
		DI_INF("infb:(phyaddr) (%lld) (%lld)\n", mdout2->inFb[0].phyaddr,
			mdout2->inFb[1].phyaddr);
		DI_INF("outfb:(phyaddr) (%lld) (%lld)\n", mdout2->outFb[0].phyaddr,
			mdout2->outFb[1].phyaddr);
	} else if (hal->diMode == DIV1XX_MODE_MD_OUT1) {
		struct fb_para_md_out1 *mdout1
				= (struct fb_para_md_out1 *)para;
		DI_INF("ttf:%d\n", mdout1->topFieldFirst);
		DI_INF("fmt:(%s)\n",
			getDiPixelFormatName(mdout1->format));
		DI_INF("width:%u height:%d\n", mdout1->size.width,
					       mdout1->size.height);
		DI_INF("infb:(phyaddr) (%lld) (%lld)\n", mdout1->inFb[0].phyaddr,
			mdout1->inFb[1].phyaddr);
		DI_INF("outfb:(phyaddr) (%lld)\n", mdout1->outFb.phyaddr);
	} else if (hal->diMode == DIV1XX_MODE_BOB_OUT2) {
		struct fb_para_bob_out2 *bobout2
				= (struct fb_para_bob_out2 *)para;
		DI_INF("ttf:%d\n", bobout2->topFieldFirst);
		DI_INF("fmt:(%s)\n",
			getDiPixelFormatName(bobout2->format));
		DI_INF("width:%u height:%d\n", bobout2->size.width,
					       bobout2->size.height);
		DI_INF("infb0:(phyaddr) (%lld)\n", bobout2->inFb[0].phyaddr);
		DI_INF("infb1:(phyaddr) (%lld)\n", bobout2->inFb[1].phyaddr);
		DI_INF("outfb:(phyaddr) (%lld) (%lld)\n", bobout2->outFb[0].phyaddr,
			bobout2->outFb[1].phyaddr);
	} else if (hal->diMode == DIV1XX_MODE_BOB_OUT1) {
		struct fb_para_bob_out1 *bobout1
				= (struct fb_para_bob_out1 *)para;
		DI_INF("ttf:%d\n", bobout1->topFieldFirst);
		DI_INF("fmt:(%s)\n",
			getDiPixelFormatName(bobout1->format));
		DI_INF("width:%u height:%d\n", bobout1->size.width,
					       bobout1->size.height);
		DI_INF("infb0:(phyaddr) (%lld)\n", bobout1->inFb[0].phyaddr);
		DI_INF("infb1:(phyaddr) (%lld)\n", bobout1->inFb[1].phyaddr);
		DI_INF("outfb:(phyaddr) (%lld)\n", bobout1->outFb.phyaddr);
	}

	DI_INF("\n\n\n");
}

int diFrameBuffer(DIDevice deinterlace, void *fb_para, unsigned int client_number)
{
	int ret = 0;
	struct fb_para_md_out2 *paraMdOut2;
	struct fb_para_md_out1 *paraMdOut1;
	struct fb_para_bob_out2 *paraBobOut2;
	struct fb_para_bob_out1 *paraBobOut1;

	struct div1xx_hal *hal = (struct div1xx_hal *)deinterlace;

	dumpDiFrambufferInfo(hal, fb_para);
	switch(hal->diMode) {
	case DIV1XX_MODE_BOB_OUT2:
		paraBobOut2 = (struct fb_para_bob_out2 *)fb_para;
		ret = diFbBobOut2(hal, paraBobOut2, client_number);
		break;
	case DIV1XX_MODE_BOB_OUT1:
		paraBobOut1 = (struct fb_para_bob_out1 *)fb_para;
		ret = diFbBobOut1(hal, paraBobOut1, client_number);
		break;

	case DIV1XX_MODE_MD_OUT2:
		paraMdOut2 = (struct fb_para_md_out2 *)fb_para;
		ret = diFbMdOut2(hal, paraMdOut2, client_number);
		break;
	case DIV1XX_MODE_MD_OUT1:
		paraMdOut1 = (struct fb_para_md_out1 *)fb_para;
		ret = diFbMdOut1(hal, paraMdOut1, client_number);
		break;
	default:
		DI_ERR("invalid hal->diMode:%d\n", hal->diMode);
		return -1;
	}

	if (ret < 0) {
		DI_ERR("di FB failed\n\n");
		return -1;
	}

	hal->processCount++;
	return 0;
}

int diExit(DIDevice deinterlace, int client_number)
{
	struct div1xx_hal *hal = (struct div1xx_hal *)deinterlace;

	sunxi_di_control(DI_IOC_DESTROY, &client_number);
	sunxi_di_release();
	hal_free(hal);

	return 0;
}
