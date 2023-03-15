/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <rtthread.h>
#include <hal_mem.h>
#include <log.h>
#include "../hal/source/g2d_rcq/g2d_driver.h"

#define IMG_SIZE1 800*480
#define X1 800
#define Y1 480
#define IMG_SIZE2 720*480
#define X2 720
#define Y2 480
#define IMG_SIZE3 480*320
#define X3 480
#define Y3 320
#define IMG_SIZE4 1280*720
#define X4 1280
#define Y4 720
#define IMG_SIZE5 333*333
#define X5 333
#define Y5 333

void hal_dcache_clean(unsigned long vaddr_start, unsigned long size);
void hal_dcache_invalidate(unsigned long vaddr_start, unsigned long size);

static int init_flag;
static int cmd_g2d_test_blend(int argc, const char **argv)
{
	int ret, i;
	void *src = NULL, *src2 = NULL, *dst = NULL;
	FILE* file[3];
	g2d_bld info;
	unsigned int src_w, src_h, src2_w, src2_h, dst_w, dst_h;
	unsigned int order = 0;
	char name[128];

	printf("hello g2d blending test\n");
	rt_device_t dev = rt_device_find("g2d");

	if (!dev) {
		printf("can not find device\n");
		ret = -1;
		goto out;
	}
	if (init_flag == 0) {
		rt_device_init(dev);
		init_flag = 1;
	}
	printf("start open\n");

	ret = rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
	if (ret){
		printf("g2d open fail\n");
		ret = -1;
		goto error;
	}

	printf("press ENTER to start test\n");
	getchar();

	//note:the tests follow need three  input image file
	src = hal_malloc(IMG_SIZE1 * 4);
	src2 = hal_malloc(IMG_SIZE1 * 4);
	dst = hal_malloc(IMG_SIZE4 * 4);

	if(src == NULL || src2 == NULL || dst == NULL) {
		printf("fatal error, buf is null.\n");
		ret = -1;
		goto error;
	}

	memset(src, 0, IMG_SIZE1 * 4);
	memset(src2, 0, IMG_SIZE1 * 4);
	memset(dst, 0, IMG_SIZE4 * 4);
	memset(&info, 0, sizeof(g2d_bld));

	//Test1------------------------------------------------------------------
	// src = src2 = dst RGB
	info.src_image[0].laddr[0] = __va_to_pa((uint32_t)src);
	info.src_image[1].laddr[0] = __va_to_pa((uint32_t)src2);
	info.dst_image.laddr[0] = __va_to_pa((uint32_t)dst);

	info.src_image[0].laddr[0] = (unsigned int)(info.src_image[0].laddr[0]);
	info.src_image[0].laddr[1] = 0;
	info.src_image[0].laddr[2] = 0;
	info.src_image[0].use_phy_addr = 1;

	info.src_image[1].laddr[0] = (unsigned int)(info.src_image[1].laddr[0]);
	info.src_image[1].laddr[1] = 0;
	info.src_image[1].laddr[2] = 0;
	info.src_image[1].use_phy_addr = 1;

	info.dst_image.laddr[0] = (unsigned int)(info.dst_image.laddr[0]);
	info.dst_image.laddr[1] = 0;
	info.dst_image.laddr[2] = 0;
	info.dst_image.use_phy_addr = 1;

	info.bld_cmd = G2D_BLD_SRCIN;//G2D_BLD_SRCOVER;

	for (i = 0; i < 2; i++) {
		info.src_image[i].format = G2D_FORMAT_ARGB8888;
		info.src_image[i].mode = G2D_GLOBAL_ALPHA;
		info.src_image[i].width = X1;
		info.src_image[i].height = Y1;
		info.src_image[i].clip_rect.x = 0;
		info.src_image[i].clip_rect.y = 0;
		info.src_image[i].clip_rect.w = X1;
		info.src_image[i].clip_rect.h = Y1;
		info.src_image[i].coor.x = 0;
		info.src_image[i].coor.y = 0;
	}

	info.src_image[0].alpha = 0xd0;
	info.src_image[1].alpha = 0x50;

	info.dst_image.format = G2D_FORMAT_ARGB8888;
	info.dst_image.mode = G2D_GLOBAL_ALPHA;
	info.dst_image.alpha = 0xff;
	info.dst_image.width = X1;
	info.dst_image.height = Y1;
	info.dst_image.clip_rect.x = 0;
	info.dst_image.clip_rect.y = 0;
	info.dst_image.clip_rect.w = X1;
	info.dst_image.clip_rect.h = Y1;

	printf("start open file\n");
	file[0] = fopen("/mnt/E/src_800x480_rgb.bin", "rb+");
	if (file[0] == NULL) {
		__err("err in fopen src file");
		ret = -1;
		goto error;
	}

	ret = fread((void*)src, IMG_SIZE1 * 4, 1, file[0]);

	printf("fread,ret=%d\n", ret);

	fclose(file[0]);

	file[1] = fopen("/mnt/E/src2_800x480_rgb.bin", "rb+");
	if (file[1] == NULL) {
		__err("err in fopen src2 file");
		ret = -1;
		goto error;
	}

	ret = fread((void*)src2, IMG_SIZE1 * 4, 1, file[1]);

	printf("fread,ret=%d\n", ret);

	fclose(file[1]);

	order++;
	sprintf(name, "/mnt/E/bld_test_%d.bin", order);
	printf("name = %s\n", name);
	file[2] = fopen(name, "wb+");
	if (file[2] == NULL)
		printf("err in fopen dst file\n");

	printf("start control G2D_CMD_BLD_H \n");

	//we use hal_malloc for iamge input,so we need to make sure caches flushed
	hal_dcache_clean((unsigned long)src, IMG_SIZE1 * 4);
	hal_dcache_clean((unsigned long)src2, IMG_SIZE1 * 4);
	hal_dcache_clean((unsigned long)dst, IMG_SIZE4 * 4);

	printf("start control\n");

	ret = rt_device_control(dev, G2D_CMD_BLD_H, &info);
	if (ret) {
		printf("g2d G2D_CMD_BLD_H fail\n");
		ret = -1;
		goto error;
	} else {
		printf("G2D_CMD_BLD_H ok\n");
	}

	printf("output image to file, may use little time\n");

	ret = fwrite(dst, IMG_SIZE4 * 4, 1, file[2]);

	printf("fwrite ,ret=%d\n",ret);

	fclose(file[2]);

	printf("\nfirst test finished!\n");

	printf("press ENTER to Next test\n");
	getchar();

	//Test2------------------------------------------------------------------
	// src2 < src = dst. src RGB dst RGB
	src_w = X1;
	src_h = Y1;
	src2_w = X2;
	src2_h = Y2;
	dst_w = X1;
	dst_h = Y1;
	info.src_image[0].laddr[0] = __va_to_pa((uint32_t)src);
	info.src_image[1].laddr[0] = __va_to_pa((uint32_t)src2);
	info.dst_image.laddr[0] = __va_to_pa((uint32_t)dst);

	info.src_image[0].laddr[0] = (unsigned int)(info.src_image[0].laddr[0]);
	info.src_image[0].laddr[1] = (unsigned int)(info.src_image[0].laddr[0] + src_w * src_h);
	info.src_image[0].laddr[2] = (unsigned int)(info.src_image[0].laddr[0] + src_w * src_h * 5 / 4);
	info.src_image[0].use_phy_addr = 1;

	info.src_image[1].laddr[0] = (unsigned int)(info.src_image[1].laddr[0]);
	info.src_image[1].laddr[1] = (unsigned int)(info.src_image[0].laddr[0] + src2_w * src2_h);
	info.src_image[1].laddr[2] = (unsigned int)(info.src_image[0].laddr[0] + src2_w * src2_h * 5 / 4);
	info.src_image[1].use_phy_addr = 1;

	info.dst_image.laddr[0] = (unsigned int)(info.dst_image.laddr[0]);
	info.dst_image.laddr[1] = 0;
	info.dst_image.laddr[2] = 0;
	info.dst_image.use_phy_addr = 1;

	info.bld_cmd = G2D_BLD_SRCOVER;

	info.src_image[0].format = G2D_FORMAT_ARGB8888;
	info.src_image[0].mode = G2D_GLOBAL_ALPHA;
	info.src_image[0].width = src_w;
	info.src_image[0].height = src_h;
	info.src_image[0].clip_rect.x = 0;
	info.src_image[0].clip_rect.y = 0;
	info.src_image[0].clip_rect.w = src_w;
	info.src_image[0].clip_rect.h = src_h;
	info.src_image[0].coor.x = 0;
	info.src_image[0].coor.y = 0;
	info.src_image[0].alpha = 0xd0;

	info.src_image[1].alpha = 0x50;
	info.src_image[1].format = G2D_FORMAT_ARGB8888;
	info.src_image[1].mode = G2D_GLOBAL_ALPHA;
	info.src_image[1].width = src2_w;
	info.src_image[1].height = src2_h;
	info.src_image[1].clip_rect.x = 0;
	info.src_image[1].clip_rect.y = 0;
	info.src_image[1].clip_rect.w = src2_w;
	info.src_image[1].clip_rect.h = src2_h;
	info.src_image[1].coor.x = 0;
	info.src_image[1].coor.y = 0;

	info.dst_image.format = G2D_FORMAT_ARGB8888;
	info.dst_image.mode = G2D_GLOBAL_ALPHA;
	info.dst_image.alpha = 0xff;
	info.dst_image.width = X1;
	info.dst_image.height = Y1;
	info.dst_image.clip_rect.x = 0;
	info.dst_image.clip_rect.y = 0;
	info.dst_image.clip_rect.w = X1;
	info.dst_image.clip_rect.h = Y1;


	printf("start open file\n");
	file[0] = fopen("/mnt/E/src_800x480_rgb.bin", "rb+");
	if (file[0] == NULL) {
		__err("err in fopen src file");
		ret = -1;
		goto error;
	}

	ret = fread((void*)src, IMG_SIZE1 * 4, 1, file[0]);

	printf("fread,ret=%d\n", ret);

	fclose(file[0]);

	file[1] = fopen("/mnt/E/bike_720x480_150.bin", "rb+");
	if (file[1] == NULL) {
		__err("err in fopen src2 file");
		ret = -1;
		goto error;
	}

	ret = fread((void*)src2, IMG_SIZE1 * 4, 1, file[1]);

	printf("fread,ret=%d\n", ret);

	fclose(file[1]);

	order++;
	sprintf(name, "/mnt/E/bld_test_%d.bin", order);
	file[2] = fopen(name, "wb+");
	if (file[2] == NULL)
		printf("err in fopen dst file\n");

	printf("start control G2D_CMD_BLD_H \n");

	//we use hal_malloc for iamge input,so we need to make sure caches flushed
	hal_dcache_clean((unsigned long)src, IMG_SIZE1 * 4);
	hal_dcache_clean((unsigned long)src2, IMG_SIZE1 * 4);
	hal_dcache_clean((unsigned long)dst, IMG_SIZE4 * 4);

	printf("start control\n");

	ret = rt_device_control(dev, G2D_CMD_BLD_H, &info);
	if (ret) {
		printf("g2d G2D_CMD_BLD_H fail\n");
		ret = -1;
		goto error;
	} else {
		printf("G2D_CMD_BLD_H ok\n");
	}

	printf("output image to file, may use little time\n");

	ret = fwrite(dst, IMG_SIZE4 * 4, 1, file[2]);

	printf("fwrite ,ret=%d\n",ret);

	fclose(file[2]);

	printf("\nsecond test finished!\n");

	printf("press ENTER to Next test\n");
	getchar();


	//Test3------------------------------------------------------------------
	// src2 < src = dst. src RGB dst RGB, coor not zero
	src_w = X1;
	src_h = Y1;
	src2_w = X3;
	src2_h = Y3;
	dst_w = X1;
	dst_h = Y1;
	info.src_image[0].laddr[0] = __va_to_pa((uint32_t)src);
	info.src_image[1].laddr[0] = __va_to_pa((uint32_t)src2);
	info.dst_image.laddr[0] = __va_to_pa((uint32_t)dst);

	info.src_image[0].laddr[0] = (unsigned int)(info.src_image[0].laddr[0]);
	info.src_image[0].laddr[1] = (unsigned int)(info.src_image[0].laddr[0] + src_w * src_h);
	info.src_image[0].laddr[2] = (unsigned int)(info.src_image[0].laddr[0] + src_w * src_h * 5 / 4);
	info.src_image[0].use_phy_addr = 1;

	info.src_image[1].laddr[0] = (unsigned int)(info.src_image[1].laddr[0]);
	info.src_image[1].laddr[1] = (unsigned int)(info.src_image[0].laddr[0] + src2_w * src2_h);
	info.src_image[1].laddr[2] = (unsigned int)(info.src_image[0].laddr[0] + src2_w * src2_h * 5 / 4);
	info.src_image[1].use_phy_addr = 1;

	info.dst_image.laddr[0] = (unsigned int)(info.dst_image.laddr[0]);
	info.dst_image.laddr[1] = 0;
	info.dst_image.laddr[2] = 0;
	info.dst_image.use_phy_addr = 1;

	info.bld_cmd = G2D_BLD_SRCOVER;

	info.src_image[0].format = G2D_FORMAT_ARGB8888;
	info.src_image[0].mode = G2D_GLOBAL_ALPHA;
	info.src_image[0].width = src_w;
	info.src_image[0].height = src_h;
	info.src_image[0].clip_rect.x = 0;
	info.src_image[0].clip_rect.y = 0;
	info.src_image[0].clip_rect.w = src_w;
	info.src_image[0].clip_rect.h = src_h;
	info.src_image[0].coor.x = 0;
	info.src_image[0].coor.y = 0;
	info.src_image[0].alpha = 0xd0;

	info.src_image[1].alpha = 0x50;
	info.src_image[1].format = G2D_FORMAT_ARGB8888;
	info.src_image[1].mode = G2D_GLOBAL_ALPHA;
	info.src_image[1].width = src2_w;
	info.src_image[1].height = src2_h;
	info.src_image[1].clip_rect.x = 0;
	info.src_image[1].clip_rect.y = 0;
	info.src_image[1].clip_rect.w = src2_w;
	info.src_image[1].clip_rect.h = src2_h;
	info.src_image[1].coor.x = 100;
	info.src_image[1].coor.y = 100;

	info.dst_image.format = G2D_FORMAT_ARGB8888;
	info.dst_image.mode = G2D_GLOBAL_ALPHA;
	info.dst_image.alpha = 0xff;
	info.dst_image.width = X1;
	info.dst_image.height = Y1;
	info.dst_image.clip_rect.x = 0;
	info.dst_image.clip_rect.y = 0;
	info.dst_image.clip_rect.w = X1;
	info.dst_image.clip_rect.h = Y1;


	printf("start open file\n");
	file[0] = fopen("/mnt/E/src_800x480_rgb.bin", "rb+");
	if (file[0] == NULL) {
		__err("err in fopen src file");
		ret = -1;
		goto error;
	}

	ret = fread((void*)src, IMG_SIZE1 * 4, 1, file[0]);

	printf("fread,ret=%d\n", ret);

	fclose(file[0]);

	file[1] = fopen("/mnt/E/bike_480x320_150.bin", "rb+");
	if (file[1] == NULL) {
		__err("err in fopen src2 file");
		ret = -1;
		goto error;
	}

	ret = fread((void*)src2, IMG_SIZE1 * 4, 1, file[1]);

	printf("fread,ret=%d\n", ret);

	fclose(file[1]);

	order++;
	sprintf(name, "/mnt/E/bld_test_%d.bin", order);
	file[2] = fopen(name, "wb+");
	if (file[2] == NULL)
		printf("err in fopen dst file\n");

	printf("start control G2D_CMD_BLD_H \n");

	//we use hal_malloc for iamge input,so we need to make sure caches flushed
	hal_dcache_clean((unsigned long)src, IMG_SIZE1 * 4);
	hal_dcache_clean((unsigned long)src2, IMG_SIZE1 * 4);
	hal_dcache_clean((unsigned long)dst, IMG_SIZE4 * 4);

	printf("start control\n");

	ret = rt_device_control(dev, G2D_CMD_BLD_H, &info);
	if (ret) {
		printf("g2d G2D_CMD_BLD_H fail\n");
		ret = -1;
		goto error;
	} else {
		printf("G2D_CMD_BLD_H ok\n");
	}

	printf("output image to file, may use little time\n");

	ret = fwrite(dst, IMG_SIZE4 * 4, 1, file[2]);

	printf("fwrite ,ret=%d\n",ret);

	fclose(file[2]);

	printf("\nthird test finished!\n");

	printf("press ENTER to Next test\n");
	getchar();


	//Test4------------------------------------------------------------------
	// src2 < src < dst. src RGB dst RGB, coor not zero
	src_w = X1;
	src_h = Y1;
	src2_w = X3;
	src2_h = Y3;
	dst_w = X4;
	dst_h = Y4;
	info.src_image[0].laddr[0] = __va_to_pa((uint32_t)src);
	info.src_image[1].laddr[0] = __va_to_pa((uint32_t)src2);
	info.dst_image.laddr[0] = __va_to_pa((uint32_t)dst);

	info.src_image[0].laddr[0] = (unsigned int)(info.src_image[0].laddr[0]);
	info.src_image[0].laddr[1] = (unsigned int)(info.src_image[0].laddr[0] + src_w * src_h);
	info.src_image[0].laddr[2] = (unsigned int)(info.src_image[0].laddr[0] + src_w * src_h * 5 / 4);
	info.src_image[0].use_phy_addr = 1;

	info.src_image[1].laddr[0] = (unsigned int)(info.src_image[1].laddr[0]);
	info.src_image[1].laddr[1] = (unsigned int)(info.src_image[0].laddr[0] + src2_w * src2_h);
	info.src_image[1].laddr[2] = (unsigned int)(info.src_image[0].laddr[0] + src2_w * src2_h * 5 / 4);
	info.src_image[1].use_phy_addr = 1;

	info.dst_image.laddr[0] = (unsigned int)(info.dst_image.laddr[0]);
	info.dst_image.laddr[1] = 0;
	info.dst_image.laddr[2] = 0;
	info.dst_image.use_phy_addr = 1;

	info.bld_cmd = G2D_BLD_SRCOVER;

	info.src_image[0].format = G2D_FORMAT_ARGB8888;
	info.src_image[0].mode = G2D_GLOBAL_ALPHA;
	info.src_image[0].width = src_w;
	info.src_image[0].height = src_h;
	info.src_image[0].clip_rect.x = 0;
	info.src_image[0].clip_rect.y = 0;
	info.src_image[0].clip_rect.w = src_w;
	info.src_image[0].clip_rect.h = src_h;
	info.src_image[0].coor.x = 100;
	info.src_image[0].coor.y = 100;
	info.src_image[0].alpha = 0xd0;

	info.src_image[1].alpha = 0x50;
	info.src_image[1].format = G2D_FORMAT_ARGB8888;
	info.src_image[1].mode = G2D_GLOBAL_ALPHA;
	info.src_image[1].width = src2_w;
	info.src_image[1].height = src2_h;
	info.src_image[1].clip_rect.x = 0;
	info.src_image[1].clip_rect.y = 0;
	info.src_image[1].clip_rect.w = src2_w;
	info.src_image[1].clip_rect.h = src2_h;
	info.src_image[1].coor.x = 150;
	info.src_image[1].coor.y = 120;

	info.dst_image.format = G2D_FORMAT_ARGB8888;
	info.dst_image.mode = G2D_GLOBAL_ALPHA;
	info.dst_image.alpha = 0xff;
	info.dst_image.width = dst_w;
	info.dst_image.height = dst_h;
	info.dst_image.clip_rect.x = 0;
	info.dst_image.clip_rect.y = 0;
	info.dst_image.clip_rect.w = dst_w;
	info.dst_image.clip_rect.h = dst_h;


	printf("start open file\n");
	file[0] = fopen("/mnt/E/src_800x480_rgb.bin", "rb+");
	if (file[0] == NULL) {
		__err("err in fopen src file");
		ret = -1;
		goto error;
	}

	ret = fread((void*)src, IMG_SIZE1 * 4, 1, file[0]);

	printf("fread,ret=%d\n", ret);

	fclose(file[0]);

	file[1] = fopen("/mnt/E/bike_480x320_150.bin", "rb+");
	if (file[1] == NULL) {
		__err("err in fopen src2 file");
		ret = -1;
		goto error;
	}

	ret = fread((void*)src2, IMG_SIZE1 * 4, 1, file[1]);

	printf("fread,ret=%d\n", ret);

	fclose(file[1]);

	order++;
	sprintf(name, "/mnt/E/bld_test_%d.bin", order);
	file[2] = fopen(name, "wb+");
	if (file[2] == NULL)
		printf("err in fopen dst file\n");

	printf("start control G2D_CMD_BLD_H \n");

	//we use hal_malloc for iamge input,so we need to make sure caches flushed
	hal_dcache_clean((unsigned long)src, IMG_SIZE1 * 4);
	hal_dcache_clean((unsigned long)src2, IMG_SIZE1 * 4);
	hal_dcache_clean((unsigned long)dst, IMG_SIZE4 * 4);

	printf("start control\n");

	ret = rt_device_control(dev, G2D_CMD_BLD_H, &info);
	if (ret) {
		printf("g2d G2D_CMD_BLD_H fail\n");
		ret = -1;
		goto error;
	} else {
		printf("G2D_CMD_BLD_H ok\n");
	}

	printf("output image to file, may use little time\n");

	ret = fwrite(dst, IMG_SIZE4 * 4, 1, file[2]);

	printf("fwrite ,ret=%d\n",ret);

	fclose(file[2]);

	printf("\nforth test finished!\n");

	printf("press ENTER to Next test5\n");
	getchar();

	//Test5------------------------------------------------------------------
	// src < src2 < dst. src YUV dst RGB, coor not zero
	src_w = X1;
	src_h = Y1;
	src2_w = X5;
	src2_h = Y5;
	dst_w = X4;
	dst_h = Y4;
	info.src_image[0].laddr[0] = __va_to_pa((uint32_t)src);
	info.src_image[1].laddr[0] = __va_to_pa((uint32_t)src2);
	info.dst_image.laddr[0] = __va_to_pa((uint32_t)dst);

	info.src_image[0].laddr[0] = (unsigned int)(info.src_image[0].laddr[0]);
	info.src_image[0].laddr[1] = (unsigned int)(info.src_image[0].laddr[0] + src_w * src_h);
	info.src_image[0].laddr[2] = (unsigned int)(info.src_image[0].laddr[0] + src_w * src_h * 5 / 4);
	info.src_image[0].use_phy_addr = 1;

	info.src_image[1].laddr[0] = (unsigned int)(info.src_image[1].laddr[0]);
	info.src_image[1].laddr[1] = (unsigned int)(info.src_image[1].laddr[0] + src2_w * src2_h);
	info.src_image[1].laddr[2] = (unsigned int)(info.src_image[1].laddr[0] + src2_w * src2_h * 5 / 4);
	info.src_image[1].use_phy_addr = 1;

	info.dst_image.laddr[0] = (unsigned int)(info.dst_image.laddr[0]);
	info.dst_image.laddr[1] = (unsigned int)(info.dst_image.laddr[0] + dst_w * dst_h);
	info.dst_image.laddr[2] = (unsigned int)(info.dst_image.laddr[0] + dst_w * dst_h * 5 / 4);
	info.dst_image.use_phy_addr = 1;

	info.bld_cmd = G2D_BLD_SRCOVER;

	info.src_image[0].format = G2D_FORMAT_YUV420UVC_U1V1U0V0;
	info.src_image[0].mode = G2D_GLOBAL_ALPHA;
	info.src_image[0].width = src_w;
	info.src_image[0].height = src_h;
	info.src_image[0].clip_rect.x = 0;
	info.src_image[0].clip_rect.y = 0;
	info.src_image[0].clip_rect.w = src_w;
	info.src_image[0].clip_rect.h = src_h;
	info.src_image[0].coor.x = 50;
	info.src_image[0].coor.y = 50;
	info.src_image[0].alpha = 0xd0;

	info.src_image[1].alpha = 0x50;
	info.src_image[1].format = G2D_FORMAT_ARGB8888;
	info.src_image[1].mode = G2D_GLOBAL_ALPHA;
	info.src_image[1].width = src2_w;
	info.src_image[1].height = src2_h;
	info.src_image[1].clip_rect.x = 0;
	info.src_image[1].clip_rect.y = 0;
	info.src_image[1].clip_rect.w = src2_w;
	info.src_image[1].clip_rect.h = src2_h;
	info.src_image[1].coor.x = 100;
	info.src_image[1].coor.y = 100;

	info.dst_image.format = G2D_FORMAT_YUV420UVC_U1V1U0V0;
	info.dst_image.mode = G2D_GLOBAL_ALPHA;
	info.dst_image.alpha = 0xff;
	info.dst_image.width = dst_w;
	info.dst_image.height = dst_h;
	info.dst_image.clip_rect.x = 0;
	info.dst_image.clip_rect.y = 0;
	info.dst_image.clip_rect.w = dst_w;
	info.dst_image.clip_rect.h = dst_h;


	printf("start open file\n");
	file[0] = fopen("/mnt/E/memin-800x480-220.bin", "rb+");
	if (file[0] == NULL) {
		__err("err in fopen src file");
		ret = -1;
		goto error;
	}

	ret = fread((void*)src, IMG_SIZE1 * 4, 1, file[0]);

	printf("fread,ret=%d\n", ret);

	fclose(file[0]);

	file[1] = fopen("/mnt/E/333_333_rgb.bin", "rb+");
	if (file[1] == NULL) {
		__err("err in fopen src2 file");
		ret = -1;
		goto error;
	}

	ret = fread((void*)src2, IMG_SIZE1 * 4, 1, file[1]);

	printf("fread,ret=%d\n", ret);

	fclose(file[1]);

	order++;
	sprintf(name, "/mnt/E/bld_test_%d.bin", order);
	file[2] = fopen(name, "wb+");
	if (file[2] == NULL)
		printf("err in fopen dst file\n");

	printf("start control G2D_CMD_BLD_H \n");

	//we use hal_malloc for iamge input,so we need to make sure caches flushed
	hal_dcache_clean((unsigned long)src, IMG_SIZE1 * 4);
	hal_dcache_clean((unsigned long)src2, IMG_SIZE1 * 4);
	hal_dcache_clean((unsigned long)dst, IMG_SIZE4 * 4);

	printf("start control\n");

	ret = rt_device_control(dev, G2D_CMD_BLD_H, &info);
	if (ret) {
		printf("g2d G2D_CMD_BLD_H fail\n");
		ret = -1;
		goto error;
	} else {
		printf("G2D_CMD_BLD_H ok\n");
	}

	printf("output image to file, may use little time\n");

	ret = fwrite(dst, IMG_SIZE4 * 4, 1, file[2]);

	printf("fwrite ,ret=%d\n",ret);

	fclose(file[2]);

	printf("\nfifth test finished!\n");

	printf("press ENTER to Next test6\n");
	getchar();

	//Test6------------------------------------------------------------------
	// src2 < src < dst. src RGB dst RGB, coor not zero clip not zero
	src_w = X1;
	src_h = Y1;
	src2_w = X3;
	src2_h = Y3;
	dst_w = X4;
	dst_h = Y4;
	info.src_image[0].laddr[0] = __va_to_pa((uint32_t)src);
	info.src_image[1].laddr[0] = __va_to_pa((uint32_t)src2);
	info.dst_image.laddr[0] = __va_to_pa((uint32_t)dst);

	info.src_image[0].laddr[0] = (unsigned int)(info.src_image[0].laddr[0]);
	info.src_image[0].laddr[1] = (unsigned int)(info.src_image[0].laddr[0] + src_w * src_h);
	info.src_image[0].laddr[2] = (unsigned int)(info.src_image[0].laddr[0] + src_w * src_h * 5 / 4);
	info.src_image[0].use_phy_addr = 1;

	info.src_image[1].laddr[0] = (unsigned int)(info.src_image[1].laddr[0]);
	info.src_image[1].laddr[1] = (unsigned int)(info.src_image[0].laddr[0] + src2_w * src2_h);
	info.src_image[1].laddr[2] = (unsigned int)(info.src_image[0].laddr[0] + src2_w * src2_h * 5 / 4);
	info.src_image[1].use_phy_addr = 1;

	info.dst_image.laddr[0] = (unsigned int)(info.dst_image.laddr[0]);
	info.dst_image.laddr[1] = 0;
	info.dst_image.laddr[2] = 0;
	info.dst_image.use_phy_addr = 1;

	info.bld_cmd = G2D_BLD_SRCOVER;

	info.src_image[0].format = G2D_FORMAT_ARGB8888;
	info.src_image[0].mode = G2D_GLOBAL_ALPHA;
	info.src_image[0].width = src_w;
	info.src_image[0].height = src_h;
	info.src_image[0].clip_rect.x = 50;
	info.src_image[0].clip_rect.y = 50;
	info.src_image[0].clip_rect.w = src_w;
	info.src_image[0].clip_rect.h = src_h;
	info.src_image[0].coor.x = 100;
	info.src_image[0].coor.y = 100;
	info.src_image[0].alpha = 0xd0;

	info.src_image[1].alpha = 0x50;
	info.src_image[1].format = G2D_FORMAT_ARGB8888;
	info.src_image[1].mode = G2D_GLOBAL_ALPHA;
	info.src_image[1].width = src2_w;
	info.src_image[1].height = src2_h;
	info.src_image[1].clip_rect.x = 50;
	info.src_image[1].clip_rect.y = 50;
	info.src_image[1].clip_rect.w = src2_w;
	info.src_image[1].clip_rect.h = src2_h;
	info.src_image[1].coor.x = 150;
	info.src_image[1].coor.y = 120;

	info.dst_image.format = G2D_FORMAT_ARGB8888;
	info.dst_image.mode = G2D_GLOBAL_ALPHA;
	info.dst_image.alpha = 0xff;
	info.dst_image.width = dst_w;
	info.dst_image.height = dst_h;
	info.dst_image.clip_rect.x = 200;
	info.dst_image.clip_rect.y = 400;
	info.dst_image.clip_rect.w = dst_w;
	info.dst_image.clip_rect.h = dst_h;


	printf("start open file\n");
	file[0] = fopen("/mnt/E/src_800x480_rgb.bin", "rb+");
	if (file[0] == NULL) {
		__err("err in fopen src file");
		ret = -1;
		goto error;
	}

	ret = fread((void*)src, IMG_SIZE1 * 4, 1, file[0]);

	printf("fread,ret=%d\n", ret);

	fclose(file[0]);

	file[1] = fopen("/mnt/E/bike_480x320_150.bin", "rb+");
	if (file[1] == NULL) {
		__err("err in fopen src2 file");
		ret = -1;
		goto error;
	}

	ret = fread((void*)src2, IMG_SIZE1 * 4, 1, file[1]);

	printf("fread,ret=%d\n", ret);

	fclose(file[1]);

	order++;
	sprintf(name, "/mnt/E/bld_test_%d.bin", order);
	file[2] = fopen(name, "wb+");
	if (file[2] == NULL)
		printf("err in fopen dst file\n");

	printf("start control G2D_CMD_BLD_H \n");

	//we use hal_malloc for iamge input,so we need to make sure caches flushed
	hal_dcache_clean((unsigned long)src, IMG_SIZE1 * 4);
	hal_dcache_clean((unsigned long)src2, IMG_SIZE1 * 4);
	hal_dcache_clean((unsigned long)dst, IMG_SIZE4 * 4);

	printf("start control\n");

	ret = rt_device_control(dev, G2D_CMD_BLD_H, &info);
	if (ret) {
		printf("g2d G2D_CMD_BLD_H fail\n");
		ret = -1;
		goto error;
	} else {
		printf("G2D_CMD_BLD_H ok\n");
	}

	printf("output image to file, may use little time\n");

	ret = fwrite(dst, IMG_SIZE4 * 4, 1, file[2]);

	printf("fwrite ,ret=%d\n",ret);

	fclose(file[2]);

	printf("\nsixth test finished!\n");

	printf("press ENTER to Next test\n");
	getchar();

	//Test7------------------------------------------------------------------
	// src < src2 < dst. src RGB dst RGB, coor not zero clip not zero
	src_w = X5;
	src_h = Y5;
	src2_w = X1;
	src2_h = Y1;
	dst_w = X4;
	dst_h = Y4;
	info.src_image[0].laddr[0] = __va_to_pa((uint32_t)src);
	info.src_image[1].laddr[0] = __va_to_pa((uint32_t)src2);
	info.dst_image.laddr[0] = __va_to_pa((uint32_t)dst);

	info.src_image[0].laddr[0] = (unsigned int)(info.src_image[0].laddr[0]);
	info.src_image[0].laddr[1] = (unsigned int)(info.src_image[0].laddr[0] + src_w * src_h);
	info.src_image[0].laddr[2] = (unsigned int)(info.src_image[0].laddr[0] + src_w * src_h * 5 / 4);
	info.src_image[0].use_phy_addr = 1;

	info.src_image[1].laddr[0] = (unsigned int)(info.src_image[1].laddr[0]);
	info.src_image[1].laddr[1] = (unsigned int)(info.src_image[0].laddr[0] + src2_w * src2_h);
	info.src_image[1].laddr[2] = (unsigned int)(info.src_image[0].laddr[0] + src2_w * src2_h * 5 / 4);
	info.src_image[1].use_phy_addr = 1;

	info.dst_image.laddr[0] = (unsigned int)(info.dst_image.laddr[0]);
	info.dst_image.laddr[1] = 0;
	info.dst_image.laddr[2] = 0;
	info.dst_image.use_phy_addr = 1;

	info.bld_cmd = G2D_BLD_SRCOVER;

	info.src_image[0].format = G2D_FORMAT_ARGB8888;
	info.src_image[0].mode = G2D_GLOBAL_ALPHA;
	info.src_image[0].width = src_w;
	info.src_image[0].height = src_h;
	info.src_image[0].clip_rect.x = 50;
	info.src_image[0].clip_rect.y = 50;
	info.src_image[0].clip_rect.w = src_w;
	info.src_image[0].clip_rect.h = src_h;
	info.src_image[0].coor.x = 100;
	info.src_image[0].coor.y = 100;
	info.src_image[0].alpha = 0xd0;

	info.src_image[1].alpha = 0x50;
	info.src_image[1].format = G2D_FORMAT_ARGB8888;
	info.src_image[1].mode = G2D_GLOBAL_ALPHA;
	info.src_image[1].width = src2_w;
	info.src_image[1].height = src2_h;
	info.src_image[1].clip_rect.x = 50;
	info.src_image[1].clip_rect.y = 50;
	info.src_image[1].clip_rect.w = src2_w;
	info.src_image[1].clip_rect.h = src2_h;
	info.src_image[1].coor.x = 150;
	info.src_image[1].coor.y = 120;

	info.dst_image.format = G2D_FORMAT_ARGB8888;
	info.dst_image.mode = G2D_GLOBAL_ALPHA;
	info.dst_image.alpha = 0xff;
	info.dst_image.width = dst_w;
	info.dst_image.height = dst_h;
	info.dst_image.clip_rect.x = 0;
	info.dst_image.clip_rect.y = 0;
	info.dst_image.clip_rect.w = dst_w;
	info.dst_image.clip_rect.h = dst_h;


	printf("start open file\n");
	file[0] = fopen("/mnt/E/333_333_rgb.bin", "rb+");
	if (file[0] == NULL) {
		__err("err in fopen src file");
		ret = -1;
		goto error;
	}

	ret = fread((void*)src, IMG_SIZE1 * 4, 1, file[0]);

	printf("fread,ret=%d\n", ret);

	fclose(file[0]);

	file[1] = fopen("/mnt/E/src_800x480_rgb.bin", "rb+");
	if (file[1] == NULL) {
		__err("err in fopen src2 file");
		ret = -1;
		goto error;
	}

	ret = fread((void*)src2, IMG_SIZE1 * 4, 1, file[1]);

	printf("fread,ret=%d\n", ret);

	fclose(file[1]);

	order++;
	sprintf(name, "/mnt/E/bld_test_%d.bin", order);
	file[2] = fopen(name, "wb+");
	if (file[2] == NULL)
		printf("err in fopen dst file\n");

	printf("start control G2D_CMD_BLD_H \n");

	//we use hal_malloc for iamge input,so we need to make sure caches flushed
	hal_dcache_clean((unsigned long)src, IMG_SIZE1 * 4);
	hal_dcache_clean((unsigned long)src2, IMG_SIZE1 * 4);
	hal_dcache_clean((unsigned long)dst, IMG_SIZE4 * 4);

	printf("start control\n");

	ret = rt_device_control(dev, G2D_CMD_BLD_H, &info);
	if (ret) {
		printf("g2d G2D_CMD_BLD_H fail\n");
		ret = -1;
		goto error;
	} else {
		printf("G2D_CMD_BLD_H ok\n");
	}

	printf("output image to file, may use little time\n");

	ret = fwrite(dst, IMG_SIZE4 * 4, 1, file[2]);

	printf("fwrite ,ret=%d\n",ret);

	fclose(file[2]);

	printf("\nseventh test finished!\n");

	printf("press ENTER to Next test\n");
	getchar();

	//Test8------------------------------------------------------------------
	// src < src2 < dst. src RGB dst YUV, coor not zero clip not zero
	src_w = X5;
	src_h = Y5;
	src2_w = X1;
	src2_h = Y1;
	dst_w = X4;
	dst_h = Y4;
	info.src_image[0].laddr[0] = __va_to_pa((uint32_t)src);
	info.src_image[1].laddr[0] = __va_to_pa((uint32_t)src2);
	info.dst_image.laddr[0] = __va_to_pa((uint32_t)dst);

	info.src_image[0].laddr[0] = (unsigned int)(info.src_image[0].laddr[0]);
	info.src_image[0].laddr[1] = (unsigned int)(info.src_image[0].laddr[0] + src_w * src_h);
	info.src_image[0].laddr[2] = (unsigned int)(info.src_image[0].laddr[0] + src_w * src_h * 5 / 4);
	info.src_image[0].use_phy_addr = 1;

	info.src_image[1].laddr[0] = (unsigned int)(info.src_image[1].laddr[0]);
	info.src_image[1].laddr[1] = (unsigned int)(info.src_image[0].laddr[0] + src2_w * src2_h);
	info.src_image[1].laddr[2] = (unsigned int)(info.src_image[0].laddr[0] + src2_w * src2_h * 5 / 4);
	info.src_image[1].use_phy_addr = 1;

	info.dst_image.laddr[0] = (unsigned int)(info.dst_image.laddr[0]);
	info.dst_image.laddr[1] = (unsigned int)(info.dst_image.laddr[0] + dst_w * dst_h);
	info.dst_image.laddr[2] = (unsigned int)(info.dst_image.laddr[0] + dst_w * dst_h * 5 / 4);
	info.dst_image.use_phy_addr = 1;

	info.bld_cmd = G2D_BLD_SRCOVER;

	info.src_image[0].format = G2D_FORMAT_ARGB8888;
	info.src_image[0].mode = G2D_GLOBAL_ALPHA;
	info.src_image[0].width = src_w;
	info.src_image[0].height = src_h;
	info.src_image[0].clip_rect.x = 50;
	info.src_image[0].clip_rect.y = 50;
	info.src_image[0].clip_rect.w = src_w;
	info.src_image[0].clip_rect.h = src_h;
	info.src_image[0].coor.x = 100;
	info.src_image[0].coor.y = 100;
	info.src_image[0].alpha = 0xd0;

	info.src_image[1].alpha = 0x50;
	info.src_image[1].format = G2D_FORMAT_ARGB8888;
	info.src_image[1].mode = G2D_GLOBAL_ALPHA;
	info.src_image[1].width = src2_w;
	info.src_image[1].height = src2_h;
	info.src_image[1].clip_rect.x = 50;
	info.src_image[1].clip_rect.y = 50;
	info.src_image[1].clip_rect.w = src2_w;
	info.src_image[1].clip_rect.h = src2_h;
	info.src_image[1].coor.x = 150;
	info.src_image[1].coor.y = 120;

	info.dst_image.format = G2D_FORMAT_YUV420UVC_U1V1U0V0;
	info.dst_image.mode = G2D_GLOBAL_ALPHA;
	info.dst_image.alpha = 0xff;
	info.dst_image.width = dst_w;
	info.dst_image.height = dst_h;
	info.dst_image.clip_rect.x = 0;
	info.dst_image.clip_rect.y = 0;
	info.dst_image.clip_rect.w = dst_w;
	info.dst_image.clip_rect.h = dst_h;


	printf("start open file\n");
	file[0] = fopen("/mnt/E/333_333_rgb.bin", "rb+");
	if (file[0] == NULL) {
		__err("err in fopen src file");
		ret = -1;
		goto error;
	}

	ret = fread((void*)src, IMG_SIZE1 * 4, 1, file[0]);

	printf("fread,ret=%d\n", ret);

	fclose(file[0]);

	file[1] = fopen("/mnt/E/src_800x480_rgb.bin", "rb+");
	if (file[1] == NULL) {
		__err("err in fopen src2 file");
		ret = -1;
		goto error;
	}

	ret = fread((void*)src2, IMG_SIZE1 * 4, 1, file[1]);

	printf("fread,ret=%d\n", ret);

	fclose(file[1]);

	order++;
	sprintf(name, "/mnt/E/bld_test_%d.bin", order);
	file[2] = fopen(name, "wb+");
	if (file[2] == NULL)
		printf("err in fopen dst file\n");

	printf("start control G2D_CMD_BLD_H \n");

	//we use hal_malloc for iamge input,so we need to make sure caches flushed
	hal_dcache_clean((unsigned long)src, IMG_SIZE1 * 4);
	hal_dcache_clean((unsigned long)src2, IMG_SIZE1 * 4);
	hal_dcache_clean((unsigned long)dst, IMG_SIZE4 * 4);

	printf("start control\n");

	ret = rt_device_control(dev, G2D_CMD_BLD_H, &info);
	if (ret) {
		printf("g2d G2D_CMD_BLD_H fail\n");
		ret = -1;
		goto error;
	} else {
		printf("G2D_CMD_BLD_H ok\n");
	}

	printf("output image to file, may use little time\n");

	ret = fwrite(dst, IMG_SIZE4 * 4, 1, file[2]);

	printf("fwrite ,ret=%d\n",ret);

	fclose(file[2]);

	printf("\neighth test finished!\n");

	free(src);
	free(src2);
	free(dst);
	rt_device_close(dev);
	dev = NULL;
out:
	return 0;
error:
	return ret;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_g2d_test_blend, __cmd_g2d_test_blend, g2d_test_blend);
