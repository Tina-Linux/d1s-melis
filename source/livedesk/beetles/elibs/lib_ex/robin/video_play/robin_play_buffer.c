/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : __robin_play_buffer.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.25
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
*      2016.10.19       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_play_buffer_c
#define  __robin_play_buffer_c

#include "robin_i.h"

#define __here__            eLIBs_printf("@L%d(%s)\n", __LINE__, __FILE__);

#if 0

__s32  robin_start_play_streem_from_buffer(void)
{
    __msg("robin_hced=0x%x\n", robin_hced);
    __msg("robin_start_play_streem_from_buffer\n");
    return esMODS_MIoctrl(robin_hced, CEDAR_CMD_PLAY_STREAM_FROM_BUFFER, 0, NULL);
}
__s32  robin_input_stream_from_buffer(__cedar_play_stream_buffer_inf *para)
{
    __msg("robin_hced=0x%x\n", robin_hced);
    __msg("robin_input_stream_from_buffer\n");
    return esMODS_MIoctrl(robin_hced, CEDAR_CMD_INPUT_STREAM_FROM_BUFFER, 0, (void *)para);
}
#endif
#if 1//20161024
__s32  robin_set_mediafile(__cedar_media_file_inf *pbuffer)
{
    __msg("robin_set_mediafile\n");
    return esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_MEDIAFILE, 0, (void *)pbuffer);
}

__s32 robin_set_play(void)
{
    __msg("robin_set_play\n");
    return esMODS_MIoctrl(robin_hced, CEDAR_CMD_PLAY, 0, NULL);
}
__s32 robin_set_stop(void)
{
    __msg("robin_set_stop\n");
    return esMODS_MIoctrl(robin_hced, CEDAR_CMD_STREAM_END, 0, NULL);
}
__s32 robin_query_buffer(void)
{
    //__msg("robin_query_buffer\n");
    return esMODS_MIoctrl(robin_hced, CEDAR_CMD_STREAM_QUERY_BUFFER, CEDAR_STREAM_QUERY_BUF_SIZE_FREE, NULL);
}

__s32 robin_write_buffer(__u32 buffer_len, __u8 *buffer)
{
    //static __u32 cnt=0;
    //eLIBs_printf("[%d]robin_write_buffer=0x%x(%d)\n",cnt++,buffer_len,buffer_len);
    return esMODS_MIoctrl(robin_hced, CEDAR_CMD_STREAM_WRITE_BUFFER, buffer_len, (void *)buffer);
}
__s32 robin_resize_buffer(void)
{
    __msg("robin_hced=0x%x\n", robin_hced);
    __msg("robin_resize_buffer\n");
    return esMODS_MIoctrl(robin_hced, CEDAR_CMD_STREAM_RESIZE_BUFFER, 0, NULL);
}


__s32 robin_set_stream_info(__stream_inf_t *streaminf)
{
    __wrn("mediainfo :0x%x\n", streaminf);
    return esMODS_MIoctrl(robin_hced, CEDAR_CMD_STREAM_SET_INFO, 0, (void *)streaminf);
}
__s32 robin_set_stream_media_mode(__u8 tvmode)
{
    return esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_TV_MODE, tvmode, NULL);
}

void robin_set_avs_time(__u32 video_pts)
{
    esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_VIDEO_TIME, video_pts, NULL);
    return;
}

void robin_reset_sys_time(void)
{
    esMODS_MIoctrl(robin_hced, CEDAR_CMD_RESET_SYS_TIME, 0, NULL);
    return;
}

__s32 robin_set_rotate_mode(__u8 mode)
{
    return esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_ROTATE, mode, NULL);
}

__s32 robin_h264raw_play_start(void)
{
    return esMODS_MIoctrl(robin_hced, CEDAR_CMD_RAWH264_PLAY_START, 0, NULL);
}

__s32 robin_h264raw_play_stop(void)
{
    return esMODS_MIoctrl(robin_hced, CEDAR_CMD_RAWH264_PLAY_STOP, 0, NULL);
}

__s32 robin_h264raw_updata_stream(__u8 *frame_data, __u32 len)
{
    return esMODS_MIoctrl(robin_hced, CEDAR_CMD_RAWH264_SUBMIT_DATA, len, frame_data);
}
__s32 robin_h264raw_pic_show(__u8 show_flag)
{
    return esMODS_MIoctrl(robin_hced, CEDAR_CMD_RAWH264_PIC_SHOW_ONOFF, show_flag, NULL);
}



#endif
#endif     //  ifndef __robin_play_buffer_c

/* end of robin_play_buffer.c */
