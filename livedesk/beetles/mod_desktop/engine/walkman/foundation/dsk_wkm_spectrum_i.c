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
#ifndef  __DSK_WKM_SPECTRUM_I_C__
#define  __DSK_WKM_SPECTRUM_I_C__


#include "foundation_i.h"


/*
****************************************************************************************************
*Name        : __dsk_wkm_enable_spectrum
*Prototype   : __s32  __dsk_wkm_enable_spectrum( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : enable parsing spectrum
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_enable_spectrum(void)
{
    if (esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_SET_SPECTRUM, 1, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_disable_spectrum
*Prototype   : __s32  __dsk_wkm_disable_spectrum( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : disable parsing spectrum
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_disable_spectrum(void)
{
    if (esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_SET_SPECTRUM, 0, NULL) == EPDK_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_get_spectrum_info
*Prototype   : __s32  __dsk_wkm_get_spectrum_info( __u16 *value_list )
*Arguments   : value_list     output. a buffer to store instant spectrum info, whose item's count is
*                             SPECT_GROUP_WIDTH.
*Return      : ==0   succeed.
*              !=0   there is no spectrum info at this moment.
*Description : get instant spectrum info
*Other       :
****************************************************************************************************
*/
__s32 __dsk_wkm_get_spectrum_info(__u16 *value_list)
{
    __s32  ret;

    if (value_list == NULL)
    {
        return -1;
    }

    ret = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_SPECTRUM, 0, value_list);

    if (ret == EPDK_OK)
    {
        return 0;
    }
    else                                // no instant spectrum info
    {
        return -1;
    }
}



#endif     //  ifndef __DSK_WKM_SPECTRUM_I_C__

/* end of dsk_wkm_spectrum_i.c */
