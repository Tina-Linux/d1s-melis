/*
 * =====================================================================================
 *
 *       Filename:  mod_Display.c
 *
 *    Description:  support the connection to drv and upper application layer
 *
 *        Version:  1.0
 *         Create:  2020-08-07 15:46:44
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  liujiaming@allwinnertech.com
 *   Organization:  PDC-PD1
 *  Last Modified:  2020-08-07 15:46:44
 *
 * =====================================================================================
 */
#include <mod_display.h>
#include "mod_display_i.h"
#include <dfs_posix.h>
#include <rtthread.h>
#include <dfs_posix.h>
#include <pthread.h>


typedef struct layer_scaler{
__u32 dst_w;
__u32 dst_h;
__u32 src_w;
__u32 src_h;
} layer_scaler_t;

typedef struct layer_rect{
__u32 x;
__u32 y;
__u32 w;
__u32 h;
} layer_rect_t;

struct layer_dev {
    unsigned int req_stat;
    bool open_flag;
    disp_layer_config config;
	__u32 addr_va[3];
    int rot_degree;
    pthread_mutex_t mlock;
};

struct disp_mgr_t {
    struct layer_dev layer[CHN_NUM][LYL_NUM];
    int req_layer_cnt;

    int disp_fd;
    int cur_screen;
    unsigned int lcd_scn_phy; 
    pthread_mutex_t mlock;
    bool is_init;
};
static layer_scaler_t winscaler;
static layer_rect_t lcd_scn[CHN_NUM][LYL_NUM];
static __disp_mp_t mod_disp_mp;
static struct disp_mgr_t disp_mgr;
static bool tv_enable;
static __s32 tv_mode_last = -1;
//static disp_layer_config reserved_config[2];

int disp_module_init(void);
int disp_module_deinit(void);
int disp_dev_layer_config(__DISP_t cmd, disp_layer_config *pinfo);
int disp_layer_release(__u32 hlay);
int layer_request(__mod_disp_layer_work_mode_t mode);   //finish
int disp_layer_get_para(disp_layer_config *pinfo);
int disp_layer_open(__u32 hlay);
int disp_layer_close(__u32 hlay);
int disp_layer_set_para(disp_layer_config *pinfo);
int disp_layer_set_fb(__u32 hlay,disp_fb_info *fb);
int disp_layer_get_fb(__u32 hlay,disp_fb_info *fb);


static int check_hlay_valid(__u32 hlay)
{
    int bValid = 0;
    int ch  = HD2CHN(hlay);
    int lyl = HD2LYL(hlay);
    if(ch >=0 && ch < CHN_NUM)
    {
        if(lyl >=0 && lyl < LYL_NUM)
        {
            bValid = 1;
        }
    }
    return bValid;
}
static __disp_pixel_type_t get_fb_type(enum disp_pixel_format  format)
{
    if(format >= DISP_FORMAT_YUV444_I_AYUV && format <= DISP_FORMAT_YUV411_SP_VUVU_10BIT)
    {
        return DISP_FB_TYPE_YUV;
    }
    else
    {
        return DISP_FB_TYPE_RGB;
    }
}

static void echo_drvlayer_info(disp_layer_config *drvinfo)
{

	__log("\n");
	__log("drvinfo->channel 					= %d",drvinfo->channel);
	__log("drvinfo->layer_id  					= %d",drvinfo->layer_id);
	__log("drvinfo->enable						= %d",drvinfo->enable);
	__log("drvinfo->info.mode					= %d",drvinfo->info.mode);
	__log("drvinfo->info.alpha_mode				= %d",drvinfo->info.alpha_mode);
	__log("drvinfo->info.alpha_value			= %x",drvinfo->info.alpha_value);
	__log("drvinfo->info.zorder					= %x",drvinfo->info.zorder);
	__log("drvinfo->info.screen_win.x 			= %d",drvinfo->info.screen_win.x);
	__log("drvinfo->info.screen_win.y 			= %d",drvinfo->info.screen_win.y);
	__log("drvinfo->info.screen_win.width 		= %d",drvinfo->info.screen_win.width);
	__log("drvinfo->info.screen_win.height		= %d",drvinfo->info.screen_win.height);
	__log("drvinfo->info.fb.crop.x 				= %d",(__u32)(drvinfo->info.fb.crop.x>>32));
	__log("drvinfo->info.fb.crop.y				= %d",(__u32)(drvinfo->info.fb.crop.y>>32));
	__log("drvinfo->info.fb.crop.width			= %d",(__u32)(drvinfo->info.fb.crop.width>>32));
	__log("drvinfo->info.fb.crop.height			= %d",(__u32)(drvinfo->info.fb.crop.height>>32));
	__log("drvinfo->info.fb.format				= %d",drvinfo->info.fb.format);
	__log("drvinfo->info.fb.size[0].width		= %d",drvinfo->info.fb.size[0].width);
	__log("drvinfo->info.fb.size[0].height		= %d",drvinfo->info.fb.size[0].height);
	__log("drvinfo->info.fb.size[1].width		= %d",drvinfo->info.fb.size[1].width);
	__log("drvinfo->info.fb.size[1].height		= %d",drvinfo->info.fb.size[1].height);
	__log("drvinfo->info.fb.size[2].width		= %d",drvinfo->info.fb.size[2].width);
	__log("drvinfo->info.fb.size[2].height		= %d",drvinfo->info.fb.size[2].height);
	__log("drvinfo->info.fb.color_space			= %x",drvinfo->info.fb.color_space);
	__log("drvinfo->info.fb.align[0]			= %d",drvinfo->info.fb.align[0]);
	//__log("drvinfo->info.fb.addr[0]				= %x",drvinfo->info.fb.addr[0]);
	//__log("drvinfo->info.fb.addr[1]				= %x",drvinfo->info.fb.addr[1]);
	//__log("drvinfo->info.fb.addr[2]				= %x",drvinfo->info.fb.addr[2]);
	__log("\n");

}

static void echo_modlayer_info(__disp_layer_info_t *modinfo)
{

	__log("\n");
	__log("modinfo->mode 					= %d",modinfo->mode);
	__log("modinfo->pipe  					= %d",modinfo->pipe);
	__log("modinfo->prio					= %d",modinfo->prio);
	__log("modinfo->alpha_en				= %d",modinfo->alpha_en);
	__log("modinfo->alpha_val				= %d",modinfo->alpha_val);

	__log("modinfo->src_win.x			= %d",modinfo->src_win.x);
	__log("modinfo->src_win.y 			= %d",modinfo->src_win.y);
	__log("modinfo->src_win.width 		= %d",modinfo->src_win.width);
	__log("modinfo->src_win.height		= %d",modinfo->src_win.height);

	__log("modinfo->scn_win.x			= %d",modinfo->scn_win.x);
	__log("modinfo->scn_win.y 			= %d",modinfo->scn_win.y);
	__log("modinfo->scn_win.width 		= %d",modinfo->scn_win.width);
	__log("modinfo->scn_win.height		= %d",modinfo->scn_win.height);

	__log("modinfo->fb.format			= %d",modinfo->fb.format);
	__log("modinfo->fb.size.width		= %d",modinfo->fb.size.width);
	__log("modinfo->fb.size.height		= %d",modinfo->fb.size.height);
	__log("modinfo->fb.cs_mode		= %x",modinfo->fb.cs_mode);
	__log("modinfo->fb.br_swap		= %d",modinfo->fb.br_swap);
	__log("modinfo->fb.seq			= %d",modinfo->fb.seq);
	__log("modinfo->fb.format		= %d",modinfo->fb.format);
	//__log("drvinfo->info.fb.addr[0]				= %x",drvinfo->info.fb.addr[0]);
	//__log("drvinfo->info.fb.addr[1]				= %x",drvinfo->info.fb.addr[1]);
	//__log("drvinfo->info.fb.addr[2]				= %x",drvinfo->info.fb.addr[2]);
	__log("\n");

}

static void mod2drv_fb(disp_fb_info *drvfb,__disp_video_fb_t *modfb,__u32 hlay)
{
	/*
	drvfb->info.fb.crop.x 			= disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].config.info.fb.crop.x;
    drvfb->info.fb.crop.y 			= disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].config.info.fb.crop.y;
	drvfb->info.fb.crop.width  		= disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].config.info.fb.crop.width;
	drvfb->info.fb.crop.height 		= disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].config.info.fb.crop.height;

	//fb
	drvfb->info.fb.format 			= disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].config.info.fb.format;
    drvfb->info.fb.size[0].width    = disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].config.info.fb.size[0].width;
	drvfb->info.fb.size[0].height	= disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].config.info.fb.size[0].height;
	drvfb->info.fb.size[1].width    = disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].config.info.fb.size[1].width;
	drvfb->info.fb.size[1].height	= disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].config.info.fb.size[1].height;
	drvfb->info.fb.size[2].width	= disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].config.info.fb.size[2].width;
	drvfb->info.fb.size[2].height	= disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].config.info.fb.size[2].height;
    drvfb->info.fb.color_space 		= disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].config.info.fb.color_space;
    drvfb->info.fb.align[0] 			= 4;//?
    //drvfb->info.fb.align[1] 			= 4;//?
   // drvfb->info.fb.align[2] 			= 4;//?
   	*/

	//drvfb->addr[0] 			= (__u32)modfb->addr[0];
	//drvfb->addr[1] 			= (__u32)modfb->addr[1];
	//drvfb->addr[2] 			= (__u32)modfb->addr[2];
	memcpy(drvfb,&(disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].config.info.fb), sizeof(disp_fb_info));

    if(DISP_FORMAT_YUV420_P_YV12 == modfb->format)
    {
        drvfb->addr[0]          = esMEMS_VA2PA((__u32)modfb->addr[0]);
        drvfb->addr[1]          = esMEMS_VA2PA((__u32)modfb->addr[2]);
        drvfb->addr[2]          = esMEMS_VA2PA((__u32)modfb->addr[1]);

        disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].addr_va[0] = (__u32)modfb->addr[0];
        disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].addr_va[1] = (__u32)modfb->addr[2];
        disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].addr_va[2] = (__u32)modfb->addr[1];
    }
    else
    {
        drvfb->addr[0]          = esMEMS_VA2PA((__u32)modfb->addr[0]);
        drvfb->addr[1]          = esMEMS_VA2PA((__u32)modfb->addr[1]);
        drvfb->addr[2]          = esMEMS_VA2PA((__u32)modfb->addr[2]);

        disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].addr_va[0] = (__u32)modfb->addr[0];
        disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].addr_va[1] = (__u32)modfb->addr[1];
        disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].addr_va[2] = (__u32)modfb->addr[2];
    }
}

static void drv2mod_fb(disp_fb_info *drvfb,__disp_fb_t *modfb)
{
	modfb->format = drvfb->format;
	modfb->size.width = drvfb->size[0].width;
	modfb->size.height = drvfb->size[0].height;

	modfb->cs_mode = drvfb->color_space;

	modfb->addr[0] = drvfb->addr[0];
	modfb->addr[1] = drvfb->addr[1];
	modfb->addr[2] = drvfb->addr[2];
}

static void mod2drv_layerinfo(disp_layer_config *drvinfo, __disp_layer_info_t *modinfo, __u32 hlay)
{

	drvinfo->channel = HD2CHN(hlay);
    drvinfo->layer_id = HD2LYL(hlay);
	drvinfo->enable = 1;
	drvinfo->info.mode = LAYER_MODE_BUFFER;
	drvinfo->info.alpha_mode 			= modinfo->alpha_en;
	drvinfo->info.alpha_value 			= modinfo->alpha_val;
	drvinfo->info.zorder 				= modinfo->prio;
	//screen
	drvinfo->info.screen_win.x 			= modinfo->scn_win.x;
	drvinfo->info.screen_win.y 			= modinfo->scn_win.y;
	drvinfo->info.screen_win.width 		= modinfo->scn_win.width;
	drvinfo->info.screen_win.height 	= modinfo->scn_win.height;
	//source

    if (get_fb_type(modinfo->fb.format) == DISP_FB_TYPE_YUV) //yuv channel
    {
        drvinfo->info.fb.crop.x = (long long)modinfo->src_win.x << 32;
        drvinfo->info.fb.crop.y = (long long)modinfo->src_win.y << 32;
        drvinfo->info.fb.crop.width = (long long)modinfo->src_win.width << 32;
        drvinfo->info.fb.crop.height = (long long)modinfo->src_win.height << 32;
        drvinfo->info.fb.color_space = modinfo->fb.cs_mode;
    }
    else
    {
        drvinfo->info.fb.crop.x = (long long)modinfo->src_win.x << 32;
        drvinfo->info.fb.crop.y = (long long)modinfo->src_win.y << 32;
        drvinfo->info.fb.crop.width = (long long)modinfo->src_win.width << 32;
        drvinfo->info.fb.crop.height = (long long)modinfo->src_win.height << 32;
        drvinfo->info.fb.color_space = 0;//
    }

	drvinfo->info.fb.format 			= modinfo->fb.format;
    drvinfo->info.fb.size[0].width    	= modinfo->fb.size.width;
	drvinfo->info.fb.size[0].height		= modinfo->fb.size.height;
    
    if ((DISP_FORMAT_YUV420_P == modinfo->fb.format)
    || (DISP_FORMAT_YUV420_SP_UVUV == modinfo->fb.format)
    || (DISP_FORMAT_YUV420_SP_VUVU == modinfo->fb.format)
    || (DISP_FORMAT_YUV420_P_YV12 == modinfo->fb.format)
    || (DISP_FORMAT_YUV420_P_YU12 == modinfo->fb.format))
    {
        drvinfo->info.fb.size[1].width = modinfo->fb.size.width / 2;
        drvinfo->info.fb.size[1].height = modinfo->fb.size.height / 2;
        drvinfo->info.fb.size[2].width = modinfo->fb.size.width / 2;
        drvinfo->info.fb.size[2].height = modinfo->fb.size.height / 2;
    }
    else if ((DISP_FORMAT_YUV422_P == modinfo->fb.format)
     || (DISP_FORMAT_YUV422_SP_UVUV == modinfo->fb.format)
     || (DISP_FORMAT_YUV422_SP_VUVU == modinfo->fb.format)
     || (DISP_FORMAT_YUV422_I_YVYU == modinfo->fb.format)
     || (DISP_FORMAT_YUV422_I_YUYV ==modinfo->fb.format)
     || (DISP_FORMAT_YUV422_I_UYVY == modinfo->fb.format)
     || (DISP_FORMAT_YUV422_I_VYUY == modinfo->fb.format))
    {
        drvinfo->info.fb.size[1].width = modinfo->fb.size.width / 2;
        drvinfo->info.fb.size[1].height =modinfo->fb.size.height;
        drvinfo->info.fb.size[2].width = modinfo->fb.size.width / 2;
        drvinfo->info.fb.size[2].height =modinfo->fb.size.height;
    }
    else if ((DISP_FORMAT_YUV411_P == modinfo->fb.format)
    || (DISP_FORMAT_YUV411_SP_UVUV == modinfo->fb.format)
    || (DISP_FORMAT_YUV411_SP_VUVU == modinfo->fb.format))
    {
        drvinfo->info.fb.size[1].width = modinfo->fb.size.width / 4;
        drvinfo->info.fb.size[1].height = modinfo->fb.size.height;
        drvinfo->info.fb.size[2].width = modinfo->fb.size.width / 4;
        drvinfo->info.fb.size[2].height = modinfo->fb.size.height;
    }
    else if ((DISP_FORMAT_YUV444_P == modinfo->fb.format)
    || (DISP_FORMAT_YUV444_I_AYUV == modinfo->fb.format)
    || (DISP_FORMAT_YUV444_I_VUYA == modinfo->fb.format))
    {
        drvinfo->info.fb.size[1].width = modinfo->fb.size.width;
        drvinfo->info.fb.size[1].height = modinfo->fb.size.height;
        drvinfo->info.fb.size[2].width = modinfo->fb.size.width;
        drvinfo->info.fb.size[2].height = modinfo->fb.size.height;
    }
    else
    {
        drvinfo->info.fb.size[1].width = modinfo->fb.size.width;
        drvinfo->info.fb.size[1].height = modinfo->fb.size.height;
        drvinfo->info.fb.size[2].width = modinfo->fb.size.width;
        drvinfo->info.fb.size[2].height = modinfo->fb.size.height;
    }
    drvinfo->info.fb.align[0] 			= 0;
    drvinfo->info.fb.align[1] 			= 0;
    drvinfo->info.fb.align[2] 			= 0;
    if(DISP_FORMAT_YUV420_P_YV12 == modinfo->fb.format)
    {
        drvinfo->info.fb.addr[0] 			= esMEMS_VA2PA((__u32)modinfo->fb.addr[0]);
        drvinfo->info.fb.addr[1] 			= esMEMS_VA2PA((__u32)modinfo->fb.addr[2]);
        drvinfo->info.fb.addr[2] 			= esMEMS_VA2PA((__u32)modinfo->fb.addr[1]);

        disp_mgr.layer[drvinfo->channel][drvinfo->layer_id].addr_va[0] = (__u32)modinfo->fb.addr[0];
        disp_mgr.layer[drvinfo->channel][drvinfo->layer_id].addr_va[1] = (__u32)modinfo->fb.addr[2];
        disp_mgr.layer[drvinfo->channel][drvinfo->layer_id].addr_va[2] = (__u32)modinfo->fb.addr[1];
        drvinfo->info.fb.format = DISP_FORMAT_YUV420_P;
    }
    else if(DISP_FORMAT_YUV420_P_YU12 == modinfo->fb.format)
    {
        drvinfo->info.fb.addr[0] 			= esMEMS_VA2PA((__u32)modinfo->fb.addr[0]);
        drvinfo->info.fb.addr[1] 			= esMEMS_VA2PA((__u32)modinfo->fb.addr[1]);
        drvinfo->info.fb.addr[2] 			= esMEMS_VA2PA((__u32)modinfo->fb.addr[2]);

        disp_mgr.layer[drvinfo->channel][drvinfo->layer_id].addr_va[0] = (__u32)modinfo->fb.addr[0];
        disp_mgr.layer[drvinfo->channel][drvinfo->layer_id].addr_va[1] = (__u32)modinfo->fb.addr[1];
        disp_mgr.layer[drvinfo->channel][drvinfo->layer_id].addr_va[2] = (__u32)modinfo->fb.addr[2];
        drvinfo->info.fb.format = DISP_FORMAT_YUV420_P;
    }
    else
    {
        drvinfo->info.fb.addr[0] 			= esMEMS_VA2PA((__u32)modinfo->fb.addr[0]);
        drvinfo->info.fb.addr[1] 			= esMEMS_VA2PA((__u32)modinfo->fb.addr[1]);
        drvinfo->info.fb.addr[2] 			= esMEMS_VA2PA((__u32)modinfo->fb.addr[2]);

        disp_mgr.layer[drvinfo->channel][drvinfo->layer_id].addr_va[0] = (__u32)modinfo->fb.addr[0];
        disp_mgr.layer[drvinfo->channel][drvinfo->layer_id].addr_va[1] = (__u32)modinfo->fb.addr[1];
        disp_mgr.layer[drvinfo->channel][drvinfo->layer_id].addr_va[2] = (__u32)modinfo->fb.addr[2];
    }

    if(modinfo->mode == MOD_DISP_LAYER_WORK_MODE_NORMAL)
    {
        drvinfo->info.fb.lbc_en = 0;
        return;
    }
    __log("lbc_mode = %d", modinfo->fb.lbc_mode);
    if (modinfo->fb.lbc_mode > 0)
    {
        u32 seg_width = 16, seg_height = 4;
        u32 bit_depth = 8;
        u32 cmp_ratio = 0;
        u32 seg_tar_bits, seg_tar_bits_out;
        int y_mode_bits, c_mode_bits, y_data_bits, c_data_bits;
        u32 segline_tar_bits = 0, segline_tar_bits_out = 0;
        int frm_width = modinfo->fb.size.width;
	
	if (modinfo->fb.is_10bit_flag == 1)
	{	
	   bit_depth = 10;
	}
        if (modinfo->fb.lbc_mode == 1)//1.5x
        {
            cmp_ratio = 666;
        }
        else if (modinfo->fb.lbc_mode == 2)//2x
        {
            cmp_ratio = 500;
        }
        else if (modinfo->fb.lbc_mode == 3)//2.5x
        {
            cmp_ratio = 400;
        }
        else
        {
            drvinfo->info.fb.lbc_en = 0;
            return;
        }

        drvinfo->info.fb.lbc_en = 1;
        drvinfo->info.fb.lbc_info.is_lossy = modinfo->fb.is_lossy;
        drvinfo->info.fb.lbc_info.rc_en = modinfo->fb.rc_en;

        if (drvinfo->info.fb.lbc_info.is_lossy)
        {
            seg_tar_bits = ((seg_width * seg_height * bit_depth * cmp_ratio * 3 / 2000) / ALIGN) * ALIGN;
            if (drvinfo->info.fb.lbc_info.rc_en == 0)
            {
                segline_tar_bits = ((frm_width + seg_width - 1) / seg_width) * seg_tar_bits;
            }
            else if (drvinfo->info.fb.lbc_info.rc_en == 1)
            {
                segline_tar_bits = ((frm_width + seg_width - 1) / seg_width * seg_width * seg_height * bit_depth * cmp_ratio * 3 / 2000 + ALIGN - 1) / ALIGN * ALIGN;
            }
        }
        else
        {
            y_mode_bits = seg_width / MB_WTH * (CODE_MODE_Y_BITS * 2 + BLK_MODE_BITS);
            c_mode_bits = 2 * (seg_width / 2 / MB_WTH * CODE_MODE_C_BITS);
            y_data_bits = seg_width * seg_height * bit_depth;
            c_data_bits = seg_width * seg_height * bit_depth / 2 + 2 * (seg_width / 2 / MB_WTH) * C_DTS_BITS;
            seg_tar_bits= (y_data_bits + c_data_bits + y_mode_bits + c_mode_bits + ALIGN - 1) / ALIGN * ALIGN;
            segline_tar_bits = ((frm_width + seg_width - 1) / seg_width * seg_width / seg_width) * seg_tar_bits;
        }

        seg_tar_bits_out = seg_tar_bits / 8;
        segline_tar_bits_out = segline_tar_bits / 8;

        drvinfo->info.fb.lbc_info.pitch = segline_tar_bits_out;
        drvinfo->info.fb.lbc_info.seg_bit = seg_tar_bits_out;
        __wrn("segline_tar_bits_out = %d, seg_tar_bits_out = %d",segline_tar_bits_out,seg_tar_bits_out);
	}


}

static void drv2mod_layerinfo(disp_layer_config *drvinfo, __disp_layer_info_t *modinfo)
{
	//drvinfo->channel 					= HD2CHN(hlay);
	//drvinfo->layer_id 					= HD2LYL(hlay);
	//drvinfo->enable = 1;
	//drvinfo->info.mode = LAYER_MODE_BUFFER;
	modinfo->mode = (drvinfo->channel==1) ? 1:4;
	modinfo->alpha_en = drvinfo->info.alpha_mode;
	modinfo->alpha_val = drvinfo->info.alpha_value;
	modinfo->prio = drvinfo->info.zorder;
	//screen
	modinfo->scn_win.x = drvinfo->info.screen_win.x;
	modinfo->scn_win.y = drvinfo->info.screen_win.y;
	modinfo->scn_win.width = drvinfo->info.screen_win.width;
	modinfo->scn_win.height = drvinfo->info.screen_win.height;
	//source
	modinfo->src_win.x = drvinfo->info.fb.crop.x>>32;
	modinfo->src_win.y = drvinfo->info.fb.crop.y>>32;
	modinfo->src_win.width = drvinfo->info.fb.crop.width>>32;
	modinfo->src_win.height = drvinfo->info.fb.crop.height>>32;

	//fb
	modinfo->fb.format = drvinfo->info.fb.format;
	modinfo->fb.size.width = drvinfo->info.fb.size[0].width;
	modinfo->fb.size.height = drvinfo->info.fb.size[0].height;

	modinfo->fb.cs_mode = drvinfo->info.fb.color_space;
	//modinfo->fb.addr[0] = drvinfo->info.fb.addr[0];
	//modinfo->fb.addr[1] = drvinfo->info.fb.addr[1];
	//modinfo->fb.addr[2] = drvinfo->info.fb.addr[2];

	modinfo->fb.addr[0] = disp_mgr.layer[drvinfo->channel][drvinfo->layer_id].addr_va[0];
	modinfo->fb.addr[1] = disp_mgr.layer[drvinfo->channel][drvinfo->layer_id].addr_va[1];
	modinfo->fb.addr[2] = disp_mgr.layer[drvinfo->channel][drvinfo->layer_id].addr_va[2];

}


static int disp_layer_manager_init(void)
{
    int ret = RET_OK;
    disp_mgr.cur_screen = SCREEN_0;
    if (disp_mgr.is_init) {
        return RET_FAIL;
    }

    disp_mgr.disp_fd = open(DISP_DEV, O_WRONLY);
    if (disp_mgr.disp_fd < 0)
    {
        __log("Failed to open disp device, ret:%x, errno: %d", disp_mgr.disp_fd, errno);
        return  RET_FAIL;
    }


   // disp_mgr.layer[UI_LYCHN][UI_LYLAY].req_stat = HWD_STATUS_REQUESTED;  //used by fb0

    //display resume
    int args[4]={0};
    int ch;
    int lyl;
    int err;

    args[0] = 0;
    args[1] = 0;
    ioctl(disp_mgr.disp_fd, DISP_BLANK, args);
    disp_mgr.is_init = 1;
    disp_mgr.req_layer_cnt = 0;

    for(ch=0; ch<CHN_NUM; ch++)
    {
        for(lyl=0; lyl<LYL_NUM; lyl++)
        {
            disp_mgr.layer[ch][lyl].config.channel = ch;
            disp_mgr.layer[ch][lyl].config.layer_id = lyl;
            #if 1
            tv_enable = 0;
            winscaler.src_w = ioctl(disp_mgr.disp_fd, DISP_GET_SCN_WIDTH, args);
            winscaler.src_h = ioctl(disp_mgr.disp_fd, DISP_GET_SCN_HEIGHT, args);
            winscaler.dst_w = winscaler.src_w;
            winscaler.dst_h = winscaler.src_h;
            disp_mgr.lcd_scn_phy =  winscaler.src_w << 16 | winscaler.src_h;           
            #endif
            err = pthread_mutex_init(&disp_mgr.layer[ch][lyl].mlock, NULL);
            if(err != 0)
            {
                __log("fatal error! pthread mutex init fail!");
                ret = RET_FAIL;
                goto _exit0;
            }
            if(UI_LYCHN == ch && UI_LYLAY == lyl)
            {
                ret = disp_dev_layer_config(DISP_LAYER_GET_CONFIG, &disp_mgr.layer[ch][lyl].config);
                if (ret < 0)
                {
                    __log("get para failed");
                }
                else
                {
                   disp_layer_config *pLayerConfig = &disp_mgr.layer[ch][lyl].config;
                  //  __log("ret[%d][%d,%d]ch[%d]lyl[%d] init: enable[%d], screenwin[%d,%d, %dx%d], zorder[%d], alpha[mode:%d, value:%d]",
                    //    ret, ch, lyl, pLayerConfig->channel, pLayerConfig->layer_id, pLayerConfig->enable,
                    //    pLayerConfig->info.screen_win.x, pLayerConfig->info.screen_win.y, pLayerConfig->info.screen_win.width, pLayerConfig->info.screen_win.height,
                    //    pLayerConfig->info.zorder, pLayerConfig->info.alpha_mode, pLayerConfig->info.alpha_value);
                }
                disp_mgr.layer[ch][lyl].config.channel = ch;
                disp_mgr.layer[ch][lyl].config.layer_id = lyl;
            }

        }
    }
_exit0:
    return ret;
}



static int disp_layer_manager_deinit(void)
{
    int ret = RET_OK;

    int ch;
    int lyl;
    int err;
    for(ch=0; ch<CHN_NUM; ch++)
    {
        for(lyl=0; lyl<LYL_NUM; lyl++)
        {
            err = pthread_mutex_destroy(&disp_mgr.layer[ch][lyl].mlock);
            if(err != 0)
            {
                __log("fatal error! pthread mutex destroy fail!");
                ret = RET_FAIL;
            }
        }
    }
    close(disp_mgr.disp_fd);
    disp_mgr.disp_fd = -1;

    return ret;
}


int disp_module_init(void)
{
    memset(&disp_mgr, 0, sizeof(disp_mgr));
    pthread_mutex_init(&disp_mgr.mlock, NULL);

    disp_mgr.disp_fd = -1;
    return disp_layer_manager_init();
}


int disp_module_deinit()
{
    if (disp_mgr.is_init) {
        disp_layer_manager_deinit();
        disp_mgr.is_init = 0;
        pthread_mutex_destroy(&disp_mgr.mlock);
    }
    return 0;
}

int disp_dev_layer_config(__DISP_t cmd, disp_layer_config *pinfo)
{
    unsigned long args[4] = {0};
    int ret = RET_OK;
#if 1
    if(tv_enable)
    {       
        if(cmd == DISP_LAYER_SET_CONFIG)
        {
            pinfo->info.screen_win.x        = pinfo->info.screen_win.x * winscaler.dst_w / winscaler.src_w;
            pinfo->info.screen_win.y        = pinfo->info.screen_win.y * winscaler.dst_h / winscaler.src_h;
            pinfo->info.screen_win.width    = pinfo->info.screen_win.width * winscaler.dst_w / winscaler.src_w;
            pinfo->info.screen_win.height   = pinfo->info.screen_win.height * winscaler.dst_h / winscaler.src_h;
        }

    }
    //if()
#endif
    args[0] = disp_mgr.cur_screen;
    args[1] = (unsigned long)pinfo;
    args[2] = 1;
    ret = ioctl(disp_mgr.disp_fd, cmd, args);
#if 1
    if(tv_enable)
    {       
        if(cmd == DISP_LAYER_GET_CONFIG)
        {
            pinfo->info.screen_win.x        = lcd_scn[pinfo->channel][pinfo->layer_id].x;
            pinfo->info.screen_win.y        = lcd_scn[pinfo->channel][pinfo->layer_id].y;
            pinfo->info.screen_win.width    = lcd_scn[pinfo->channel][pinfo->layer_id].w;
            pinfo->info.screen_win.height   = lcd_scn[pinfo->channel][pinfo->layer_id].h;
        }
    }
#endif    
    if(ret < 0){
        __log("ioctl failed: %s", strerror(errno));
        ret = RET_FAIL;
	}
    return ret;
}

int layer_request(__mod_disp_layer_work_mode_t mode)   //finish
{
    int ch;
    int id;

    pthread_mutex_lock(&disp_mgr.mlock);
	if(mode == MOD_DISP_LAYER_WORK_MODE_SCALER)
	{
		__wrn("layer_request MOD_DISP_LAYER_WORK_MODE_SCALER");
	    for(id=0; id<LYL_NUM; id++)
		{
            ch = 0;
            {
	            if(!(disp_mgr.layer[ch][id].req_stat & HWD_STATUS_REQUESTED))
				{
	                disp_mgr.layer[ch][id].req_stat |= HWD_STATUS_REQUESTED;
	                goto out;
	            }
            }
    	}

		__log(" err :scaler layer use up");
	}
	else
	{
		__wrn("layer_request MOD_DISP_LAYER_WORK_MODE_NORMAL");

	    for(id=0; id<LYL_NUM; id++)
		{
	        ch = 1;
			{
	            if(!(disp_mgr.layer[ch][id].req_stat & HWD_STATUS_REQUESTED))
				{
	                disp_mgr.layer[ch][id].req_stat |= HWD_STATUS_REQUESTED;
	                goto out;
	            }
            }
        }
	}

out:
    if (id==LYL_NUM) {
        __log("layer use up.");
		pthread_mutex_unlock(&disp_mgr.mlock);

        return 0;
    }

    disp_mgr.req_layer_cnt++;
	pthread_mutex_unlock(&disp_mgr.mlock);
    __log("requested: ch:%d, id:%d   disp_mgr.req_layer_cnt = %d", ch, id,disp_mgr.req_layer_cnt);
    return HLAY(ch, id);
}


int disp_layer_release(__u32 hlay)
{
    if(!check_hlay_valid(hlay))
    {
        __log("fatal error! hlay[%d] invalid!", hlay);
        return RET_FAIL;
    }
	disp_layer_config config;
    //memset(&config, 0, sizeof(disp_layer_config));

    int chn = HD2CHN(hlay);
    int lyl = HD2LYL(hlay);
    int ret = disp_layer_close(hlay);
    #ifdef DE_LAYER_BUG
    if(lyl != 0)
    {
		memset(&config, 0, sizeof(disp_layer_config));
    }
    #endif
    if (RET_OK == ret) {
        pthread_mutex_lock(&disp_mgr.mlock);
        if (disp_mgr.layer[chn][lyl].req_stat)
		{
    		memset(&config, 0, sizeof(disp_layer_config));
            config.channel = chn;
            config.layer_id = lyl;
			disp_layer_set_para(&config);
            disp_mgr.layer[chn][lyl].req_stat = 0;
			disp_mgr.layer[chn][lyl].open_flag = 0;
			memset(&disp_mgr.layer[chn][lyl].config,0, sizeof(disp_layer_config));
			disp_mgr.layer[chn][lyl].addr_va[0] = 0;
			disp_mgr.layer[chn][lyl].addr_va[1] = 0;
			disp_mgr.layer[chn][lyl].addr_va[2] = 0;
			disp_mgr.layer[chn][lyl].rot_degree = 0;
            disp_mgr.req_layer_cnt--;
            if(tv_enable)
            {
                memset(&lcd_scn[chn][lyl], 0, sizeof(layer_rect_t)); 
            }
        }
		pthread_mutex_unlock(&disp_mgr.mlock);
    }
    return ret;

}

int disp_layer_open(__u32 hlay)
{
    if(!check_hlay_valid(hlay))
    {
        __log("fatal error! hlay[%d] invalid!", hlay);
        return RET_FAIL;
    }
    int ret = 0;
    __u64 tempw,temph;
    disp_layer_config config;
    memset(&config, 0, sizeof(disp_layer_config));
    config.channel  = HD2CHN(hlay);
    config.layer_id = HD2LYL(hlay);
    pthread_mutex_lock(&disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].mlock);
    #ifdef DE_LAYER_BUG
    if(config.layer_id == 0)
    {
        memcpy(&config,&reserved_config[config.channel],sizeof(disp_layer_config));
        config.info.screen_win.width = reserved_config[config.channel].info.screen_win.width;
        config.info.screen_win.height = reserved_config[config.channel].info.screen_win.height;
        tempw = reserved_config[config.channel].info.fb.crop.width;
        temph = reserved_config[config.channel].info.fb.crop.height;
        __log("temp w h %llx  %llx ",tempw,temph);
        config.info.fb.crop.width = tempw << 32;//reserved_config[config.channel].info.fb.crop.width << 32;
        config.info.fb.crop.height = temph << 32;// reserved_config[config.channel].info.fb.crop.height << 32;
    }
    else
    #endif
    {
        disp_layer_get_para(&config);
    }
    config.enable = 1;
    ret = disp_layer_set_para(&config);
    if (0 == ret) {
        disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].open_flag = config.enable;
        disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].config.enable = config.enable;
    }
    pthread_mutex_unlock(&disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].mlock);
    return ret;
}




int disp_layer_close(__u32 hlay)
{
    if(!check_hlay_valid(hlay))
    {
        __log("fatal error! hlay[%d] invalid!", hlay);
        return RET_FAIL;
    }
    int ret = 0;
    unsigned int i;
    disp_layer_config config;
    memset(&config, 0, sizeof(disp_layer_config));
    config.channel  = HD2CHN(hlay);
    config.layer_id = HD2LYL(hlay);
    pthread_mutex_lock(&disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].mlock);
    #ifdef DE_LAYER_BUG
    if(config.layer_id == 0)
    {
        disp_layer_get_para(&config);
        __log("000 fb.crop.width = %x  fb.crop.height = %x",config.info.fb.crop.width,config.info.fb.crop.height);
        memcpy(&reserved_config[config.channel],&config,sizeof(disp_layer_config));
        reserved_config[config.channel].info.screen_win.width = config.info.screen_win.width;
        reserved_config[config.channel].info.screen_win.height = config.info.screen_win.height;
        __log("reserved con w = %d  h =%d",reserved_config[config.channel].info.screen_win.width,reserved_config[config.channel].info.screen_win.height);
        reserved_config[config.channel].info.fb.crop.width =config.info.fb.crop.width >> 32;
        reserved_config[config.channel].info.fb.crop.height =config.info.fb.crop.height >> 32;
        __log("111 fb.crop.width = %x  fb.crop.height = %x",reserved_config[config.channel].info.fb.crop.width,reserved_config[config.channel].info.fb.crop.height);
        config.enable = 1;
        config.info.screen_win.width = 1;
        config.info.screen_win.height = 1;
        config.info.fb.crop.width = 1ul<<32;
        config.info.fb.crop.height = 1ul<<32;
        ret = disp_layer_set_para(&config);

        if (0 == ret) {
            disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].open_flag = config.enable;
            disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].config.enable = config.enable;
        }
    }
    else
    #endif
    {
        disp_layer_get_para(&config);
        config.enable = 0;
        __wrn("......close layer %d   ch = %d  ly = %d",hlay,config.channel,config.layer_id);
        ret = disp_layer_set_para(&config);
        if(0 == ret)
        {
            disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].open_flag = config.enable;
            disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].config.enable = config.enable;
        }
        
        for(i=0; i< LYL_NUM; i++)
        {
            if(disp_mgr.layer[config.channel][i].config.enable)
            {
                 disp_layer_open(HLAY(config.channel,i));
            }
        }
    }
    pthread_mutex_unlock(&disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].mlock);
    return ret;
}



int disp_layer_get_para(disp_layer_config *pinfo)
{
    //__log("xxxxx disp_layer_get_para");
    /*int ret = layer_config(DISP_LAYER_GET_CONFIG, pinfo);
    if (ret < 0)
        __log("get para failed");*/
    int bValid = 0;
    if(pinfo != NULL)
    {
        if(pinfo->channel < CHN_NUM && pinfo->layer_id < LYL_NUM)
        {
            bValid = 1;
        }
        else
        {
            __log("fatal error! pinfo->channel[%d]layer_id[%d] is invalid!", pinfo->channel, pinfo->layer_id);
        }
    }
    else
    {
        __log("fatal error! pinfo == NULL!");
    }
    int ret = RET_OK;
    if(bValid)
    {
        if(!disp_mgr.layer[pinfo->channel][pinfo->layer_id].config.enable)    //if disable, then driver will not store para, use our storage.
        {
            //__log("ch[%d]lyl[%d] disable, use storage value", pinfo->channel, pinfo->layer_id);
            memcpy(pinfo, &disp_mgr.layer[pinfo->channel][pinfo->layer_id].config, sizeof(disp_layer_config));
            ret = RET_OK;
        }
        else
        {
            ret = disp_dev_layer_config(DISP_LAYER_GET_CONFIG, pinfo);
            if (ret < 0)
            {
                __log("get para failed");
            }
            else
            {
                if(ret > 0)
                {
                    __log("Be careful! layer_config ret[%d] > 0, is it normal?", ret);
                }
                /*if(memcmp(&disp_mgr.layer[pinfo->channel][pinfo->layer_id].config, pinfo, sizeof(disp_layer_config))!=0)
                {
                    disp_layer_config *pUserConfig = &disp_mgr.layer[pinfo->channel][pinfo->layer_id].config;
                    __log("fatal error! ch[%d]lyl[%d] disp layer config is not match!", pinfo->channel, pinfo->layer_id);
                }*/
            }
        }
    }
    else
    {
        ret = RET_FAIL;
    }
    return ret;
}


int disp_layer_set_para(disp_layer_config *pinfo)
{
    int ret = disp_dev_layer_config(DISP_LAYER_SET_CONFIG, pinfo);
    if (ret < 0)
    {
        __log("set para failed");
    }
    else
    {
        if(ret > 0)
        {
            __log("Be careful! layer_config ret[%d] > 0, is it normal?", ret);
        }
        int bValid = 0;
        if(pinfo != NULL)
        {
            if(pinfo->channel < CHN_NUM && pinfo->layer_id < LYL_NUM)
            {
                bValid = 1;
            }
            else
            {
                __log("fatal error! pinfo->channel[%d]layer_id[%d] is invalid!", pinfo->channel, pinfo->layer_id);
            }
        }
        else
        {
            __log("fatal error! pinfo == NULL!");
        }
        if(bValid)
        {
            memcpy(&disp_mgr.layer[pinfo->channel][pinfo->layer_id].config, pinfo, sizeof(disp_layer_config));
        }
    }
    return ret;
}


int disp_layer_set_fb(__u32 hlay,disp_fb_info *fb)
{
    if(!check_hlay_valid(hlay))
    {
        __log("fatal error! hlay[%d] invalid!", hlay);
        return RET_FAIL;
    }

	if(!fb)
    {
        __log("fatal error! input fb is null!", hlay);
        return RET_FAIL;
    }

    disp_layer_config config;
    memset(&config, 0, sizeof(disp_layer_config));
    config.channel  = HD2CHN(hlay);
    config.layer_id = HD2LYL(hlay);
    //pthread_mutex_lock(&disp_mgr.mlock);
    pthread_mutex_lock(&disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].mlock);
    disp_layer_get_para(&config);

	memcpy(&(config.info.fb), fb, sizeof(disp_fb_info));
	disp_layer_set_para(&config);
    pthread_mutex_unlock(&disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].mlock);
	return RET_OK;
}

int disp_layer_get_fb(__u32 hlay,disp_fb_info *fb)
{
    if(!check_hlay_valid(hlay))
    {
        __log("fatal error! hlay[%d] invalid!", hlay);
        return RET_FAIL;
    }

	if(!fb)
    {
        __log("fatal error! input fb is null!", hlay);
        return RET_FAIL;
    }

	memcpy(fb,&(disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].config.info.fb), sizeof(disp_fb_info));

	fb->addr[0] = disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].addr_va[0];
	fb->addr[1] = disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].addr_va[1];
	fb->addr[2] = disp_mgr.layer[HD2CHN(hlay)][HD2LYL(hlay)].addr_va[2];
	return RET_OK;
}

 void auto_resolution_adapt(__u32 mode)
 {
    __u32 i,j;
    disp_layer_config config;

    switch(mode)
    {
        case MOD_DISP_TV_MOD_PAL:
            tv_enable = 1;
            winscaler.dst_w  = 720;
            winscaler.dst_h = 576;
            if(tv_mode_last == MOD_DISP_TV_MOD_NTSC)
            {
                winscaler.src_w  = 720;
                winscaler.src_h = 480;                
            }
            else
            {
                winscaler.src_w = disp_mgr.lcd_scn_phy >> 16;
                winscaler.src_h = disp_mgr.lcd_scn_phy & 0xffff;                    
            }           
        break;

        case MOD_DISP_TV_MOD_NTSC:
            tv_enable = 1;
            winscaler.dst_w  = 720;
            winscaler.dst_h = 480;
            if(tv_mode_last == MOD_DISP_TV_MOD_PAL)
            {
                winscaler.src_w  = 720;
                winscaler.src_h = 576;                
            }
            else
            {
                winscaler.src_w = disp_mgr.lcd_scn_phy >> 16;
                winscaler.src_h = disp_mgr.lcd_scn_phy & 0xffff;                    
            }                     
        break;

        case MOD_DISP_TV_MOD_NULL:
            tv_enable = 0;        
            winscaler.dst_w = disp_mgr.lcd_scn_phy >> 16;
            winscaler.dst_h = disp_mgr.lcd_scn_phy & 0xffff;
            winscaler.src_w = disp_mgr.lcd_scn_phy >> 16;
            winscaler.src_h = disp_mgr.lcd_scn_phy & 0xffff;                  
            
        break;
        
    }
    for(i=0; i< LYL_NUM; i++)
    {
        for(j=0; j< CHN_NUM; j++)
        {
            if(disp_mgr.layer[j][i].config.enable)
            {
               if(mode == MOD_DISP_TV_MOD_NULL)
               {//exit tv
                disp_mgr.layer[j][i].config.info.screen_win.x      = lcd_scn[j][i].x;
                disp_mgr.layer[j][i].config.info.screen_win.y      = lcd_scn[j][i].y;
                disp_mgr.layer[j][i].config.info.screen_win.width  = lcd_scn[j][i].w;
                disp_mgr.layer[j][i].config.info.screen_win.height = lcd_scn[j][i].h;          
               }
               else
               {//enter tv
                lcd_scn[j][i].x = disp_mgr.layer[j][i].config.info.screen_win.x;
                lcd_scn[j][i].y = disp_mgr.layer[j][i].config.info.screen_win.y;
                lcd_scn[j][i].w = disp_mgr.layer[j][i].config.info.screen_win.width;
                lcd_scn[j][i].h = disp_mgr.layer[j][i].config.info.screen_win.height;
               }                
                disp_layer_set_para(&disp_mgr.layer[j][i].config);
            }
        }
    }
 }

__s32 Mod_Display_MInit(void)
{

	__wrn("...Mod_Display_MInit");

	return 0;
}

__s32 Mod_Display_MExit(void)
{
	__wrn("...Mod_Display_MExit");
	return 0;
}

__mp *Mod_Display_MOpen(__u32 mid, __u32 mode)
{
	__wrn("....Mod_Display_MOpen  mp.used = %d",mod_disp_mp.used);
    #if 1
	if(mod_disp_mp.used)
	{
		mod_disp_mp.used++;
	    return (__mp *)&mod_disp_mp;
	}
	mod_disp_mp.used++;
	mod_disp_mp.mid = mid;
	disp_module_init();
	__wrn("....Mod_Display_MOpen   mod_disp_mp.mid = %x  (__mp *)&mod_disp_mp = %x",mod_disp_mp.mid,(__s32 *)&mod_disp_mp);
    #endif
    esKSRV_Save_Display_Hld(&mod_disp_mp);
    return (__mp *)&mod_disp_mp;
}

__s32 Mod_Display_MClose(__mp *mp)
{
	__wrn("....Mod_Display_MClose");
	//mod_disp_mp = NULL;
	if(mod_disp_mp.used == 0)
	{
		disp_module_deinit();
	}
	mod_disp_mp.used --;

    return EPDK_OK;
}

__u32 Mod_Display_MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
	__inf("....Mod_Display_MRead");

    return n;
}

__u32 Mod_Display_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
	__inf("....Mod_Display_MWrite");

    return n;
}



long Mod_Display_MIoctrl(__mp *mp, __u32 cmd,  __s32 aux, void *pbuffer)
{
    __u64 para[4] = {0};
	long ret;
    if (pbuffer != NULL)
    {
    	para[0] = aux;
        para[1] = *((__u64 *)pbuffer);                //argc0
        para[2] = *((__u64 *)((__u64)pbuffer + 8));   //argc1
        para[3] = *((__u64 *)((__u64)pbuffer + 16));   //argc2
    }

    __inf("cmd:%d  aux:%d  para:%d  %d  %d",cmd,aux,para[1],para[2],para[3]);
    switch (cmd)
    {
	    case MOD_DISP_SET_BKCOLOR:
        {
            disp_layer_config colorinfo;
            __inf("para[3] = %x",para[3]);
            memset(&colorinfo,0,sizeof(disp_layer_config));
            colorinfo.enable = 1;
            __inf("para[1] = %d  HD2CHN(para[1]) = %d",para[1],HD2CHN(para[1]));
            colorinfo.channel = HD2CHN(para[1]);
            colorinfo.layer_id = HD2LYL(para[1]);
    		colorinfo.info.zorder = 1;
    		colorinfo.info.mode = LAYER_MODE_COLOR;
    		colorinfo.info.fb.format = DISP_FORMAT_ARGB_8888;
    		colorinfo.info.screen_win.width = ioctl(disp_mgr.disp_fd, DISP_GET_SCN_WIDTH, para);
    		colorinfo.info.screen_win.height = ioctl(disp_mgr.disp_fd, DISP_GET_SCN_HEIGHT, para);
    		colorinfo.info.screen_win.x = 0;
    		colorinfo.info.screen_win.y = 0;
    		colorinfo.info.fb.crop.x =
    		    ((long long)(colorinfo.info.screen_win.x) << 32);
    		colorinfo.info.fb.crop.y =
    		    ((long long)(colorinfo.info.screen_win.y) << 32);
    		colorinfo.info.fb.crop.width =
    		    ((long long)(colorinfo.info.screen_win.width) << 32);
    		colorinfo.info.fb.crop.height =
    		    ((long long)(colorinfo.info.screen_win.height) << 32);
            __inf("ch[%d] id[%d] ",colorinfo.channel,colorinfo.layer_id);
            colorinfo.info.color = para[2];
            __log("color = 0x%x ",colorinfo.info.color);
            if(tv_enable)
            {
                lcd_scn[colorinfo.channel][colorinfo.layer_id].x = 0;
                lcd_scn[colorinfo.channel][colorinfo.layer_id].y = 0;
                lcd_scn[colorinfo.channel][colorinfo.layer_id].x = colorinfo.info.screen_win.width;
                lcd_scn[colorinfo.channel][colorinfo.layer_id].x = colorinfo.info.screen_win.height;
            }
            disp_layer_set_para(&colorinfo);
			break;
        }
		case MOD_DISP_GET_BKCOLOR:
		{
             disp_layer_config *colorinfo;
            colorinfo = &disp_mgr.layer[HD2CHN(para[1])][HD2LYL(para[1])].config;
            if(colorinfo->info.mode != LAYER_MODE_COLOR)
            {
                __err("layer not color mode");
                return 0;
            }
            ret = colorinfo->info.color;
			break;
		}
		case MOD_DISP_GET_OUTPUT_TYPE:
		{
		    ret = ioctl(disp_mgr.disp_fd, DISP_GET_OUTPUT_TYPE, para);
			break;
		}

        case MOD_DISP_GET_SCN_WIDTH:
        {
            if(tv_enable)
            {
                ret = disp_mgr.lcd_scn_phy >> 16;
            }
            else
            {
                ret = ioctl(disp_mgr.disp_fd, DISP_GET_SCN_WIDTH, para);
            }
			break;
        }

        case MOD_DISP_GET_SCN_HEIGHT:
        {
            if(tv_enable)
            {
                ret = disp_mgr.lcd_scn_phy & 0xffff;
            }
            else
            {
                ret = ioctl(disp_mgr.disp_fd, DISP_GET_SCN_HEIGHT, para);
            }
			break;
        }

		case MOD_DISP_VSYNC_EVENT_EN:
		{
		    ret = ioctl(disp_mgr.disp_fd, DISP_VSYNC_EVENT_EN, para);
			break;
		}

		case MOD_DISP_SHADOW_PROTECT:
		{
		    ret = ioctl(disp_mgr.disp_fd, DISP_SHADOW_PROTECT, para);
			break;
		}

		case MOD_DISP_BLANK:
		{
		    ret = ioctl(disp_mgr.disp_fd, DISP_BLANK, para);
			break;
		}


		case MOD_DISP_DEVICE_SWITCH:
		{
            __u32 now_type;
            __u32 goto_type;
            __s32 tv_mode;
            __u64 para_get[2];//no use

            goto_type = para[1];
            tv_mode = para[2];

            para_get[0] = 0;
            now_type = ioctl(disp_mgr.disp_fd, DISP_GET_OUTPUT_TYPE, para_get);

            __log("now_type:%d  goto_type = %d  tv_mode:%d",now_type,goto_type, tv_mode);

            if(now_type == goto_type && tv_mode_last == tv_mode)
                return 0;
            if(goto_type > MOD_DISP_OUTPUT_TYPE_TV)
                return 0;
            if(tv_mode > MOD_DISP_TV_MOD_PAL)
                return 0;

            if(goto_type == MOD_DISP_OUTPUT_TYPE_TV)
            {
                if(tv_mode == MOD_DISP_TV_MOD_PAL)
                {

                    auto_resolution_adapt(MOD_DISP_TV_MOD_PAL);
                    para[2] = DISP_TV_MOD_PAL;

                }
                else if(tv_mode == MOD_DISP_TV_MOD_NTSC)
                {
                    auto_resolution_adapt(MOD_DISP_TV_MOD_NTSC);                
                    para[2] = DISP_TV_MOD_NTSC;
                }
                tv_mode_last = tv_mode;
            }

            if(goto_type == MOD_DISP_OUTPUT_TYPE_LCD)
            {
                auto_resolution_adapt(MOD_DISP_TV_MOD_NULL);                
                para[2] = 0;
                tv_mode_last = MOD_DISP_TV_MOD_NULL;
            }

		    ret = ioctl(disp_mgr.disp_fd, DISP_DEVICE_SWITCH, para);		 

			break;
		}

        case MOD_DISP_LAYER_SET_CONFIG:
        {
			ret = disp_dev_layer_config(DISP_LAYER_SET_CONFIG,(void *)para);
			break;
        }

        case MOD_DISP_LAYER_GET_CONFIG:
        {
			ret = disp_dev_layer_config(DISP_LAYER_GET_CONFIG,(void *)para);
			break;
        }

        case MOD_DISP_LAYER_REQUEST:
        {
			ret = layer_request(para[1]);
			break;
        }

        case MOD_DISP_LAYER_RELEASE:
        {
			ret = disp_layer_release(para[1]);
			break;
        }

        case MOD_DISP_CMD_LAYER_SET_PARA:
        {
			disp_layer_config drvinfo;
			//__log("1 para[1] = %d",para[1]);
			memset(&drvinfo, 0, sizeof(drvinfo));
			mod2drv_layerinfo(&drvinfo,(__disp_layer_info_t *)para[2],para[1]);
			//echo_drvlayer_info(&drvinfo);
			//echo_modlayer_info((__disp_layer_info_t *)para[1]);
            if(tv_enable)
            {
                lcd_scn[drvinfo.channel][drvinfo.layer_id].x = drvinfo.info.screen_win.x;
                lcd_scn[drvinfo.channel][drvinfo.layer_id].y = drvinfo.info.screen_win.y;
                lcd_scn[drvinfo.channel][drvinfo.layer_id].w = drvinfo.info.screen_win.width;
                lcd_scn[drvinfo.channel][drvinfo.layer_id].h = drvinfo.info.screen_win.height;
            }
			ret = disp_layer_set_para(&drvinfo);

			break;
        }

        case MOD_DISP_CMD_LAYER_GET_PARA:
        {
			disp_layer_config drvinfo;

			memset(&drvinfo, 0, sizeof(drvinfo));

			drvinfo.channel = HD2CHN(para[1]);
			drvinfo.layer_id = HD2LYL(para[1]);

			ret = disp_layer_get_para(&drvinfo);
			//echo_drvlayer_info(&drvinfo);
			drv2mod_layerinfo(&drvinfo,(__disp_layer_info_t *)para[2]);
			//echo_modlayer_info((__disp_layer_info_t *)para[1]);
			break;
        }

        case MOD_DISP_CMD_LAYER_OPEN:
        {
			ret = disp_layer_open(para[1]);
			break;
        }

        case MOD_DISP_CMD_LAYER_CLOSE:
        {
			ret = disp_layer_close(para[1]);

			break;
        }

        case MOD_DISP_CMD_LAYER_SET_FB:
        {

			disp_fb_info drvfb;
			memset(&drvfb, 0, sizeof(drvfb));
			mod2drv_fb(&drvfb,(void *)para[2],para[1]);
			ret = disp_layer_set_fb(para[1],&drvfb);

			//echo_drvlayer_info(&(disp_mgr.layer[HD2CHN(para[1])][HD2LYL(para[1])].config.info));
			break;
        }

        case MOD_DISP_CMD_LAYER_GET_FB:
        {
			disp_fb_info drvfb;
			memset(&drvfb, 0, sizeof(drvfb));

			ret = disp_layer_get_fb(para[1],&drvfb);

			drv2mod_fb(&drvfb,(void *)para[2]);
			break;
        }

        case MOD_DISP_CMD_LAYER_SET_SCN_WINDOW:
        {
			disp_layer_config drvinfo;
		    __disp_rect_t  *scn_win;

			memset(&drvinfo, 0, sizeof(drvinfo));

			drvinfo.channel = HD2CHN(para[1]);
			drvinfo.layer_id = HD2LYL(para[1]);
			ret = disp_layer_get_para(&drvinfo);
			scn_win = (__disp_rect_t *)para[2];

			drvinfo.info.screen_win.x 			= scn_win->x;
			drvinfo.info.screen_win.y 			= scn_win->y;
			drvinfo.info.screen_win.width 		= scn_win->width;
			drvinfo.info.screen_win.height 		= scn_win->height;
			ret = disp_layer_set_para(&drvinfo);
			break;
        }

        case MOD_DISP_CMD_LAYER_GET_SCN_WINDOW:
        {
			disp_layer_config drvinfo;
		    __disp_rect_t  *scn_win;

			memset(&drvinfo, 0, sizeof(drvinfo));

			drvinfo.channel = HD2CHN(para[1]);
			drvinfo.layer_id = HD2LYL(para[1]);
			ret = disp_layer_get_para(&drvinfo);
			scn_win = (__disp_rect_t *)para[2];

			scn_win->x = drvinfo.info.screen_win.x;
			scn_win->y = drvinfo.info.screen_win.y;
			scn_win->width = drvinfo.info.screen_win.width;
			scn_win->height = drvinfo.info.screen_win.height;
			break;
        }

        case MOD_DISP_CMD_LAYER_SET_SRC_WINDOW:
        {
			disp_layer_config drvinfo;
		    __disp_rect_t  *src_win;

			memset(&drvinfo, 0, sizeof(drvinfo));

			drvinfo.channel = HD2CHN(para[1]);
			drvinfo.layer_id = HD2LYL(para[1]);
			ret = disp_layer_get_para(&drvinfo);
			src_win = (__disp_rect_t *)para[2];
			__inf("set %d  %d  %d  %d",src_win->x,src_win->y,src_win->width,src_win->height);
			drvinfo.info.fb.crop.x 			= src_win->x;
		    drvinfo.info.fb.crop.y 			= src_win->y;
			drvinfo.info.fb.crop.width  		= src_win->width;
			drvinfo.info.fb.crop.height 		= src_win->height;
			__inf("drv %d  %d  %d  %d",(__u32)drvinfo.info.fb.crop.x, \
				(__u32)drvinfo.info.fb.crop.y,(__u32)drvinfo.info.fb.crop.width,\
				(__u32)drvinfo.info.fb.crop.height);

			drvinfo.info.fb.crop.x 			= drvinfo.info.fb.crop.x<<32;
		    drvinfo.info.fb.crop.y 			= drvinfo.info.fb.crop.y<<32;
			drvinfo.info.fb.crop.width  		= drvinfo.info.fb.crop.width<<32;
			drvinfo.info.fb.crop.height 		= drvinfo.info.fb.crop.height<<32;

			ret = disp_layer_set_para(&drvinfo);
			break;
        }

        case MOD_DISP_CMD_LAYER_GET_SRC_WINDOW:
        {
			disp_layer_config drvinfo;
		    __disp_rect_t  *src_win;

			memset(&drvinfo, 0, sizeof(drvinfo));

			drvinfo.channel = HD2CHN(para[1]);
			drvinfo.layer_id = HD2LYL(para[1]);
			ret = disp_layer_get_para(&drvinfo);
			src_win = (__disp_rect_t *)para[2];

			src_win->x = drvinfo.info.fb.crop.x >>32;
			src_win->y = drvinfo.info.fb.crop.y >>32;
			src_win->width = drvinfo.info.fb.crop.width >>32;
			src_win->height = drvinfo.info.fb.crop.height >>32;
			break;
        }

        case MOD_DISP_CMD_LAYER_ALPHA_ON:
        {
			disp_layer_config drvinfo;

			memset(&drvinfo, 0, sizeof(drvinfo));

			drvinfo.channel = HD2CHN(para[1]);
			drvinfo.layer_id = HD2LYL(para[1]);
			ret = disp_layer_get_para(&drvinfo);
			drvinfo.enable = 1;
			ret = disp_layer_set_para(&drvinfo);
			break;
        }

        case MOD_DISP_CMD_LAYER_ALPHA_OFF:
        {
			disp_layer_config drvinfo;

			memset(&drvinfo, 0, sizeof(drvinfo));

			drvinfo.channel = HD2CHN(para[1]);
			drvinfo.layer_id = HD2LYL(para[1]);
			ret = disp_layer_get_para(&drvinfo);
			drvinfo.enable = 0;
			ret = disp_layer_set_para(&drvinfo);
			break;
        }

        case MOD_DISP_CMD_LAYER_SET_ALPHA_VALUE:
        {
			disp_layer_config drvinfo;

			memset(&drvinfo, 0, sizeof(drvinfo));

			drvinfo.channel = HD2CHN(para[1]);
			drvinfo.layer_id = HD2LYL(para[1]);
			disp_layer_get_para(&drvinfo);
			drvinfo.info.alpha_value  = para[2];
			__log("drvinfo.info.alpha_value = %x",drvinfo.info.alpha_value);
			disp_layer_set_para(&drvinfo);
			break;
        }

        case MOD_DISP_CMD_LAYER_GET_ALPHA_VALUE:
        {
			disp_layer_config drvinfo;

			memset(&drvinfo, 0, sizeof(drvinfo));

			drvinfo.channel = HD2CHN(para[1]);
			drvinfo.layer_id = HD2LYL(para[1]);
			disp_layer_get_para(&drvinfo);
			ret = drvinfo.info.alpha_value;
			__log("drvinfo.info.alpha_value = %x",drvinfo.info.alpha_value);
			break;
        }

        case MOD_DISP_CMD_VIDEO_SET_FB:
        {
			disp_fb_info drvfb;
			memset(&drvfb, 0, sizeof(drvfb));
			mod2drv_fb(&drvfb,(__disp_video_fb_t *)para[2],para[1]);
			ret = disp_layer_set_fb(para[1],&drvfb);
			break;
        }

        case MOD_DISP_CMD_VIDEO_GET_FB:
        {
			disp_fb_info drvfb;
			memset(&drvfb, 0, sizeof(drvfb));

			ret = disp_layer_get_fb(para[1],&drvfb);

			drv2mod_fb(&drvfb,(void *)para[2]);
			break;
        }

        case MOD_DISP_CMD_START_CMD_CACHE:
        {
			para[1] = 1;
		    ret = ioctl(disp_mgr.disp_fd, DISP_SHADOW_PROTECT, para);
			break;
        }

        case MOD_DISP_CMD_EXECUTE_CMD_AND_STOP_CACHE:
        {
			para[1] = 0;
		    ret = ioctl(disp_mgr.disp_fd, DISP_SHADOW_PROTECT, para);
			break;
        }

        case MOD_DISP_CMD_LCD_OFF:
        {
			break;
        }

        case MOD_DISP_CMD_LCD_ON:
        {
			break;
        }

		case MOD_DISP_LCD_BACKLIGHT_ENABLE:
		{
		    ret = ioctl(disp_mgr.disp_fd, DISP_LCD_BACKLIGHT_ENABLE, para);
			break;
		}

		case MOD_DISP_LCD_BACKLIGHT_DISABLE:
		{
		    ret = ioctl(disp_mgr.disp_fd, DISP_LCD_BACKLIGHT_DISABLE, para);
			break;
		}

		case MOD_DISP_LCD_SET_BRIGHTNESS:
		{
		    ret = ioctl(disp_mgr.disp_fd, DISP_LCD_SET_BRIGHTNESS, para);
			break;
		}

		case MOD_DISP_LCD_GET_BRIGHTNESS:
		{
		    ret = ioctl(disp_mgr.disp_fd, DISP_LCD_GET_BRIGHTNESS, para);
			break;
		}

		case MOD_DISP_TV_SET_GAMMA_TABLE:
		{
		    ret = ioctl(disp_mgr.disp_fd, DISP_TV_SET_GAMMA_TABLE, para);
			break;
		}

		case MOD_DISP_LCD_GAMMA_CORRECTION_ENABLE:
		{
		    ret = ioctl(disp_mgr.disp_fd, DISP_LCD_GAMMA_CORRECTION_ENABLE, para);
			break;
		}

		case MOD_DISP_LCD_GAMMA_CORRECTION_DISABLE:
		{
		    ret = ioctl(disp_mgr.disp_fd, DISP_LCD_GAMMA_CORRECTION_DISABLE, para);
			break;
		}


		case MOD_DISP_LCD_SET_GAMMA_TABLE:
		{
		    ret = ioctl(disp_mgr.disp_fd, DISP_LCD_SET_GAMMA_TABLE, para);
			break;
		}

			/* ---- hdmi --- */
		case MOD_DISP_HDMI_SUPPORT_MODE:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_HDMI_SUPPORT_MODE, para);
				break;
			}

		case MOD_DISP_SET_TV_HPD:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_SET_TV_HPD, para);
				break;
			}

			/* ----enhance---- */
		case MOD_DISP_ENHANCE_ENABLE:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_ENHANCE_ENABLE, para);
				break;
			}

		case MOD_DISP_ENHANCE_DISABLE:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_ENHANCE_DISABLE, para);
				break;
			}

		case MOD_DISP_ENHANCE_DEMO_ENABLE:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_ENHANCE_DEMO_ENABLE, para);
				break;
			}

		case MOD_DISP_ENHANCE_DEMO_DISABLE:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_ENHANCE_DEMO_DISABLE, para);
				break;
			}

		case MOD_DISP_ENHANCE_SET_MODE:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_ENHANCE_SET_MODE, para);
				break;
			}

		case MOD_DISP_ENHANCE_GET_MODE:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_ENHANCE_GET_MODE, para);
				break;
			}

		case MOD_DISP_ENHANCE_SET_BRIGHT:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_ENHANCE_SET_BRIGHT, para);
				break;
			}

		case MOD_DISP_ENHANCE_GET_BRIGHT:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_ENHANCE_GET_BRIGHT, para);
				break;
			}

		case MOD_DISP_ENHANCE_SET_CONTRAST:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_ENHANCE_SET_CONTRAST, para);
				break;
			}

		case MOD_DISP_ENHANCE_GET_CONTRAST:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_ENHANCE_GET_CONTRAST, para);
				break;
			}

		case MOD_DISP_ENHANCE_SET_SATURATION:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_ENHANCE_SET_SATURATION, para);
				break;
			}

		case MOD_DISP_ENHANCE_GET_SATURATION:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_ENHANCE_GET_SATURATION, para);
				break;
			}

			/* ---smart backlight -- */
		case MOD_DISP_SMBL_ENABLE:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_SMBL_ENABLE, para);
				break;
			}

		case MOD_DISP_SMBL_DISABLE:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_SMBL_DISABLE, para);
				break;
			}

		case MOD_DISP_SMBL_SET_WINDOW:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_SMBL_SET_WINDOW, para);
				break;
			}

			/* ---capture -- */
		case MOD_DISP_CAPTURE_START:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_CAPTURE_START, para);
				break;
			}

		case MOD_DISP_CAPTURE_STOP:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_CAPTURE_STOP, para);
				break;
			}

		case MOD_DISP_CAPTURE_COMMIT:
			{
			    ret = ioctl(disp_mgr.disp_fd, DISP_CAPTURE_COMMIT, para);
				break;
			}

		case MOD_DISP_LCD_CHECK_OPEN_FINISH:
			{
				ret = ioctl(disp_mgr.disp_fd, DISP_LCD_CHECK_OPEN_FINISH, para);
				break;
			}

        case 0xff:
        {
			__log("mod_display test case!!");
            ret = ioctl(disp_mgr.disp_fd, 0xffff, para);

			break;
        }

        default:
        __err("no this CMD in display mod");
    }
	return ret;
}




