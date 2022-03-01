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

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <aw_dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <hal_mem.h>
#include <hal_cache.h>

#include "../div1xx_uapi.h"
#include "../div1xx_hal.h"
#include "test_file.h"

#define DI_ALIGN(value, align) ((align == 0) ? \
			value : \
			(((value) + ((align) - 1)) & ~((align) - 1)))

enum {
	YUV420_PIXEL_FMT = 0,
	YUV422_PIXEL_FMT = 1,
};

static inline int getSubSampleFormat(unsigned int fmt)
{
	if (fmt == DI_FORMAT_YUV420_PLANNER
		|| fmt == DI_FORMAT_YUV420_NV21
		|| fmt == DI_FORMAT_YUV420_NV12)
		return YUV420_PIXEL_FMT;
	else if (fmt == DI_FORMAT_YUV422_PLANNER
		|| fmt == DI_FORMAT_YUV422_NV61
		|| fmt == DI_FORMAT_YUV422_NV16)
		return YUV422_PIXEL_FMT;

	return -1;
}

static inline char *getFormatName(unsigned int fmt)
{
	if (fmt == DI_FORMAT_YUV420_PLANNER)
		return "yv12";
	else if (fmt == DI_FORMAT_YUV420_NV21)
		return "nv21";
	else if (fmt == DI_FORMAT_YUV420_NV12)
		return "nv12";
	else if (fmt == DI_FORMAT_YUV422_PLANNER)
		return "yv16";
	else if (fmt == DI_FORMAT_YUV422_NV61)
		return "nv61";
	else if (fmt == DI_FORMAT_YUV422_NV16)
		return "nv16";

	return NULL;
}

struct dma_mem
{
	unsigned int phy;     /* physical address */
	unsigned char *virt;  /* virtual address */
	unsigned long size;   /* ion buffer size */
	int dmafd;            /* ion dmabuf fd */
};

struct di_demo {
	const char *name;
	unsigned int dimode;
	unsigned char debug_en;

	unsigned int width, height;
	unsigned int format;

	struct dma_mem inFb[2];
	struct dma_mem outFb[2];

	unsigned int test_file_num;
	const char **y_path, **cb_path, **cr_path;

};

static int dma_ion_alloc(struct dma_mem *mem, unsigned int size)
{
	int ret = 0;
	void *virt_addr = NULL;

	virt_addr = hal_malloc(size);

	memset(virt_addr, 0, size);
	hal_dcache_clean(virt_addr, size);

	mem->phy = __va_to_pa(virt_addr);
	mem->virt = (unsigned char *)virt_addr;
	mem->size = size;

	return ret;
}

static void dma_ion_free(struct dma_mem *mem)
{
	hal_free(mem->virt);
}

void setDiBuffer(struct di_buffer *dibuf, struct dma_mem *mem)
{
	dibuf->phyaddr = mem->phy;
}

void outputRenderResult(struct dma_mem *mem,
			const char *path, unsigned int size)
{
	FILE *fp;

	fp=fopen(path, "wb+");
	if (!fp) {
		printf("fopen %s failed\n", path);
		return;
	}

	fwrite(mem->virt, 1, size, fp);
	hal_dcache_clean(mem->virt, size);

	fclose(fp);
}

int loadFile(struct dma_mem *mem, unsigned int ion_offset,
			const char *path, unsigned int size)
{
	FILE *file_fd;
	int ret;

	file_fd = fopen(path, "rb+");
	if (file_fd < 0) {
		printf("open %s err.\n", path);
		return -1;
	}

	ret = fread(mem->virt + ion_offset, 1, size, file_fd);
	hal_dcache_clean(mem->virt + ion_offset, size);
	printf("fread, ret = %d \n", ret);

	close(file_fd);

	return 0;
}

int getInterlaceData(struct dma_mem *mem, unsigned int size,
	const char *y_path, unsigned int y_offset, unsigned int y_size,
	const char *cb_path, unsigned int cb_offset, unsigned int cb_size,
	const char *cr_path, unsigned int cr_offset, unsigned int cr_size)
{
	if (dma_ion_alloc(mem, size) < 0) {
		printf("dma_ion_alloc for interlace data failed\n");
		return -1;
	}

	if (loadFile(mem, y_offset, y_path, y_size) < 0) {
		printf("load file for %s failed\n", y_path);

		return -1;
	}

	if (loadFile(mem, cb_offset, cb_path, cb_size) < 0) {
		printf("load file for %s failed\n", cb_path);
		return -1;
	}

	if (cr_path && cr_size
		&& loadFile(mem, cr_offset, cr_path, cr_size) < 0) {
		printf("load file for %s failed\n", cr_path);
		return -1;
	}

	return 0;
}

void releaseInterlaceData(struct dma_mem *mem)
{
	dma_ion_free(mem);
}

int getDiOutMem(struct dma_mem *mem, unsigned int size)
{
	return dma_ion_alloc(mem, size);
}

void releaseDiOutMem(struct dma_mem *mem)
{
	dma_ion_free(mem);
}

int setInit(DIDevice *di_dev, struct di_demo *para)
{
	struct init_para init;
	int client_number;

	memset(&init, 0, sizeof(init));
	init.debug_en = para->debug_en;

	if ((client_number = diInit(di_dev, para->dimode, (void *)&init)) < 0) {
		printf("diInit failed\n");
		return -1;
	}

	return client_number;
}

void calColorComponentSize(unsigned int format,
			   unsigned int width,
			   unsigned int height,
			   unsigned int *ySize,
			   unsigned int *cbSize,
			   unsigned int *crSize)
{
	unsigned int y_size = 0, cb_size = 0, cr_size = 0;

	y_size = width * height;
	switch (format) {
	case DI_FORMAT_YUV420_PLANNER:
		cb_size = y_size / 4;
		cr_size = cb_size;
		break;

	case DI_FORMAT_YUV420_NV21:
	case DI_FORMAT_YUV420_NV12:
		cb_size = y_size / 2;
		cr_size = 0;
		break;

	case DI_FORMAT_YUV422_PLANNER:
		cb_size = y_size / 2;
		cr_size = cb_size;
		break;
	case DI_FORMAT_YUV422_NV61:
	case DI_FORMAT_YUV422_NV16:
		cb_size = y_size;
		cr_size = 0;
		break;
	default:
		y_size = 0;
		cb_size = 0;
		cr_size = 0;
		printf("invalid input format\n");
		break;
	}

	*ySize = y_size;
	*cbSize = cb_size;
	*crSize = cr_size;
}

static int processMdOut2(struct di_demo *demo, DIDevice di_dev, unsigned int client_number)
{
	int i = 0;
	struct fb_para_md_out2 fbout2;
	unsigned int size;
	unsigned int y_size;
	unsigned int cb_size;
	unsigned int cr_size;

	memset(&fbout2, 0, sizeof(fbout2));

	calColorComponentSize(demo->format, demo->width, demo->height,
				&y_size, &cb_size, &cr_size);
	size = y_size + cb_size + cr_size;

	fbout2.topFieldFirst = true;
	fbout2.size.width = demo->width;
	fbout2.size.height = demo->height;
	fbout2.format = demo->format;

	if (getInterlaceData(&demo->inFb[0], size,
		demo->y_path[0], 0, y_size,
		demo->cb_path[0], y_size, cb_size,
		demo->cr_path ? demo->cr_path[0] : NULL,
		y_size + cb_size, cr_size) < 0) {
		printf("getInterlaceData for infb[0] failed!\n");
		return -1;
	}
	setDiBuffer(&fbout2.inFb[0], &demo->inFb[0]);

	i = 1;
	while (i < demo->test_file_num - 1) {
//get the new picture for the next deinterlacing
		if (getInterlaceData(&demo->inFb[1], size,
			demo->y_path[i], 0, y_size,
			demo->cb_path[i], y_size, cb_size,
			demo->cr_path ? demo->cr_path[i] : NULL,
			y_size + cb_size, cr_size) < 0) {
			printf("getInterlaceData for infb[0] failed!\n");
			return -1;
		}
		setDiBuffer(&fbout2.inFb[1], &demo->inFb[1]);

		if (getDiOutMem(&demo->outFb[0], size) < 0) {
			printf("getDiOutMem for out fb0 failed\n");
			return -1;
		}
		setDiBuffer(&fbout2.outFb[0], &demo->outFb[0]);

		if (getDiOutMem(&demo->outFb[1], size) < 0) {
			printf("getDiOutMem for out fb0 failed\n");
			return -1;
		}
		setDiBuffer(&fbout2.outFb[1], &demo->outFb[1]);

		printf("iFrameBuffer start!\n");
		if (diFrameBuffer(di_dev, (void *)&fbout2, client_number) < 0) {
			printf("diFrameBuffer failed\n");
			return -1;
		}

		printf("iFrameBuffer finish!\n");

//you can use the out memorys to display them or do other things
		char path[50];

		sprintf(path, "./%s/%dx%d_%d_f.%s", demo->name, demo->width, demo->height, i, getFormatName(demo->format));
		outputRenderResult(&demo->outFb[0], path, size);

		sprintf(path, "./%s/%dx%d_%d_s.%s", demo->name, demo->width, demo->height, i, getFormatName(demo->format));
 		outputRenderResult(&demo->outFb[1], path, size);

//the memory of in_fb0 is free, you can use it to do other things or release it
		releaseInterlaceData(&demo->inFb[0]);

//set the infb0/infb1 for the next deinterlace processing
		memcpy(&fbout2.inFb[0], &fbout2.inFb[1],
			sizeof(struct di_buffer));

		releaseDiOutMem(&demo->outFb[0]);
		releaseDiOutMem(&demo->outFb[1]);


		i++;
	}

	return 0;
}

static int processMdOut1(struct di_demo *demo, DIDevice di_dev, unsigned int client_number)
{
	int i = 0;
	struct fb_para_md_out1 fbout1;
	unsigned int size;
	unsigned int y_size;
	unsigned int cb_size;
	unsigned int cr_size;

	printf("%s\n", __func__);
	memset(&fbout1, 0, sizeof(fbout1));

	calColorComponentSize(demo->format, demo->width, demo->height,
				&y_size, &cb_size, &cr_size);
	size = y_size + cb_size + cr_size;

	fbout1.topFieldFirst = true;
	fbout1.size.width = demo->width;
	fbout1.size.height = demo->height;
	fbout1.format = demo->format;

	if (getInterlaceData(&demo->inFb[0], size,
		demo->y_path[0], 0, y_size,
		demo->cb_path[0], y_size, cb_size,
		demo->cr_path ? demo->cr_path[0] : NULL,
		y_size + cb_size, cr_size) < 0) {
		printf("getInterlaceData for infb[0] failed!\n");
		return -1;
	}
	setDiBuffer(&fbout1.inFb[0], &demo->inFb[0]);

	i = 1;
	while (i < demo->test_file_num - 1) {
//get the new picture for the next deinterlacing
		if (getInterlaceData(&demo->inFb[1], size,
			demo->y_path[i], 0, y_size,
			demo->cb_path[i], y_size, cb_size,
			demo->cr_path ? demo->cr_path[i] : NULL,
			y_size + cb_size, cr_size) < 0) {
			printf("getInterlaceData for infb[0] failed!\n");
			return -1;
		}
		setDiBuffer(&fbout1.inFb[1], &demo->inFb[1]);

		if (getDiOutMem(&demo->outFb[0], size) < 0) {
			printf("getDiOutMem for out fb0 failed\n");
			return -1;
		}
		setDiBuffer(&fbout1.outFb, &demo->outFb[0]);

		printf("iFrameBuffer start!\n");
		if (diFrameBuffer(di_dev, (void *)&fbout1, client_number) < 0) {
			printf("diFrameBuffer failed\n");
			return -1;
		}
		printf("iFrameBuffer end!\n");

//you can use the out memorys to display them or do other things
		char path[50];

		sprintf(path, "./%s/%dx%d_%d.%s", demo->name, demo->width, demo->height, i, getFormatName(demo->format));
		outputRenderResult(&demo->outFb[0], path, size);

//the memory of in_fb0 is free, you can use it to do other things or release it
		releaseInterlaceData(&demo->inFb[0]);

//set the infb0/infb1 for the next deinterlace processing
		memcpy(&fbout1.inFb[0], &fbout1.inFb[1],
			sizeof(struct di_buffer));

		releaseDiOutMem(&demo->outFb[0]);

		i++;
	}
	return 0;
}

static int processBobOut2(struct di_demo *demo, DIDevice di_dev, unsigned int client_number)
{
	int i = 0;
	struct fb_para_bob_out2 fbout2;
	unsigned int size;
	unsigned int y_size;
	unsigned int cb_size;
	unsigned int cr_size;

	printf("%s\n", __func__);
	memset(&fbout2, 0, sizeof(fbout2));

	calColorComponentSize(demo->format, demo->width, demo->height,
				&y_size, &cb_size, &cr_size);
	size = y_size + cb_size + cr_size;

	fbout2.topFieldFirst = true;
	fbout2.size.width = demo->width;
	fbout2.size.height = demo->height;
	fbout2.format = demo->format;

	i = 0;
	while (i < demo->test_file_num) {
//get the new picture for the next deinterlacing
		if (getInterlaceData(&demo->inFb[1], size,
			demo->y_path[i], 0, y_size,
			demo->cb_path[i], y_size, cb_size,
			demo->cr_path ? demo->cr_path[i] : NULL,
			y_size + cb_size, cr_size) < 0) {
			printf("getInterlaceData for infb[0] failed!\n");
			return -1;
		}
		setDiBuffer(&fbout2.inFb[1], &demo->inFb[1]);

		if (getDiOutMem(&demo->outFb[0], size) < 0) {
			printf("getDiOutMem for out fb0 failed\n");
			return -1;
		}
		setDiBuffer(&fbout2.outFb[0], &demo->outFb[0]);

		if (getDiOutMem(&demo->outFb[1], size) < 0) {
			printf("getDiOutMem for out fb0 failed\n");
			return -1;
		}
		setDiBuffer(&fbout2.outFb[1], &demo->outFb[1]);
	
		printf("iFrameBuffer start!\n");
		if (diFrameBuffer(di_dev, (void *)&fbout2, client_number) < 0) {
			printf("diFrameBuffer failed\n");
			return -1;
		}
		printf("iFrameBuffer end!\n");
//you can use the out memorys to display them or do other things
		char path[50];

		sprintf(path, "./%s/%dx%d_%d_f.%s", demo->name, demo->width, demo->height, i, getFormatName(demo->format));
		outputRenderResult(&demo->outFb[0], path, size);

		sprintf(path, "./%s/%dx%d_%d_s.%s", demo->name, demo->width, demo->height, i, getFormatName(demo->format));
 		outputRenderResult(&demo->outFb[1], path, size);

//the memory of in_fb0 is free, you can use it to do other things or release it
		releaseInterlaceData(&demo->inFb[1]);

		releaseDiOutMem(&demo->outFb[0]);
		releaseDiOutMem(&demo->outFb[1]);

		i++;
	}
	return 0;
}

static int processBobOut1(struct di_demo *demo, DIDevice di_dev, unsigned int client_number)
{
	int i = 0;
	struct fb_para_bob_out1 fbout1;
	unsigned int size;
	unsigned int y_size;
	unsigned int cb_size;
	unsigned int cr_size;

	printf("%s\n", __func__);
	memset(&fbout1, 0, sizeof(fbout1));

	calColorComponentSize(demo->format, demo->width, demo->height,
				&y_size, &cb_size, &cr_size);
	size = y_size + cb_size + cr_size;

	fbout1.topFieldFirst = true;
	fbout1.size.width = demo->width;
	fbout1.size.height = demo->height;
	fbout1.format = demo->format;

	i = 0;
	while (i < demo->test_file_num) {
//get the new picture for the next deinterlacing
		if (getInterlaceData(&demo->inFb[1], size,
			demo->y_path[i], 0, y_size,
			demo->cb_path[i], y_size, cb_size,
			demo->cr_path ? demo->cr_path[i] : NULL,
			y_size + cb_size, cr_size) < 0) {
			printf("getInterlaceData for infb[0] failed!\n");
			return -1;
		}
		setDiBuffer(&fbout1.inFb[1], &demo->inFb[1]);

		if (getDiOutMem(&demo->outFb[0], size) < 0) {
			printf("getDiOutMem for out fb0 failed\n");
			return -1;
		}
		setDiBuffer(&fbout1.outFb, &demo->outFb[0]);

		printf("iFrameBuffer start!\n");
		if (diFrameBuffer(di_dev, (void *)&fbout1, client_number) < 0) {
			printf("diFrameBuffer failed\n");
			return -1;
		}
		printf("iFrameBuffer end!\n");

//you can use the out memorys to display them or do other things
		char path[50];

		sprintf(path, "./%s/%dx%d_%d.%s", demo->name, demo->width, demo->height, i, getFormatName(demo->format));
		outputRenderResult(&demo->outFb[0], path, size);

//the memory of in_fb0 is free, you can use it to do other things or release it
		releaseInterlaceData(&demo->inFb[1]);

		releaseDiOutMem(&demo->outFb[0]);

		i++;
	}
	return 0;
}

static int cmd_di_test(int argc, char **argv)
{
	struct di_demo demo;
	DIDevice di_dev;
	int ret = -1;
	char out_put_full_path[60] = {0};

	memset(&demo, 0, sizeof(demo));

	if (argc != 5) {
		printf("Invalid param input!\n");
		return -1;
	}

	demo.width = DI_ALIGN(atoi(argv[1]), 2);
	demo.height = DI_ALIGN(atoi(argv[2]), 2);
	//demo.width = DI_ALIGN(1920, 2);
	//demo.height = DI_ALIGN(1080, 4);

	demo.format = atoi(argv[3]);
	//demo.format = DI_FORMAT_YUV420_PLANNER;
	//demo.format = DI_FORMAT_YUV420_NV21;
	//demo.format = DI_FORMAT_YUV420_NV12;
	//demo.format = DI_FORMAT_YUV422_PLANNER;
	//demo.format = DI_FORMAT_YUV422_NV61;
	//demo.format = DI_FORMAT_YUV422_NV16;

	demo.dimode = atoi(argv[4]);
	//demo.dimode = DIV1XX_MODE_BOB_OUT2;
	//demo.dimode = DIV1XX_MODE_BOB_OUT1;
	//demo.dimode = DIV1XX_MODE_MD_OUT2;
	//demo.dimode = DIV1XX_MODE_MD_OUT1;

	demo.debug_en = 0;

	demo.test_file_num = 4;

	if (demo.format == DI_FORMAT_YUV420_PLANNER) {
		demo.y_path = test_pics_yv12_y;
		demo.cb_path = test_pics_yv12_cb;
		demo.cr_path = test_pics_yv12_cr;

		switch (demo.dimode) {
		case DIV1XX_MODE_BOB_OUT2:
			demo.name = "bob_out2_yv12";
			break;
		case DIV1XX_MODE_BOB_OUT1:
			demo.name = "bob_out1_yv12";
			break;
		case DIV1XX_MODE_MD_OUT2:
			demo.name = "md_out2_yv12";
			break;
		case DIV1XX_MODE_MD_OUT1:
			demo.name = "md_out1_yv12";
			break;
		default:
			printf("invalid diMode\n");
			return -1;
		}
	} else if (demo.format == DI_FORMAT_YUV420_NV21) {
		demo.y_path = test_pics_nv21_y;
		demo.cb_path = test_pics_nv21_c;
		demo.cr_path = NULL;

		switch (demo.dimode) {
		case DIV1XX_MODE_BOB_OUT2:
			demo.name = "bob_out2_nv21";
			break;
		case DIV1XX_MODE_BOB_OUT1:
			demo.name = "bob_out1_nv21";
			break;
		case DIV1XX_MODE_MD_OUT2:
			demo.name = "md_out2_nv21";
			break;
		case DIV1XX_MODE_MD_OUT1:
			demo.name = "md_out1_nv21";
			break;
		default:
			printf("invalid diMode\n");
			return -1;
		}
	} else if (demo.format == DI_FORMAT_YUV420_NV12) {
		demo.y_path = test_pics_nv12_y;
		demo.cb_path = test_pics_nv12_c;
		demo.cr_path = NULL;

		switch (demo.dimode) {
		case DIV1XX_MODE_BOB_OUT2:
			demo.name = "bob_out2_nv12";
			break;
		case DIV1XX_MODE_BOB_OUT1:
			demo.name = "bob_out1_nv12";
			break;
		case DIV1XX_MODE_MD_OUT2:
			demo.name = "md_out2_nv12";
			break;
		case DIV1XX_MODE_MD_OUT1:
			demo.name = "md_out1_nv12";
			break;
		default:
			printf("invalid diMode\n");
			return -1;
		}
	} else if (demo.format == DI_FORMAT_YUV422_PLANNER) {
		demo.y_path = test_pics_yv16_y;
		demo.cb_path = test_pics_yv16_cb;
		demo.cr_path = test_pics_yv16_cr;

		switch (demo.dimode) {
		case DIV1XX_MODE_BOB_OUT2:
			demo.name = "bob_out2_yv16";
			break;
		case DIV1XX_MODE_BOB_OUT1:
			demo.name = "bob_out1_yv16";
			break;
		case DIV1XX_MODE_MD_OUT2:
			demo.name = "md_out2_yv16";
			break;
		case DIV1XX_MODE_MD_OUT1:
			demo.name = "md_out1_yv16";
			break;
		default:
			printf("invalid diMode\n");
			return -1;
		}
	} else if (demo.format == DI_FORMAT_YUV422_NV61) {
		demo.y_path = test_pics_nv61_y;
		demo.cb_path = test_pics_nv61_c;
		demo.cr_path = NULL;

		switch (demo.dimode) {
		case DIV1XX_MODE_BOB_OUT2:
			demo.name = "bob_out2_nv61";
			break;
		case DIV1XX_MODE_BOB_OUT1:
			demo.name = "bob_out1_nv61";
			break;
		case DIV1XX_MODE_MD_OUT2:
			demo.name = "md_out2_nv61";
			break;
		case DIV1XX_MODE_MD_OUT1:
			demo.name = "md_out1_nv61";
			break;
		default:
			printf("invalid diMode\n");
			return -1;
		}
	} else if (demo.format == DI_FORMAT_YUV422_NV16) {
		demo.y_path = test_pics_nv16_y;
		demo.cb_path = test_pics_nv16_c;
		demo.cr_path = NULL;

		switch (demo.dimode) {
		case DIV1XX_MODE_BOB_OUT2:
			demo.name = "bob_out2_nv16";
			break;
		case DIV1XX_MODE_BOB_OUT1:
			demo.name = "bob_out1_nv16";
			break;
		case DIV1XX_MODE_MD_OUT2:
			demo.name = "md_out2_nv16";
			break;
		case DIV1XX_MODE_MD_OUT1:
			demo.name = "md_out1_nv16";
			break;
		default:
			printf("invalid diMode\n");
			return -1;
		}
	} else {
		printf("Invalid pixel format\n");
		return -1;
	}

	snprintf(out_put_full_path, 60, "%s%s", "/mnt/F/", demo.name);
	demo.name = out_put_full_path;

	if(opendir(demo.name) == NULL) {
		ret = mkdir(demo.name, S_IRWXU | S_IRWXG | S_IRWXO);
		if (ret != 0) {
			printf("mkdir %s failed, maybe you have creat it, please delete it before running that program\n", demo.name);
			return -1;
		}
	}


	printf("Start DI Test!!!\n");
	int client_number;
	if ((client_number = setInit(&di_dev, &demo)) < 0) {
		printf("setInit failed\n");
		return -1;
	}

	printf("dimode:%d\n", demo.dimode);
	if (demo.dimode == DIV1XX_MODE_MD_OUT2)
		ret = processMdOut2(&demo, di_dev, client_number);
	else if (demo.dimode == DIV1XX_MODE_MD_OUT1)
	      ret = processMdOut1(&demo, di_dev, client_number);
	else if (demo.dimode == DIV1XX_MODE_BOB_OUT2)
	      ret = processBobOut2(&demo, di_dev, client_number);
	else if (demo.dimode == DIV1XX_MODE_BOB_OUT1)
	      ret = processBobOut1(&demo, di_dev, client_number);
	else
		printf("Invalid dimode\n");

	diExit(di_dev, client_number);
	return ret;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_di_test, __cmd_di_test, di_test);
