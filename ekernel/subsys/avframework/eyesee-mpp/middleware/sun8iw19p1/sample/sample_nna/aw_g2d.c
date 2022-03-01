/*
 * =====================================================================================
 *
 *       Filename:  aw_g2d.c
 *
 *    Description:  use for scale picture
 *
 *        Version:  1.0
 *        Created:  07/23/2019 11:21:26 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jilinglin
 *        Company:  allwinnertech.com
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "aw_g2d.h"
AwG2d* AwG2dCreate()
{
    printf("AwG2dCreate!\n");
    AwG2d* p = (AwG2d*)malloc(sizeof(AwG2d));
    if(p == NULL)
        return NULL;

    p->g2dFd = open("/dev/g2d",O_RDWR);
    if(p->g2dFd == -1) {
        printf("open g2d device fail!\n");
        free(p);
        return NULL;
    }
    p->g2dInfo = NULL;
    return p;
}

int AwG2dInit(AwG2d* self, AwG2dParam param)
{
    struct mixer_para* g2d_info = NULL;
    ScaleInfo* scale_info = param.scaleinfo;
    int fb_width, fb_height;

    if(self == NULL)
        return -1;

    self->g2dInfoNum = param.g2dTotalNum;
    if(self->g2dInfo == NULL){
        g2d_info = (struct mixer_para*)malloc(sizeof(*g2d_info)*param.g2dTotalNum);
        if(g2d_info == NULL){
            printf("malloc failed!\n");
        }
        self->g2dInfo = g2d_info;
    }else{
        printf("g2d info is not null please check\n");
    }
    //for scale
    for(int idx=0; idx<param.g2dTotalNum; idx++){
    
        g2d_info[idx].flag_h = G2D_BLT_NONE_H;
        g2d_info[idx].op_flag = OP_BITBLT;

		g2d_info[idx].src_image_h.format = (g2d_fmt_enh)scale_info[idx].src.format;
		g2d_info[idx].dst_image_h.format = (g2d_fmt_enh)scale_info[idx].dst.format;
    	g2d_info[idx].src_image_h.width  = scale_info[idx].src.w;
    	g2d_info[idx].src_image_h.height = scale_info[idx].src.h;
    	g2d_info[idx].src_image_h.mode = G2D_GLOBAL_ALPHA;// G2D_PIXEL_ALPHA;
    	
    	g2d_info[idx].src_image_h.alpha = 0x0;
    	g2d_info[idx].src_image_h.color = 0xFFFFFFFF;
        g2d_info[idx].dst_image_h.alpha = 0x0;
    	g2d_info[idx].dst_image_h.color = 0xFFFFFFFF;	

    	g2d_info[idx].src_image_h.clip_rect.x = scale_info[idx].crop_info.x;
    	g2d_info[idx].src_image_h.clip_rect.y = scale_info[idx].crop_info.y;
    	g2d_info[idx].src_image_h.clip_rect.w = scale_info[idx].crop_info.width;
    	g2d_info[idx].src_image_h.clip_rect.h = scale_info[idx].crop_info.height;
    	g2d_info[idx].src_image_h.align[0] = 0;
    	g2d_info[idx].src_image_h.align[1] = 0;
    	g2d_info[idx].src_image_h.align[2] = 0;

    	g2d_info[idx].dst_image_h.width  = scale_info[idx].dst.w;
    	g2d_info[idx].dst_image_h.height = scale_info[idx].dst.h;
    	g2d_info[idx].dst_image_h.mode = G2D_GLOBAL_ALPHA; //G2D_PIXEL_ALPHA;

    	g2d_info[idx].dst_image_h.clip_rect.x = 0;
    	g2d_info[idx].dst_image_h.clip_rect.y = 0;
    	g2d_info[idx].dst_image_h.clip_rect.w = scale_info[idx].dst.w;
    	g2d_info[idx].dst_image_h.clip_rect.h = scale_info[idx].dst.h;

    	g2d_info[idx].dst_image_h.align[0] = 0;
    	g2d_info[idx].dst_image_h.align[1] = 0;
    	g2d_info[idx].dst_image_h.align[2] = 0;
        
    	g2d_info[idx].src_image_h.laddr[0] = scale_info[idx].src.addr;
    	g2d_info[idx].dst_image_h.laddr[0] = scale_info[idx].dst.addr;

    	fb_width = g2d_info[idx].src_image_h.width;
    	fb_height = g2d_info[idx].src_image_h.height;

    	if((g2d_info[idx].src_image_h.format == G2D_FORMAT_YUV420UVC_U1V1U0V0) ||
    		(g2d_info[idx].src_image_h.format == G2D_FORMAT_YUV420_PLANAR) ||
    		(g2d_info[idx].src_image_h.format == G2D_FORMAT_YUV420UVC_V1U1V0U0)) {

    		g2d_info[idx].src_image_h.laddr[1] = (int)(g2d_info[idx].src_image_h.laddr[0] + fb_width*fb_height);
    		g2d_info[idx].src_image_h.laddr[2] = (int)(g2d_info[idx].src_image_h.laddr[0] + fb_width*fb_height*5/4);
    	}

    	if((g2d_info[idx].src_image_h.format == G2D_FORMAT_YUV422UVC_V1U1V0U0) ||
    		(g2d_info[idx].src_image_h.format == G2D_FORMAT_YUV422_PLANAR) ||
    		(g2d_info[idx].src_image_h.format == G2D_FORMAT_YUV422UVC_V1U1V0U0)) {

    		g2d_info[idx].src_image_h.laddr[1] = (int)(g2d_info[idx].src_image_h.laddr[0] + fb_width*fb_height);
    		g2d_info[idx].src_image_h.laddr[2] = (int)(g2d_info[idx].src_image_h.laddr[0] + fb_width*fb_height*3/2);
    	}

    	fb_width = g2d_info[idx].dst_image_h.width;
    	fb_height = g2d_info[idx].dst_image_h.height;
    	if((g2d_info[idx].dst_image_h.format == G2D_FORMAT_YUV420UVC_U1V1U0V0) ||
    	   (g2d_info[idx].dst_image_h.format == G2D_FORMAT_YUV420_PLANAR) ||
    	   (g2d_info[idx].dst_image_h.format == G2D_FORMAT_YUV420UVC_V1U1V0U0)) {

    		g2d_info[idx].dst_image_h.laddr[1] = (int)(g2d_info[idx].dst_image_h.laddr[0] + fb_width*fb_height);
    		g2d_info[idx].dst_image_h.laddr[2] = (int)(g2d_info[idx].dst_image_h.laddr[0] + fb_width*fb_height*5/4);
    	}

    	if((g2d_info[idx].dst_image_h.format == G2D_FORMAT_YUV422UVC_V1U1V0U0) ||
    	   (g2d_info[idx].dst_image_h.format == G2D_FORMAT_YUV422UVC_U1V1U0V0) ||
    	   (g2d_info[idx].dst_image_h.format == G2D_FORMAT_YUV422_PLANAR)) {

    		g2d_info[idx].dst_image_h.laddr[1] = (int)(g2d_info[idx].dst_image_h.laddr[0] + fb_width*fb_height);
    		g2d_info[idx].dst_image_h.laddr[2] = (int)(g2d_info[idx].dst_image_h.laddr[0] + fb_width*fb_height*3/2);

    	}
    	g2d_info[idx].src_image_h.use_phy_addr = 1;
    	g2d_info[idx].dst_image_h.use_phy_addr = 1;
//		printf("idx: %d dst use phy:%d\n",idx,g2d_info[idx].src_image_h.use_phy_addr);
   }
   return 0;
}

int AwG2dProcess(AwG2d* self)
{
    unsigned long arg[6] = {0};

    arg[0] = (unsigned long)self->g2dInfo;
    arg[1] = self->g2dInfoNum;
    if (ioctl(self->g2dFd, G2D_CMD_MIXER_TASK, (arg)) < 0)
    {
        //printf("[%d][%s][%s]G2D_CMD_MIXER_TASK failure!\n", __LINE__,
         //      __FILE__, __FUNCTION__);
        return -1;
    }

    //printf("[%d][%s][%s]G2D_CMD_MIXER_TASK successfull!\n", __LINE__,
    //       __FILE__, __FUNCTION__);
    if(self->g2dInfo != NULL){
         free(self->g2dInfo);
         self->g2dInfo = NULL;
    }

    return 0;
}

int AwG2dCopy(AwG2d *self, void *phy_dst, ImageAddr phy_src, unsigned int width,
	      unsigned int height, int format)
{
	unsigned long arg[6] = {0};
	g2d_blt_h info;

	memset(&info, 0, sizeof(g2d_blt_h));

	info.flag_h = (g2d_blt_flags_h)1024;
	info.dst_image_h.use_phy_addr = 1;
	info.dst_image_h.format = (g2d_fmt_enh)format;
	info.dst_image_h.width = width;
	info.dst_image_h.height = height;
	info.dst_image_h.clip_rect.w = width;
	info.dst_image_h.clip_rect.h = height;
	memcpy(&info.src_image_h, &info.dst_image_h, sizeof(g2d_image_enh));

	info.dst_image_h.laddr[0] = (int)(phy_dst);

	if ((format == G2D_FORMAT_YUV420UVC_U1V1U0V0) ||
	    (format == G2D_FORMAT_YUV420_PLANAR) ||
	    (format == G2D_FORMAT_YUV420UVC_V1U1V0U0)) {
		info.dst_image_h.laddr[1] =
			(int)(info.dst_image_h.laddr[0] + width * height);
		info.dst_image_h.laddr[2] =
			(int)(info.dst_image_h.laddr[0] + width * height * 5 / 4);
	}

	if ((format == G2D_FORMAT_YUV422UVC_V1U1V0U0) ||
	    (format == G2D_FORMAT_YUV422_PLANAR) ||
	    (format == G2D_FORMAT_YUV422UVC_V1U1V0U0)) {

		info.dst_image_h.laddr[1] =
		    (int)(info.dst_image_h.laddr[0] + width * height);
		info.dst_image_h.laddr[2] =
		    (int)(info.dst_image_h.laddr[0] + width * height * 3 / 2);
	}
	info.src_image_h.laddr[0] = phy_src.addr1;
	info.src_image_h.laddr[1] = phy_src.addr2;
	info.src_image_h.laddr[2] = phy_src.addr3;

	if (ioctl(self->g2dFd, G2D_CMD_BITBLT_H, (unsigned long)&info) < 0) 
	{
		printf("[%d][%s][%s]G2D_CMD_BITBLT_H failure!\n", __LINE__, __FILE__, __FUNCTION__);
		return -1;
	}
}

void AwG2dDestroy(AwG2d* self)
{
    if(self != NULL){
        close(self->g2dFd);
        free(self);
    }       
}
