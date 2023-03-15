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
#ifndef  __robin_disp_output_c
#define  __robin_disp_output_c

#include <emodules/mod_display.h>
#include "robin_i.h"


/***************************************************************************************************
*Name        : robin_get_screen_size
*Prototype   : __s32  robin_get_screen_size( __u32 *width_p, __u32 *height_p )
*Arguments   : width_p   output. width.
*              height_p  output. height.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the width and height of the screen.
*Other       :
***************************************************************************************************/
__s32  robin_get_screen_size(__u32 *width_p, __u32 *height_p)
{
    if (width_p == NULL || height_p == NULL)
    {
        return -1;
    }

    *width_p  = g_fioctrl(robin_hdis, MOD_DISP_GET_SCN_WIDTH, 0, 0);
    *height_p = g_fioctrl(robin_hdis, MOD_DISP_GET_SCN_HEIGHT, 0, 0);
    return 0;
}



/***************************************************************************************************
*Name        : robin_get_disp_output_type
*Prototype   : __disp_output_type_t  robin_get_disp_output_type( void )
*Arguments   : void
*Return      : the current display output type, such as tv, lcd and so on.
*Description : get the current display output type.
*Other       :
***************************************************************************************************/
enum disp_output_type  robin_get_disp_output_type(void)
{
    return (enum disp_output_type)g_fioctrl(robin_hdis, MOD_DISP_GET_OUTPUT_TYPE, 0, 0);
}



#endif     //  ifndef __robin_disp_output_c

/* end of robin_disp_output.c */
