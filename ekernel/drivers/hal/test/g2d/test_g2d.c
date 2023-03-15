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
void hal_dcache_invalidate(unsigned long vaddr_start, unsigned long size);

static int cmd_g2d_test(int argc, const char **argv)
{
	int ret;
	//char *buf1 = NULL,*buf2 = NULL;
	unsigned long buf1 = 0, buf2 =0 ;
	unsigned long phy_addr1 = 0, phy_addr2 = 0;
	char *temp;
	FILE* file1;
	int fb_width, fb_height;

	g2d_fillrect_h info;
	g2d_blt_h blit_para;
	unsigned long i;

	printf("hello g2d_test\n");
	ret = sunxi_g2d_open();
	if (ret) {
		printf("g2d open fail\n");
		return -1;
	}
	// phy_addr1=dma_coherent_alloc(&buf1,1280*720*4);
	buf1 = hal_malloc(1280 * 720 * 4);
	memset(&info, 0, sizeof(g2d_fillrect_h));
	memset((void *)buf1, 0, 1280*720*4);



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
	info.dst_image_h.laddr[0] = __va_to_pa((uint32_t)buf1);//phy_addr1;
	fb_width = info.dst_image_h.width;
	fb_height = info.dst_image_h.height;
	info.dst_image_h.use_phy_addr = 1;

	printf("dst: addr=0x%lx, 0x%lx, 0x%lx, format=0x%x, img w=%ld, h=%ld, rect x=%ld, y=%ld, w=%ld, h=%ld, align=%ld\n\n",
			info.dst_image_h.laddr[0], info.dst_image_h.laddr[1], info.dst_image_h.laddr[2],
			info.dst_image_h.format, info.dst_image_h.width, info.dst_image_h.height,
			info.dst_image_h.clip_rect.x, info.dst_image_h.clip_rect.y,
			info.dst_image_h.clip_rect.w, info.dst_image_h.clip_rect.h,
			info.dst_image_h.align[0]);

	hal_dcache_clean((unsigned long)buf1, 1280 * 720 * 4);
	printf("start control\n");

	ret = sunxi_g2d_control(G2D_CMD_FILLRECT_H, &info);
	if (ret){
		printf("g2d G2D_CMD_FILLRECT_H fail\n");
		sunxi_g2d_close();
		free(buf1);
		return -1;
		}
	else {
		printf("G2D_CMD_FILLRECT_H ok\n");
	}

	temp = (char *)buf1;

	printf("------dump G2D_CMD_FILLRECT_H output --------\n");
	i = 1280;//only dump the first 1280 byte

	while(i--)
	{
		printf("%x ",*temp );
		temp +=1;
		if(i%32==0)
			printf("\n");
	}

	printf("\nfinished\n");
	//hal_free(buf1);
	free(buf1);

	printf("press ENTER to the next test\n");
	getchar();

	//note:the test follow need an input image file 1.bin
	buf1 = hal_malloc(1280*720*4);
	buf2 = hal_malloc(1280*720*4);
//	phy_addr1=dma_coherent_alloc(&buf1,1280*720*3/2);
//	phy_addr2=dma_coherent_alloc(&buf2,1280*720*3/2*4);

	memset(&blit_para, 0, sizeof(blit_para));
	memset((void *)buf1, 0, 1280*720*3/2);
	memset((void *)buf2, 0, 1280*720*3/2);
	blit_para.src_image_h.laddr[0] = __va_to_pa((uint32_t)buf1);//phy_addr1;
	blit_para.dst_image_h.laddr[0] = __va_to_pa((uint32_t)buf2);//phy_addr2;


	blit_para.src_image_h.laddr[0] = (int)(blit_para.src_image_h.laddr[0] );
	blit_para.src_image_h.laddr[1] = (int)(blit_para.src_image_h.laddr[0] + 1280*720);
	blit_para.src_image_h.laddr[2] = (int)(blit_para.src_image_h.laddr[0] + 1280*720*5/4);
	blit_para.src_image_h.use_phy_addr = 1;



	blit_para.dst_image_h.laddr[0] = (int)(blit_para.dst_image_h.laddr[0] );
	blit_para.dst_image_h.laddr[1] = (int)(blit_para.dst_image_h.laddr[0] + 1280*720);
	blit_para.dst_image_h.laddr[2] = (int)(blit_para.dst_image_h.laddr[0] + 1280*720*5/4);
	blit_para.dst_image_h.use_phy_addr = 1;


	blit_para.src_image_h.clip_rect.x = 0;
	blit_para.src_image_h.clip_rect.y = 0;
	blit_para.src_image_h.clip_rect.w = 1280;
	blit_para.src_image_h.clip_rect.h = 720;

	blit_para.dst_image_h.clip_rect.x = 0;
	blit_para.dst_image_h.clip_rect.y = 0;
	blit_para.dst_image_h.clip_rect.w = 1280;
	blit_para.dst_image_h.clip_rect.h = 720;


	blit_para.src_image_h.format = G2D_FORMAT_YUV420_PLANAR;
	blit_para.src_image_h.width = 1280;
	blit_para.src_image_h.height = 720;

	blit_para.dst_image_h.format = G2D_FORMAT_YUV420_PLANAR;
	blit_para.dst_image_h.width = 1280;
	blit_para.dst_image_h.height = 720;

	blit_para.flag_h = G2D_ROT_0;

	printf("start open file\n");
	file1 = fopen("/mnt/F/bike_1280x720_220.bin", "rb+");
	if (file1 == NULL) {
		printf("err in fopen\n");
	ret = -1;
	goto out;
	}
	ret = fread((void*)buf1, 1280*720*3/2, 1, file1);
	printf("fread,ret=%d\n", ret);
	fclose(file1);

	hal_dcache_clean((unsigned long)buf1, 1280 * 720 * 4);
	hal_dcache_clean((unsigned long)buf2, 1280 * 720 * 4);

	printf("start control\n");

	ret = sunxi_g2d_control(G2D_CMD_BITBLT_H, &blit_para);
	if (ret) {
		printf("g2d G2D_CMD_BITBLT_H fail\n");
		ret = -1;
		goto out;
	}
	else {
		printf("G2D_CMD_BITBLT_H ok\n");
	}

	temp = (char *)buf2;

	printf("------dump G2D_CMD_BITBLT_H output--------\n");
	i = 1280;

	while(i--) {//only dump the first 1280 byte
		printf("%x ",*temp );
		temp +=1;
		if(i%32==0)
			printf("\n");
	}
	printf("\nfinished\n");
out:
	free(buf1);
	free(buf2);
	sunxi_g2d_close();
	return ret;

}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_g2d_test, __cmd_g2d_test, g2d_test);

