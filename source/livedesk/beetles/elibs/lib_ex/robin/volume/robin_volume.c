/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_volume.c
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
#ifndef  __robin_volume_c
#define  __robin_volume_c


#include "robin_i.h"



/***************************************************************************************************
*Name        : robin_set_volume
*Prototype   : __s32  robin_set_volume( __u32 volume )
*Arguments   : volume   input. volume value ranges AUDIO_DEVICE_VOLUME_MIN~~AUDIO_DEVICE_VOLUME_MAX,
*                       that are defined in drv_audio.h.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the volume.
*Other       :
***************************************************************************************************/
__s32  robin_set_volume(__u32 volume)
{
/*
    G_FILE  *fp = NULL;
    fp = g_fopen(AUDIO_DRV_FILE_PATH, "rb");

    if (fp == NULL)
    {
        __wrn("Fail in open dac device!\n");
        return -1;
    }

    g_fioctrl(fp, AUDIO_DEV_CMD_SET_VOLUME, volume, 0);
    g_fclose(fp);
    */
    __err("wait to deal..");
    return 0;
}



/***************************************************************************************************
*Name        : robin_get_volume
*Prototype   : __u32  robin_get_volume( void )
*Arguments   : void
*Return      : the current volume mode
*Description : get the current volume mode
*Other       :
***************************************************************************************************/
__u32  robin_get_volume(void)
{
    G_FILE  *fp = NULL;
    __s32    volume;
    /*
    fp = g_fopen(AUDIO_DRV_FILE_PATH, "rb");

    if (fp == NULL)
    {
        __wrn("Fail in open dac device!\n");
        return 0;
    }

    volume = g_fioctrl(fp, AUDIO_DEV_CMD_GET_VOLUME, 0, 0);
    g_fclose(fp);
    */
    __err("wait to deal!!");
    volume = 10;
    return volume;
}



#endif     //  ifndef __robin_volume_c

/* end of robin_volume.c */
