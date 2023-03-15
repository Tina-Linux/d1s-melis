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
#ifndef  __DSK_DISPLAY_LCD_H__
#define  __DSK_DISPLAY_LCD_H__
#include <kconfig.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
****************************************************************************************************
*Name        : dsk_display_lcd_on
*Prototype   : __s32 dsk_display_lcd_on(void)
*Arguments   : void
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : open the LCD display.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_display_lcd_on(void);

/*
****************************************************************************************************
*Name        : dsk_display_lcd_off
*Prototype   : __s32 dsk_display_lcd_off(void)
*Arguments   : void
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : close the LCD display.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_display_lcd_off(void);

/*
****************************************************************************************************
*Name        : dsk_display_set_lcd_bright
*Prototype   : __s32 dsk_display_set_lcd_bright(__dsk_bright_t level)
*Arguments   : level       input. bright level.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : set the bright level of the LCD.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_display_set_lcd_bright(__s32 level);

/*
****************************************************************************************************
*Name        : dsk_display_get_lcd_bright
*Prototype   : __dsk_bright_t dsk_display_get_lcd_bright( void )
*Arguments   : void
*Return      : the bright level of the LCD.
*Description : get the bright level of the LCD.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_display_get_lcd_bright(void);


extern __s32  dsk_display_set_lcd_constract(__s32  value);
extern __s32  dsk_display_get_lcd_constract(void);
extern __s32  dsk_display_set_lcd_saturation(__s32  value);
extern __s32  dsk_display_get_lcd_saturation(void);
extern __s32  dsk_display_set_lcd_hue(__s32  value);
extern __s32  dsk_display_get_lcd_hue(void);

__s32  dsk_display_set_lcd_edge(__s32  value);
__s32  dsk_display_get_lcd_edge(void);
__s32  dsk_display_set_lcd_detail(__s32  value);
__s32  dsk_display_get_lcd_detail(void);
__s32  dsk_display_set_lcd_denoise(__s32  value);
__s32  dsk_display_get_lcd_denoise(void);
__s32  dsk_display_enhance_enable(void);
__s32  dsk_display_enhance_disable(void);
__s32  dsk_display_get_enhance_enable(void);
__s32  dsk_display_enhance_demo_enable(void);
__s32  dsk_display_enhance_demo_disable(void);
__s32 dsk_display_set_lcd_brightness(__lion_bright_t level);
__lion_bright_t dsk_display_get_lcd_brightness(void);

#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_DISPLAY_LCD_H__

/* end of dsk_display_lcd.h */
