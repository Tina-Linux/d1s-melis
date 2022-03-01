/*
*********************************************************************************************************
*                                                   eMOD
*                                  the Easy Portable/Player Operation System
*                                              mod_willow sub-system
*
*                               (c) Copyright 2006-2009, ALLWINNERME, China
*                                           All Rights Reserved
*
* File   : backlayer_lib.c
* Version: V1.0
* By     : xiechuanrong
*********************************************************************************************************
*/
#include "mod_init_i.h"
#include "backlayer_lib.h"
#include "fb_file.h"
#include <emodules/mod_display.h>
#include <kconfig.h>
#include "mod_codec_cmd.h"
#include "mod_mixture.h"

#define ALIGN_TO_16B(x)   ((((x) + (1 <<  4) - 1) >>  4) <<  4)
#define ALIGN_TO_32B(x)   ((((x) + (1 <<  5) - 1) >>  5) <<  5)

typedef struct
{
    uint32_t width;
    uint32_t height;
} JPG_SIZE;

static  uint32_t        bk_mode;
static  stJpegFrameInfo *jpeg_info = NULL;

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
/************************************************************************
* Function: backlayer_create_layer
* Description: ´´½¨±³¾°Í¼²ã
* Input:
* Output:
* Return:
*     > 0: backlayer_handle
*     0: Ê§°Ü
*************************************************************************/
int32_t backlayer_create_layer(__mp *de_hdle)
{
    int32_t     layer_hdle;
    __u64    arg[3];
    static __u32 bk_flag;
    if (!bk_flag)
    {
        layer_hdle = 0x64;
        bk_flag = 1;
    }
    else
    {
        arg[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
        arg[1] = 0;
        arg[2] = 0;
        layer_hdle = esMODS_MIoctrl(de_hdle, MOD_DISP_LAYER_REQUEST, 0, (void *)arg);
    }
    __inf("backlayer_create_layer  layer_hdle = %d",layer_hdle);
    return layer_hdle;
}


/************************************************************************
* Function: backlayer_delete_layer
* Description: É¾³ý±³¾°Í¼²ã
* Input:
    uint32_t layer_handle: ±³¾°Í¼²ã¾ä±ú
* Output:
* Return:
*************************************************************************/
void backlayer_delete_layer(__mp *de_hdle, unsigned long layer_handle)
{
    if (layer_handle != 0)
    {
        unsigned long arg[3];

        arg[0] = layer_handle;
        arg[1] = 0;
        arg[2] = 0;

        esMODS_MIoctrl(de_hdle, MOD_DISP_CMD_LAYER_CLOSE, 0, (void *)arg);
        esMODS_MIoctrl(de_hdle, MOD_DISP_LAYER_RELEASE, 0, (void *)arg);
    }
    
    if (jpeg_info && (bk_mode == JPG_MODE))
    {
        if (jpeg_info->jpegData)
        {
            esMEMS_Mfree(0, jpeg_info->jpegData);
            jpeg_info->jpegData = NULL;
        }
        if (jpeg_info->yuv_buf)
        {
            esMEMS_Mfree(0,jpeg_info->yuv_buf);
            jpeg_info->yuv_buf = NULL;

        }
        esMEMS_Mfree(0, jpeg_info);
        jpeg_info = NULL;
    }
    
    esKRNL_TimeDly(3);//add by bayden, ÑÓÊ±30ms£¬µÈ´ýÏÔÊ¾Çý¶¯VÖÐ¶Ïµ½´ï

    return;
}

/************************************************************************
* Function: backlayer_set_fb
* Description: ÉèÖÃ±³¾°Í¼²ãframe buffer
* Input:
*     uint32_t layer_handle: ±³¾°Í¼²ã¾ä±ú
*     FB *fb: ´ýÉèÖÃµÄ±³¾°frame buffer
* Output:
* Return:
*************************************************************************/
void  backlayer_set_fb_jpg(__mp *de_hdle, unsigned long layer_handle, char *fname, FB *fb, __u8 mod) //JPG mode
{
     __u32 mid_vcoder;
    __s32 *mp_vcoder = NULL;
    __u64 arg[3];
    JPG_SIZE jpg_size;
	__hdle fh = NULL;
//    __u32 fb = NULL;
    __u32 file_size,tmpsize;
    __u32 yuv_size = 0;
    __s32 ret = -1;
	__u64 para[3];
    __disp_layer_info_t layer;

    jpg_size.height = 0;
    jpg_size.width = 0;

    jpeg_info = (stJpegFrameInfo *)esMEMS_Malloc(0,sizeof(stJpegFrameInfo));

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

	fh = fopen(fname,"r");
    if (fh == NULL)
    {
        __log("open  file err!\n");
        return EPDK_FAIL;
    }

    fseek(fh, 0, SEEK_END);
    file_size = ftell(fh);
    fseek(fh, 0, SEEK_SET);
    jpeg_info->jpegData = esMEMS_Malloc(0,file_size);
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
    jpeg_info->yuv_buf = (__u32)esMEMS_Malloc(0,yuv_size);
    esMEMS_CleanFlushDCacheRegion((void *)jpeg_info->yuv_buf, yuv_size);

    ret = esMODS_MIoctrl(mp_vcoder, MPEJ_CODEC_CMD_DECODER, 0, jpeg_info);
    fclose(fh);
    esMODS_MClose(mp_vcoder);
    esMODS_MUninstall(mid_vcoder);

    layer.fb.format        = DISP_FORMAT_YUV420_P;//DISP_FORMAT_YUV420_SP_UVUV;
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
    layer.scn_win.width    = esMODS_MIoctrl(de_hdle, MOD_DISP_GET_SCN_WIDTH, 0, 0);
    layer.scn_win.height   = esMODS_MIoctrl(de_hdle, MOD_DISP_GET_SCN_HEIGHT, 0, 0);
    layer.fb.addr[0]       = (__u32)jpeg_info->y_buf;
    layer.fb.addr[1]       = (__u32)jpeg_info->v_buf;
    layer.fb.addr[2]       = (__u32)jpeg_info->u_buf;
    layer.fb.size.width = jpeg_info->pic_width;
    layer.fb.size.height = jpeg_info->pic_height;

    arg[0] = layer_handle;
    arg[1] = (__u32)&layer;
    arg[2] = 0;
    esMODS_MIoctrl(de_hdle, MOD_DISP_CMD_LAYER_SET_PARA, 0, (void *)arg);

}

void  backlayer_set_fb_bmp(__mp *de_hdle, unsigned long layer_handle, char *fname, FB *fb, uint8_t mod)
{
    uint32_t            lcd_height, lcd_width;
    __disp_layer_info_t layer;
    __disp_fb_t         disp_fb;
    uint32_t            size_fb;
    //static uint32_t release_flag;
    __u64            arg[3];

    lcd_height  = esMODS_MIoctrl(de_hdle, MOD_DISP_GET_SCN_HEIGHT, 0, 0);
    lcd_width   = esMODS_MIoctrl(de_hdle, MOD_DISP_GET_SCN_WIDTH,   0, 0);
    memset(&layer, 0x00, sizeof(layer));

    layer.pipe  = 0;

	__inf("back lcd_height = %d  lcd_width = %d",lcd_height,lcd_width);

    if (fb->fmt.type == FB_TYPE_YUV)
    {
    	__inf("layer.mode = MOD_DISP_LAYER_WORK_MODE_SCALER");
        layer.mode = MOD_DISP_LAYER_WORK_MODE_SCALER;
    }
    else
    {
    	__inf("layer.mode = MOD_DISP_LAYER_WORK_MODE_NORMAL");    
        layer.mode = MOD_DISP_LAYER_WORK_MODE_NORMAL;
    }

    layer.src_win.x         = 0;
    layer.src_win.y         = 0;
    layer.src_win.height    = fb->size.height;
    layer.src_win.width     = fb->size.width;
	__log("fb->size.height = %d  fb->size.height = %d",fb->size.height,fb->size.width);

    if (layer.mode == MOD_DISP_LAYER_WORK_MODE_NORMAL)
    {
        layer.scn_win.height    = fb->size.height;
        layer.scn_win.width     = fb->size.width;
    }
    else
    {
        if (mod == BACKLAYER_MOD_RATIO)
        {
            layer.scn_win.width     = lcd_width;
            layer.scn_win.height    = fb->size.height * layer.scn_win.width / fb->size.width;

            if (layer.scn_win.height > lcd_height)
            {
                layer.scn_win.height    = lcd_height;
                layer.scn_win.width     = fb->size.width * layer.scn_win.height / fb->size.height;
            }
        }
        else
        {
            layer.scn_win.width     = lcd_width;
            layer.scn_win.height    = lcd_height;
        }
    }
	layer.prio      = 1;
    layer.scn_win.x = ((int32_t)lcd_width - (int32_t)layer.scn_win.width) / 2;
    layer.scn_win.y = ((int32_t)lcd_height - (int32_t)layer.scn_win.height) / 2;

    GUI_Fb2Dispfb(fb, &disp_fb);
    layer.fb.lbc_mode       = 0;
    layer.fb        = disp_fb;

    __log("x:%d, y:%d, w:%d, h:%d, laymode:%d.", layer.scn_win.x, layer.scn_win.y, layer.scn_win.width, layer.scn_win.height, layer.mode);
    //esMODS_MIoctrl(de_hdle, MOD_DISP_CMD_START_CMD_CACHE, 0, (void *)arg);

    arg[0] = layer_handle;
    arg[1] = (unsigned long)&layer;
    arg[2] = 0;

    esMODS_MIoctrl(de_hdle, MOD_DISP_CMD_LAYER_SET_PARA, 0, (void *)arg);

    size_fb = (fb->size.width * fb->size.height * 32 + 7) >> 3;

    esMEMS_CleanFlushDCacheRegion(fb->addr[0], size_fb);
}

void  backlayer_set_fb(__mp *de_hdle, unsigned long layer_handle, char *fname, FB *fb, uint8_t mod)
{
#if CONFIG_MOD_MIXTURE_ENABLE
	uint32_t m_mid = 0;
	unsigned long mp_mod = 0;
	uint32_t        mid_mixture = 0;
	__mp        *mod_mixture = NULL;
	static  uint32_t release_flag = 0;
	__u64 arg[3];
	__inf(".......backlayer_set_fb");
	if (!release_flag)
	{
		esKSRV_Get_Mixture_Hld(&m_mid, &mp_mod);
        	__inf("mid = %x  mp = %x",m_mid,mp_mod);
		if(m_mid==0 || mp_mod==0)
		{
			__log("esKSRV get logo hdle failed\n");
			release_flag = 1;
			return;
		}
		mid_mixture = m_mid;
		mod_mixture = (__mp *)mp_mod;
		// qury carback and anintion are finished or not
		while(esMODS_MIoctrl(mod_mixture, MOD_MIXTURE_QURY_FINISH, 0, 0) != 1)
		{
			esKRNL_TimeDly(3);
		}
    	arg[0] = STARTUP;
		esMODS_MIoctrl(mod_mixture, MOD_MIXTURE_STOP, 0, (void *)arg);
		if(mod_mixture)
		{
			esMODS_MClose(mod_mixture);
		}
		if (mid_mixture)
		{
			esMODS_MUninstall(mid_mixture);
		}
		release_flag = 1;        
	}
    if (bk_mode == JPG_MODE)
    {
        backlayer_set_fb_jpg(de_hdle, layer_handle, fname, NULL, mod);
    }
    else
    {
        backlayer_set_fb_bmp(de_hdle, layer_handle, NULL, fb, mod);
    }
#else
    backlayer_set_fb_bmp(de_hdle, layer_handle, NULL, fb, mod);
#endif
}
/************************************************************************
* Function: backlayer_set_top
* Description: ÉèÖÃ±³¾°Í¼²ãÎªtop
* Input:
*     uint32_t layer_handle: ±³¾°Í¼²ã¾ä±ú
* Output:
* Return:
*************************************************************************/
void  backlayer_set_top(__mp *de_hdle, unsigned long layer_handle)
{
    unsigned long  arg[3];

    arg[0] = layer_handle;
    arg[1] = 0;
    arg[2] = 0;
    //esMODS_MIoctrl(de_hdle, DISP_CMD_LAYER_TOP, 0, (void *)arg);
}


/************************************************************************
* Function: backlayer_set_bottom
* Description: ÉèÖÃ±³¾°Í¼²ãÎªbottom
* Input:
*     uint32_t layer_handle: ±³¾°Í¼²ã¾ä±ú
* Output:
* Return:
*************************************************************************/
void  backlayer_set_bottom(__mp *de_hdle, unsigned long layer_handle)
{
    unsigned long arg[3];

    arg[0] = layer_handle;
    arg[1] = 0;
    arg[2] = 0;
    //esMODS_MIoctrl(de_hdle, DISP_CMD_LAYER_BOTTOM, 0, (void *)arg);
}

uint32_t get_logo_mode(void)
{
    if (esCFG_GetKeyValue("backlayer", "backlayer_mode", &bk_mode, 1) != EPDK_OK)
    {
        bk_mode = 0;
        __err("get_logo_mode err!");
    }
    return bk_mode;
}
