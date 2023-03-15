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
#include"mod_defs.h"
#include "mod_cedar.h"
#include <mod_cedar.h>
#include <mod_audio.h>
#include <mod_defs.h>
#include <mod_display.h>
#include "aw_dirent.h"

__mp *volatile robin_hce = NULL;
static __u32 mid_display = 0;
static __mp *mp_display = NULL;
__hdle video_layer;
__u32 mid_cedar;


static uint32_t my_esMEMS_FreeMemSize(void)
{
	rt_uint32_t total, used, max_used;
	rt_uint32_t aval;

	rt_memory_info(&total, &used, &max_used);
	aval = total - used;
	return aval;
}

static __s32 disp_open(void)
{
    mp_display = esKSRV_Get_Display_Hld();
/*
	if(!mid_display)
	{
		mid_display = esMODS_MInstall(DISP_MOD_PATH, 0);
		mp_display = esMODS_MOpen(mid_display, 0);
	}
*/
	printf("========mp_display:%p=========\n",mp_display);
    return EPDK_OK;
}
__hdle request_video_layer(__hdle cedar, const RECT *rect_p, __s32 pipe, __s32 prio)
{
    __disp_layer_info_t     image_layer_info;
    __disp_fb_t             image_fb_para;
    RECT                    image_win;
    __hdle                  hlay = NULL;
    __hdle                  hdis = NULL;
    unsigned long arg[3] = {0, };
    if (rect_p == NULL)
    {
        return NULL;
    }
	printf("===11=====mp_display:%p=========\n",mp_display);

    memset(&image_layer_info, 0x00, sizeof(image_layer_info));
    memset(&image_fb_para, 0x00, sizeof(image_fb_para));
    memset(&image_win, 0x00, sizeof(RECT));

    image_fb_para.size.height       = 0;                   // DONT'T CARE
    image_fb_para.size.width        = 0;                   // DONT'T CARE
    image_fb_para.addr[0]           = 0;
    image_fb_para.addr[1]           = 0;
    image_fb_para.addr[2]           = 0;
    image_fb_para.format            = DISP_FORMAT_RGB_565;   // DONT'T CARE
    image_fb_para.seq               = DISP_SEQ_ARGB;        // DONT'T CARE
    image_fb_para.mode              = 0;                    // DONT'T CARE
    image_fb_para.br_swap           = 0;                    // DONT'T CARE
    image_fb_para.cs_mode           = 0;

    image_layer_info.mode           = MOD_DISP_LAYER_WORK_MODE_SCALER;

    image_layer_info.pipe           = pipe;
    image_layer_info.prio           = prio;
    image_layer_info.alpha_en       = 0;
    image_layer_info.alpha_val      = 255;
    image_layer_info.ck_enable      = 0;
    image_layer_info.src_win.x      = 0;
    image_layer_info.src_win.y      = 0;
    image_layer_info.src_win.width  = rect_p->width ;
    image_layer_info.src_win.height = rect_p->height;
    image_layer_info.scn_win.x      = rect_p->x     ;
    image_layer_info.scn_win.y      = rect_p->y     ;
    image_layer_info.scn_win.width  = rect_p->width ;
    image_layer_info.scn_win.height = rect_p->height;
    image_layer_info.fb             = image_fb_para;


    arg[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
    arg[1] = 0;
    arg[2] = 0;
    hlay = (__hdle)esMODS_MIoctrl(mp_display, MOD_DISP_LAYER_REQUEST, 0, (void *)arg);
    if (hlay == NULL)
    {
        printf("Error in applying for the video layer.\n");
        goto error;
    }

   // arg[0] = (__u32)hlay;
   // arg[1] = (__u32)&image_layer_info;
   // arg[2] = 0;
 //   esMODS_MIoctrl(mp_display, MOD_DISP_CMD_LAYER_SET_PARA, 0, (void *)arg);

    image_win.x      = rect_p->x;
    image_win.y      = rect_p->y;
    image_win.width  = rect_p->width ;
    image_win.height = rect_p->height;
    printf("pipe = %d, prio = %d, w:%d, h:%d\n", pipe, prio, rect_p->width, rect_p->height);
    if (esMODS_MIoctrl(cedar, CEDAR_CMD_SET_VID_LAYERHDL, 0, (void *)hlay) != EPDK_OK)
    {
        printf("Fail in setting video layer handle to cedar!\n");
        goto error;
    }
    //set video window information to cedar
    if (esMODS_MIoctrl(cedar, CEDAR_CMD_SET_VID_WINDOW, 0, &image_win) != EPDK_OK)
    {
        printf("Fail in setting video window information to cedar!\n");
        goto error;
    }

    return hlay;

error:
    if (hlay != NULL)
    {
        arg[0] = (__u32)hlay;
        arg[1] = 0;
        arg[2] = 0;
        esMODS_MIoctrl(mp_display, MOD_DISP_LAYER_RELEASE, 0, (void *)arg);
        hlay = NULL;
    }

    return NULL;
}

void release_video_layer(__hdle hlay_video)
{
    __hdle hdis;
    if (hlay_video != NULL)
    {
        unsigned long arg[3];
        arg[0] = (__u32)hlay_video;
        arg[1] = 0;
        arg[2] = 0;
		esMODS_MIoctrl(mp_display, MOD_DISP_CMD_LAYER_CLOSE, 0, (void *)arg);
        esMODS_MIoctrl(mp_display, MOD_DISP_LAYER_RELEASE, 0, (void *)arg);
    }
}

__s32 display_get_size(__s32 *p_width, __s32 *p_height)
{
    ES_FILE              *p_disp = NULL;


    *p_width  = esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_WIDTH, 0, 0);
    *p_height = esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_HEIGHT, 0, 0);

    return EPDK_OK;
}

__hdle setup(__hdle cedar)
{
    __s32 ret;
    __s32 screen_width, screen_height;
    RECT vedio_rect;
    __hdle hvideo_lyr;

    display_get_size(&screen_width, &screen_height);
    vedio_rect.x = 0;
    vedio_rect.y = 0;
    vedio_rect.width = screen_width;
    vedio_rect.height = screen_height;
    printf("vedio screen=%d %d %d %d\n", vedio_rect.x, vedio_rect.y, vedio_rect.width, vedio_rect.height);

    hvideo_lyr = request_video_layer(cedar, &vedio_rect, 0, 0xff);
    if (hvideo_lyr)
    {
        printf("request_vedio_layer success...\n");
        return hvideo_lyr;
    }
    printf("request_vedio_layer fail...\n");
    return NULL;
}

void cmd_cedar_close(int argc, const char **argv)
{
	__u32 ret = 0;
    printf("cmd_cedar_close end,free mem size:%d\n",my_esMEMS_FreeMemSize());

#if 1
	esMODS_MIoctrl(robin_hce, CEDAR_CMD_STOP, 0, NULL);

	while(1)
	{
		ret = esMODS_MIoctrl(robin_hce, CEDAR_CMD_GET_STATUS, 0, NULL);

        if (ret == CEDAR_STAT_STOP)
        {
            break;
        }
		esKRNL_TimeDly(5);
	}
#endif
    esMODS_MClose(robin_hce);
    esMODS_MUninstall(mid_cedar);
 //   release_video_layer(video_layer);
    robin_hce = NULL;
    video_layer = NULL;
        printf("cmd_cedar_close end,free mem size:%d\n",my_esMEMS_FreeMemSize());

    return ;
}

#define  FilePath           "/mnt/F"
#define  AUDIO_DEV          "/dev/audio_play0"
#define  AUDIO_DEV_VOLUME   30

static __s32 do_play_file(__cedar_media_file_inf *param)
{
    __cedar_status_t status;

    mid_cedar = esMODS_MInstall("d:\\mod\\cedar.mod", 0);
    if (!mid_cedar)
    {
        printf("open cedar fail!\n");
        return -1;
    }
    robin_hce = esMODS_MOpen(mid_cedar, 0);

#if 1
    disp_open();

    video_layer = setup(robin_hce);
    if (!video_layer)
    {
        printf("set up layer failed!\n");
        return -1;
    }

    esMODS_MIoctrl(robin_hce, CEDAR_CMD_SET_VOL, AUDIO_DEV_VOLUME, NULL);
    esMODS_MIoctrl(robin_hce, CEDAR_CMD_SET_MEDIAFILE, 0, (void *)param);
    esMODS_MIoctrl(robin_hce, CEDAR_CMD_STOP, 0, NULL);
    esMODS_MIoctrl(robin_hce, CEDAR_CMD_PLAY, 0, NULL);
  #endif
    return 0;
}

static int cmd_cedar_test(int argc, const char **argv)
{
    __cedar_media_file_inf  file_info;


    printf("cmd_cedar_test start, free mem size:%d\n",my_esMEMS_FreeMemSize());
    if (argv[1] == NULL)
    {
        int i = 0;
        int filesize = 0;
        DIR *dir = NULL;
        struct dirent *entry;

        if((dir = opendir(FilePath))==NULL)
        {
            printf("opendir failed!");
            return -1;
        }
        else
        {
            while((entry=readdir(dir)))
            {
                i++;
                printf("filename%d = %s\n", i, entry->d_name);  //输出文件或者目录的名称
                printf("filetype%d = %d\n", i, entry->d_type);  //输出文件类型

                memset(&file_info, 0, sizeof(__cedar_media_file_inf));
                sprintf(file_info.file_path, "%s/%s", FilePath, entry->d_name);
                printf("filepath%d = %s\n", i, file_info.file_path);

                do_play_file(&file_info);
            }

            closedir(dir);
        }
    }
    else
    {
        printf("vplay filename = %s\n", argv[1]);

        memset(&file_info, 0, sizeof(__cedar_media_file_inf));
        strcpy(file_info.file_path, argv[1]);

        do_play_file(&file_info);
    }
    printf("cmd_cedar_test end, free mem size:%d\n",my_esMEMS_FreeMemSize());

    return 0;
}

#if 0
int doaplay(struct op *t, const char **args)
{
    __u8 mid_dis;
    __mp *hDrv;
    __u32 mid_cedar;
    __cedar_media_file_inf  file_info;
    ES_FILE *audio;
    __cedar_status_t status;
    __hdle video_layer;

    if (args[1] == NULL)
    {
        Esh_printf("file name can not be not!\n");
        return -1;
    }
    char *filename = Esh_get_name(args[1]);
    Esh_printf("aplay filename = %s", filename);

    esDEV_Plugin("\\drv\\audio.drv", 0, 0, 1);
    esKRNL_TimeDly(5);
    audio = eLIBs_fopen("b:\\AUDIO\\CTRL", "r+");
    if (!audio)
    {
        eLIBs_printf("open audio  ctrl failed!\n");
    }
    eLIBs_fioctrl(audio, AUDIO_DEV_CMD_SET_VOLUME, 60, (void *)AUDIO_DEV_PLAY);

    memset(&file_info, 0, sizeof(__cedar_media_file_inf));
    strcpy(file_info.file_path, filename);
    Esh_free_name(filename);
    filename = NULL;

    mid_cedar = esMODS_MInstall("d:\\mod\\cedar.mod", 0);
    if (!mid_cedar)
    {
        eLIBs_printf("open cedar fail!\n");
        return -1;
    }
    robin_hce = esMODS_MOpen(mid_cedar, 0);

    esMODS_MIoctrl(robin_hce, CEDAR_CMD_SET_MEDIAFILE, 0, (void *)&file_info);
    esMODS_MIoctrl(robin_hce, CEDAR_CMD_STOP, 0, NULL);
    esMODS_MIoctrl(robin_hce, CEDAR_CMD_PLAY, 0, NULL);

    __u8 ch;
    while (1)
    {
        ch = __getc();
        if (ch == 3 || ch == 'q')
        {
            Esh_printf("ch = %c, %d\n", ch, ch);
            esMODS_MIoctrl(robin_hce, CEDAR_CMD_STOP, 0, NULL);
            esMODS_MClose(robin_hce);
            eLIBs_fclose(audio);
            esMODS_MUninstall(mid_cedar);
            robin_hce = NULL;
            break;
        }
    }
    return 0;
}
#endif

FINSH_FUNCTION_EXPORT_ALIAS(cmd_cedar_test, __cmd_cedar_test, cedar_test);
FINSH_FUNCTION_EXPORT_ALIAS(cmd_cedar_close, __cmd_cedar_close, cedar_close);

