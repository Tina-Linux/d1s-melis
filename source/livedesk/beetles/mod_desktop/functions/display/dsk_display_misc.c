/*
************************************************************************************************************************
*                                                     display misc
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_display_misc.c
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.09.02
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2010.09.02        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_DISPLAY_MISC_C__
#define  __DSK_DISPLAY_MISC_C__


#include "dsk_display_i.h"
#include <mod_hdmi.h>
#include <mod_display.h>

/*
****************************************************************************************************
*Name        : dsk_display_hdmi_audio_enable
*Prototype   : __s32 dsk_display_hdmi_audio_enable( __bool flag )
*Arguments   : flag       input. true or false.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : enable or disable the audio output through HDMI.
*Other       :
****************************************************************************************************
*/
__s32 dsk_display_hdmi_audio_enable(__bool flag)
{
    __wrn("need_fix");
#if 0/*langaojie temp test*/
    ES_FILE       *p_hdmi;
    __s32          ret;
    /*open display driver handle*/
    p_hdmi = eLIBs_fopen("b:\\BUS\\HDMI", "r+");

    if (!p_hdmi)
    {
        __err("open hdmi device fail!");
        return EPDK_FAIL;
    }

    ret = esMODS_MIoctrl(p_hdmi, HDMI_CMD_AUDIO_ENABLE, flag, 0);

    if (ret == EPDK_FAIL)
    {
        __wrn("fail in setting hdmi audio para.!");
        eLIBs_fclose(p_hdmi);
        return EPDK_FAIL;
    }
    eLIBs_fclose(p_hdmi);
    
#endif
    return EPDK_OK;
}



/*
****************************************************************************************************
*Name        : dsk_display_get_size
*Prototype   : int32_t dsk_display_get_size(int32_t *p_width, int32_t *p_height)
*Arguments   : p_width     output. width
*              p_height    output. height
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : get the current screen size.
*Other       :
****************************************************************************************************
*/
int32_t dsk_display_get_size(int32_t *p_width, int32_t *p_height)
{
	__mp        *mp_display = NULL;    

    mp_display  = esKSRV_Get_Display_Hld();
	
    if (mp_display == NULL)
    {
        __err("Error in opening display driver.");
        return EPDK_FAIL;
    }

    *p_width    = esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_WIDTH,    0, 0);
    *p_height   = esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_HEIGHT,   0, 0);
	__inf("xxxxxxxxxx width :%d   height :%d",*p_width,*p_height);
    return EPDK_OK;
}



/*
****************************************************************************************************
*Name        : dsk_display_set_backcolor
*Prototype   : __s32 dsk_display_set_backcolor(__u32 de_color)
*Arguments   : de_color    input. color format is DE format "ARGB".
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : set the backcolor of the display system.
*Other       :
****************************************************************************************************
*/
__s32 dsk_display_set_backcolor(__u32 hlay, __u32 de_color)
{
    unsigned long arg[3];
    __mp *mp_display = NULL;    
	
    /* open display driver */
	mp_display = esKSRV_Get_Display_Hld();

    if (mp_display == NULL)
    {
        __err("Error in opening display driver.");
        return EPDK_FAIL;
    }

    arg[0] = hlay;
    arg[1] = de_color;
    arg[2] = 0;
    esMODS_MIoctrl(mp_display, MOD_DISP_SET_BKCOLOR, 0, (void *)arg);
    return EPDK_OK;
}



/*
****************************************************************************************************
*Name        : dsk_display_get_backcolor
*Prototype   : __u32 dsk_display_get_backcolor(void)
*Arguments   : void
*Return      : the backcolor of the display system.
*Description : get the backcolor of the display system. color format is DE format "ARGB".
*Other       :
****************************************************************************************************
*/
__u32 dsk_display_get_backcolor(__u32 hlay)
{
    __s32                 color;
    unsigned long arg[3];
    /* open display driver */
	__mp *mp_display = NULL;    	
	mp_display = esKSRV_Get_Display_Hld();

    if (mp_display == NULL)
    {
        __err("Error in opening display driver.");
        return 0;
    }

    arg[0] = hlay;
    arg[1] = 0;
    arg[2] = 0;
    color = esMODS_MIoctrl(mp_display, MOD_DISP_GET_BKCOLOR, 0, (void *)arg);
    return color;
}



#endif     //  ifndef __DSK_DISPLAY_MISC_C__

/* end of dsk_display_misc.c */
