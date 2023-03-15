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
