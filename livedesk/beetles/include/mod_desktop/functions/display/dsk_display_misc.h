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
#ifndef  __DSK_DISPLAY_MISC_H__
#define  __DSK_DISPLAY_MISC_H__

#ifdef __cplusplus
extern "C" {
#endif


/*
****************************************************************************************************
*Name        : dsk_display_hdmi_audio_enable
*Prototype   : __s32 dsk_display_hdmi_audio_enable( __bool flag )
*Arguments   : flag       input. true or false.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : enable or disable the audio output through HDMI.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_display_hdmi_audio_enable(__bool flag);

/*
****************************************************************************************************
*Name        : dsk_display_get_size
*Prototype   : __s32 dsk_display_get_size(__s32 *p_width, __s32 *p_height)
*Arguments   : p_width     output. width
*              p_height    output. height
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : get the current screen size.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_display_get_size(__s32 *p_width, __s32 *p_height);

/*
****************************************************************************************************
*Name        : dsk_display_set_backcolor
*Prototype   : __s32 dsk_display_set_backcolor(__u32 de_color)
*Arguments   : de_color    input. color format is DE format "ARGB".
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : set the backcolor of the display system.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_display_set_backcolor(__u32 hlay, __u32 de_color);

/*
****************************************************************************************************
*Name        : dsk_display_get_backcolor
*Prototype   : __u32 dsk_display_get_backcolor(void)
*Arguments   : void
*Return      : the backcolor of the display system.
*Description : get the backcolor of the display system. color format is DE format "ARGB".
*Other       :
****************************************************************************************************
*/
extern __u32 dsk_display_get_backcolor(__u32 hlay);



#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_DISPLAY_MISC_H__

/* end of dsk_display_misc.h */
