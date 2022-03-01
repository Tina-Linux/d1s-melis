/*
************************************************************************************************************************
*                                                 desktop audio_if manage
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_audio_if.c
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.23
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
* Gary.Wang      2010.10.23        1.0         build the file
*
************************************************************************************************************************
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
