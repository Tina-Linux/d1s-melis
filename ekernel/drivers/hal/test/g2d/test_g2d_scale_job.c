#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <hal_mem.h>
#include <hal_cmd.h>
#include "../source/g2d_rcq/g2d_driver.h"
extern int sunxi_g2d_control(int cmd, void *arg);
extern int sunxi_g2d_close(void);
extern int sunxi_g2d_open(void);
#define IMG_SIZE1 480*320
#define X1 480
#define Y1 320
#define IMG_SIZE2 320*240
#define X2 320
#define Y2 240
#define IMG_SIZE3 960*640
#define X3 960
#define Y3 640

void hal_dcache_clean(unsigned long vaddr_start, unsigned long size);
void hal_dcache_invalidate(unsigned long vaddr_start, unsigned long size);

static int init_flag;
static int cmd_g2d_test_scale(int argc, const char **argv)
{
    	int ret;
	unsigned long arg[6] = {0};
	void *buf1 = NULL,*buf2 = NULL,*buf3 = NULL,*buf4 = NULL,*buf5 = NULL,*buf6 = NULL;
	char *temp;
	FILE* file1;
	int fb_width, fb_height;
	g2d_blt_h info;
	struct mixer_para info2[2];
	g2d_blt_h blit_para;
	unsigned long i;

	printf("hello g2d_test\n");

	ret = sunxi_g2d_open();
	if (ret){
		printf("g2d open fail\n");
		return -1;
	}

	printf("press ENTER to start test\n");
	getchar();
	memset(&info2, 0, 2*sizeof(struct mixer_para));
	memset(&info, 0, sizeof(g2d_blt_h));

	//note:the tests follow need three  input image file
	buf1 = hal_malloc(IMG_SIZE1*3/2);
	buf2 = hal_malloc(IMG_SIZE1*3/2*4);
	buf3 = hal_malloc(IMG_SIZE2*3/2);
	buf4 = hal_malloc(IMG_SIZE2*3/2/4);
	buf5 = hal_malloc(IMG_SIZE3*3/2);
	buf6 = hal_malloc(IMG_SIZE3*3/2/16);


	if(buf1 == NULL || buf2 == NULL || buf3 == NULL || buf4 == NULL || buf5 == NULL || buf6 == NULL)
	{
		printf("fatal error, buf is null.\n");
		ret = -1;
		sunxi_g2d_close();
		return ret;
	}

	memset(buf1, 0, IMG_SIZE1*3/2);
	memset(buf2, 0, IMG_SIZE1*3/2*4);
	memset(buf3, 0, IMG_SIZE2*3/2);
	memset(buf4, 0, IMG_SIZE2*3/2/4);
	memset(buf5, 0, IMG_SIZE3*3/2);
	memset(buf6, 0, IMG_SIZE3*3/2/16);

	// use G2D_CMD_MIXER_TASK,task 1:scale G2D_FORMAT_YUV420UVC_U1V1U0V0 from 480*320 to (480*2)*(320*2)
	info2[0].src_image_h.laddr[0] = __va_to_pa((uint32_t)buf1);
	info2[0].dst_image_h.laddr[0] = __va_to_pa((uint32_t)buf2);

	info2[0].src_image_h.laddr[0] = (int)(info2[0].src_image_h.laddr[0] );
	info2[0].src_image_h.laddr[1] = (int)(info2[0].src_image_h.laddr[0] + IMG_SIZE1);
	info2[0].src_image_h.laddr[2] = (int)(info2[0].src_image_h.laddr[0] + IMG_SIZE1*5/4);
	info2[0].src_image_h.use_phy_addr = 1;

	info2[0].dst_image_h.laddr[0] = (int)(info2[0].dst_image_h.laddr[0] );
	info2[0].dst_image_h.laddr[1] = (int)(info2[0].dst_image_h.laddr[0] + IMG_SIZE1*4);
	info2[0].dst_image_h.laddr[2] = (int)(info2[0].dst_image_h.laddr[0] + IMG_SIZE1*4*5/4);
	info2[0].dst_image_h.use_phy_addr = 1;

	info2[0].flag_h = G2D_BLT_NONE_H;
	info2[0].op_flag = OP_BITBLT;
	info2[0].src_image_h.format = G2D_FORMAT_YUV420UVC_U1V1U0V0;
	info2[0].src_image_h.width = X1;
	info2[0].src_image_h.height = Y1;
	info2[0].src_image_h.clip_rect.x = 0;
	info2[0].src_image_h.clip_rect.y = 0;
	info2[0].src_image_h.clip_rect.w = X1;
	info2[0].src_image_h.clip_rect.h = Y1;

	info2[0].dst_image_h.format = G2D_FORMAT_YUV420UVC_U1V1U0V0;
	info2[0].dst_image_h.width = X1*2;
	info2[0].dst_image_h.height = Y1*2;
	info2[0].dst_image_h.clip_rect.x = 0;
	info2[0].dst_image_h.clip_rect.y = 0;
	info2[0].dst_image_h.clip_rect.w = X1*2;
	info2[0].dst_image_h.clip_rect.h = Y1*2;

	// use G2D_CMD_MIXER_TASK,task 2:scale G2D_FORMAT_YUV420_PLANAR from 320*240 to (320/2)*(240/2)
	info2[1].src_image_h.laddr[0] = __va_to_pa((uint32_t)buf3);
	info2[1].dst_image_h.laddr[0] = __va_to_pa((uint32_t)buf4);

	info2[1].src_image_h.laddr[0] = (int)(info2[1].src_image_h.laddr[0] );
	info2[1].src_image_h.laddr[1] = (int)(info2[1].src_image_h.laddr[0] + IMG_SIZE2);
	info2[1].src_image_h.laddr[2] = (int)(info2[1].src_image_h.laddr[0] + IMG_SIZE2*5/4);
	info2[1].src_image_h.use_phy_addr = 1;

	info2[1].dst_image_h.laddr[0] = (int)(info2[1].dst_image_h.laddr[0] );
	info2[1].dst_image_h.laddr[1] = (int)(info2[1].dst_image_h.laddr[0] + IMG_SIZE2/4);
	info2[1].dst_image_h.laddr[2] = (int)(info2[1].dst_image_h.laddr[0] + IMG_SIZE2/4*5/4);
	info2[1].dst_image_h.use_phy_addr = 1;

	info2[1].flag_h = G2D_BLT_NONE_H;
	info2[1].op_flag = OP_BITBLT;
	info2[1].src_image_h.format = G2D_FORMAT_YUV420_PLANAR;
	info2[1].src_image_h.width = X2;
	info2[1].src_image_h.height = Y2;

	info2[1].src_image_h.clip_rect.w = X2;
	info2[1].src_image_h.clip_rect.h = Y2;

	info2[1].dst_image_h.format = G2D_FORMAT_YUV420_PLANAR;
	info2[1].dst_image_h.width = X2/2;
	info2[1].dst_image_h.height = Y2/2;

	info2[1].dst_image_h.clip_rect.w = X2/2;
	info2[1].dst_image_h.clip_rect.h = Y2/2;

	// use G2D_CMD_BITBLT_H to scale G2D_FORMAT_YUV420UVC_V1U1V0U0 from 960*640 to (960/4)*(640/4)
	info.flag_h=G2D_BLT_NONE_H;
	info.src_image_h.use_phy_addr = 1;
	info.src_image_h.format = G2D_FORMAT_YUV420UVC_V1U1V0U0;

	info.src_image_h.width = X3;
	info.src_image_h.height = Y3;
	info.src_image_h.clip_rect.w = X3;
	info.src_image_h.clip_rect.h = Y3;

	info.dst_image_h.use_phy_addr = 1;
	info.dst_image_h.format = G2D_FORMAT_YUV420UVC_V1U1V0U0;
	info.dst_image_h.width = X3/4;
	info.dst_image_h.height = Y3/4;
	info.dst_image_h.clip_rect.w = X3/4;
	info.dst_image_h.clip_rect.h = Y3/4;

	info.src_image_h.laddr[0] = __va_to_pa((uint32_t)buf5);
	info.dst_image_h.laddr[0] = __va_to_pa((uint32_t)buf6);

	info.src_image_h.laddr[0] = (int)(info.src_image_h.laddr[0] );
	info.src_image_h.laddr[1] = (int)(info.src_image_h.laddr[0] + IMG_SIZE3);
	info.src_image_h.laddr[2] = (int)(info.src_image_h.laddr[0] + IMG_SIZE3*5/4);
	info.src_image_h.use_phy_addr = 1;

	info.dst_image_h.laddr[0] = (int)(info.dst_image_h.laddr[0] );
	info.dst_image_h.laddr[1] = (int)(info.dst_image_h.laddr[0] + IMG_SIZE3/16);
	info.dst_image_h.laddr[2] = (int)(info.dst_image_h.laddr[0] + IMG_SIZE3/4/4*5/4);

	info.dst_image_h.color = 0xee8899;
	info.dst_image_h.mode = G2D_PIXEL_ALPHA;
	info.dst_image_h.alpha = 255;
	info.src_image_h.color = 0xee8899;
	info.src_image_h.mode = G2D_PIXEL_ALPHA;
	info.src_image_h.alpha = 255;

	printf("start open file\n");
	file1 = fopen("/mnt/F/bike_480x320_220.bin", "rb+");
	if (file1 == NULL)
		printf("err in fopen");
	ret = fread((void*)buf1, IMG_SIZE1*3/2, 1, file1);
	printf("fread,ret=%d\n", ret);
	fclose(file1);

	file1 = fopen("/mnt/F/bike_320x240_020.bin", "rb+");
	if (file1 == NULL)
		printf("err in fopen");
	ret = fread((void*)buf3, IMG_SIZE2*3/2, 1, file1);
	printf("fread,ret=%d\n", ret);
	fclose(file1);

	file1 = fopen("/mnt/F/bike_960x640_221.bin", "rb+");
	if (file1 == NULL)
		printf("err in fopen");
	ret = fread((void*)buf5, IMG_SIZE3*3/2, 1, file1);
	printf("fread,ret=%d\n", ret);
	fclose(file1);

	printf("start control G2D_CMD_MIXER_TASK \n");

	//we use hal_malloc for iamge input,so we need to make sure caches flushed
	hal_dcache_clean((unsigned long)buf1, IMG_SIZE1*3/2);
	hal_dcache_clean((unsigned long)buf3, IMG_SIZE2*3/2);
	hal_dcache_clean((unsigned long)buf5, IMG_SIZE3*3/2);

	//keep clean to avoid eviction.
	hal_dcache_clean((unsigned long)buf2, IMG_SIZE1*4*3/2);
	hal_dcache_clean((unsigned long)buf4, IMG_SIZE2/4*3/2);
	hal_dcache_clean((unsigned long)buf6, IMG_SIZE3/16*3/2);

	printf("start control\n");
	arg[0] = (unsigned long)(info2);
	arg[1] = 2;
	ret = sunxi_g2d_control(G2D_CMD_MIXER_TASK, arg);
	if (ret)
	{
		printf("g2d G2D_CMD_MIXER_TASK fail\n");
		ret = -1;
		goto out;
	}
	else 
	{
		printf("G2D_CMD_MIXER_TASK ok\n");
	}

	ret = sunxi_g2d_control(G2D_CMD_BITBLT_H, &info);
	if (ret)
	{
		printf("g2d G2D_CMD_BITBLT_H fail\n");
		ret = -1;
		goto out;
	}
	else
	{
		printf("G2D_CMD_BITBLT_H ok\n");
	}
	//after the process of g2d hardware,now we invalidate the caches and get the output image
	hal_dcache_invalidate((unsigned long)buf2, IMG_SIZE1*4*3/2);
	hal_dcache_invalidate((unsigned long)buf4, IMG_SIZE2/4*3/2);
	hal_dcache_invalidate((unsigned long)buf6, IMG_SIZE3/16*3/2);

	printf("------write output image to file, may take a little time--------\n");
	file1 = fopen("/mnt/F/out1.bin", "wb+");
	if (file1 == NULL)
		printf("err in fopen");
	ret = fwrite(buf2,IMG_SIZE1*4*3/2, 1, file1);
	printf("fwrite ,ret=%d\n",ret);
	fclose(file1);

	file1 = fopen("/mnt/F/out2.bin", "wb+");
	if (file1 == NULL)
		printf("err in fopen");
	ret = fwrite(buf4,IMG_SIZE2/4*3/2, 1, file1);
	printf("fwrite ,ret=%d\n",ret);
	fclose(file1);

	file1 = fopen("/mnt/F/out3.bin", "wb+");
	if (file1 == NULL)
		printf("err in fopen");
	ret = fwrite(buf6,IMG_SIZE3/16*3/2, 1, file1);
	printf("fwrite ,ret=%d\n",ret);
	fclose(file1);
	printf("\nfinished\n");

out:
	free(buf1);
	free(buf2);
	free(buf3);
	free(buf4);
	free(buf5);
	free(buf6);
	sunxi_g2d_close();
	return ret;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_g2d_test_scale, __cmd_g2d_test_scale, g2d_test_scale);
