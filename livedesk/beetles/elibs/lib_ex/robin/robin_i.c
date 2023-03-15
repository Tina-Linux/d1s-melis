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
#ifndef  __robin_i_c
#define  __robin_i_c
#include "robin_i.h"
#include <log.h>



//G_FILE  *volatile  robin_horc       = NULL;
//HNPL     volatile  robin_hnpl       = NULL;
HRATNPL    volatile  rat_hnpl         = NULL;

__mp    *volatile  robin_hced       = NULL;
g_Queue  volatile  robin_cedar_msgQ = NULL;
__mp    *volatile  robin_hdis       = NULL;
__mp    *volatile  robin_hwlo       = NULL;


__bool   volatile  to_quit_robin    = EPDK_FALSE;


static __u8  volatile  mid_ced = 0;
static __u8  volatile  mid_wlo = 0;


/***************************************************************************************************
*Name        : orchid_open
*Prototype   : __s32 orchid_open( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : open orchid
*Other       :
***************************************************************************************************/
__s32 orchid_open(void)
{
    /*if( robin_horc == NULL )
    {
        robin_horc = g_fopen( ORCHID_FILE_PATH, "r+");
        if( robin_horc == NULL)
        {
            __err("Error in opening orchid!");
            return -1;
        }
        // check valid
        g_fioctrl( robin_horc, ORC_CMD_ORCHID_IS_VALID, 0, 0 );
    }
    */
    return 0;
}



/***************************************************************************************************
*Name        : orchid_close
*Prototype   : void  orchid_close( void )
*Arguments   : void
*Return      : void
*Description : close orchid
*Other       :
***************************************************************************************************/
void  orchid_close(void)
{
    /*if( robin_horc != NULL )
    {
        g_fclose( robin_horc );
        robin_horc = NULL;
    }
    */
}



/***************************************************************************************************
*Name        : npl_create
*Prototype   : __s32 npl_create( orc_media_type_e media_type, orc_npl_work_mode_e  work_mode )
*Arguments   : media_type     input. media type, such as audio, video and so on
*              work_mode      input. work mode.
*Return      : ==0    succeed
*              !=0    fail
*Description : create now playlist
*Other       :
***************************************************************************************************/
__s32 npl_create(rat_media_type_t  media_type)
{
    if (rat_hnpl == NULL)
    {
        rat_hnpl = rat_npl_open(media_type);

        if (rat_hnpl == NULL)
        {
            __wrn("rat npl open fail ....");
            return -1;
        }
    }
    else
    {
        __wrn("npl is not null ....");
    }

    /*if( robin_hnpl == NULL )
    {
        orc_arg_npl_t  arg;

        arg.media_type = media_type;
        arg.work_mode  = work_mode;
        robin_hnpl = (HNPL)g_fioctrl( robin_horc, ORC_CMD_CONSTRUCT_NOW_PLAYLIST, 0, (void *)&arg );
        if( robin_hnpl == NULL )
        {
            __err("Error in opening now play list!");
            return -1;
        }
    }
    */
    return 0;
}



/***************************************************************************************************
*Name        : npl_delete
*Prototype   : void  npl_delete( void )
*Arguments   : void
*Return      : void
*Description : delete now playlist
*Other       :
***************************************************************************************************/
void  npl_delete(void)
{
    if (rat_hnpl != NULL)
    {
        rat_npl_close(rat_hnpl);
        rat_hnpl = NULL;
    }

    /*if( robin_hnpl != NULL )
    {
        g_fioctrl( robin_horc, ORC_CMD_DESTROY_NOW_PLAYLIST, 0, (void *)robin_hnpl );
        robin_hnpl = NULL;
    }
    */
}

/***************************************************************************************************
*Arguments   : arg[0](video_type)   only use LBC on H.264/H.265 video type
               arg[1](frame_size)   video frame size, (frame_size>>16) is width, (frame_size&0xffff)
             is height
*Return      : lbc          contains two values, (lbc>>8) is lbc mode(0/1/2/3), mode==3 compress the most,
            (lbc&0xff) is lbc lossy(0/1), lossy==0 means the lossless LBC, cost the most memory(even
             bigger than LBC OFF). lossy==1 means the lossy LBC,cost the least memory(overall effects
             batter than scaledown). DEFAULT should be lossy==1.
*Description : User set LBC mode according to video resolution.
***************************************************************************************************/
static __s32 CB_UserDef_LBC(void *arg)
{
    __s32 lbc = 0;
    __u32 width, height;
    long *ubuffer = NULL;
    long video_type, frame_size;

    ubuffer    = (long *) arg;
    video_type = (long) ubuffer[0];
    frame_size = (long) ubuffer[1];

    if (video_type == CEDAR_VBS_TYPE_H264 || video_type == CEDAR_VBS_TYPE_H265)
    {
        width   = frame_size >> 16;
        height  = frame_size & 0xffff;

        if (width * height >= 1920 * 1080)
        {
            lbc = 0x0301;
        }
        else if (width * height >= 1280 * 720)
        {
            lbc = 0x0201;
        }
        else if (width * height >= 640 * 480)
        {
            lbc = 0x0101;
        }
        else
        {
			lbc = 0;    // width * height == 0 will enter here
        }
    }
    else
    {
        lbc = 0;
    }

    __wrn("LBC mode:%d, lossy:%d", (lbc >> 8), (lbc & 0xff));
    return lbc;
}

/***************************************************************************************************
*Arguments   : arg[0](video_type)   scaledown can be used on all video types
               arg[1](frame_size)   video frame size, (frame_size>>16) is width, (frame_size&0xffff)
            is height
*Return      : scaledown    scaledown(0/1/2/3),  scaledown==1, the width and height is original's 1/2.
            scaledown==2, means 1/4. scaledown==3, means 1/8.
*Description : User set scaledown mode according to video resolution.
***************************************************************************************************/
static __s32 CB_UserDef_Scaledown(void *arg)
{
    __u8 scaledown = 0;
    __u32 width, height;
    long *ubuffer = NULL;
    long video_type, frame_size;

    ubuffer    = (long *) arg;
    video_type = (long) ubuffer[0];
    frame_size = (long) ubuffer[1];
    width   = frame_size >> 16;
    height  = frame_size & 0xffff;

    if (video_type != CEDAR_VBS_TYPE_H264 && video_type != CEDAR_VBS_TYPE_H265)
    {
		if (width * height >= 2560 * 1920)
        {
            scaledown = 2;
        }
        else if (width * height >= 1920 * 1080)
        {
            scaledown = 1;
        }
        else
        {
            scaledown = 0;
        }
    }
    else
    {
        if (width * height == 0)
        {
            scaledown = 1;
        }
        else
        {
            scaledown = 0;
        }
    }

    __wrn("SCALEDOWN:%d", scaledown);
    return scaledown;
}

/***************************************************************************************************
*Name        : cedar_open
*Prototype   : __s32 cedar_open( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : open cedar
*Other       :
***************************************************************************************************/
__s32 cedar_open(robin_open_arg_t *arg_p)
{
    __u32 reserve_mem_size ;
    CedarStopMode cedar_stop_mode;
    CedarFileSwitchVplyMode switch_vplay_mode;
    __s32 ret;
    __pCBK_t call_back;
    __wrn(" cedar_open ");

    if (arg_p)
    {
        reserve_mem_size = arg_p->reserve_mem_size;
        cedar_stop_mode = arg_p->cedar_stop_mode;
        switch_vplay_mode = arg_p->switch_vplay_mode;
    }
    else
    {
        reserve_mem_size = 512 * 1024;
        cedar_stop_mode = CEDAR_STOP_MODE_UNINSTALL_PLUGINS;
        switch_vplay_mode = CEDAR_FILE_SWITCH_VPLY_MODE_GAP;
    }

    if (robin_hced == NULL)
    {
        /* load cedar */
        mid_ced = esMODS_MInstall(CEDAR_FILE_PATH, 0);

        if (mid_ced == 0)
        {
            __err("Error in loading cedar module!");
            return -1;
        }

        robin_hced = esMODS_MOpen(mid_ced, 0);

        if (robin_hced == NULL)
        {
            __wrn("Error in opening a handle of cedar!");
            esMODS_MUninstall(mid_ced);
            mid_ced = 0;
            return -1;
        }

        ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_RESERVED_MEM_SIZE, reserve_mem_size, NULL);

        if (ret == EPDK_FAIL)
        {
            __wrn("Error in set reserved memory %d Kbyte", reserve_mem_size / 1024);
            return -1;
        }
        else
        {
            __wrn("set reserved memory %d Kbyte", reserve_mem_size / 1024);
        }

        //设置停止时，是否卸载所有插件。
        esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_STOP_MODE, cedar_stop_mode, NULL);
        //设置切换下一首时，是否无缝切换
        esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_FILE_SWITCH_VPLY_MODE, switch_vplay_mode, NULL);
        call_back = esKRNL_GetCallBack((__pCBK_t)CB_UserDef_LBC);
        esMODS_MIoctrl(robin_hced, CEDAR_CMD_USER_SET_LBC_MODE, 0, (void *)call_back);
        call_back = esKRNL_GetCallBack((__pCBK_t)CB_UserDef_Scaledown);
        esMODS_MIoctrl(robin_hced, CEDAR_CMD_USER_SET_SCALEDOWN, 0, (void *)call_back);
        esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_AUDIO_DEV_INTERFACE, 0, NULL);
    }

    return 0;
}



/***************************************************************************************************
*Name        : cedar_close
*Prototype   : void  cedar_close( void )
*Arguments   : void
*Return      : void
*Description : close cedar
*Other       :
***************************************************************************************************/
void  cedar_close(void)
{
    syn_op_stop();

    if (robin_hced != NULL)
    {
        esMODS_MClose(robin_hced);
        robin_hced = NULL;
    }

    if (mid_ced != 0)
    {
        esMODS_MUninstall(mid_ced);
        mid_ced = 0;
    }
}



/***************************************************************************************************
*Name        : get_feedback_msgQ
*Prototype   : __s32  get_feedback_msgQ( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : get the feedback message queue handle.
*Other       :
***************************************************************************************************/
__s32  get_feedback_msgQ(void)
{
    robin_cedar_msgQ = (g_Queue) (unsigned long)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_MESSAGE_CHN, 0, NULL);

    if (robin_cedar_msgQ == NULL)
    {
        __err("Error in getting cedar error channel.");
        return -1;
    }

    return 0;
}



/***************************************************************************************************
*Name        : disp_open
*Prototype   : __s32 disp_open( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : open display driver
*Other       :
***************************************************************************************************/
__s32 disp_open(void)
{
    if (robin_hdis == NULL)
    {
        //robin_hdis = g_fopen(DISPLAY_DRV_FILE_PATH, "r+");
        robin_hdis = esKSRV_Get_Display_Hld();

        if (robin_hdis == NULL)
        {
            __err("Error in opening disp!");
            return -1;
        }
    }

    return 0;
}



/***************************************************************************************************
*Name        : disp_close
*Prototype   : void  disp_close( void )
*Arguments   : void
*Return      : void
*Description : close display driver
*Other       :
***************************************************************************************************/
void  disp_close(void)
{
    if (robin_hdis != NULL)
    {
        //g_fclose(robin_hdis);
        robin_hdis = NULL;
    }
}



/***************************************************************************************************
*Name        : willow_open
*Prototype   : __s32 willow_open( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : open willow
*Other       :
***************************************************************************************************/
__s32 willow_open(void)
{
    if (robin_hwlo == NULL)
    {
        /* load willow */
        mid_wlo = esMODS_MInstall(WILLOW_FILE_PATH, 0);

        if (mid_wlo == 0)
        {
            __wrn("Error in loading willow module!");
            return -1;
        }

        robin_hwlo = esMODS_MOpen(mid_wlo, 0);

        if (robin_hwlo == NULL)
        {
            __wrn("Error in opening a handle of willow!");
            esMODS_MUninstall(mid_wlo);
            mid_wlo = 0;
            return -1;
        }
    }

    return 0;
}



/***************************************************************************************************
*Name        : willow_close
*Prototype   : void  willow_close( void )
*Arguments   : void
*Return      : void
*Description : close willow
*Other       :
***************************************************************************************************/
void  willow_close(void)
{
    if (robin_hwlo != NULL)
    {
        esMODS_MClose(robin_hwlo);
        robin_hwlo = NULL;
    }

    if (mid_wlo != 0)
    {
        esMODS_MUninstall(mid_wlo);
        mid_wlo = 0;
    }
}



#endif     //  ifndef __robin_i_c

/* end of robin_i.c */
