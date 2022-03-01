/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                           libs for thumb modlue
*
*                                   (c) Copyright 2006-2009, China
*                                            All Rights Reserved
*
* File    : thumb_dec.c
* By      : lyn
* Version : V1.00
* time    : 2011-03-16
*********************************************************************************************************
*/

//#include "epdk.h"
#include "beetles_app.h"


#if 1
static __u8     willow_mid;
static __mp    *willow_mod;

static __u8     cedar_mid;
static __mp    *cedar_mod;


/*

static void wait_willow_ready(void)
{
    __u8 status;

    while(1)
    {
        status = esMODS_MIoctrl(willow_mod, WILLOW_CMD_QUERY_STATUS, 0, 0);
        if(status != WILLOW_STATUS_BUSY)
            break;
        esKRNL_TimeDly(1);
    }
}

static void wait_willow_finish(void)
{
    __u8 status;

    while(1)
    {
        status = esMODS_MIoctrl(willow_mod, WILLOW_CMD_QUERY_STATUS, 0, 0);
        if(status == WILLOW_STATUS_FINISH)
            break;
        esKRNL_TimeDly(1);
    }
}


__s32   photo_thumb_open(void)
{
    return EPDK_OK;

    willow_mid = esMODS_MInstall("d:\\mod\\willow\\willow.mod", 0);
    if(willow_mid == 0)
    {
        __wrn("install willow fail!\n");
        return EPDK_FAIL;
    }

    willow_mod = esMODS_MOpen(willow_mid, 0);
    if(willow_mod == NULL)
    {
        __wrn("open willow fail!\n");
        esMODS_MUninstall(willow_mid);
        willow_mid = 0;
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

__s32   photo_thumb_close(void)
{
    return EPDK_OK;

    if(willow_mod != NULL)
    {
        esMODS_MClose(willow_mod);
        willow_mod = NULL;
    }

    if(willow_mid != 0)
    {
        esMODS_MUninstall(willow_mid);
        willow_mid = 0;
    }

    return EPDK_OK;
}
void    thumb_data_rotate(void * des_data, void * src_data, __s32 width, __s32 height, __s32 rotate)    //顺时针旋转方向
{
    __s32           i, j;
    __u32           *p_des, *p_src, *p_tmp;

    p_des = (__u32 *)des_data;
    p_src = (__u32 *)src_data;
    if((rotate == 1)||(rotate == 90))
    {
        p_des = p_des + height - 1;
        p_tmp = p_des;
        for(i = 0; i < height; i++)
        {
            p_des = p_tmp - i;
            for(j = 0; j < width; j++)
            {
                *p_des = *p_src++;
                p_des = p_des + height;
            }
        }
    }
    else if((rotate == 2)||(rotate == 180))
    {
        p_des = p_des + width * height - 1;
        p_tmp = p_des;
        for(i = 0; i < height; i++)
        {
            for(j = 0; j < width; j++)
            {
                *p_des-- = *p_src++;
            }
        }
    }
    else if((rotate == 3)||(rotate == 270))
    {
        p_des = p_des + (width - 1) * height;
        p_tmp = p_des;
        for(i = 0; i < height; i++)
        {
            p_des = p_tmp + i;
            for(j = 0; j < width; j++)
            {
                *p_des = *p_src++;
                p_des = p_des - height;
            }
        }
    }
    else
    {

    }
}


__s32   photo_thumb_get(char * file, thumb_para_t para, void * thumb_buf)
{
    __u32       mode;
    __s32       result = -1;
    __willow_get_thumbs_param_t     param;
    __u32       scn_dir ;
    void *      tmp_buf = NULL ;
    __disp_output_type_t    output;

    output = get_display_output_type();
    if(output == DISP_OUTPUT_TYPE_LCD)
    {
        scn_dir = MGUI_GetDispDirection();
        __log("scn_dir=0x%x\n",scn_dir);
    }
    else
    {
        scn_dir = SCREEN_DIR_0 ;
    }
    willow_mid = esMODS_MInstall("d:\\mod\\willow\\willow.mod", 0);
    if(willow_mid == 0)
    {
        __wrn("install willow fail!\n");
        return EPDK_FAIL;
    }

    willow_mod = esMODS_MOpen(willow_mid, 0);
    if(willow_mod == NULL)
    {
        __wrn("open willow fail!\n");
        esMODS_MUninstall(willow_mid);
        willow_mid = 0;
        return EPDK_FAIL;
    }

    mode = WILLOW_SCALE_RATIO_SCALE | 1;
    param.filename      = file;
    param.format        = para.format;

    if( scn_dir == SCREEN_DIR_90 || scn_dir == SCREEN_DIR_270 )
    {
        __u32   tmp_len = (para.size.width * para.size.height * 4 + 1023) / 1024;
        tmp_buf = (void *)esMEMS_Palloc(tmp_len, MEMS_PALLOC_MODE_BND_NONE | MEMS_PALLOC_MODE_BNK_NONE);
        param.buf = tmp_buf;
        param.size.width    = para.size.height;
        param.size.height   = para.size.width;
    }
    else
    {
        param.buf = thumb_buf;
        param.size.width    = para.size.width;
        param.size.height   = para.size.height;
    }

    wait_willow_ready();
    result = esMODS_MIoctrl(willow_mod, WILLOW_CMD_GET_THUMBS, 0, &param);
    if(result == -1)
    {
        goto __photo_thumb_exit;
    }
    wait_willow_finish();
    if(param.filename)
    {
        if(esMODS_MIoctrl(willow_mod, WILLOW_CMD_CHECK_IMG, 0, &param) == EPDK_FAIL)
        {
            goto __photo_thumb_exit;
        }

        wait_willow_ready();
        esMODS_MIoctrl(willow_mod, WILLOW_CMD_START_DEC, 0, 0);

        esMODS_MIoctrl(willow_mod ,WILLOW_CMD_START_SHOW_EXT, 0, (void *)mode);

        wait_willow_finish();
        if( scn_dir == SCREEN_DIR_90 || scn_dir == SCREEN_DIR_270 )
        {
            __u32   tmp_len = (para.size.width * para.size.height * 4 + 1023) / 1024;
            thumb_data_rotate(thumb_buf,tmp_buf,para.size.height,para.size.width,scn_dir);
            esMEMS_Pfree(tmp_buf, tmp_len);
        }

        //return EPDK_OK;
    }

__photo_thumb_exit:
    if(willow_mod != NULL)
    {
        esMODS_MClose(willow_mod);
        willow_mod = NULL;
    }

    if(willow_mid != 0)
    {
        esMODS_MUninstall(willow_mid);
        willow_mid = 0;
    }
    return EPDK_OK;
}
*/
#if 1

__s32  video_thumb_open(void)
{
    cedar_mid = esMODS_MInstall("d:\\mod\\cedar.mod", 0);
    if (cedar_mid == 0)
    {
        __wrn("install cedar fail!\n");
        return EPDK_FAIL;
    }

    cedar_mod = esMODS_MOpen(cedar_mid, 0);
    if (cedar_mod == NULL)
    {
        __wrn("cedar mod open fail!\n");
        esMODS_MUninstall(cedar_mid);
        cedar_mid = 0;
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

__s32  video_thumb_close(void)
{
    if (cedar_mod != NULL)
    {
        esMODS_MClose(cedar_mod);
        cedar_mod = NULL;
    }

    if (cedar_mid != 0)
    {
        esMODS_MUninstall(cedar_mid);
        cedar_mid = 0;
    }

    return EPDK_OK;
}


__s32  video_thumb_get(char *file, void *thumb_buf)
{
    FB          fb;
    __s32       result = -1;
    __cedar_media_file_inf       media_file;
    __u32       scn_dir ;
    void       *tmp_buf = NULL ;
    enum disp_output_type    output;

    ES_FILE    *fp_video_1 = NULL;
    __u32 data_len = 0;
    __s8 file_name[64] = {0};
    __u32 time_cnt = 0;


    eLIBs_printf("-------------------video_thumb_get start------------------------\n");

    eLIBs_memset(&media_file, 0, sizeof(__cedar_media_file_inf));
    eLIBs_strcpy(media_file.file_path, file);
    esMODS_MIoctrl(cedar_mod, CEDAR_CMD_SET_MEDIAFILE, 0, &media_file);
    result = esMODS_MIoctrl(cedar_mod, CEDAR_DUCKWEED_CMD_OPEN_MEDIAFILE, 0, NULL);
    if (result == EPDK_FAIL)
    {
        goto __video_thumb_exit;
    }

    while (1)
    {
        eLIBs_memset(&fb, 0, sizeof(FB));

        {
            fb.size.width = 640;
            fb.size.height = 480;
            fb.addr[0] = thumb_buf;
        }

        fb.addr[1] = NULL;
        fb.addr[2] = NULL;
        fb.fmt.type = FB_TYPE_RGB;
        fb.fmt.cs_mode = BT601;
        fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
        fb.fmt.fmt.rgb.br_swap = 0;
        fb.fmt.fmt.rgb.pixseq = RGB_SEQ_ARGB;//RGB_SEQ_BRGA;
        fb.fmt.fmt.rgb.palette.addr = NULL;
        fb.fmt.fmt.rgb.palette.size = 0;
        esMODS_MIoctrl(cedar_mod, CEDAR_DUCKWEED_CMD_GET_PREVIEW_FB_BY_PTS, time_cnt, &fb);

        eLIBs_printf("-------------------2222-----------------------\n");

        eLIBs_sprintf(file_name, "e:\\tset_%d.dat", time_cnt);
        fp_video_1 = eLIBs_fopen(file_name, "wb");
        if (!fp_video_1)
        {
            eLIBs_printf("=======open logo file fail=========\n");
            //return -1;
            //goto l_decode_exit;
        }
        data_len = fb.size.width * fb.size.height * 4;
        eLIBs_fwrite(thumb_buf, 1, data_len, fp_video_1);

        esKRNL_TimeDly(10);

        eLIBs_fclose(fp_video_1);

        time_cnt += 30 * 1000;
    }

    esMODS_MIoctrl(cedar_mod, CEDAR_DUCKWEED_CMD_CLOSE_MEDIAFILE, 0, NULL);

    eLIBs_printf("-------------------33333333-----------------------\n");

    return EPDK_OK;


__video_thumb_exit:

    eLIBs_printf("-------------------video_thumb_get fail------------------------\n");

    if (cedar_mod != NULL)
    {
        esMODS_MClose(cedar_mod);
        cedar_mod = NULL;
    }

    if (cedar_mid != 0)
    {
        esMODS_MUninstall(cedar_mid);
        cedar_mid = 0;
    }
    return EPDK_OK;
}

void file_thumb_open(void)
{
    // photo_thumb_open();
    video_thumb_open();
    return;
}

void file_thumb_close(void)
{
    // photo_thumb_close();
    video_thumb_close();
    return;
}

__s32   file_thumb_get(char *file, void *thumb_buf)
{
    __s32 file_format = 0;

    // if(eLIBs_GetVideoFormat(file, &file_format))
    {
        __log("---------------------file_thumb_get 33-----------------------\n");
        // if(file_format != 0)
        {
            return video_thumb_get(file, thumb_buf);
        }
    }
#if 0


    if (eLIBs_GetPicFormat(file, &file_format))
    {
        if (file_format != 0)
        {
            return photo_thumb_get(file, para, thumb_buf);
        }
    }
    else if (eLIBs_GetVideoFormat(file, &file_format))
    {
        if (file_format != 0)
        {
            return video_thumb_get(file, para, thumb_buf);
        }
    }
#endif
    return 0;
}
#endif

void thumb_video_preview(void)
{
    char file_name[20] = "e:\\test.avi";
    __u8 *tmp_buff = NULL;

    __log("=====================1===========================\n");
    esMEMS_Info();

    tmp_buff = (__u8 *)esMEMS_Malloc(0, 2 * 1024 * 1024);
    if (tmp_buff == NULL)
    {
        eLIBs_printf("malloc fail\n");
    }

    __log("---------------------thumb_video_preview 1------------------------\n");
    file_thumb_open();
    __log("---------------------thumb_video_preview 2-----------------------\n");
    file_thumb_get(file_name, (void *)tmp_buff);

    __log("=====================2===========================\n");
    file_thumb_close();
    esMEMS_Mfree(0, tmp_buff);
    esMEMS_Info();
    return;
}


#endif

