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
#include <rtthread.h>
#include <log.h>
#include <mod_display.h>
#include <stdlib.h>
#include <stdio.h>
#include <mod_mixture.h>
#include "mod_codec_cmd.h"

typedef struct
{
    __u32 y;
    __u32 u;
    __u32 v;
} YUV_BUF;

/*
static long int atol(char *s)
{
    long r = 0;
    int neg = 0;
    switch (*s)
    {
        case '-':
            neg = 1;
        // 这里没有break
        case '+':
            s++;
            break;
    }
    while (*s >= '0' && *s <= '9')
    {
        int n = *s++ - '0';
        if (neg)
        {
            n = -n;
        }
        r = r * 10 + n;
    }
    return r;
}
*/
static __u32 get_buff_from_bmp(const char *path, __u32 w, __u32 h)
{
	__hdle fh = NULL;
    __u32 fb = NULL;

	fb = rt_malloc(w*h*4);
    if(!fb)
    {
        __err("malloc fail path[%s]",path);
        return EPDK_FAIL;
    }
    rt_memset(fb, 0,w*h*4);

	fh = fopen(path,"r");
    fseek(fh,54,SEEK_SET);
    if(!fh)
    {
        __err("file open fail path[%s]",path);
        free(fb);
        return EPDK_FAIL;
    }

	fread((void *)fb, w*h*4, 1, fh);


__log("get bmp buff success");
    return fb;
}

static __s32 get_buff_from_yuv(const char *path1,const char *path2,const char *path3, __u32 w, __u32 h,YUV_BUF *fb)// YUV420
{
	__hdle fh1 = NULL;
	__hdle fh2 = NULL;
	__hdle fh3 = NULL;

	fb->y = rt_malloc(w*h);
	fb->u = rt_malloc(w*h/2);
	fb->v = rt_malloc(w*h/2);
    if(!fb->y || !fb->u  || !fb->v)
    {
        __err("malloc fail path[0] = %s",path1);
        return -1;
    }
	rt_memset(fb->y, 0,w*h);
	rt_memset(fb->u, 0,w*h/2);
	rt_memset(fb->v, 0,w*h/2);


	fh1 = fopen(path1,"r");
	fh2 = fopen(path2,"r");
	fh3 = fopen(path3,"r");

    if(!fh1 || !fh2 || !fh3)
    {
        __err("file open fail path[0] = %s",path1);
        free(fb->y);
        free(fb->u);
        free(fb->v);
        return -1;
    }


	fread((void *)fb->y, w*h, 1, fh1);
	fread((void *)fb->u, w*h/2, 1, fh2);
	fread((void *)fb->v, w*h/2, 1, fh3);
    return EPDK_OK;
}

int argb_show_test(int argc, char **argv)
{
	__hdle fh;
	__u32 fb_addr;
	__u32 fb_addr3;
	__u32 pic_w,pic_h,i,sc_w,sc_h;
	__u64 para[3];
	__u32 hlay,hlay2;
    __s32 ret = -1;
    __disp_layer_info_t logo_layer_para;
    __mp *mp_display = NULL;

    mp_display = esKSRV_Get_Display_Hld();
	if(mp_display == NULL)
    {
        __log("get mp_display fail!");
        return;
    }
    int de = atol(argv[1]);
    __log("de = %d",de);

    	pic_w = 720;
    	pic_h = 576;
        sc_w = 720;
        sc_h = 576;

    	fb_addr = get_buff_from_bmp("F:\\allwinner.bmp",pic_w,pic_h);//32bit 240x320 bmp picture
        if(!fb_addr)
        {
            __log("show bmp fail");
            return -1;
        }
        para[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;//MOD_DISP_LAYER_WORK_MODE_NORMAL;
        para[1] = 0;
        para[2] = 0;
        hlay2 = esMODS_MIoctrl(mp_display,MOD_DISP_LAYER_REQUEST,de,para);

        logo_layer_para.fb.format        = DISP_FORMAT_ARGB_8888;//DISP_FORMAT_YUV420_SP_UVUV;
        logo_layer_para.fb.seq           = DISP_SEQ_ARGB;
        logo_layer_para.fb.mode          = DISP_MOD_INTERLEAVED;//
        logo_layer_para.fb.br_swap       = 1;//
        logo_layer_para.fb.cs_mode       = DISP_BT709;
        logo_layer_para.mode             = MOD_DISP_LAYER_WORK_MODE_SCALER;//MOD_DISP_LAYER_WORK_MODE_NORMAL;//
        logo_layer_para.pipe             = 0;
        logo_layer_para.prio             = 2;
        logo_layer_para.alpha_en         = 0;
        logo_layer_para.alpha_val        = 0xff;
        logo_layer_para.ck_enable        = 0;
        logo_layer_para.src_win.x        = 0;
        logo_layer_para.src_win.y        = 0;
        logo_layer_para.src_win.width    = pic_w;
        logo_layer_para.src_win.height   = pic_h;
        logo_layer_para.scn_win.x        = 0;
        logo_layer_para.scn_win.y        = 0;
        logo_layer_para.scn_win.width    = sc_w;
        logo_layer_para.scn_win.height   = sc_h;
        logo_layer_para.fb.addr[0]       = ((__u32)fb_addr);
        logo_layer_para.fb.addr[1]       = NULL;
        logo_layer_para.fb.addr[2]       = NULL;
        logo_layer_para.fb.size.width = pic_w;
        logo_layer_para.fb.size.height = pic_h;

        para[0] = hlay2;
        para[1] = &logo_layer_para;
        para[2] = 0;
        esMODS_MIoctrl(mp_display,MOD_DISP_CMD_LAYER_SET_PARA,de,para);

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(argb_show_test, __cmd_argb_show_test, argb_show_test);

int argb_show_test2(int argc, char **argv)
{
	__hdle fh;
	__u32 fb_addr;
	__u32 fb_addr3;
	__u32 pic_w,pic_h,i;
	__u64 para[3];
	__u32 hlay,hlay2;
    __s32 ret = -1;
    __disp_layer_info_t logo_layer_para;
    __mp *mp_display = NULL;

    mp_display = esKSRV_Get_Display_Hld();
	if(mp_display == NULL)
    {
        __log("get mp_display fail!");
        return;
    }

    	pic_w = 126;
    	pic_h = 60;

    	fb_addr = get_buff_from_bmp("F:\\12660.bmp",pic_w,pic_h);//32bit 240x320 bmp picture
        if(!fb_addr)
        {
            __log("show bmp fail");
            return -1;
        }
        para[0] = MOD_DISP_LAYER_WORK_MODE_NORMAL;
        para[1] = 0;
        para[2] = 0;
        hlay2 = esMODS_MIoctrl(mp_display,MOD_DISP_LAYER_REQUEST,0,para);

        logo_layer_para.fb.format        = DISP_FORMAT_ARGB_8888;//DISP_FORMAT_YUV420_SP_UVUV;
        logo_layer_para.fb.seq           = DISP_SEQ_ARGB;
        logo_layer_para.fb.mode          = DISP_MOD_INTERLEAVED;//
        logo_layer_para.fb.br_swap       = 0;//
        logo_layer_para.fb.cs_mode       = DISP_BT601;
        logo_layer_para.mode             = MOD_DISP_LAYER_WORK_MODE_NORMAL;//
        logo_layer_para.pipe             = 0;
        logo_layer_para.prio             = 4;
        logo_layer_para.alpha_en         = 0;
        logo_layer_para.alpha_val        = 0xff;
        logo_layer_para.ck_enable        = 0;
        logo_layer_para.src_win.x        = 0;
        logo_layer_para.src_win.y        = 0;
        logo_layer_para.src_win.width    = pic_w;
        logo_layer_para.src_win.height   = pic_h;
        logo_layer_para.scn_win.x        = 200;
        logo_layer_para.scn_win.y        = 0;
        logo_layer_para.scn_win.width    = 126;
        logo_layer_para.scn_win.height   = 60;
        logo_layer_para.fb.addr[0]       = ((__u32)fb_addr);
        logo_layer_para.fb.addr[1]       = NULL;
        logo_layer_para.fb.addr[2]       = NULL;
        logo_layer_para.fb.size.width = pic_w;
        logo_layer_para.fb.size.height = pic_h;


        para[0] = hlay2;
        para[1] = &logo_layer_para;
        para[2] = 0;
        esMODS_MIoctrl(mp_display,MOD_DISP_CMD_LAYER_SET_PARA,0,para);

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(argb_show_test2, __cmd_argb_show_test2,argb_show_test2);

int yuv_show_test(int argc, char **argv)
{

	YUV_BUF fb = {0,0,0};
    __u32 fb_addr,fb_addr2;
	__u32 pic_w,pic_h;
	__u64 para[3];
	__u32 hlay,hlay2,haly3;
    __s32 ret = -1;
    __disp_layer_info_t logo_layer_para;
      __mp *mp_display = NULL;
    __hdle filep = NULL;
     mp_display = esKSRV_Get_Display_Hld();
     if(!mp_display)
     {
         __log("get mp_display fail!");
         return;
     }

      pic_w = 720;
      pic_h = 576;
//------------------------------------------------------
    filep = fopen("F:\\allwinner.bin","r");

	fb.y = rt_malloc(pic_w*pic_h*3);

    if(!fb.y)
    {
        __err("malloc fail");
        return -1;
    }
	rt_memset(fb.y, 0x80,pic_w*pic_h*3);

	fread((void *)fb.y, pic_w*pic_h*3, 1, filep);


    //ret = get_buff_from_yuv("F:\\test1.y","F:\\test1.u","F:\\test1.v",pic_w, pic_h,&fb);//800x480 yuv420 data

	para[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
	para[1] = 0;
	para[2] = 0;
	hlay = esMODS_MIoctrl(mp_display,MOD_DISP_LAYER_REQUEST,1,para);

    logo_layer_para.fb.format        = DISP_FORMAT_YUV420_SP_UVUV;//DISP_FORMAT_YUV420_P;//DISP_FORMAT_YUV420_SP_UVUV;
    logo_layer_para.fb.seq           = DISP_SEQ_VUYA;
    logo_layer_para.fb.mode          = DISP_MOD_INTERLEAVED;//DISP_MOD_NON_MB_PLANAR;//
    logo_layer_para.fb.br_swap       = 0;//
    logo_layer_para.fb.cs_mode       = DISP_BT601_F;
    logo_layer_para.mode             = MOD_DISP_LAYER_WORK_MODE_SCALER;//
    logo_layer_para.pipe             = 0;
    logo_layer_para.prio             = 6;
    logo_layer_para.alpha_en         = 1;
    logo_layer_para.alpha_val        = 100;
    logo_layer_para.ck_enable        = 0;
    logo_layer_para.src_win.x        = 0;
    logo_layer_para.src_win.y        = 0;
    logo_layer_para.src_win.width    = pic_w;
    logo_layer_para.src_win.height   = pic_h;
    logo_layer_para.scn_win.x        = 0;
    logo_layer_para.scn_win.y        = 0;
    logo_layer_para.scn_win.width    = pic_w;
    logo_layer_para.scn_win.height   = pic_h;
    logo_layer_para.fb.addr[0]       = ((__u32)fb.y);
    logo_layer_para.fb.addr[1]       = ((__u32)fb.u);
    logo_layer_para.fb.addr[2]       = ((__u32)fb.v);
    logo_layer_para.fb.size.width = pic_w;
    logo_layer_para.fb.size.height = pic_h;

	para[0] = hlay;
	para[1] = &logo_layer_para;
	para[2] = 0;
	esMODS_MIoctrl(mp_display,MOD_DISP_CMD_LAYER_SET_PARA,1,para);


    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(yuv_show_test, __cmd_yuv_show_test, yuv_show_test);

int lbc_show_test(int argc, char **argv)
{

	__hdle fh1 = NULL;

    void *fb_addr = NULL;
	__u32 pic_w,pic_h;
	__u64 para[3];
	__u32 hlay;
    __s32 ret = -1;
    __disp_layer_info_t logo_layer_para;
      __mp *mp_display = NULL;

     mp_display = esKSRV_Get_Display_Hld();
     if(!mp_display)
     {
         __log("get mp_display fail!");
         return;
     }


//------------------------------------------------------
    pic_w = 1280;
    pic_h = 720;
	fb_addr = rt_malloc(pic_w*pic_h *4);
    if(!fb_addr)
    {
        __err("malloc fail");
        return -1;
    }
	rt_memset(fb_addr, 0,pic_w*pic_h *4);


	fh1 = fopen("F:\\lbc1280720.bin","r");//这张图的格式和VE出图格式有点差异，
	                                      //需要在mod_display.c中 LBC_RC_EN 改为0。
	                                      //仅做测试用

    if(!fh1)
    {
        __err("file open fail");
        free(fb_addr);
        return -1;
    }


	fread(fb_addr, pic_w*pic_h *4, 1, fh1);

	para[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
	para[1] = 0;
	para[2] = 0;
	hlay = esMODS_MIoctrl(mp_display,MOD_DISP_LAYER_REQUEST,0,para);

    logo_layer_para.fb.format        = DISP_FORMAT_YUV420_P;//DISP_FORMAT_YUV420_SP_UVUV;
    logo_layer_para.fb.seq           = DISP_SEQ_UVUV;
    logo_layer_para.fb.mode          = DISP_MOD_NON_MB_PLANAR;//
    logo_layer_para.fb.br_swap       = 0;//
    logo_layer_para.fb.cs_mode       = DISP_BT601;
    logo_layer_para.mode             = MOD_DISP_LAYER_WORK_MODE_SCALER;//
    logo_layer_para.pipe             = 0;
    logo_layer_para.prio             = 6;
    logo_layer_para.alpha_en         = 0;
    logo_layer_para.alpha_val        = 0;
    logo_layer_para.ck_enable        = 0;
    logo_layer_para.src_win.x        = 0;
    logo_layer_para.src_win.y        = 0;
    logo_layer_para.src_win.width    = pic_w;
    logo_layer_para.src_win.height   = pic_h;
    logo_layer_para.scn_win.x        = 0;
    logo_layer_para.scn_win.y        = 0;
    logo_layer_para.scn_win.width    = 800;
    logo_layer_para.scn_win.height   = 480;
    logo_layer_para.fb.lbc_mode        = 3;
    logo_layer_para.fb.addr[0]       = fb_addr;
    logo_layer_para.fb.addr[1]       = NULL;
    logo_layer_para.fb.addr[2]       = NULL;
    logo_layer_para.fb.size.width = pic_w;
    logo_layer_para.fb.size.height = pic_h;

	para[0] = hlay;
	para[1] = &logo_layer_para;
	para[2] = 0;
	esMODS_MIoctrl(mp_display,MOD_DISP_CMD_LAYER_SET_PARA,0,para);


    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(lbc_show_test, __cmd_lbc_show_test, lbc_show_test);


int mod_display_alpha_test(int argc, char **argv)
{
	__hdle fh;
	__u32 fb_addr;
	__u32 fb_addr2;
	__u32 pic_w,pic_h;
	__u64 para[3];
	__u32 hlay,hlay2;
    __u32 i = 0;
	YUV_BUF fb = {0,0,0};
    __s32 ret = -1;
    __disp_layer_info_t logo_layer_para;
    __mp *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();
	if(!mp_display)
    {
        __log("get mp_display fail!");
        return;
    }
      pic_w = 400;
      pic_h = 240;

    ret = get_buff_from_yuv("F:\\test1.y","F:\\test1.u","F:\\test1.v",pic_w, pic_h,&fb);//400x240 yuv420 data
    if(ret < 0)
    {
        __log("show yuv fail");
        return;
    }
	para[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
	para[1] = 0;
	para[2] = 0;
	hlay = esMODS_MIoctrl(mp_display,MOD_DISP_LAYER_REQUEST,0,para);

    logo_layer_para.fb.format        = DISP_FORMAT_YUV420_P;//DISP_FORMAT_YUV420_SP_UVUV;
    logo_layer_para.fb.seq           = DISP_SEQ_UVUV;
    logo_layer_para.fb.mode          = DISP_MOD_NON_MB_PLANAR;//
    logo_layer_para.fb.br_swap       = 0;//
    logo_layer_para.fb.cs_mode       = DISP_BT601;
    logo_layer_para.mode             = MOD_DISP_LAYER_WORK_MODE_SCALER;//
    logo_layer_para.pipe             = 0;
    logo_layer_para.prio             = 6;
    logo_layer_para.alpha_en         = 1;
    logo_layer_para.alpha_val        = 100;
    logo_layer_para.ck_enable        = 0;
    logo_layer_para.src_win.x        = 0;
    logo_layer_para.src_win.y        = 0;
    logo_layer_para.src_win.width    = pic_w;
    logo_layer_para.src_win.height   = pic_h;
    logo_layer_para.scn_win.x        = 0;
    logo_layer_para.scn_win.y        = 0;
    logo_layer_para.scn_win.width    = pic_w;
    logo_layer_para.scn_win.height   = pic_h;
    logo_layer_para.fb.addr[0]       = ((__u32)fb.y);
    logo_layer_para.fb.addr[1]       = ((__u32)fb.u);
    logo_layer_para.fb.addr[2]       = ((__u32)fb.v);
    logo_layer_para.fb.size.width = pic_w;
    logo_layer_para.fb.size.height = pic_h;
	para[0] = hlay;
	para[1] = &logo_layer_para;
	para[2] = 0;
	esMODS_MIoctrl(mp_display,MOD_DISP_CMD_LAYER_SET_PARA,0,para);

/*
      pic_w = 400;
      pic_h = 240;

    fb_addr = get_buff_from_bmp("F:\\red400240.bmp",pic_w,pic_h);
    if(!fb_addr)
    {
        __log("show bmp fail");
        return -1;
    }
      para[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
      para[1] = 0;
      para[2] = 0;
      hlay2 = esMODS_MIoctrl(mp_display,MOD_DISP_LAYER_REQUEST,0,para);

      logo_layer_para.fb.format        = DISP_FORMAT_ARGB_8888;//DISP_FORMAT_YUV420_SP_UVUV;
      logo_layer_para.fb.seq           = DISP_SEQ_ARGB;
      logo_layer_para.fb.mode          = DISP_MOD_INTERLEAVED;//
      logo_layer_para.fb.br_swap       = 0;//
      logo_layer_para.fb.cs_mode       = DISP_BT601;
      logo_layer_para.mode             = MOD_DISP_LAYER_WORK_MODE_SCALER;//
      logo_layer_para.pipe             = 0;
      logo_layer_para.prio             = 11;
      logo_layer_para.alpha_en         = 1;
      logo_layer_para.alpha_val        = 30;
      logo_layer_para.ck_enable        = 0;
      logo_layer_para.src_win.x        = 0;
      logo_layer_para.src_win.y        = 0;
      logo_layer_para.src_win.width    = pic_w;
      logo_layer_para.src_win.height   = pic_h;
      logo_layer_para.scn_win.x        = 0;
      logo_layer_para.scn_win.y        = 0;
      logo_layer_para.scn_win.width    = pic_w;
      logo_layer_para.scn_win.height   = pic_h;
      logo_layer_para.fb.addr[0]       = ((__u32)fb_addr);
      logo_layer_para.fb.addr[1]       = NULL;//(__u32)jpeg_info->v_buf;
      logo_layer_para.fb.addr[2]       = NULL;//(__u32)jpeg_info->u_buf;
      logo_layer_para.fb.size.width = pic_w;
      logo_layer_para.fb.size.height = pic_h;
    para[0] = hlay2;
    para[1] = &logo_layer_para;
    para[2] = 0;
    esMODS_MIoctrl(mp_display,MOD_DISP_CMD_LAYER_SET_PARA,0,para);
*/
    //-----------------------------------------

    pic_w = 240;
    pic_h = 320;

    fb_addr2 = get_buff_from_bmp("F:\\240320.bmp",pic_w,pic_h);//32bit 240x320 bmp picture
    if(!fb_addr2)
    {
        __log("show bmp fail");
        return -1;
    }

    para[0] = MOD_DISP_LAYER_WORK_MODE_NORMAL;
    para[1] = 0;
    para[2] = 0;
    hlay = esMODS_MIoctrl(mp_display,MOD_DISP_LAYER_REQUEST,0,para);

    logo_layer_para.fb.format        = DISP_FORMAT_ARGB_8888;//DISP_FORMAT_YUV420_SP_UVUV;
    logo_layer_para.fb.seq           = DISP_SEQ_ARGB;
    logo_layer_para.fb.mode          = DISP_MOD_INTERLEAVED;//
    logo_layer_para.fb.br_swap       = 0;//
    logo_layer_para.fb.cs_mode       = DISP_BT601;
    logo_layer_para.mode             = MOD_DISP_LAYER_WORK_MODE_NORMAL;//
    logo_layer_para.pipe             = 0;
    logo_layer_para.prio             = 10;
    logo_layer_para.alpha_en         = 1;
    logo_layer_para.alpha_val        = 125;
    logo_layer_para.ck_enable        = 0;
    logo_layer_para.src_win.x        = 0;
    logo_layer_para.src_win.y        = 0;
    logo_layer_para.src_win.width    = pic_w;
    logo_layer_para.src_win.height   = pic_h;
    logo_layer_para.scn_win.x        = 0;
    logo_layer_para.scn_win.y        = 0;
    logo_layer_para.scn_win.width    = 240;
    logo_layer_para.scn_win.height   = 320;
    logo_layer_para.fb.addr[0]       = ((__u32)fb_addr2);
    logo_layer_para.fb.addr[1]       = NULL;//(__u32)jpeg_info->v_buf;
    logo_layer_para.fb.addr[2]       = NULL;//(__u32)jpeg_info->u_buf;
    logo_layer_para.fb.size.width = pic_w;
    logo_layer_para.fb.size.height = pic_h;

   // while(1)
    {
        i++;
   //     logo_layer_para.alpha_val        = i;

        para[0] = hlay;
        para[1] = &logo_layer_para;
        para[2] = 0;
        esMODS_MIoctrl(mp_display,MOD_DISP_CMD_LAYER_SET_PARA,0,para);
        esKRNL_TimeDly(5);
    if(i == 255)
        i=0;
    }
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(mod_display_alpha_test, __cmd_mod_display_alpha_test, mod_display_alpha_test);



int mod_display_scaler_test(int argc, char **argv)
{
	__hdle fh;
	__u32 fb_addr;
	__u32 fb_addr3;
	__u32 pic_w,pic_h;
    __u32 i = 0;
	__u64 para[3];
	__u32 hlay,hlay2;
    __s32 ret = -1;
    __disp_layer_info_t logo_layer_para;
    __mp *mp_display = NULL;

    mp_display = esKSRV_Get_Display_Hld();

	if(!mp_display)
    {
        __log("get mp_display fail!");
        return;
    }
    	pic_w = 400;
    	pic_h = 240;

    	fb_addr = get_buff_from_bmp("F:\\red400240.bmp",pic_w,pic_h);
    if(!fb_addr)
    {
        __log("show bmp fail");
        return -1;
    }
        para[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
        para[1] = 0;
        para[2] = 0;
        hlay = esMODS_MIoctrl(mp_display,MOD_DISP_LAYER_REQUEST,0,para);

        logo_layer_para.fb.format        = DISP_FORMAT_ARGB_8888;//DISP_FORMAT_YUV420_SP_UVUV;
        logo_layer_para.fb.seq           = DISP_SEQ_ARGB;
        logo_layer_para.fb.mode          = DISP_MOD_INTERLEAVED;//
        logo_layer_para.fb.br_swap       = 0;//
        logo_layer_para.fb.cs_mode       = DISP_BT601;
        logo_layer_para.mode             = MOD_DISP_LAYER_WORK_MODE_SCALER;//
        logo_layer_para.pipe             = 0;
        logo_layer_para.prio             = 10;
        logo_layer_para.alpha_en         = 0;
        logo_layer_para.alpha_val        = 0xff;
        logo_layer_para.ck_enable        = 0;
        logo_layer_para.src_win.x        = 0;
        logo_layer_para.src_win.y        = 0;
        logo_layer_para.src_win.width    = pic_w;
        logo_layer_para.src_win.height   = pic_h;
        logo_layer_para.scn_win.x        = 0;
        logo_layer_para.scn_win.y        = 0;
        logo_layer_para.scn_win.width    = pic_w;
        logo_layer_para.scn_win.height   = pic_h;
        logo_layer_para.fb.addr[0]       = ((__u32)fb_addr);
        logo_layer_para.fb.addr[1]       = NULL;//(__u32)jpeg_info->v_buf;
        logo_layer_para.fb.addr[2]       = NULL;//(__u32)jpeg_info->u_buf;
        logo_layer_para.fb.size.width = pic_w;
        logo_layer_para.fb.size.height = pic_h;

    while(1)
    {
        i++;
        logo_layer_para.scn_win.width    = pic_w*(50 + i)/100;
        logo_layer_para.scn_win.height   = pic_h*(50 + i)/100;
        para[0] = hlay;
        para[1] = &logo_layer_para;
        para[2] = 0;
        esMODS_MIoctrl(mp_display,MOD_DISP_CMD_LAYER_SET_PARA,0,para);
        esKRNL_TimeDly(5);
        if(i == 150)
            i=0;
    }
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(mod_display_scaler_test, __cmd_mod_display_scaler_test, mod_display_scaler_test);


int lcdset_backcolor(int argc, char **argv)
{
	__u64 para[3];
    __mp *mp_display = NULL;
    static __u8 i = 0;
    __u32 color[3] = {
        0xffff0000,
        0xff00ff00,
        0xff0000ff,
    };

	mp_display = esKSRV_Get_Display_Hld();
	if(!mp_display)
    {
        __log("get mp_display fail!");
        return;
    }

	para[0] = 100;
	para[1] = color[i++];
	para[2] = 0;
 	esMODS_MIoctrl(mp_display,MOD_DISP_SET_BKCOLOR,0,para);
    if(i == 3)
        i = 0;
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(lcdset_backcolor, __cmd_lcdset_backcolor, lcdset_backcolor);

int lcdget_backcolor(int argc, char **argv)
{
	__u64 para[3];
    int ret;
    __mp *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

	if(!mp_display)
    {
        __log("get mp_display fail!");
        return;
    }

	para[0] = 100;
	para[1] = 0;
	para[2] = 0;
 	ret = esMODS_MIoctrl(mp_display,MOD_DISP_GET_BKCOLOR,0,para);
    __log("color = 0x%x",ret);
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(lcdget_backcolor, __cmd_lcdget_backcolor, lcdget_backcolor);

int get_lcd_size(int argc, char **argv)
{
	__u64 para[4];
    __u32 width,height;
    __mp *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

	if(!mp_display)
    {
        __log("get mp_display fail!..x");
        return;
    }
	para[0] = 0;
	para[1] = 0;
	para[2] = 0;
	width = esMODS_MIoctrl(mp_display,MOD_DISP_GET_SCN_WIDTH,0,para);
	height = esMODS_MIoctrl(mp_display,MOD_DISP_GET_SCN_HEIGHT,0,para);
    __log("lcd width = %d  height = %d",width,height);

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(get_lcd_size, __cmd_get_lcd_size, get_lcd_size);



int mod_layer_release(int argc, char **argv)
{
	__u64 para[4];
    __mp *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();
	if(!mp_display)
    {
        __log("get mp_display fail!");
        return;
    }
	para[0] = atol(argv[1]);
	para[1] = 0;
	para[2] = 0;

	esMODS_MIoctrl(mp_display,MOD_DISP_LAYER_RELEASE,0,para);

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(mod_layer_release, __cmd_layer_release, mod_layer_release);


int mod_layer_request(int argc, char **argv)
{
	__u64 para[3];
    __mp *mp_display = NULL;
	mp_display = esKSRV_Get_Display_Hld();
	if(!mp_display)
    {
        __log("get mp_display fail!");
        return;
    }
	para[0] = atol(argv[1]);
	para[1] = 0;
	para[2] = 0;
	esMODS_MIoctrl(mp_display,MOD_DISP_LAYER_REQUEST,0,para);
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(mod_layer_request, __cmd_layer_request, mod_layer_request);

int install_mod_display(int argc, char **argv)
{
	__u64 para[3];
    __u32 mid_display = 0;
    __mp *mp_display = NULL;
    mid_display = esMODS_MInstall("/data/mod/display.mod", 0);
    __log("install mod display --- mid_display = %x",mid_display);
    mp_display = esMODS_MOpen(mid_display, 0);
    __log("install mod display --- mp_display = %x",mp_display);
    esKSRV_Save_Display_Hld(mp_display);
   return 0;

}
FINSH_FUNCTION_EXPORT_ALIAS(install_mod_display, __cmd_install_mod_display, install_mod_display);



int mod_layer_close(int argc, char **argv)
{
	__u64 para[3];
    __mp *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();
	if(!mp_display)
    {
        __log("get mp_display fail!");
        return;
    }
	para[0] = atol(argv[1]);
	para[1] = 0;
	para[2] = 0;
	__log("para[0] = %d",para[0]);
	esMODS_MIoctrl(mp_display,MOD_DISP_CMD_LAYER_CLOSE,0,para);
    return 0;

}
FINSH_FUNCTION_EXPORT_ALIAS(mod_layer_close, __cmd_layer_close, mod_layer_close);


int mod_layer_open(int argc, char **argv)
{
	__u64 para[3];
    __mp *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();
	if(!mp_display)
    {
        __log("get mp_display fail!");
        return;
    }
	para[0] = atol(argv[1]);
	para[1] = 0;
	para[2] = 0;

	esMODS_MIoctrl(mp_display,MOD_DISP_CMD_LAYER_OPEN,0,para);
    return 0;

}
FINSH_FUNCTION_EXPORT_ALIAS(mod_layer_open, __cmd_layer_open, mod_layer_open);


int lcdbl_on(int argc, char **argv)
{
	__u64 para[3] = {0};
    __mp *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();
	if(!mp_display)
    {
        __log("get mp_display fail!");
        return;
    }
	esMODS_MIoctrl(mp_display,MOD_DISP_LCD_BACKLIGHT_ENABLE,0,para);
  return 0;

}
FINSH_FUNCTION_EXPORT_ALIAS(lcdbl_on, __cmd_lcdbl_on, lcdbl_on);




int lcdbl_off(int argc, char **argv)
{
	__u64 para[3] = {0};
    __mp *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();
	if(!mp_display)
    {
        __log("get mp_display fail!");
        return;
    }

	esMODS_MIoctrl(mp_display,MOD_DISP_LCD_BACKLIGHT_DISABLE,0,para);
  return 0;

}
FINSH_FUNCTION_EXPORT_ALIAS(lcdbl_off, __cmd_lcdbl_off, lcdbl_off);


int lcdbl_num(int argc, char **argv)
{
	__u64 para[3] = {0};
    __mp *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();
	if(!mp_display)
    {
        __log("get mp_display fail!");
        return;
    }
	para[0] = atol(argv[1]);
	para[1] = 0;
	esMODS_MIoctrl(mp_display,MOD_DISP_LCD_SET_BRIGHTNESS,0,para);
    return 0;

}
FINSH_FUNCTION_EXPORT_ALIAS(lcdbl_num, __cmd_lcdbl_num, lcdbl_num);

int mod_display_testcmd(int argc, char **argv)
{
	__u64 para[4] = {0};
    __mp *mp_display = NULL;
    __u64 cmd;
    __s32 ret;
    cmd = atol(argv[1]);

	mp_display = esKSRV_Get_Display_Hld();
	if(!mp_display)
    {
        __log("get mp_display fail!");
        return;
    }
    if(argv[2])
	para[0] = atol(argv[2]);
    if(argv[3])
	para[1] = atol(argv[3]);
    if(argv[4])
	para[2] = atol(argv[4]);
    if(argv[5])
	para[3] = atol(argv[5]);
    __err("cmd:%d  para %d  %d  %d  %d",cmd,para[0],para[1],para[2],para[3]);
	ret = esMODS_MIoctrl(mp_display,cmd,0,para);
    __err("ret = %d",ret);
    return 0;

}
FINSH_FUNCTION_EXPORT_ALIAS(mod_display_testcmd, __cmd_mod_display_testcmd, mod_display_testcmd);

int display_switch(int argc, char **argv)
{
	__u64 para[4] = {0};
    __mp *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();
	if(!mp_display)
    {
        __log("get mp_display fail!");
        return;
    }
	//MOD_DISP_OUTPUT_TYPE_NONE = 0,
	//MOD_DISP_OUTPUT_TYPE_LCD = 1,
	//MOD_DISP_OUTPUT_TYPE_TV = 2,
	para[0] = atol(argv[1]);//0:close  1:tv  2:tv  3:lcd_tv_same  4:lcd_tv_diff
	para[1] = atol(argv[2]);//ntsc:1 or  pal:2
	esMODS_MIoctrl(mp_display,MOD_DISP_DEVICE_SWITCH,0,para);
    return 0;

}
FINSH_FUNCTION_EXPORT_ALIAS(display_switch, __cmd_display_switch, display_switch);

int mod_save_layer_fb(int argc, char **argv)
{
	__u64 para[3];
    __mp *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();
	if(!mp_display)
    {
        __log("get mp_display fail!");
        return;
    }
	__hdle fh;
    __disp_layer_info_t layer_para;
	__u32 width=0,height=0;

	para[0] = atol(argv[1]);
	para[1] = &layer_para;
	para[2] = 0;
	esMODS_MIoctrl(mp_display,MOD_DISP_CMD_LAYER_GET_PARA,0,para);


    width = 800;//atol(argv[2]);
    height = 480;//atol(argv[3]);


	fh = fopen("F:\\save.bin","wb");

	if(!fh)
	{
		__log("fh fail");
		return -1;
	}

	fwrite((void *)layer_para.fb.addr[0], width*height*4, 1, fh);

    return 0;

}
FINSH_FUNCTION_EXPORT_ALIAS(mod_save_layer_fb, __cmd_save_layer_fb, mod_save_layer_fb);

#if 1
void wintest(int argc, char **argv)
{
    __u32 mid_mixture;
    __s32 *mp_mixture = NULL;
    __u64 arg[3];

    mid_mixture = esMODS_MInstall("d:\\mod\\mixture.mod", 0);
	mp_mixture = esMODS_MOpen(mid_mixture, 0);
    __log("x");
    arg[0] = STARTUP;
    esMODS_MIoctrl(mp_mixture, MOD_MIXTURE_START, 0, (void *)arg);
    		rt_thread_delay(300);

	while(esMODS_MIoctrl(mp_mixture, MOD_MIXTURE_QURY_FINISH, 0, 0) != 1)
	{
		rt_thread_delay(3);
	}
    __log("y");
    esMODS_MIoctrl(mp_mixture, MOD_MIXTURE_STOP, 0, (void *)arg);
    esMODS_MClose(mp_mixture);
    esMODS_MUninstall(mid_mixture);
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(wintest, __cmd_wintest, wintest);
#endif



#if 0
typedef struct
{
    __u32 width;
    __u32 height;
} JPG_SIZE;

#define ALIGN_TO_16B(x)   ((((x) + (1 <<  4) - 1) >>  4) <<  4)
#define ALIGN_TO_32B(x)   ((((x) + (1 <<  5) - 1) >>  5) <<  5)

static  stJpegFrameInfo *jpeg_info;

static __s32 get_jpg_size(JPG_SIZE *size, __hdle fh)
{
    char mark[4] = {0};
    __u32 offset;
    fread(mark, 1, 2, fh);
    if (!(mark[0] == 0xFF && mark[1] == 0xD8))
    {
        __log("get jpg size fail!, (%x, %x)", mark[0], mark[1]);
        fclose(fh);
        fh = NULL;
        return -1;
    }

    mark[0] = 0;
    mark[1] = 0;
    fread(mark, 1, 2, fh);


    while (mark[0] == 0xFF)
    {
        mark[0] = 0;
        mark[1] = 0;
        fread(mark, 1, 2, fh);
        offset = ((mark[1]) | (mark[0] << 8));
        fseek(fh, offset - 2, SEEK_CUR);
        mark[0] = 0;
        mark[1] = 0;
        fread(mark, 1, 2, fh);
        if (mark[1] == 0xC0 || mark[1] == 0xC2)
        {
            __u8 temp[4] = {0};
            fread(temp, 1, 2, fh);//length
            fread(temp, 1, 1, fh);//data_precision
            fread(temp, 1, 2, fh);//height
            size->height = ((temp[1]) | (temp[0] << 8));
            fread(temp, 1, 2, fh);//width
            size->width = ((temp[1]) | (temp[0] << 8));
            break;
        }
    }
    return 0;
}

void jpg_show_test(int argc, char **argv)
{
    __u32 mid_vcoder;
    __s32 *mp_vcoder = NULL;
    __u64 arg[3];
    JPG_SIZE jpg_size;
	__hdle fh = NULL;
    __u32 fb = NULL;
    __u32 file_size,tmpsize;
    __u32 yuv_size = 0;
    __s32 ret = -1;
	__u64 para[3];
    __mp *mp_display = NULL;
    __u32 hlay = 0;
    __disp_layer_info_t layer;

    int de = atol(argv[1]);

    jpg_size.height = 0;
    jpg_size.width = 0;
	mp_display = esKSRV_Get_Display_Hld();
    jpeg_info = (stJpegFrameInfo *)rt_malloc(sizeof(stJpegFrameInfo));

    if (!jpeg_info)
    {
        __log("malloc failed...\n");
        return EPDK_FAIL;
    }

    mid_vcoder = esMODS_MInstall("d:\\mod\\vcoder.mod", 0);
	mp_vcoder = esMODS_MOpen(mid_vcoder, 0);

    if (!mid_vcoder)
    {
        __log("install vcoder failed...\n");
        return EPDK_FAIL;
    }

    if (!mp_vcoder)
    {
        __log("open vcoder mod failed!\n");
        return EPDK_FAIL;
    }

	fh = fopen("/mnt/F/allwinner.jpg","r");
    if (fh == NULL)
    {
        __log("open  file err!\n");
        return EPDK_FAIL;
    }

    fseek(fh, 0, SEEK_END);
    file_size = ftell(fh);
    fseek(fh, 0, SEEK_SET);
    jpeg_info->jpegData = rt_malloc(file_size);
    if (jpeg_info->jpegData == NULL)
    {
        __log("jpeg_info->jpegData malloc fail,file_size:%d\n", file_size);
    }
    tmpsize = fread(jpeg_info->jpegData, 1, file_size, fh);

    if (tmpsize != file_size)
    {
        __log("write file err! %d- %d\n", tmpsize, file_size);
    }
    jpeg_info->jpegData_len = tmpsize;
    fseek(fh, 0, SEEK_SET);

    get_jpg_size(&jpg_size, fh);

    yuv_size = ALIGN_TO_16B(jpg_size.width) * ALIGN_TO_16B(jpg_size.height) * 3 / 2;
    jpeg_info->yuv_buf = (__u32)rt_malloc(yuv_size);
    esMEMS_CleanFlushDCacheRegion((void *)jpeg_info->yuv_buf, yuv_size);

    ret = esMODS_MIoctrl(mp_vcoder, MPEJ_CODEC_CMD_DECODER, 0, jpeg_info);
    fclose(fh);
    esMODS_MClose(mp_vcoder);
    esMODS_MUninstall(mid_vcoder);

    layer.fb.format        = DISP_FORMAT_YUV420_P;//DISP_FORMAT_YUV420_SP_UVUV
    layer.fb.seq           = DISP_SEQ_UVUV;
    layer.fb.mode          = DISP_MOD_NON_MB_UV_COMBINED;
    layer.fb.br_swap       = 0;
    layer.fb.cs_mode      = DISP_BT601;
    layer.mode = MOD_DISP_LAYER_WORK_MODE_SCALER;
    layer.pipe             = 0;
    layer.prio             = 1;
    layer.alpha_en         = 0;
    layer.alpha_val        = 0;
    layer.ck_enable        = 0;
    layer.src_win.x        = 0;
    layer.src_win.y        = 0;
    layer.src_win.width    = jpg_size.width;
    layer.src_win.height   = jpg_size.height;
    layer.scn_win.x        = 0;
    layer.scn_win.y        = 0;
    layer.scn_win.width    = 720;//esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_WIDTH, 0, 0);
    layer.scn_win.height   = 576;//esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_HEIGHT, 0, 0);
    layer.fb.addr[0]       = (__u32)jpeg_info->y_buf;
    layer.fb.addr[1]       = (__u32)jpeg_info->v_buf;
    layer.fb.addr[2]       = (__u32)jpeg_info->u_buf;
    layer.fb.size.width = jpeg_info->pic_width;
    layer.fb.size.height = jpeg_info->pic_height;

    para[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
    para[1] = 0;
    para[2] = 0;
    hlay = esMODS_MIoctrl(mp_display,MOD_DISP_LAYER_REQUEST,de,para);

    arg[0] = hlay;
    arg[1] = (__u32)&layer;
    arg[2] = 0;
    esMODS_MIoctrl(mp_display, MOD_DISP_CMD_LAYER_SET_PARA, de, (void *)arg);
}
FINSH_FUNCTION_EXPORT_ALIAS(jpg_show_test, __cmd_jpg_show_test, jpg_show_test);
#endif

