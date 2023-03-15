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
#ifndef  __DSK_AUDIO_IF_C__
#define  __DSK_AUDIO_IF_C__


#include "functions_i.h"
#include <mod_hdmi.h>


/*
****************************************************************************************************
*Name        : dsk_set_hdmi_audio_gate
*Prototype   : __s32 dsk_set_hdmi_audio_gate( __bool flag )
*Arguments   : flag      input. on or off.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : enable or disable hdmi to output audio.
*Other       :
****************************************************************************************************
*/
__s32 dsk_set_hdmi_audio_gate(__bool flag)
{
    __wrn("need fix");
    return EPDK_FAIL;
#if 0 /*langaojie temp test*/

    ES_FILE       *p_hdmi;
    __s32          ret;

    /*open display driver handle*/
    p_hdmi = eLIBs_fopen("b:\\BUS\\HDMI", "r+");

    if (!p_hdmi)
    {
        __err("open hdmi device fail!\n");
        return EPDK_FAIL;
    }

    ret  = esMODS_MIoctrl(p_hdmi, HDMI_CMD_AUDIO_ENABLE, flag, 0);

    if (ret == EPDK_FAIL)
    {
        __wrn("fail in setting hdmi audio para.!\n");
        eLIBs_fclose(p_hdmi);
        return EPDK_FAIL;
    }

    eLIBs_fclose(p_hdmi);
    return EPDK_OK;
#endif
}



/*
****************************************************************************************************
*Name        : dsk_set_audio_if
*Prototype   : __s32 dsk_set_audio_if( __audio_dev_interface_t dev_if_type )
*Arguments   : dev_if_type   input. audio device interface type.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : set the audio device interface, such as iis, codac, ...
*Other       :
****************************************************************************************************
*/
__s32 dsk_set_audio_if(__audio_dev_interface_t dev_if_type)
{
    __wrn("need fix");
    return EPDK_FAIL;
#if 0 /*langaojie temp test*/

    ES_FILE       *p_audio;
    __s32          ret;

    /*open display driver handle*/
    p_audio = eLIBs_fopen("b:\\AUDIO\\CTRL", "r+");

    if (!p_audio)
    {
        __err("open dac device fail!\n");
        return EPDK_FAIL;
    }

    ret  = esMODS_MIoctrl(p_audio, AUDIO_DEV_CMD_CHANGE_IF, dev_if_type, 0);

    if (ret == EPDK_FAIL)
    {
        __wrn("switch audio mode %d fail!\n", dev_if_type);
        eLIBs_fclose(p_audio);
        return EPDK_FAIL;
    }

    __msg("switch audio mode %d succeed!\n", dev_if_type);
    eLIBs_fclose(p_audio);
    return EPDK_OK;
#endif
}


/*
****************************************************************************************************
*Name        : __audio_dev_interface_t
*Prototype   : __audio_dev_interface_t  dsk_get_audio_if( void )
*Arguments   : void
*Return      : the audio device interface type.
*Description : get the current audio device interface, such as iis, codac, ...
*Other       :
****************************************************************************************************
*/
__audio_dev_interface_t  dsk_get_audio_if(void)
{
    __wrn("need fix");
    return AUDIO_DEV_IF_CODEC;
#if 0 /*langaojie temp test*/

    ES_FILE       *p_audio;
    __audio_dev_interface_t ret;

    /*open display driver handle*/
    p_audio = eLIBs_fopen("b:\\AUDIO\\CTRL", "r+");

    if (!p_audio)
    {
        __err("open dac device fail!\n");
    }

    ret  = (__audio_dev_interface_t)esMODS_MIoctrl(p_audio, AUDIO_DEV_CMD_GET_INTERFACE, 0, 0);
    eLIBs_fclose(p_audio);
    return ret;
#endif
}



#endif     //  ifndef __DSK_AUDIO_IF_C__

/* end of dsk_audio_if.h */
