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
#ifndef  __DSK_WKM_EQ_C__
#define  __DSK_WKM_EQ_C__


#include "dsk_walkman_i.h"


/*
****************************************************************************************************
*Name        : dsk_wkm_set_eq_mode
*Prototype   : __s32  dsk_wkm_set_eq_mode( __cedar_audio_eq_t  eq_mode )
*Arguments   : eq_mode    input. eq mode.
*Return      : ==0       succeed
*              !=0       fail
*Description : set eq mode, such Jazz, Rock and so on.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_set_eq_mode(__cedar_audio_eq_t  eq_mode)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_set_eq_mode(eq_mode);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_get_eq_mode
*Prototype   : __cedar_audio_eq_t  dsk_wkm_get_eq_mode( void )
*Arguments   : void
*Return      : the current eq mode.
*Description : get the current eq mode.
*Other       :
****************************************************************************************************
*/
__cedar_audio_eq_t  dsk_wkm_get_eq_mode(void)
{
    __u8   err;
    __cedar_audio_eq_t  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return CEDAR_AUD_EQ_TYPE_;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_get_eq_mode();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_set_eq_value
*Prototype   : __s32 dsk_wkm_set_eq_value( const __s8 *eq_value_list )
*Arguments   : eq_value_list   input. eq value list. the count of the items in the list should be
*                              USR_EQ_BAND_CNT.
*Return      : ==0   succeed.
*              !=0   fail.
*Description : set eq value
*Other       :
****************************************************************************************************
*/
__s32 dsk_wkm_set_eq_value(const __s8 *eq_value_list)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_set_eq_value(eq_value_list);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_get_eq_value
*Prototype   : __s32 dsk_wkm_get_eq_value( __s8 *eq_value_list )
*Arguments   : eq_value_list   output. a buffer to store eq value. the count of the items in the list
*                              should be USR_EQ_BAND_CNT.
*Return      : ==0   succeed.
*              !=0   fail.
*Description : get eq value
*Other       :
****************************************************************************************************
*/
__s32 dsk_wkm_get_eq_value(__s8 *eq_value_list)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_get_eq_value(eq_value_list);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_eq_mode2value
*Prototype   : __s32 dsk_wkm_eq_mode2value( __cedar_audio_eq_t eq_mode, __s8 *eq_value_list )
*Arguments   : eq_mode         input.
*              eq_value_list   output. a buffer to store eq value. the count of the items in the list
*                              should be USR_EQ_BAND_CNT.
*Return      : ==0   succeed.
*              !=0   fail.
*Description : get the eq value list corresponding to a eq mode.
*Other       :
****************************************************************************************************
*/
__s32 dsk_wkm_eq_mode2value(__cedar_audio_eq_t eq_mode, __s8 *eq_value_list)
{
    __u8   err;
    __s32  ret;

    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_eq_mode2value(eq_mode, eq_value_list);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



#endif     //  ifndef __DSK_WKM_EQ_C__

/* end of dsk_wkm_eq.c */
