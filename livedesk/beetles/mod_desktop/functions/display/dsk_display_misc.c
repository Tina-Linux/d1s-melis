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
