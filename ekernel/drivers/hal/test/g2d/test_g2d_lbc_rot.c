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
void hal_dcache_clean(unsigned long vaddr_start, unsigned long size);

static int cmd_g2d_test_lbc_rot(int argc, const char **argv)
{
	int ret;
	unsigned long buf1 = 0, buf2 =0 ;
	unsigned long phy_addr1 = 0, phy_addr2 = 0;
	char *temp;
	FILE* file;
	g2d_lbc_rot info;
	int fb_width, fb_height;
	unsigned long i;

	memset(&info, 0, sizeof(g2d_lbc_rot));
	printf("hello g2d_lbc_test\n");
	ret = sunxi_g2d_open();
	if (ret) {
		printf("g2d open fail\n");
		return -1;
	}
	fb_width = 1920;
	fb_height = 1088;

	printf("start open file\n");
	file = fopen("/mnt/E/BasketballDrive_1920x1088_50.bs", "rb+");
	if (file == NULL) {
		printf("err in fopen src file");
		ret = -1;
		return ret;
	}
	buf1=hal_malloc(1920*1088*4);
	memset((void *)buf1, 0, 1920*1088*4);

	buf2=hal_malloc(1920*1088*4);
	memset((void *)buf2, 0, 1920*1088*4);

	ret = fread((void*)buf1, 1920, 1088*4, file);

	printf("fread,ret=%d\n", ret);

	fclose(file);

/*
	info.dst_image_h.format = G2D_FORMAT_ARGB8888;
	info.dst_image_h.width = 1280;
	info.dst_image_h.height = 720;
	info.dst_image_h.clip_rect.x = 0;
	info.dst_image_h.clip_rect.y = 0;
	info.dst_image_h.clip_rect.w = 1280;
	info.dst_image_h.clip_rect.h = 720;
	info.dst_image_h.color = 0xff0000ff;
	info.dst_image_h.mode = 1;
	info.dst_image_h.alpha = 255;
	info.dst_image_h.laddr[0] = phy_addr1;//__va_to_pa((uint32_t)buf1);
	fb_width = info.dst_image_h.width;
	fb_height = info.dst_image_h.height;
*/
	info.blt.flag_h = 256;/*rotate 90*/
	info.lbc_cmp_ratio = 400;
	info.enc_is_lossy = 1;
	info.dec_is_lossy = 1;

	info.blt.src_image_h.format = 42;
	info.blt.src_image_h.width = 1920;
	info.blt.src_image_h.height = 1088;
	info.blt.src_image_h.clip_rect.x = 0;
	info.blt.src_image_h.clip_rect.y = 0;
	info.blt.src_image_h.clip_rect.w = 1920;
	info.blt.src_image_h.clip_rect.h = 1088;
	info.blt.src_image_h.use_phy_addr = 1;

	info.blt.dst_image_h.format = 42;
	info.blt.dst_image_h.width = 1920;
	info.blt.dst_image_h.height = 1088;
	info.blt.dst_image_h.clip_rect.x = 0;
	info.blt.dst_image_h.clip_rect.y = 0;
	info.blt.dst_image_h.clip_rect.w = 1920;
	info.blt.dst_image_h.clip_rect.h = 1088;
	info.blt.dst_image_h.use_phy_addr = 1;

	info.blt.src_image_h.laddr[0] = buf1;
	info.blt.src_image_h.laddr[1] = (int)(info.blt.src_image_h.laddr[0] + fb_width*fb_height);
	info.blt.src_image_h.laddr[2] = (int)(info.blt.src_image_h.laddr[0] + fb_width*fb_height*5/4);

	info.blt.dst_image_h.laddr[0] = buf2;
	info.blt.dst_image_h.laddr[1] = (int)(info.blt.dst_image_h.laddr[0] + fb_width*fb_height);
	info.blt.dst_image_h.laddr[2] = (int)(info.blt.dst_image_h.laddr[0] + fb_width*fb_height*5/4);

	printf("dst: addr=0x%lx, 0x%lx, 0x%lx, format=0x%x, img w=%ld, h=%ld, rect x=%ld, y=%ld, w=%ld, h=%ld, align=%ld\n\n",
			info.blt.dst_image_h.laddr[0], info.blt.dst_image_h.laddr[1], info.blt.dst_image_h.laddr[2],
			info.blt.dst_image_h.format, info.blt.dst_image_h.width, info.blt.dst_image_h.height,
			info.blt.dst_image_h.clip_rect.x, info.blt.dst_image_h.clip_rect.y,
			info.blt.dst_image_h.clip_rect.w, info.blt.dst_image_h.clip_rect.h,
			info.blt.dst_image_h.align[0]);

	hal_dcache_clean((unsigned long)buf1, 1920 * 1088 * 4);
	hal_dcache_clean((unsigned long)buf2, 1920 * 1088 * 4);

	printf("start control\n");

	if(sunxi_g2d_control(G2D_CMD_LBC_ROT ,(unsigned long)(&info)) < 0)
	{
		printf("[%d][%s][%s]G2D_CMD_LBC_ROT failure!\n",__LINE__, __FILE__,__FUNCTION__);
		sunxi_g2d_close();
		free(buf1);
		free(buf2);
		return -1;
	}
	printf("[%d][%s][%s]G2D_CMD_LBC_ROT Successful!\n",__LINE__, __FILE__,__FUNCTION__);

	printf("start open file\n");
	file = fopen("/mnt/E/lbc_output.bs", "wb+");
	if (file == NULL) {
		printf("err in fopen src file");
		ret = -1;
	}

	printf("writing file...\n");
	ret = fwrite((void*)buf2, 1920, 1088*4, file);

	printf("fwrite,ret=%d\n", ret);

	fclose(file);
	free(buf2);
	free(buf1);
	sunxi_g2d_close();


	printf("\nfinished, please get output file  /mnt/E/lbc_output.bs\n");

	return ret;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_g2d_test_lbc_rot, __cmd_g2d_test_lbc_rot, g2d_test_lbc_rot);

