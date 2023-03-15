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
#ifndef  __DSK_WKM_PLAY_MODE_C__
#define  __DSK_WKM_PLAY_MODE_C__


#include "dsk_walkman_i.h"


/*
****************************************************************************************************
*Name        : dsk_wkm_set_play_mode
*Prototype   : __s32  dsk_wkm_set_play_mode( orc_play_mode_e  play_mode )
*Arguments   : play_mode   input. play mode are defined in drv_orchid.h, see orc_play_mode_e.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the play mode.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_set_play_mode(orc_play_mode_e  play_mode)
{
    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return -1;
    }

    if (__dsk_wkm_npl_is_dirty())
    {
        __dsk_wkm_set_npl_dirty_flag(EPDK_TRUE);
        __dsk_wkm_npl_clear_dirty_flag();
    }

    return __dsk_wkm_set_play_mode(play_mode);
}



/*
****************************************************************************************************
*Name        : dsk_wkm_get_play_mode
*Prototype   : orc_play_mode_e  dsk_wkm_get_play_mode( void )
*Arguments   : void
*Return      : the current play mode
*Description : get the current play mode
*Other       :
****************************************************************************************************
*/
orc_play_mode_e  dsk_wkm_get_play_mode(void)
{
    if (__dsk_wkm_get_open_state() == EPDK_FALSE)
    {
        __wrn("Warning! the walkman is NOT open.\n");
        return ORC_PLAY_MODE_UNKNOWN;
    }

    if (__dsk_wkm_npl_is_dirty())
    {
        __dsk_wkm_npl_clear_dirty_flag();
    }

    return __dsk_wkm_get_play_mode();
}



#endif     //  ifndef __DSK_WKM_PLAY_MODE_C__

/* end of dsk_wkm_play_mode.c */
