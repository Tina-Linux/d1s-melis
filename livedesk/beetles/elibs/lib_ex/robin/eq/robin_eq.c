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
#ifndef  __robin_eq_c
#define  __robin_eq_c


#include "robin_i.h"
#include <kconfig.h>


/***************************************************************************************************
*Name        : robin_set_eq_mode
*Prototype   : __s32  robin_set_eq_mode( __cedar_audio_eq_t  eq_mode )
*Arguments   : eq_mode    input. eq mode.
*Return      : ==0       succeed
*              !=0       fail
*Description : set eq mode, such Jazz, Rock and so on.
*Other       :
***************************************************************************************************/
__s32  robin_set_eq_mode(__cedar_audio_eq_t  eq_mode)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_EQ, eq_mode, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}



/***************************************************************************************************
*Name        : robin_get_eq_mode
*Prototype   : __cedar_audio_eq_t  robin_get_eq_mode( void )
*Arguments   : void
*Return      : the current eq mode.
*Description : get the current eq mode.
*Other       :
***************************************************************************************************/
__cedar_audio_eq_t  robin_get_eq_mode(void)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return CEDAR_AUD_EQ_TYPE_;
    }

    return (__cedar_audio_eq_t)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_EQ, 0, NULL);
}



/***************************************************************************************************
*Name        : robin_set_eq_value
*Prototype   : __s32 robin_set_eq_value( const __s8 *eq_value_list )
*Arguments   : eq_value_list   input. eq value list. the count of the items in the list should be
*                              USR_EQ_BAND_CNT.
*Return      : ==0   succeed.
*              !=0   fail.
*Description : set eq value
*Other       :
***************************************************************************************************/
__s32 robin_set_eq_value(const __s8 *eq_value_list)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }


    if (eq_value_list == NULL)
    {
        return -1;
    }

    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_SET_EQ, CEDAR_AUD_EQ_TYPE_USR_MODE, (void *)eq_value_list) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}



/***************************************************************************************************
*Name        : robin_get_eq_value
*Prototype   : __s32 robin_get_eq_value( __s8 *eq_value_list )
*Arguments   : eq_value_list   output. a buffer to store eq value. the count of the items in the list
*                              should be USR_EQ_BAND_CNT.
*Return      : ==0   succeed.
*              !=0   fail.
*Description : get eq value
*Other       :
***************************************************************************************************/
__s32 robin_get_eq_value(__s8 *eq_value_list)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    if (eq_value_list == NULL)
    {
        return -1;
    }

    /*---------------====== to be filled ======-----------------*/
    return 0;
}



/***************************************************************************************************
*Name        : robin_eq_mode2value
*Prototype   : __s32 robin_eq_mode2value( __cedar_audio_eq_t eq_mode, __s8 *eq_value_list )
*Arguments   : eq_mode         input.
*              eq_value_list   output. a buffer to store eq value. the count of the items in the list
*                              should be USR_EQ_BAND_CNT.
*Return      : ==0   succeed.
*              !=0   fail.
*Description : get the eq value list corresponding to a eq mode.
*Other       :
***************************************************************************************************/
__s32 robin_eq_mode2value(__cedar_audio_eq_t eq_mode, __s8 *eq_value_list)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    if (eq_value_list == NULL)
    {
        return -1;
    }

    /*---------------====== to be filled ======-----------------*/
    return 0;
}



#endif     //  ifndef __robin_eq_c

/* end of robin_eq.c */
