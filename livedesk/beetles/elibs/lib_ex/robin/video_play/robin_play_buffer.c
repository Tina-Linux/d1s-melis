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
