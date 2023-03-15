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
