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
#ifndef  __DSK_DISPLAY_OUTPUT_H__
#define  __DSK_DISPLAY_OUTPUT_H__
#include <kconfig.h>
#ifdef __cplusplus
extern "C" {
#endif


/*
****************************************************************************************************
*Name        : dsk_display_get_format
*Prototype   : __lion_disp_format_e  dsk_display_get_format( void )
*Arguments   : void
*Return      : the current display format.
*Description : get the current display format.
*Other       :
****************************************************************************************************
*/
extern __lion_disp_format_e  dsk_display_get_format(void);

/*
****************************************************************************************************
*Name        : dsk_display_set_format
*Prototype   : __s32 dsk_display_set_format( __lion_disp_format_e type );
*Arguments   : type       input. display format.
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : switch the display format.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_display_set_format(__lion_disp_format_e type);

/*
****************************************************************************************************
*Name        : dsk_display_get_output_type
*Prototype   : __u32  dsk_display_get_output_type( void )
*Arguments   : void
*Return      : the current display output type: TV, HDMI, and so on.
*Description : get the current display output type.
*Other       :
****************************************************************************************************
*/
extern __u32  dsk_display_get_output_type(void);


/*
****************************************************************************************************
*Name        : dsk_display_dskfmt2epdkfmt
*Prototype   : __s32  dsk_display_dskfmt2epdkfmt( __lion_disp_format_e format, __disp_output_type_t *output_p )
*Arguments   : format   input. the output format defined in desktop.
*              output_p output. the output type defined in ePDK.
*Return      : the format corresponding to the output type defined in ePDK.
*Description : convert the output format defined in desktop to the output format defined in ePDK.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_display_dskfmt2epdkfmt(__lion_disp_format_e format, enum disp_output_type *output_p);

/*
****************************************************************************************************
*Name        : dsk_display_epdkfmt2dskfmt
*Prototype   : __lion_disp_format_e  dsk_display_epdkfmt2dskfmt( __disp_output_type_t output, __s32 format );
*Arguments   : output   input. the output type defined in ePDK.
*              format   input. the format corresponding to the output type defined in ePDK. if the
*                       output type is LCD, this parameter may be ignored.
*Return      :
*Description : convert the output format defined in ePDK to the output format defined in desktop.
*Other       :
****************************************************************************************************
*/
extern __lion_disp_format_e  dsk_display_epdkfmt2dskfmt(enum disp_output_type output, __s32 format);

/*
****************************************************************************************************
*Name        : dsk_display_on
*Prototype   : __s32  dsk_display_on( __disp_output_type_t output )
*Arguments   : output     input. the display output type.
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : open the display output.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_display_on(enum disp_output_type output);

/*
****************************************************************************************************
*Name        : dsk_display_off
*Prototype   : __s32  dsk_display_off( void )
*Arguments   : void
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : close the current display output.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_display_off(void);




#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_DISPLAY_OUTPUT_H__

/* end of dsk_display_output.h */
