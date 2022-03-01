/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_channel.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.27
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
* Gary.Wang      2009.09.27       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_channel_c
#define  __robin_channel_c

#include "dfs_posix.h"
#include "robin_i.h"
#include <kconfig.h>




/***************************************************************************************************
*Name        : robin_set_channel
*Prototype   : __s32  robin_set_channel( __audio_dev_chan_mode_t  channel )
*Arguments   : channel   input. channel mode are defined in drv_audio.h, see __audio_dev_chan_mode_t.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the audio channel.
*Other       :
***************************************************************************************************/

__s32  robin_set_channel(__audio_dev_chan_mode_t      channel)
{
#ifdef  CONFIG_SOC_SUN20IW1P1

    int audio_fd  = -1;
    int ret = -1;
    unsigned long arg[1] ={0};
    
    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    audio_fd = open("/dev/audio_play0", O_RDWR);
    if (audio_fd < 0)
    {
        __err("Fail in open dac device!");
        return EPDK_FAIL;
    }

    arg[0] = channel;
    __msg("audio channel = %ld", channel);
    ret = (__s32)ioctl(audio_fd, AUDIO_DEV_CMD_SET_CHAN_MODE, (void*)arg);
    if (ret <0)
    {
        __err("Fail in set audio channel!");
        return EPDK_FAIL;
    }
    
    close(audio_fd);
    
#else
    __wrn("Not support channel setting!");
    return EPDK_FAIL;
    
#endif

    return EPDK_OK;
}



/***************************************************************************************************
*Name        : robin_get_channel
*Prototype   : __audio_dev_chan_mode_t  robin_get_channel( void )
*Arguments   : void
*Return      : the current channel mode
*Description : get the current channel mode
*Other       :
***************************************************************************************************/
__audio_dev_chan_mode_t  robin_get_channel(void)
{
#ifdef CONFIG_SOC_SUN20IW1P1
    
    if (EPDK_FALSE == robin_is_open())
    {
        return (__audio_dev_chan_mode_t)0;
        }
#else
    __wrn("Not support channel setting, using default!");
    return (__audio_dev_chan_mode_t)0;
    
#if 0
    G_FILE  *fp = NULL;
    __s32    channel;

    fp = g_fopen(AUDIO_DRV_FILE_PATH, "rb");

    if (fp == NULL)
    {
        __wrn("Fail in open dac device!");
        return (__audio_dev_chan_mode_t)0;
    }

    channel = g_fioctrl(fp, AUDIO_DEV_CMD_GET_CHAN_MODE, 0, 0);
    __wrn("current channel is %d.", channel);
    g_fclose(fp);
    return (__audio_dev_chan_mode_t)channel;
        
#endif
    
#endif
}



#endif     //  ifndef __robin_channel_c

/* end of robin_channel.c */
