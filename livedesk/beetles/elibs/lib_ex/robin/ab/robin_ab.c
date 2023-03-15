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
#ifndef  __robin_ab_c
#define  __robin_ab_c


#include "robin_i.h"
#include <kconfig.h>


/***************************************************************************************************
*Name        : robin_set_ab_a
*Prototype   : __s32  robin_set_ab_a( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set the current time point as the A point in AB play mode.
*Other       :
***************************************************************************************************/
__s32  robin_set_ab_a(void)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_AB_A, 0, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

__s32  robin_set_ab_a_v2(void)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_AB_A_V2, -1, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/***************************************************************************************************
*Name        : robin_set_ab_b
*Prototype   : __s32  robin_set_ab_b( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set the current time point as the B point in AB play mode and start AB play mode.
*Other       :
***************************************************************************************************/
__s32  robin_set_ab_b(void)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_AB_B, 0, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

__s32  robin_set_ab_b_v2(void)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_AB_B_V2, -1, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}


/***************************************************************************************************
*Name        : robin_set_ab_loop_count
*Prototype   : __s32  robin_set_ab_loop_count( __u32 count )
*Arguments   : count     input. AB loop count.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the loop count in AB play mode.
*Other       :
***************************************************************************************************/
__s32  robin_set_ab_loop_count(__u32 count)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_AB_LOOPCNT, count, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

__s32  robin_set_ab_loop_count_v2(__u32 count)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    __u32 ret;

    if ((ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_AB_LOOPCNT_V2, count, NULL)) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/***************************************************************************************************
*Name        : robin_cancle_ab
*Prototype   : __s32  robin_cancle_ab( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : cancle AB play.
*Other       :
***************************************************************************************************/
__s32  robin_cancle_ab(void)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_CLEAR_AB, 0, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

__s32  robin_cancle_ab_v2(void)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_CLEAR_AB_V2, 0, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/***************************************************************************************************
*Name        : robin_cancle_ab
*Prototype   : __s32  robin_cancle_ab( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : cancle AB play.
*Other       :
***************************************************************************************************/
__s32  robin_enable_ab(void)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    __u32 ret;

    if ((ret = esMODS_MIoctrl(robin_hced, CEDAR_CMD_ENABLE_AB_V2, 0, NULL)) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}



__s32  robin_set_audio_ab_mode(__u32 mode)
{
    /*-----------------===== to be refreshed =====-----------------------*/
    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_AUDIO_AB_MODE_V2, mode, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

#endif     //  ifndef __robin_ab_c
