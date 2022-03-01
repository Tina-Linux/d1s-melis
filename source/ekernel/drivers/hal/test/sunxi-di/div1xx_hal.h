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
#ifndef _DIV3X_HAL_H
#define _DIV3X_HAL_H

#include "div1xx_uapi.h"
#include "ioctl.h"

#define u8	unsigned char
#define u16	unsigned short
#define u32	unsigned int
#define u64	unsigned long long

enum {
	DI_INTP_MODE_INVALID = 0x0,
	DI_INTP_MODE_BOB,
	DI_INTP_MODE_MOTION,
};

enum {
	DI_OUT_1FRAME = 0,
	DI_OUT_2FRAME,
};


enum {
	DI_BOTTOM_FIELD_FIRST = 0,
	DI_TOP_FIELD_FIRST = 1,
};

enum {
	DI_TNR_MODE_INVALID = 0,
	DI_TNR_MODE_ADAPTIVE,
	DI_TNR_MODE_FIX,
};

enum {
	DI_TNR_LEVEL_HIGH = 0,
	DI_TNR_LEVEL_MIDDLE,
	DI_TNR_LEVEL_LOW,
};

struct di_version {
	u32 version_major;
	u32 version_minor;
	u32 version_patchlevel;

	u32 ip_version;
};

struct di_timeout_ns {
	unsigned int client_number;
	u64 wait4start;
	u64 wait4finish;
};

/*
 * @mode:
 * @level:
 */
struct di_tnr_mode {
	u32 mode;
	u32 level;
};

struct di_addr {
	u64 y_addr;
	u64 cb_addr;
	u64 cr_addr;
};

struct di_offset {
	u64 y_offset;
	u64 cb_offset;
	u64 cr_offset;
};

/*
 * support dma_buf method or phy_addr_buf method.
 * 1.On dma_buf method: use di_offset
 * 2.On phy_addr_buf method: use di_addr
 */
union di_buf {
	struct di_addr addr;
	struct di_offset offset;
};

/*
 * @dma_buf_fd: dma buf fd that from userspace.
 *    @dma_buf_fd must be invalid(<0) on phy_addr_buf method.
 * @size.width,@size.height: size of pixel datas of image. unit: pixel.
 */
struct di_fb {
	int dma_buf_fd;
	union di_buf buf;
	struct di_size size;
};


/*@pixel_format: see DRM_FORMAT_XXX in drm_fourcc.h.
*/
struct di_process_fb_arg {
	unsigned int client_number;
	u8 is_interlace;
	u8 field_order; /*1:top field first 0:bottom field first*/
	u32 pixel_format;

	struct di_size size; /*size of source interlace picture*/

	u8 output_mode; /*0: 1-frame 1: 2-frame*/
	u8 di_mode; /*0:motion adaptive mode  1:inter field interpolation mode*/

	struct di_tnr_mode tnr_mode;

	struct di_fb in_fb0;
	struct di_fb in_fb1; /*NOTE: when BOB, infb1 must be filled*/
	struct di_fb in_fb2;

	struct di_fb out_fb0;
	struct di_fb out_fb1;
	struct di_fb out_tnr_fb;

};

struct di_mem_arg {
	unsigned int size;
	unsigned int handle;
	u64 phys_addr;
};

#define DI_IOC_MAGIC 'D'
#define DI_IO(nr)          _IO(DI_IOC_MAGIC, nr)
#define DI_IOR(nr, size)   _IOR(DI_IOC_MAGIC, nr, size)
#define DI_IOW(nr, size)   _IOW(DI_IOC_MAGIC, nr, size)
#define DI_IOWR(nr, size)  _IOWR(DI_IOC_MAGIC, nr, size)
#define DI_IOCTL_NR(n)     _IOC_NR(n)

#define DI_IOC_GET_VERSION    DI_IOR(0x0, struct di_version)
#define DI_IOC_SET_TIMEOUT    DI_IOW(0x3, struct di_timeout_ns)
#define DI_IOC_PROCESS_FB     DI_IOW(0x8, struct di_process_fb_arg)
#define DI_IOC_DESTROY     DI_IOW(0x9, int)
#define DI_IOC_MEM_REQUEST    DI_IOWR(0x10, struct di_mem_arg)
#define DI_IOC_MEM_RELEASE    DI_IOWR(0x11, struct di_mem_arg)
#endif
