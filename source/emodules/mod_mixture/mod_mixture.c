/*
 * =====================================================================================
 *
 *       Filename:  mod_Mixture.c
 *
 *       Description:  support the connection to drv and upper application layer
 *
 *       Version:  1.0
 *       Create:  2020-08-07 15:46:44
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *       Author:  liujiaming@allwinnertech.com
 *       Organization:  PDC-PD1
 *       Last Modified:  2020-08-07 15:46:44
 *
 * =====================================================================================
 */
#include "mod_mixture_i.h"
#include <mod_mixture.h>
#include <mod_display.h>
#include "mod_codec_cmd.h"
#include "bmp_parser/Parse_Picture.h"
#include "bmp_parser/bmp.h"
#include <kapi.h>
#include <rtthread.h>
#include "startup_check/startup_check.h"
//#include <mod_video_play.h>
__mixture_mp_t mod_mixture_mp;

typedef enum
{
	STATIC_LOGO = 0,
	CHANGE_LOGO,
	ANIMATION
} Logo_type;

typedef enum
{
	JPG = 0,
	BMP
}Picture_type;

typedef struct
{
    __u32 width;
    __u32 height;
} JPG_SIZE;
/*
startup_logo_en         
startup_logo_type        
startup_picture_type        
startup_logo_enlarge        
startup_ani_total_frame      
startup_ani_inter_time       
*/
typedef struct
{
    bool            logo_en;
    Logo_type       logo_type;
    Picture_type    picture_type;
    __u32           logo_enlarge;
    __u32           logo_index;
    __u32           ani_total_frame;
    __u32           ani_cur_frame;    
    __u32           ani_inter_time;
    char            *logo_path;
    unsigned long   logo_addr;
    __u32           logo_size;
	Picture_t       *bmp_info;
	stJpegFrameInfo *jpeg_info;
}logo_info;

typedef struct
{
    __mp* mp_display;
	__u32 b_stop;
	__u32 boot_logo_en;
	__hdle logo_layer;	
	__u32 jpeg_yuv_size;
	__u32 t_animation_task;
	__u32 b_animation;
	logo_info logoinfo[TYPE_MAX];
	//logo_info shutdown;
	__u32 carback_en;
	__u32 t_carback_task;
	__u32 b_carback;
	__krnl_event_t *cvbs_sem;
	__krnl_event_t *anim_sem;
	
} mod_logo_data_t;

#define ALIGN_TO_16B(x)   ((((x) + (1 <<  4) - 1) >>  4) <<  4)
#define ALIGN_TO_32B(x)   ((((x) + (1 <<  5) - 1) >>  5) <<  5)

#define LOGO_PATH	"d:\\res\\"
mod_logo_data_t *plogo_data = NULL;
static __mp *mod_video_play = NULL;
static __u8  mid_video_play = 0;

static Picture_t *picture_nosi = NULL;
static __s32  nosi_layer = 0;

__s32 read_picture_index(void)
{
	char str;
	__u32 read_len;
	__s32 value = -1;
	ES_FILE *fp = NULL;
	
	fp = eLIBs_fopen(LOGO_PATH"index.bin", "rb");
    if (fp == NULL)
    {
        __log("open file err\n");
        goto err;
    }

	read_len = eLIBs_fread(&str, 1, 1, fp);
	if (read_len != 1)
    {
        __log("fread fail...\n");
        goto err;
    }
	value = eLIBs_atoi(&str);
	
	err:
    if (fp)
    {
        eLIBs_fclose(fp);
    }
    return value;
}

__s32 LoadDataFromFile(const char *path, char **data, __u32 *size)
{
    ES_FILE *fp = NULL;
    char *tmpBuf = NULL;
    __u32 fileLen;
    __u32 realLen;

    fp = eLIBs_fopen(path, "rb");
    if (fp == NULL)
    {
        __log("open file err\n");
        goto err;
    }

    eLIBs_fllseek(fp, 0, SEEK_END);
    fileLen = eLIBs_flltell(fp);
    __wrn("fileLen=%d\n", fileLen);

    tmpBuf = eLIBs_malloc(fileLen);
    if (tmpBuf == NULL)
    {
        __log("malloc err\n");
        goto err;
    }

    eLIBs_fllseek(fp, 0, SEEK_SET);
    realLen = eLIBs_fread(tmpBuf, 1, fileLen, fp);
    if (realLen != fileLen)
    {
        __log("fread fail...\n");
        goto err;
    }
	*data = tmpBuf;
	*size = realLen;
    if (fp)
    {
        eLIBs_fclose(fp);
    }    
	return realLen;
	
	err:
	if(tmpBuf)
	{
		eLIBs_free(tmpBuf);
	}
	
    if (fp)
    {
        eLIBs_fclose(fp);
    }
    return -1;
}

static __u32 get_size_jpg(JPG_SIZE *size, uint8_t *pic_buf)
{
    char *mark;
    char temp1, temp2;
    __u32 offset;

    mark = pic_buf;

    if (!(mark[0] == 0xFF && mark[1] == 0xD8))
    {
        __log("get jpg size fail!, (%x, %x)", mark[0], mark[1]);
        mark = NULL;
        return -1;
    }

    mark += 2;
    temp1 = mark[0];
    mark += 2;
    while (temp1 == 0xFF)
    {
        offset = ((mark[1]) | (mark[0] << 8));
        mark += 2;
        //__log(" offset = %x", offset);
        mark += (offset - 2);
        temp1 = mark[0];
        temp2 = mark[1];
        mark += 2;
        if (temp2 == 0xC0 || temp2 == 0xC2)
        {
            __u8 *temp;
            mark += 3;
            temp = mark;
            size->height = ((temp[1]) | (temp[0] << 8));
            mark += 2;
            temp = mark;
            size->width = ((temp[1]) | (temp[0] << 8));
            __wrn("jpg picture width = %d, height = %d  ", size->width, size->height);
            temp += 2;
            break;
        }

    }
    return 0;

}

int ReadDataFromFile(unsigned char *buf, unsigned char *path, int size)
{
	int ret;
	ES_FILE *fp = NULL;

	fp = eLIBs_fopen(path, "rb");
	if(fp == 0)
    {
        __log("open failed\n");
        return -1;
    }

	ret = eLIBs_fread(buf, 1, size, fp);
	__wrn("read ret=%d, size=%d\n", ret, size);
	
    eLIBs_fclose(fp);
	return ret;
}
#if 0
static __s32 show_jpg_logo(void)
{
    JPG_SIZE *size;
    __u32 file_size, tmpsize;
    __u32 arg[3] = {0};
    __u32 yuv_size = 0;
    __s32 ret = -1;
    __u32 vcoder_mod_id = 0;
    __mp *vcoder = NULL;
    __u32 logo_enlarge;
    __u32 scn_w = 0, scn_h = 0, disp_w = 0, disp_h = 0;
    __u32 *pic_width, *pic_height;
    __disp_layer_info_t logo_layer_para;
    char *mark;
    char temp1, temp2;
    __u16 offset;
	int src_width;
	int src_height;
	
    eLIBs_memset(&logo_layer_para, 0x00, sizeof(logo_layer_para));
    plogo_data->jpeg_info = (stJpegFrameInfo *)esMEMS_Malloc(0, sizeof(stJpegFrameInfo));
    size = (JPG_SIZE *)esMEMS_Malloc(0, sizeof(JPG_SIZE));

    arg[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
    arg[1] = 0;
    arg[2] = 0;
    plogo_data->logo_layer = esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_LAYER_REQUEST, 0, (void *)arg);
    if (plogo_data->logo_layer == 0)
    {
        __log("request logo_layer fail");
		return -1;
    }

    disp_w = esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_GET_SCN_WIDTH, 0, 0);
    disp_h = esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_GET_SCN_HEIGHT, 0, 0);

    vcoder_mod_id = esMODS_MInstall("c:\\mod\\vcoder.mod", 0);
    if (!vcoder_mod_id)
    {
        __log("install vcoder failed...\n");
		return -1;
    }
    if (!vcoder)
    {
        vcoder = esMODS_MOpen(vcoder_mod_id, 0);
        if (!vcoder)
        {
            __log("open mod ailed!\n");
            return -1;
        }
    }

    if (plogo_data->picture_addr == 0 || plogo_data->picture_size == 0)
    {
    	__log("no picture\n");
        return -1;
    }
    plogo_data->jpeg_info->jpegData = plogo_data->picture_addr;
    plogo_data->jpeg_info->jpegData_len = plogo_data->picture_size;

    if (get_size_jpg(size, plogo_data->picture_addr) == EPDK_FAIL)
    {
        __log("get the jpg size fail!");
		return -1;
    }

    plogo_data->jpeg_yuv_size = (ALIGN_TO_16B(size->width)) * (ALIGN_TO_16B(size->height)) * 3 / 2;
    plogo_data->jpeg_info->yuv_buf = (__u32)esMEMS_Palloc(((plogo_data->jpeg_yuv_size + 1023) >> 10), 0);
    if (plogo_data->jpeg_info->yuv_buf == NULL)
    {
        __log("jpeg_info->yuv_buf malloc fail,size:%d\n", plogo_data->jpeg_yuv_size);
		return -1;
    }
    esMEMS_CleanFlushDCacheRegion((void *)plogo_data->jpeg_info->yuv_buf, plogo_data->jpeg_yuv_size);
    ret = esMODS_MIoctrl(vcoder, MPEJ_CODEC_CMD_DECODER, 0, plogo_data->jpeg_info);
    esMODS_MClose(vcoder);
    esMODS_MUninstall(vcoder_mod_id);

	logo_enlarge = plogo_data->logo_enlarge;
    if (logo_enlarge == 1)
    {
        scn_w = disp_w;
        scn_h = disp_h;
    }
    else if (logo_enlarge == 2)
    {
        scn_w = size->width * 2;
        scn_h = size->height * 2;
    }
    else
    {
        scn_w = size->width;
        scn_h = size->height;
    }
	
	if(scn_w > disp_w)
	{
		scn_w = disp_w;
	}
	if(scn_h > disp_h)
	{
		scn_h = disp_h;
	}

    logo_layer_para.fb.format        = DISP_FORMAT_YUV420_P;//DISP_FORMAT_YUV420_SP_UVUV;
    logo_layer_para.fb.seq           = DISP_SEQ_UVUV;
    logo_layer_para.fb.mode          = DISP_MOD_MB_PLANAR;
    logo_layer_para.fb.br_swap       = 0;
    logo_layer_para.fb.cs_mode      = DISP_BT601;
    logo_layer_para.mode = MOD_DISP_LAYER_WORK_MODE_SCALER;
    logo_layer_para.pipe             = 0;
    logo_layer_para.prio             = 0xFF;
    logo_layer_para.alpha_en         = 0;
    logo_layer_para.alpha_val        = 0;
    logo_layer_para.ck_enable        = 0;
    logo_layer_para.src_win.x        = 0;
    logo_layer_para.src_win.y        = 0;
    logo_layer_para.src_win.width    = size->width;
    logo_layer_para.src_win.height   = size->height;
    logo_layer_para.scn_win.x        = (disp_w - scn_w) >> 1;
    logo_layer_para.scn_win.y        = (disp_h - scn_h) >> 1;
    logo_layer_para.scn_win.width    = scn_w;
    logo_layer_para.scn_win.height   = scn_h;
    logo_layer_para.fb.lbc_mode      = 0;
    logo_layer_para.fb.addr[0]       = (__u32)plogo_data->jpeg_info->y_buf;
    logo_layer_para.fb.addr[1]       = (__u32)plogo_data->jpeg_info->v_buf;
    logo_layer_para.fb.addr[2]       = (__u32)plogo_data->jpeg_info->u_buf;
    logo_layer_para.fb.size.width = plogo_data->jpeg_info->pic_width;
    logo_layer_para.fb.size.height = plogo_data->jpeg_info->pic_height;
	
    arg[0] = plogo_data->logo_layer;
    arg[1] = (__u32)&logo_layer_para;
    arg[2] = 0;
    esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_CMD_LAYER_SET_PARA, 0, (void *)arg);
    //esMODS_MIoctrl(mp_display, DISP_CMD_LAYER_OPEN, 0, (void *)arg);

    esMEMS_Mfree(0, size);
    return 0;

}

__s32 close_jpg_logo(void)
{
	__u32 arg[3] = {0};

	// not close this layer, and background receive this layer
	if(plogo_data->b_stop != 1)
	{
		arg[0] = plogo_data->logo_layer;
	    arg[1] = 0;
	    arg[2] = 0;
	    esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_CMD_LAYER_CLOSE, 0, (void *)arg);
		esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_LAYER_RELEASE, 0, (void *)arg);
	}
	
	if (plogo_data->jpeg_info)
    {
        if (plogo_data->jpeg_info->yuv_buf)
        {
            esMEMS_Pfree(plogo_data->jpeg_info->yuv_buf, ((plogo_data->jpeg_yuv_size + 1023) >> 10));
            plogo_data->jpeg_info->yuv_buf = NULL;
        }
        esMEMS_Mfree(0, plogo_data->jpeg_info);
        plogo_data->jpeg_info = NULL;
    }
	return 0;
}
#endif
static __s32 show_bmp_logo(logo_execute i)
{
	__u32 value;
    __u64 arg[3];
    __s32 ret;
	__u32 logo_enlarge;
	__u32 scn_w = 0, scn_h = 0;
	 __s32 width, height;
    __disp_layer_info_t layer_para;
    //Picture_t *bmp_info = NULL;
	char *tmp_buf = NULL;
	__u32 tmp_size = 0;

	eLIBs_memset(&layer_para, 0, sizeof(__disp_layer_info_t));
    plogo_data->logoinfo[i].bmp_info = (Picture_t *)eLIBs_malloc(sizeof(Picture_t));
    if (plogo_data->logoinfo[i].bmp_info == NULL)
    {
        __log("malloc logo bmp buffer failed!");
        return -1;
    }
    eLIBs_memset(plogo_data->logoinfo[i].bmp_info, 0, sizeof(Picture_t));

	tmp_size = plogo_data->logoinfo[i].logo_size;
	tmp_buf = eLIBs_malloc(tmp_size);
	if (tmp_buf == NULL)
	{
		__log("malloc tmp_buf buffer failed!");
        return -1;
	}
	eLIBs_memcpy(tmp_buf, (void *)plogo_data->logoinfo[i].logo_addr, plogo_data->logoinfo[i].logo_size);
	
	// note: this function has been aready relesed frist param memory
	ret = Parse_Pic_BMP_ByRam(tmp_buf, tmp_size, plogo_data->logoinfo[i].bmp_info, NULL);
    if (-1 == ret)
    {
    	__log("Parse BMP pic failed\n");
        return -1;
    }
	tmp_buf = NULL;
	tmp_size = 0;

    width  = esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_GET_SCN_WIDTH, 0, 0);
    height = esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_GET_SCN_HEIGHT, 0, 0);

    arg[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
    arg[1] = 0;
    arg[2] = 0;
    plogo_data->logo_layer = (__hdle)esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_LAYER_REQUEST, 0, (void *)arg);

	logo_enlarge = plogo_data->logoinfo[i].logo_enlarge;
	if (logo_enlarge == 1)
    {
        scn_w = width;
        scn_h = height;
    }
    else if (logo_enlarge == 2)
    {
        scn_w = plogo_data->logoinfo[i].bmp_info->Width * 2;
        scn_h = plogo_data->logoinfo[i].bmp_info->Height * 2;
    }
    else
    {
        scn_w = plogo_data->logoinfo[i].bmp_info->Width;
        scn_h = plogo_data->logoinfo[i].bmp_info->Height;
    }
	
	if(scn_w > width)
	{
		scn_w = width;
	}
	if(scn_h > height)
	{
		scn_h = height;
	}
	esMEMS_CleanFlushDCacheRegion((void *)plogo_data->logoinfo[i].bmp_info->Buffer, plogo_data->logoinfo[i].bmp_info->BufferSize);

    layer_para.mode = MOD_DISP_LAYER_WORK_MODE_SCALER;
    layer_para.fb.lbc_mode      = 0;    
    layer_para.fb.addr[0]       = (__u64)plogo_data->logoinfo[i].bmp_info->Buffer;
    layer_para.fb.size.width    = plogo_data->logoinfo[i].bmp_info->Width;
    layer_para.fb.size.height   = plogo_data->logoinfo[i].bmp_info->Height;
    layer_para.fb.format        = DISP_FORMAT_ARGB_8888;
	layer_para.fb.seq           = DISP_SEQ_ARGB;
	layer_para.fb.mode          = DISP_MOD_INTERLEAVED;
	layer_para.fb.cs_mode       = DISP_BT601;
    layer_para.fb.br_swap       = 0;
    layer_para.ck_enable        = 0;
    layer_para.alpha_en         = 0;
    layer_para.alpha_val        = 0xff;
    layer_para.pipe             = 0;
    layer_para.src_win.x        = 0;
    layer_para.src_win.y        = 0;
    layer_para.src_win.width    = plogo_data->logoinfo[i].bmp_info->Width;
    layer_para.src_win.height   = plogo_data->logoinfo[i].bmp_info->Height;
    layer_para.scn_win.x        = (width - scn_w) >> 1;
    layer_para.scn_win.y        = (height - scn_h) >> 1;
    layer_para.scn_win.width    = scn_w;
    layer_para.scn_win.height   = scn_h;
	
    arg[0] = (__u64)plogo_data->logo_layer;
    arg[1] = (__u64)&layer_para;
    arg[2] = 0;
    esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_CMD_LAYER_SET_PARA, 0, (void *)arg);
    return 0;
}


__s32 close_bmp_logo(logo_execute i)
{
	__u64 arg[3] = {0};

	// not close this layer, and background receive this layer
	if(plogo_data->b_stop != 1)
	{
		arg[0] = (__u64)plogo_data->logo_layer;
	    arg[1] = 0;
	    arg[2] = 0;
	    esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_CMD_LAYER_CLOSE, 0, (void *)arg);
		esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_LAYER_RELEASE, 0, (void *)arg);
        
	}
	if (plogo_data->logoinfo[i].bmp_info)
    {
        if (plogo_data->logoinfo[i].bmp_info->Buffer)
        {
        	esMEMS_Pfree(plogo_data->logoinfo[i].bmp_info->Buffer, ((plogo_data->logoinfo[i].bmp_info->BufferSize + 1023) >> 10));
			plogo_data->logoinfo[i].bmp_info->Buffer = NULL;
        }
        eLIBs_free(plogo_data->logoinfo[i].bmp_info);
        plogo_data->logoinfo[i].bmp_info = NULL;
    }
    
    if (plogo_data->logoinfo[i].logo_addr)
    {
        eLIBs_free((void *)plogo_data->logoinfo[i].logo_addr);
        plogo_data->logoinfo[i].logo_addr = 0;
        plogo_data->logoinfo[i].logo_size = 0;
    }
    
	return 0;
}

__s32 show_picture(logo_execute i)
{
	if (plogo_data->logoinfo[i].picture_type == JPG)
	{
    	__err("not support jpg now");
		//show_jpg_logo(i);
	}
	else if(plogo_data->logoinfo[i].picture_type == BMP)
	{

		show_bmp_logo(i);
	}
    else
    {
        __err("no this logo");
    }
	return 0;
}

__s32 stop_picture(logo_execute i)
{
	if (plogo_data->logoinfo[i].picture_type == JPG)
	{
		//close_jpg_logo();
	}
	else if(plogo_data->logoinfo[i].picture_type == BMP)
	{
		close_bmp_logo(i);
	}
    else
    {
        __err("no this logo");
    }
	return 0;
}
static void open_lcd_backlight(void)
{
    __u64 arg[3];
    __u32 backlight = 0;
    ES_FILE * fp = NULL;    
	fp = eLIBs_fopen("e:\\mixture.bin", "rb+");
	if(fp)
	{
		eLIBs_fread((void *)&backlight, 1,4, fp);
		eLIBs_fclose(fp);
	}
	if(backlight > 255 || backlight == 0)
	{
		__err("get backlight err,default to 100");
		backlight = 100;
	}
    __inf("mixture backlight = %d",backlight);
    arg[0] = backlight;
    arg[1] = 0;
    arg[2] = 0;
    esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_LCD_SET_BRIGHTNESS, 0, (void *)arg);
    return;
}

static void close_lcd_backlight(void)
{
    __u64 arg[3];
    arg[0] = DISP_OUTPUT_TYPE_NONE;
    arg[1] = 0;
    arg[2] = 0;
    esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_DEVICE_SWITCH, 0, (void *)arg);
    return;
}

#if 0
static void __jpg_animation(mod_logo_data_t *plogo_data)
{
    JPG_SIZE *size = NULL;
    __u32 file_size, tmpsize;
    __u32 arg[3] = {0};
    __u32 yuv_size = 0;
    __s32 ret = -1;
    __u32 vcoder_mod_id = 0;
    __mp *vcoder = NULL;
    __u32 logo_enlarge;
    __u32 scn_w = 0, scn_h = 0, disp_w = 0, disp_h = 0;
    __u32 *pic_width, *pic_height;
    __disp_layer_info_t logo_layer_para;
    char *mark;
    char temp1, temp2;
    __u16 offset;
	char path[64];
	__u8 err;

	plogo_data->b_animation = 1; 
	eLIBs_memset(&logo_layer_para, 0x00, sizeof(logo_layer_para));
    plogo_data->jpeg_info = (stJpegFrameInfo *)esMEMS_Malloc(0, sizeof(stJpegFrameInfo));
    size = (JPG_SIZE *)esMEMS_Malloc(0, sizeof(JPG_SIZE));

	arg[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
    arg[1] = 0;
    arg[2] = 0;
    plogo_data->logo_layer = esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_LAYER_REQUEST, 0, (void *)arg);
    if (plogo_data->logo_layer == 0)
    {
        __log("request logo_layer fail");
		goto EXIT_TASK;
    }

    disp_w = esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_GET_SCN_WIDTH, 0, 0);
    disp_h = esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_GET_SCN_HEIGHT, 0, 0);

    vcoder_mod_id = esMODS_MInstall("c:\\mod\\vcoder.mod", 0);
    if (!vcoder_mod_id)
    {
        __log("install vcoder failed...\n");
		goto EXIT_TASK;
    }
    if (!vcoder)
    {
        vcoder = esMODS_MOpen(vcoder_mod_id, 0);
        if (!vcoder)
        {
            __log("open mod ailed!\n");
            goto EXIT_TASK;
        }
    }

	eLIBs_sprintf(path, LOGO_PATH"logo%d.bin", plogo_data->cur_frame);
	LoadDataFromFile(path, &plogo_data->frame_addr, &plogo_data->frame_size);
    plogo_data->jpeg_info->jpegData = plogo_data->frame_addr;
	plogo_data->jpeg_info->jpegData_len = plogo_data->frame_size;

	if (get_size_jpg(size, plogo_data->frame_addr) == EPDK_FAIL)
	{
		__log("get the jpg size fail!");
		goto EXIT_TASK;
	}

	logo_enlarge = plogo_data->logo_enlarge;
	if (logo_enlarge == 1)
    {
        scn_w = disp_w;
        scn_h = disp_h;
    }
    else if (logo_enlarge == 2)
    {
        scn_w = size->width * 2;
        scn_h = size->height * 2;
    }
    else
    {
        scn_w = size->width;
        scn_h = size->height;
    }
	
	if(scn_w > disp_w)
	{
		scn_w = disp_w;
	}
	if(scn_h > disp_h)
	{
		scn_h = disp_h;
	}

	plogo_data->jpeg_yuv_size = (ALIGN_TO_16B(size->width)) * (ALIGN_TO_16B(size->height)) * 3 / 2;
	plogo_data->jpeg_info->yuv_buf = (__u32)esMEMS_Palloc(((plogo_data->jpeg_yuv_size + 1023) >> 10), 0);
	if (plogo_data->jpeg_info->yuv_buf == NULL)
	{
		__log("jpeg_info->yuv_buf malloc fail,size:%d\n", plogo_data->jpeg_yuv_size);
		goto EXIT_TASK;
	}
	esMEMS_CleanFlushDCacheRegion((void *)plogo_data->jpeg_info->yuv_buf, plogo_data->jpeg_yuv_size);
	while (1)
    {
	    ret = esMODS_MIoctrl(vcoder, MPEJ_CODEC_CMD_DECODER, 0, plogo_data->jpeg_info);
	    logo_layer_para.fb.format        = DISP_FORMAT_YUV420_P;
	    logo_layer_para.fb.seq           = DISP_SEQ_UVUV;
	    logo_layer_para.fb.mode          = DISP_MOD_NON_MB_UV_COMBINED;
	    logo_layer_para.fb.br_swap       = 0;
	    logo_layer_para.fb.cs_mode      = DISP_BT601;
	    logo_layer_para.mode = MOD_DISP_LAYER_WORK_MODE_SCALER;
	    logo_layer_para.pipe             = 0;
	    logo_layer_para.prio             = 0xFF;
	    logo_layer_para.alpha_en         = 0;
	    logo_layer_para.alpha_val        = 0;
	    logo_layer_para.ck_enable        = 0;
	    logo_layer_para.src_win.x        = 0;
	    logo_layer_para.src_win.y        = 0;
	    logo_layer_para.src_win.width    = size->width;
	    logo_layer_para.src_win.height   = size->height;
	    logo_layer_para.scn_win.x        = (disp_w - scn_w) >> 1;
	    logo_layer_para.scn_win.y        = (disp_h - scn_h) >> 1;
	    logo_layer_para.scn_win.width    = scn_w;
	    logo_layer_para.scn_win.height   = scn_h;
        logo_layer_para.fb.lbc_mode      = 0;            
	    logo_layer_para.fb.addr[0]       = (__u32)plogo_data->jpeg_info->y_buf;
	    logo_layer_para.fb.addr[1]       = (__u32)plogo_data->jpeg_info->v_buf;
	    logo_layer_para.fb.addr[2]       = (__u32)plogo_data->jpeg_info->u_buf;
	    logo_layer_para.fb.size.width = plogo_data->jpeg_info->pic_width;
	    logo_layer_para.fb.size.height = plogo_data->jpeg_info->pic_height;
	    arg[0] = plogo_data->logo_layer;
	    arg[1] = (__u32)&logo_layer_para;
	    arg[2] = 0;
	    esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_CMD_LAYER_SET_PARA, 0, (void *)arg);
	   // esMODS_MIoctrl(mp_display, DISP_CMD_LAYER_OPEN, 0, (void *)arg);
		if (plogo_data->inter_time)
		{
			esKRNL_TimeDly(plogo_data->inter_time);
		}

		if (plogo_data->carback_en && plogo_data->b_carback)
		{
			arg[0] = plogo_data->logo_layer;
			arg[1] = 0;
			arg[2] = 0;
			esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_CMD_LAYER_CLOSE, 0, (void *)arg);
			esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_LAYER_RELEASE, 0, (void *)arg);

			esKRNL_SemPost(plogo_data->cvbs_sem);
			esKRNL_SemPend(plogo_data->anim_sem, 0, &err);
		    if (OS_NO_ERR != err)
		    {
		        __log("anim_sem pend fail, err=%d", err);
		    }

			arg[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
		    arg[1] = 0;
		    arg[2] = 0;
		    plogo_data->logo_layer = esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_LAYER_REQUEST, 0, (void *)arg);
		    if (plogo_data->logo_layer == 0)
		    {
		        __log("request logo_layer fail");
				goto EXIT_TASK;
		    }
		}
		
		plogo_data->cur_frame++;
		__log("cur_frame=%d, total_frame=%d\n", plogo_data->cur_frame, plogo_data->total_frame);
		if (plogo_data->cur_frame > plogo_data->total_frame)
		{
			break;
		}

		if(plogo_data->frame_addr)
		{
			eLIBs_free(plogo_data->frame_addr);
			plogo_data->frame_addr = NULL;
			plogo_data->frame_size = 0;
		}

		eLIBs_sprintf(path, LOGO_PATH"logo%d.bin", plogo_data->cur_frame);
		LoadDataFromFile(path, &plogo_data->frame_addr, &plogo_data->frame_size);
	    plogo_data->jpeg_info->jpegData = plogo_data->frame_addr;
		plogo_data->jpeg_info->jpegData_len = plogo_data->frame_size;

    }

	EXIT_TASK:
	plogo_data->b_animation = 0; 
	if(plogo_data->frame_addr)
	{
		eLIBs_free(plogo_data->frame_addr);
		plogo_data->frame_addr = NULL;
		plogo_data->frame_size = 0;
	}
	if(vcoder)
	{
		esMODS_MClose(vcoder);
		vcoder = NULL;
	}
	if (vcoder_mod_id)
	{
		esMODS_MUninstall(vcoder_mod_id);
		vcoder_mod_id = 0;
	}
	if(size)
	{
		esMEMS_Mfree(0, size);
		size = NULL;
	}
    
    esKRNL_TDel(EXEC_prioself);
}
#endif

static void __bmp_animation(logo_execute i)
{
	__u32 value;
    __u64 arg[3];
    __s32 ret;
	__u8 err;
	char path[64];
	__u32 logo_enlarge;
	__u32 scn_w = 0, scn_h = 0;
	 __s32       width, height;
    __disp_layer_info_t layer_para;
	plogo_data->b_animation = 1; 
	eLIBs_memset(&layer_para, 0, sizeof(__disp_layer_info_t));
    plogo_data->logoinfo[i].bmp_info = (Picture_t *)eLIBs_malloc(sizeof(Picture_t));
    if(plogo_data->logoinfo[i].bmp_info== NULL)
    {
        __log("malloc logo bmp buffer failed!");
        goto EXIT_TASK;
    }
    eLIBs_memset(plogo_data->logoinfo[i].bmp_info, 0, sizeof(Picture_t));

    width  = esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_GET_SCN_WIDTH, 0, 0);
    height = esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_GET_SCN_HEIGHT, 0, 0);

    arg[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
    arg[1] = 0;
    arg[2] = 0;
    plogo_data->logo_layer = (__hdle)esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_LAYER_REQUEST, 0, (void *)arg);
	if(i == STARTUP)
    {
    	eLIBs_sprintf(path, LOGO_PATH"stologo%d.bin", plogo_data->logoinfo[i].ani_cur_frame);
    }
    else if(i == SHUTDOWN)
    {
    	eLIBs_sprintf(path, LOGO_PATH"stalogo%d.bin", plogo_data->logoinfo[i].ani_cur_frame);
    }
    else
    {
        __err("no this logo");
    }
	LoadDataFromFile(path, (char **)&plogo_data->logoinfo[i].logo_addr, &plogo_data->logoinfo[i].logo_size);
	Parse_Pic_BMP_ByRam((void *)plogo_data->logoinfo[i].logo_addr, plogo_data->logoinfo[i].logo_size, plogo_data->logoinfo[i].bmp_info, NULL);
	plogo_data->logoinfo[i].logo_addr = 0;
	//plogo_data->frame_size = 0;

	logo_enlarge = plogo_data->logoinfo[i].logo_enlarge;
	if (logo_enlarge == 1)
    {
        scn_w = width;
        scn_h = height;
    }
    else if (logo_enlarge == 2)
    {
        scn_w = plogo_data->logoinfo[i].bmp_info->Width * 2;
        scn_h = plogo_data->logoinfo[i].bmp_info->Height * 2;
    }
    else
    {
        scn_w = plogo_data->logoinfo[i].bmp_info->Width;
        scn_h = plogo_data->logoinfo[i].bmp_info->Height;
    }
	
	if(scn_w > width)
	{
		scn_w = width;
	}
	if(scn_h > height)
	{
		scn_h = height;
	}

	while(1)
	{
	    //esMEMS_CleanFlushDCacheRegion((void *)plogo_data->picture->Buffer, plogo_data->frame_size);        
	    layer_para.mode = MOD_DISP_LAYER_WORK_MODE_SCALER;
        layer_para.fb.lbc_mode      = 0;                    
	    layer_para.fb.addr[0]       = (__u64)plogo_data->logoinfo[i].bmp_info->Buffer;
	    layer_para.fb.addr[1]       = 0;
	    layer_para.fb.addr[2]       = 0;
	    layer_para.fb.size.width    = plogo_data->logoinfo[i].bmp_info->Width;
	    layer_para.fb.size.height   = plogo_data->logoinfo[i].bmp_info->Height;
	    layer_para.fb.format        = DISP_FORMAT_ARGB_8888;
		layer_para.fb.seq           = DISP_SEQ_ARGB;
		layer_para.fb.mode          = DISP_MOD_INTERLEAVED;
		layer_para.fb.cs_mode       = DISP_BT601;
	    layer_para.fb.br_swap       = 0;
	    layer_para.ck_enable        = 0;
	    layer_para.alpha_en         = 0;
	    layer_para.alpha_val        = 0;
	    layer_para.pipe             = 0;
	    layer_para.src_win.x        = 0;
	    layer_para.src_win.y        = 0;
	    layer_para.src_win.width    = plogo_data->logoinfo[i].bmp_info->Width;
	    layer_para.src_win.height   = plogo_data->logoinfo[i].bmp_info->Height;
	    layer_para.scn_win.x        = (width - scn_w) >> 1;
	    layer_para.scn_win.y        = (height - scn_h) >> 1;
	    layer_para.scn_win.width    = scn_w;
	    layer_para.scn_win.height   = scn_h;
	    arg[0] = (__u64)plogo_data->logo_layer;
	    arg[1] = (__u64)&layer_para;
	    arg[2] = 0;
	    esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_CMD_LAYER_SET_PARA, 0, (void *)arg);

		if (plogo_data->logoinfo[i].ani_inter_time)
		{
			esKRNL_TimeDly(plogo_data->logoinfo[i].ani_inter_time);
		}

		if (plogo_data->carback_en && plogo_data->b_carback)
		{
			arg[0] = (__u64)plogo_data->logo_layer;
			arg[1] = 0;
			arg[2] = 0;
			esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_CMD_LAYER_CLOSE, 0, (void *)arg);
			esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_LAYER_RELEASE, 0, (void *)arg);

			esKRNL_SemPost(plogo_data->cvbs_sem);
			esKRNL_SemPend(plogo_data->anim_sem, 0, &err);
		    if (OS_NO_ERR != err)
		    {
		        __log("anim_sem pend fail, err=%d", err);
		    }

			arg[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
		    arg[1] = 0;
		    arg[2] = 0;
		    plogo_data->logo_layer = (void *)esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_LAYER_REQUEST, 0, (void *)arg);
		    if (plogo_data->logo_layer == 0)
		    {
		        __log("request logo_layer fail");
				goto EXIT_TASK;
		    }
		}

		plogo_data->logoinfo[i].ani_cur_frame++;
		__inf("total_frame=%d, cur_frame=%d\n", plogo_data->logoinfo[i].ani_total_frame, plogo_data->logoinfo[i].ani_cur_frame);
		if (plogo_data->logoinfo[i].ani_cur_frame > plogo_data->logoinfo[i].ani_total_frame)
		{
			break;
		}
        
        if(i == STARTUP)
        {
    		eLIBs_sprintf(path, LOGO_PATH"stalogo%d.bin", plogo_data->logoinfo[i].ani_cur_frame);
        }
        else if(i == SHUTDOWN)
        {
    		eLIBs_sprintf(path, LOGO_PATH"stologo%d.bin", plogo_data->logoinfo[i].ani_cur_frame);
        }
		LoadDataFromFile(path, (char **)&plogo_data->logoinfo[i].logo_addr, &plogo_data->logoinfo[i].logo_size);
		Parse_Pic_BMP_ByRam((void *)plogo_data->logoinfo[i].logo_addr, plogo_data->logoinfo[i].logo_size, 
                            plogo_data->logoinfo[i].bmp_info, plogo_data->logoinfo[i].bmp_info->Buffer);
        
		plogo_data->logoinfo[i].logo_addr = 0;
		plogo_data->logoinfo[i].logo_size = 0;
	}

	EXIT_TASK:
	plogo_data->b_animation = 0; 
	esKRNL_TDel(EXEC_prioself);
}


static void __animation_task(void *arg)
{
    logo_execute i = *((logo_execute *)arg);
	if (plogo_data->logoinfo[i].picture_type == JPG)
	{
    	__err("not support jpg now");
		//__jpg_animation(plogo_data);
	}
	else if(plogo_data->logoinfo[i].picture_type == BMP)
	{
		__bmp_animation(i);
	}
    else
    {
        __err("no this logo.");
    }

}
#if 0

__s32 close_jpg_animation(void)
{
	__u32 arg[3] = {0};

	if(plogo_data->b_stop != 1)
	{
		arg[0] = plogo_data->logo_layer;
	    arg[1] = 0;
	    arg[2] = 0;
	    esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_CMD_LAYER_CLOSE, 0, (void *)arg);
		esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_LAYER_RELEASE, 0, (void *)arg);
	}

	if (plogo_data->jpeg_info)
    {
        if (plogo_data->jpeg_info->yuv_buf)
        {
            esMEMS_Pfree(plogo_data->jpeg_info->yuv_buf, ((plogo_data->jpeg_yuv_size + 1023) >> 10));
            plogo_data->jpeg_info->yuv_buf = NULL;
        }
        esMEMS_Mfree(0, plogo_data->jpeg_info);
        plogo_data->jpeg_info = NULL;
    }

	return 0;
}
#endif

__s32 close_bmp_animation(logo_execute i)
{
	__u64 arg[3] = {0};

	if(plogo_data->b_stop != 1)
	{
		arg[0] = (__u64)plogo_data->logo_layer;
	    arg[1] = 0;
	    arg[2] = 0;
	    esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_CMD_LAYER_CLOSE, 0, (void *)arg);
		esMODS_MIoctrl(plogo_data->mp_display, MOD_DISP_LAYER_RELEASE, 0, (void *)arg);
	}

	if (plogo_data->logoinfo[i].bmp_info)
    {
        if (plogo_data->logoinfo[i].bmp_info->Buffer)
        {
        	esMEMS_Pfree(plogo_data->logoinfo[i].bmp_info->Buffer, ((plogo_data->logoinfo[i].bmp_info->BufferSize + 1023) >> 10));
			plogo_data->logoinfo[i].bmp_info->Buffer = NULL;
        }
        eLIBs_free(plogo_data->logoinfo[i].bmp_info);
        plogo_data->logoinfo[i].bmp_info = NULL;
    }
	return 0;
}

static __u32 stop_animation(logo_execute i)
{
	if (plogo_data->t_animation_task)
	{
		while (1)
		{
			if (esKRNL_TDelReq(plogo_data->t_animation_task) == OS_TASK_NOT_EXIST)
			{
				break;
			}
			esKRNL_TimeDly(1);
		}
		plogo_data->t_animation_task = 0;
	}
	if (plogo_data->logoinfo[i].picture_type == JPG)
	{
		//close_jpg_animation();
	}
	else if(plogo_data->logoinfo[i].picture_type == BMP)
	{
		close_bmp_animation(i);
	}
    else
    {
        __err("no this logo");
    }
	
	return 0;
	
}

#if 0
__u32 check_io(void)
{
    __u8   i = 0, byte = 0;
    __u32  ret = 0;
    __hdle hPins = 0;
    __s32   io_used = 0;
    __s32   io_data = 0;

    user_gpio_set_t gpio_set;
    eLIBs_memset(&gpio_set, 0x00, sizeof(gpio_set));

    if (esCFG_GetKeyValue("mixture_para", "carback_io_used", &io_used, 1) != EPDK_OK)
    {
        __log("get back_io_used failed!\n");
        io_used = 0;
    }

    if (esCFG_GetKeyValue("mixture_para", "carback_io_data", &io_data, 1) != EPDK_OK)
    {
        __log("get back_io_data failed!\n");
        io_data = 0;
    }

    if (io_used == 0)
    {
        __wrn("io_used is invalid!\n");
        return -1;
    }

    ret = esCFG_GetGPIOSecData("mixture_para", (void *)&gpio_set, 1);
    hPins = esPINS_PinGrpReq(&gpio_set, 1);

    esPINS_SetPinIO(hPins, 0, NULL);
    esPINS_SetPinPull(hPins, 1, NULL);


    byte = 0;

    //for(i=0;i<8;i++)
    //{
    ret = esPINS_ReadPinData(hPins, 0);
    //__log("av in singal ret = %d\n",ret);

    if (io_data == 1)
    {
        if (ret == 0)
        {
            byte = 1;
        }
    }
    else
    {
        if (ret == 1)
        {
            byte = 1;
        }
    }

    //}

    if (byte == 0)
    {
        ret = esPINS_PinGrpRel(hPins, 0);
       wrn__log("av in singal is checked\n");
        return 0;
    }
    else
    {
        ret = esPINS_PinGrpRel(hPins, 0);
        //__log("av in singal is not checked\n");
        return -1;
    }

}

__s32 video_init(void)
{
    __s32 index = 0;
    mid_video_play = esMODS_MInstall("d:\\mod\\video_play.mod", 0);
    if (!mid_video_play)
    {
        __wrn("install video_play mod fail!\n");
        return EPDK_FAIL;
    }
    
    mod_video_play = esMODS_MOpen(mid_video_play, index);
    if (!mod_video_play)
    {
        __wrn("video_play mod failed!\n");
        return EPDK_FAIL;
    }
    return EPDK_OK;
}

__s32 video_exit(void)
{
    if (mod_video_play)
    {
        esMODS_MClose(mod_video_play);
        esMODS_MUninstall(mid_video_play);
        return EPDK_OK;
    }
    return EPDK_FAIL;
}

__s32 video_open(void)
{
    __s32 channel_select = 0, dir = 0;

    if (mod_video_play)
    {
        if (esCFG_GetKeyValue("mixture_para", "channel_select", &channel_select, 1) != EPDK_OK)
        {
            channel_select = 0;
        }
        if (esCFG_GetKeyValue("mixture_para", "dir", &dir, 1) != EPDK_OK)
        {
            dir = 0;
        }
        esMODS_MIoctrl(mod_video_play, VIDEO_PLAY_SET_ROTATE_ANGLE, dir, 0);
        esMODS_MIoctrl(mod_video_play, VIDEO_PLAY_START, channel_select, 0);
        return EPDK_OK;
    }
    return EPDK_FAIL;
}

__s32 video_close(void)
{
    if (mod_video_play)
    {
        return esMODS_MIoctrl(mod_video_play, VIDEO_PLAY_STOP, 0, 0);
    }
    return EPDK_FAIL;
}
__s32 video_get_status(void)
{
    if (mod_video_play)
    {
        return esMODS_MIoctrl(mod_video_play, VIDEO_PLAY_GET_SIGNALE_DECT, 0, 0);
    }
    return EPDK_FAIL;
}

static __s32 show_nosignal_bmp_picture(void)
{
	__s32 ret = 0;
	__u32 arg[3] = {0, 0, 0};
    __s32 width = 0, height = 0;
    __disp_layer_info_t layer_para;
    void *p_disp = NULL;
    static __u8 is_read = 0;
	char *pic_addr;
	__u32 pic_size;

    eLIBs_memset(&layer_para, 0x00, sizeof(layer_para));

	//parse logo bmp
    picture_nosi = (Picture_t *)eLIBs_malloc(sizeof(Picture_t));
    if (picture_nosi == NULL)
    {
        __log("malloc picture_nosi failed!\n");
        return EPDK_FAIL;
    }
    eLIBs_memset(picture_nosi, 0, sizeof(Picture_t));

	LoadDataFromFile(LOGO_PATH"nosignal.bmp", &pic_addr, &pic_size);
	Parse_Pic_BMP_ByRam(pic_addr, pic_size, picture_nosi, NULL);
	pic_addr = NULL;
	pic_size = 0;
	
    /*open display driver handle*/
    p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if (!p_disp)
    {
        __log("open display device fail!\n");
        return EPDK_FAIL;
    }

    width  = esMODS_MIoctrl(p_disp, MOD_DISP_GET_SCN_WIDTH, 0, 0);
    height = esMODS_MIoctrl(p_disp, MOD_DISP_GET_SCN_HEIGHT, 0, 0);
    __wrn("lcd width = %d, height = %d\n", width, height);

    arg[0] = MOD_DISP_LAYER_WORK_MODE_NORMAL;
    arg[1] = 0;
    arg[2] = 0;
    nosi_layer = esMODS_MIoctrl(p_disp, MOD_DISP_LAYER_REQUEST, 0, (void *)arg);
	
    layer_para.mode = MOD_DISP_LAYER_WORK_MODE_NORMAL;
    layer_para.fb.addr[0]       = (__u32)picture_nosi->Buffer;
    layer_para.fb.addr[1]       = 0;
    layer_para.fb.addr[2]       = 0;
    layer_para.fb.size.width    =  picture_nosi->Width;
    layer_para.fb.size.height    = picture_nosi->Height;
    layer_para.fb.mode          = DISP_MOD_INTERLEAVED;
    layer_para.fb.format        = DISP_FORMAT_ARGB_8888;
    layer_para.fb.br_swap       = 0;
    layer_para.fb.seq           = DISP_SEQ_ARGB;
    layer_para.ck_enable        = 0;
    layer_para.alpha_en         = 1;
    layer_para.alpha_val        = 0xff;
    layer_para.pipe             = 0;
    layer_para.src_win.x        = 0;
    layer_para.src_win.y        = 0;
    layer_para.src_win.width    = picture_nosi->Width;
    layer_para.src_win.height   = picture_nosi->Height;

    layer_para.scn_win.x        = (width - picture_nosi->Width) >> 1;
    layer_para.scn_win.y        = (height - picture_nosi->Height) >> 1;
    layer_para.scn_win.width    = picture_nosi->Width;
    layer_para.scn_win.height   = picture_nosi->Height;
	
    __wrn("picture_nosi->Width = %d, picture_nosi->Height = %d\n", picture_nosi->Width, picture_nosi->Height);
    arg[0] = nosi_layer;
    arg[1] = (__u32)&layer_para;
    arg[2] = 0;
    esMODS_MIoctrl(p_disp, MOD_DISP_CMD_LAYER_SET_PARA, 0, (void *)arg);

	arg[0] = nosi_layer;
    arg[1] = 0;
    arg[2] = 0;
  //  esMODS_MIoctrl(p_disp, DISP_CMD_LAYER_OPEN, 0, (void *)arg);
    eLIBs_fclose(p_disp);
	
    return EPDK_OK;
}

__s32 close_nosignal_bmp_picture(void)
{
    void *p_disp = NULL;
    __u32 arg[3] = {0, 0, 0};

    p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if (!p_disp)
    {
        __log("open display device fail!\n");
        return EPDK_FAIL;
    }
	
    if (nosi_layer)
    {
        arg[0] = nosi_layer;
        arg[1] = 0;
        arg[2] = 0;
		esMODS_MIoctrl(p_disp, MOD_DISP_CMD_LAYER_CLOSE, 0, (void *)arg);
        esMODS_MIoctrl(p_disp, MOD_DISP_LAYER_RELEASE, 0, (void *)arg);
        nosi_layer = 0;
    }
	
    if (picture_nosi != NULL)
    {
        if (picture_nosi->Buffer != NULL)
        {
        	esMEMS_Pfree(picture_nosi->Buffer, ((picture_nosi->BufferSize + 1023) >> 10));
            picture_nosi->Buffer = NULL;
            picture_nosi->BufferSize = 0;
        }
        eLIBs_free(picture_nosi->Buffer);
        picture_nosi = NULL;
    }
	
	if(p_disp)
	{
		eLIBs_fclose(p_disp);
	}
	
    return EPDK_OK;
}

static void __carback_task(void *p_arg)
{
	__u8 err;
	__s32 nosignal_pic_enable = 0;
	__s32 video_status = 0;
	__s32 nosignal_pic_showed = 0;
	__s32 debug_count = 0;
	ES_FILE *fp = NULL;
	mod_logo_data_t *plogo_data = (mod_logo_data_t *)p_arg;

	if (esCFG_GetKeyValue("mixture_para", "nosignal_pic_enable", &nosignal_pic_enable, 1) != EPDK_OK)
    {
        nosignal_pic_enable = 0;
    }
	__log("nosignal_pic_enable=%d\n", nosignal_pic_enable);
	
    if (check_io() != 0)
    {
        __log("not det carback");
		 goto EXIT_TASK;
    }
    
	// waiting for mount part D: plugin
	while((fp = eLIBs_fopen("d:\\mod\\video_play.mod", "rb")) == NULL)
	{
		esKRNL_TimeDly(1);
	}
	eLIBs_fclose(fp);
	
	__log("enter carback!");
	plogo_data->b_carback = 1;
	if(plogo_data->stype == STATIC_LOGO || plogo_data->stype == CHANGE_LOGO)
	{
		stop_picture();
	}
	else if (plogo_data->stype == ANIMATION)
	{
	    esKRNL_SemPend(plogo_data->cvbs_sem, 0, &err);
	    if (OS_NO_ERR != err)
	    {
	        __log("cvbs pend fail, err=%d", err);
	    }
	}
	
	video_init();
    video_open();

	while (1)
    {
    	#if 0
    	debug_count++;
		if(debug_count >= 500)
		{
			if (nosignal_pic_enable == 1)
			{
				if(nosignal_pic_showed == 1)
				{
					__log("close nosignal picture\n");
					close_nosignal_bmp_picture();
					nosignal_pic_showed = 0;
				}
			}
			break;
		}
		#endif
		
		if (check_io() != 0)
		{
			if (nosignal_pic_enable == 1)
			{
				if(nosignal_pic_showed == 1)
				{
					__log("close nosignal picture\n");
					close_nosignal_bmp_picture();
					nosignal_pic_showed = 0;
				}
			}
			break;
		}

		if (nosignal_pic_enable == 1)
		{
			video_status = video_get_status();
			// nosignal show a bmp picture
			if (video_status == 0)
			{
				if(nosignal_pic_showed == 0)
				{
					__log("show nosignal picture\n");
					show_nosignal_bmp_picture();
					nosignal_pic_showed = 1;
				}
			}
			else
			{
				if(nosignal_pic_showed == 1)
				{
					__log("close nosignal picture\n");
					close_nosignal_bmp_picture();
					nosignal_pic_showed = 0;
				}
			}
		}
		
		esKRNL_TimeDly(1);
    }

	video_close();
	video_exit();
	if(plogo_data->stype == STATIC_LOGO || plogo_data->stype == CHANGE_LOGO)
	{
		show_picture();
	}
	else if (plogo_data->stype == ANIMATION)
	{
		esKRNL_SemPost(plogo_data->anim_sem);
	}
	esKRNL_TimeDly(100);
	plogo_data->b_carback = 0;
	__log("exit carback!");

	EXIT_TASK:
    esKRNL_TDel(EXEC_prioself);
}
	
#endif
__s32 plogo_data_init(void)
{
	//__u32 addr = 0;
	//__u32 size = 0;
	int param = 0;
	//char path[64];
	
	if (esCFG_GetKeyValue("mixture_para", "boot_logo_en", (int32_t *)&param, 1) != EPDK_OK)
    {
        param = 0;
    }
	plogo_data->boot_logo_en = param;
///////////////////////////////startup//////////////////////////////////////    
    
	if (esCFG_GetKeyValue("mixture_para", "startup_logo_en", (int32_t *)&param, 1) != EPDK_OK)
    {
        param = 0;
    }
	plogo_data->logoinfo[STARTUP].logo_en= param;

    if(plogo_data->logoinfo[STARTUP].logo_en)
    {
    	if (esCFG_GetKeyValue("mixture_para", "startup_logo_type", (int32_t *)&param, 1) != EPDK_OK)
        {
            param = 0;
        }
    	plogo_data->logoinfo[STARTUP].logo_type= (Logo_type)param;

    	if (esCFG_GetKeyValue("mixture_para", "startup_picture_type", (int32_t *)&param, 1) != EPDK_OK)
        {
            param = 0;
        }
    	plogo_data->logoinfo[STARTUP].picture_type= (Picture_type)param;

    	if (esCFG_GetKeyValue("mixture_para", "startup_logo_enlarge", (int32_t *)&param, 1) != EPDK_OK)
        {
            param = 0;
        }
    	plogo_data->logoinfo[STARTUP].logo_enlarge = param;
        
		if (esCFG_GetKeyValue("mixture_para", "startup_ani_total_frame", &param, 1) != EPDK_OK)
	    {
	        param = 0;
	    }
		plogo_data->logoinfo[STARTUP].ani_total_frame = param;

		if (esCFG_GetKeyValue("mixture_para", "startup_ani_inter_time", &param, 1) != EPDK_OK)
	    {
	        param = 0;
	    }
		plogo_data->logoinfo[STARTUP].ani_inter_time = param;

    	__inf("startup::logo_type=%d, picture_type=%d,logo_enlarge=%d  ani_time = %d  ani_total = %d\n",
		plogo_data->logoinfo[STARTUP].logo_type, plogo_data->logoinfo[STARTUP].picture_type,plogo_data->logoinfo[STARTUP].logo_enlarge,
		plogo_data->logoinfo[STARTUP].ani_inter_time,plogo_data->logoinfo[STARTUP].ani_total_frame);        
    }
///////////////////////////////shutdown//////////////////////////////////////    
	if (esCFG_GetKeyValue("mixture_para", "shutdown_logo_en", (int32_t *)&param, 1) != EPDK_OK)
    {
        param = 0;
    }
	plogo_data->logoinfo[SHUTDOWN].logo_en= param;
    
    if(plogo_data->logoinfo[SHUTDOWN].logo_en)
    {
    	if (esCFG_GetKeyValue("mixture_para", "shutdown_logo_type", (int32_t *)&param, 1) != EPDK_OK)
        {
            param = 0;
        }
    	plogo_data->logoinfo[SHUTDOWN].logo_type= (Logo_type)param;

    	if (esCFG_GetKeyValue("mixture_para", "shutdown_picture_type", (int32_t *)&param, 1) != EPDK_OK)
        {
            param = 0;
        }
    	plogo_data->logoinfo[SHUTDOWN].picture_type= (Picture_type)param;

    	if (esCFG_GetKeyValue("mixture_para", "shutdown_logo_enlarge", (int32_t *)&param, 1) != EPDK_OK)
        {
            param = 0;
        }
    	plogo_data->logoinfo[SHUTDOWN].logo_enlarge = param;
        
		if (esCFG_GetKeyValue("mixture_para", "shutdown_ani_total_frame", &param, 1) != EPDK_OK)
	    {
	        param = 0;
	    }
		plogo_data->logoinfo[SHUTDOWN].ani_total_frame = param;

		if (esCFG_GetKeyValue("mixture_para", "shutdown_ani_inter_time", &param, 1) != EPDK_OK)
	    {
	        param = 0;
	    }
		plogo_data->logoinfo[SHUTDOWN].ani_inter_time = param;
        
    	__inf("shutdown::logo_type=%d, picture_type=%d,logo_enlarge=%d  ani_time = %d  ani_total = %d\n",
		plogo_data->logoinfo[SHUTDOWN].logo_type, plogo_data->logoinfo[SHUTDOWN].picture_type,plogo_data->logoinfo[SHUTDOWN].logo_enlarge,
		plogo_data->logoinfo[SHUTDOWN].ani_inter_time,plogo_data->logoinfo[SHUTDOWN].ani_total_frame);
    }
////////////////////////////////carback//////////////////////////////
	if (esCFG_GetKeyValue("mixture_para", "carback_en", (int32_t *)&param, 1) != EPDK_OK)
    {
        param = 0;
    }
	plogo_data->carback_en = param;
    
	if(plogo_data->carback_en)
	{
		 plogo_data->cvbs_sem = esKRNL_SemCreate(0);
		 plogo_data->anim_sem = esKRNL_SemCreate(0);
	}    
	#if 0
	if (plogo_data->stype == STATIC_LOGO)
	{
		if(plogo_data->boot_logo_en)
		{
		__err("not support now");
			/*
			esKSRV_get_logo_info(&addr, &size);
			if(addr == NULL || size == 0)
			{
				__log("boot addr is null failed\n");
				return -1;
			}
			plogo_data->picture_addr = (char *)addr;
			plogo_data->picture_size = size;
			__log("picture addr=0x%x, size=%d\n", plogo_data->picture_addr, plogo_data->picture_size);
			*/
		}
		else
		{
			plogo_data->picture_index = 1;
			eLIBs_sprintf(path, LOGO_PATH"startlogo%d.bin", plogo_data->picture_index);
			LoadDataFromFile(path, &plogo_data->picture_addr, &plogo_data->picture_size);
			__inf("picture index=%d, addr=0x%x, size=%d\n", plogo_data->picture_index, 
				plogo_data->picture_addr, plogo_data->picture_size);
		}
	}
	else if (plogo_data->stype == CHANGE_LOGO)
	{
		plogo_data->picture_index = read_picture_index();
		eLIBs_sprintf(path, LOGO_PATH"logo%d.bin", plogo_data->picture_index);
		LoadDataFromFile(path, &plogo_data->picture_addr, &plogo_data->picture_size);
		__inf("picture index=%d, addr=0x%x, size=%d\n", plogo_data->picture_index, 
			plogo_data->picture_addr, plogo_data->picture_size);
	}
	else if (plogo_data->stype == ANIMATION)
	{		
		plogo_data->cur_frame = 1;
		__inf("total_frame=%d, cur_frame=%d, inter_time=%d\n", plogo_data->total_frame, 
			plogo_data->cur_frame, plogo_data->inter_time);
		if(plogo_data->carback_en)
		{
			 plogo_data->cvbs_sem = esKRNL_SemCreate(0);
			 plogo_data->anim_sem = esKRNL_SemCreate(0);
		}
	}
	#endif
	return 0;
}

__s32 plogo_data_uninit(void)
{

#if 0
	__u8 err = 0;
	if (plogo_data->stype == STATIC_LOGO)
	{
		if(plogo_data->boot_logo_en)
		{
			if (plogo_data->picture_addr)
			{
				esMEMS_Mfree(0, plogo_data->picture_addr);
				plogo_data->picture_addr = NULL;
				plogo_data->picture_size= 0;
			}
		}
		else
		{
			if (plogo_data->picture_addr)
			{
				eLIBs_free(plogo_data->picture_addr);
				plogo_data->picture_addr = NULL;
				plogo_data->picture_size= 0;
			}
		}
	}
	else if (plogo_data->stype == CHANGE_LOGO)
	{
		if (plogo_data->picture_addr)
		{
			eLIBs_free(plogo_data->picture_addr);
			plogo_data->picture_addr = NULL;
			plogo_data->picture_size= 0;
		}
	}
	else if (plogo_data->stype == ANIMATION)
	{
		if(plogo_data->carback_en)
		{
			 esKRNL_SemDel(plogo_data->cvbs_sem, OS_DEL_ALWAYS, &err);
			 esKRNL_SemDel(plogo_data->anim_sem, OS_DEL_ALWAYS, &err);
		}
	}
#endif
	return 0;
}

#if 0

rt_err_t mixture_open(struct rt_device *dev, rt_uint16_t oflag)
{

	rt_size_t ret = 0;
    if (dev == NULL)
    {
        return -1;
    }
    return 0;
}

static rt_err_t mixture_close(struct rt_device *dev)
{
	rt_size_t ret = 0;
    if (dev == NULL)
    {
        while (1);
    }

    return 0;
}

static rt_err_t mixture_init(struct rt_device *dev)
{
    return 0;
}

static rt_err_t mixture_read(struct rt_device *dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    return 0;
}

static rt_err_t mixture_write(struct rt_device *dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    return 0;
}

static rt_err_t mixture_control(struct rt_device *dev, int cmd, void *args)
{
	if (dev == NULL)
    {
        while (1);
    }
	switch(cmd)
	{
 
	}
    return 0;
}

static __u32 mixture_reg_dev()
{
    struct rt_device *device;

    device = rt_device_create(RT_Device_Class_Char, 0);
    if (!device)
    {
        __wrn("init mixture err");
        return;
    }
    
    rt_err_t ret = -1;
    
    if(!device)
    {
        return ret;
    }
    device->init            = mixture_init;
    device->open            = mixture_open;
    device->close           = mixture_close;
    device->read            = mixture_read;
    device->write           = mixture_write;
    device->control         = mixture_control;
    device->user_data       = &mod_mixture_mp;

    ret = rt_device_register(device, "mod_mixture", RT_DEVICE_FLAG_RDWR);
    if (ret != 0)
    {
        __err("dev reg fail");
        return ret;
    } 
}

#endif

__s32 Mod_Mixture_MInit(void)
{
   return 0;
}

__s32 Mod_Mixture_MExit(void)
{
	return 0;
}

__mp *Mod_Mixture_MOpen(__u32 mid, __u32 mode)
{
	mod_mixture_mp.used = 1;
	mod_mixture_mp.mid = mid;
  
	plogo_data = (mod_logo_data_t *)eLIBs_malloc(sizeof(mod_logo_data_t));
	if(plogo_data == NULL)
	{
		__log("malloc fail...\n");
		return NULL;
	}
	eLIBs_memset(plogo_data, 0, sizeof(mod_logo_data_t));

	plogo_data->mp_display = esKSRV_Get_Display_Hld();
	if(!plogo_data->mp_display)
    {
        __log("get mp_display fail!");
        return NULL;
    }
	plogo_data_init();

	return (void *)&mod_mixture_mp;    
}

__s32 Mod_Mixture_MClose(__mp *mp)
{
    if(mp == NULL)
        return EPDK_FAIL;
    plogo_data_uninit();
	mod_mixture_mp.mid = 0;
	mod_mixture_mp.used = 0;
    plogo_data->mp_display = NULL;
    eLIBs_free(plogo_data);
    return EPDK_OK;
}

__u32 Mod_Mixture_MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{

    return n;
}

__u32 Mod_Mixture_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{

    return n;
}



long Mod_Mixture_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    __s32 retVal = EPDK_OK;
    logo_execute i;
    
    if (pbuffer != NULL)
    {
        i = *((logo_execute *)pbuffer);
    }
    switch (cmd)
    {
        case MOD_MIXTURE_START:
        {
            char path[64];
			plogo_data->b_stop = 0;
#ifdef CONFIG_PLAN_SPRITE
            if(i == STARTUP)
            {
                __wrn("check power ");
                check_if_lowpower();//don't go down to light up the lcd if low power, because lcd maybe show err.
            }
#endif
			if(plogo_data->logoinfo[i].logo_type == STATIC_LOGO || plogo_data->logoinfo[i].logo_type == CHANGE_LOGO)
			{
    			plogo_data->logoinfo[i].logo_index = 1;
                if(i == STARTUP)
                {
        			eLIBs_sprintf(path, LOGO_PATH"stalogo%d.bin", plogo_data->logoinfo[i].logo_index);
                }
                else if(i == SHUTDOWN)
                {
        			eLIBs_sprintf(path, LOGO_PATH"stologo%d.bin", plogo_data->logoinfo[i].logo_index);
                }
                else
                {
                    __err("no this logo");
                }
                __inf("path = %s",path);
    			LoadDataFromFile(path, (char **)&plogo_data->logoinfo[i].logo_addr, &plogo_data->logoinfo[i].logo_size);
    			__inf("picture index=%d, addr=0x%x, size=%d\n", plogo_data->logoinfo[i].logo_index, 
    				plogo_data->logoinfo[i].logo_addr,plogo_data->logoinfo[i].logo_size);
				show_picture(i);

			}
			else if(plogo_data->logoinfo[i].logo_type == ANIMATION)
			{
				plogo_data->logoinfo[i].ani_cur_frame = 1;
				plogo_data->t_animation_task = esKRNL_TCreate(__animation_task, &i, 0x8000, KRNL_priolevel4);
				esKRNL_TimeDly(1);
			}

            if(i == STARTUP)
            {
    			open_lcd_backlight();
            }  

			if (plogo_data->carback_en)
			{
    			__err("not support carback now");
				//plogo_data->t_carback_task = esKRNL_TCreate(__carback_task, plogo_data, 0x8000, KRNL_priolevel3);
			}
            break;
        }

        case MOD_MIXTURE_STOP:
        {
			if(plogo_data->logoinfo[i].logo_type == STATIC_LOGO || plogo_data->logoinfo[i].logo_type == CHANGE_LOGO)
			{
				stop_picture(i);
			}
			else if(plogo_data->logoinfo[i].logo_type == ANIMATION)
			{
				stop_animation(i);
    			plogo_data->b_stop = 1;
			}
			
			if (plogo_data->carback_en)
			{
				if (plogo_data->t_carback_task)
			    {
			        while (1)
			        {
			            if (esKRNL_TDelReq(plogo_data->t_carback_task) == OS_TASK_NOT_EXIST)
			            {
			                break;
			            }
			            esKRNL_TimeDly(1);
			        }
			        plogo_data->t_carback_task = 0;
			    }
			}

            if(i == SHUTDOWN)
            {
                close_lcd_backlight();
            }            
            break;
        }

		case MOD_MIXTURE_QURY_FINISH:
		{
			if (plogo_data->b_carback == 0 && plogo_data->b_animation == 0)
			{
				retVal = 1;
			}
			else
			{
				retVal = 0;
			}
			break;
		}
		
        default:
        {
            retVal = EPDK_FAIL;
            break;
        }
    }
    return retVal;
}
