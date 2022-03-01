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
#ifndef _DIV1XX_UAPI_H
#define _DIV1XX_UAPI_H

#include <stdbool.h>

#define DI_MAX_PLANE_NUM 3

typedef void *DIDevice;

enum {
	DIV1XX_MODE_BOB_OUT2 = 0,
	DIV1XX_MODE_BOB_OUT1,
	DIV1XX_MODE_MD_OUT2,
	DIV1XX_MODE_MD_OUT1,
	DIV1XX_MODE_NUM,
};

enum di_pixel_format {
	DI_FORMAT_YUV420_PLANNER = 0,
	DI_FORMAT_YUV420_NV21, /*UV UV UV*/
	DI_FORMAT_YUV420_NV12, /*VU VU VU*/

	DI_FORMAT_YUV422_PLANNER,
	DI_FORMAT_YUV422_NV61, /*UV UV UV*/
	DI_FORMAT_YUV422_NV16, /*VU VU VU*/

	DI_FORMAT_NUM,
};

struct di_size {
	unsigned int width;
	unsigned int height;
};

struct di_buffer {
	unsigned long long phyaddr;
};

struct init_para {
	unsigned int debug_en;
};

/* Note:
 * memory sizes of fb_para_bob_out2 equals fb_para_md_out2
 * memory sizes of fb_para_bob_out1 equals fb_para_md_out1
 */

struct fb_para_bob_out2 {
	bool topFieldFirst;
	struct di_size size;
	enum di_pixel_format format;

	struct di_buffer inFb[2]; /*Note: when BOB, inFb[1] must be filled, inFb[0] can be empty*/
	struct di_buffer outFb[2];
};

struct fb_para_md_out2 {
	bool topFieldFirst;
	struct di_size size;
	enum di_pixel_format format;

	struct di_buffer inFb[2];
	struct di_buffer outFb[2];
};

struct fb_para_bob_out1 {
	bool topFieldFirst;
	struct di_size size;
	enum di_pixel_format format;

	struct di_buffer inFb[2];/*Note: when BOB, inFb[1] must be filled, inFb[0] can be empty*/
	struct di_buffer outFb;
};

struct fb_para_md_out1 {
	bool topFieldFirst;
	struct di_size size;
	enum di_pixel_format format;
	
	struct di_buffer inFb[2];
	struct di_buffer outFb;
};

static inline char *getDiPixelFormatName(unsigned int fmt)
{
	switch(fmt) {
	case DI_FORMAT_YUV420_PLANNER:
		return "yuv420_planner";
	case DI_FORMAT_YUV420_NV12:
		return "yuv420_nv12";
	case DI_FORMAT_YUV420_NV21:
		return "yuv420_nv21";
	case DI_FORMAT_YUV422_PLANNER:
		return "yuv422_planner";
	case DI_FORMAT_YUV422_NV16:
		return "yuv422_nv16";
	case DI_FORMAT_YUV422_NV61:
		return "yuv422_nv61";
	}

	return NULL;
}

int diInit(DIDevice *deinterlace, unsigned int mode, struct init_para *para);
int diFrameBuffer(DIDevice deinterlace, void *fb_para, unsigned int client_number);
int diExit(DIDevice deinterlace, int client_number);
#endif
