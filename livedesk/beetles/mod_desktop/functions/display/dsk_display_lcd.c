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
#ifndef  __DSK_DISPLAY_LCD_C__
#define  __DSK_DISPLAY_LCD_C__


#include "dsk_display_i.h"
#include <emodules/mod_display.h>
#include <kconfig.h>


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
__s32 dsk_display_lcd_on(void)
{
    enum disp_output_type   output;
    __mp *mp_display        = NULL;  
    __u64 arg[3];

	mp_display = esKSRV_Get_Display_Hld();

    if (mp_display == NULL)
    {
        __err("Error in opening display driver.");
        goto error;
    }

    /* get output type and format */
    output = (enum disp_output_type)esMODS_MIoctrl(mp_display, MOD_DISP_GET_OUTPUT_TYPE, 0, 0);
    __msg("output===========%d ", output);

    if(output == DISP_OUTPUT_TYPE_NONE)
    {
        arg[0] = DISP_OUTPUT_TYPE_LCD;
        arg[1] = 0;
        esMODS_MIoctrl(mp_display, MOD_DISP_DEVICE_SWITCH, 0, (void *)arg);
    }

    /* close display driver */
    mp_display = NULL;
    return EPDK_OK;
error:

    mp_display = NULL;

    return EPDK_FAIL;
}
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
__s32 dsk_display_lcd_off(void)
{
    enum disp_output_type   output;
    __mp *mp_display        = NULL;
    __u64 arg[3];
	mp_display = esKSRV_Get_Display_Hld();

    if (mp_display == NULL)
    {
        __err("Error in opening display driver.");
        goto error;
    }

    /* get output type and format */
    output = (enum disp_output_type)esMODS_MIoctrl(mp_display, MOD_DISP_GET_OUTPUT_TYPE, 0, 0);
    __msg("output===========%d ", output);

    if (output == DISP_OUTPUT_TYPE_LCD)
    {
        arg[0] = DISP_OUTPUT_TYPE_NONE;
        arg[1] = 0;
        esMODS_MIoctrl(mp_display, MOD_DISP_DEVICE_SWITCH, 0, (void *)arg);  // note: 这句有延时
    }

    mp_display = NULL;
    return EPDK_OK;
error:

        mp_display = NULL;

    return EPDK_FAIL;
}
/*
****************************************************************************************************
*Name        : dsk_display_lcd_backlight_enable
*Prototype   : __s32 dsk_display_lcd_backlight_enable(bool enable)
*Arguments   : enable flag
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : open or close the backlight
*Other       :
****************************************************************************************************
*/
__s32 dsk_display_lcd_backlight_enable(bool enable)
{
    enum disp_output_type   output;
    __mp *mp_display        = NULL;
    __u64 arg[3];
	mp_display = esKSRV_Get_Display_Hld();

    if (mp_display == NULL)
    {
        __err("Error in opening display driver.");
        goto error;
    }

    /* get output type and format */
    output = (enum disp_output_type)esMODS_MIoctrl(mp_display, MOD_DISP_GET_OUTPUT_TYPE, 0, 0);
    __msg("output===========%d ", output);

    if (output == DISP_OUTPUT_TYPE_LCD)
    {
        __err("lcd bcaklight enable = %d",enable);
        if(1 == enable)
        {
            esMODS_MIoctrl(mp_display, MOD_DISP_LCD_BACKLIGHT_ENABLE, 0, 0);
        }
        else
        {
            esMODS_MIoctrl(mp_display, MOD_DISP_LCD_BACKLIGHT_DISABLE, 0, 0);
        }
    }

    mp_display = NULL;
    return EPDK_OK;
error:

        mp_display = NULL;

    return EPDK_FAIL;
}

/*
****************************************************************************************************
*Name        : dsk_display_set_lcd_bright
*Prototype   : __s32 dsk_display_set_lcd_bright(__LION_BRIGHT_t level)
*Arguments   : level       input. bright level.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : set the bright level of the LCD.
*Other       :
****************************************************************************************************
*/
void save_backlight_to_mixture(__u32 light)
{
    __hdle fh;

    fh = fopen("e:\\mixture.bin","wb");
    if(!fh)
    {
        __log("fh fail");
        return;
    }

    fwrite((void *)&light, 4, 1, fh);
    fclose(fh);
 }


__s32 dsk_display_set_lcd_brightness(__lion_bright_t level)
{
    enum disp_output_type   output;
    __disp_lcd_bright_t     lcd_bright;
    __u64                   arg[3];
	__mp                    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

    if (mp_display == NULL)
    {
        __err("Error in opening display driver.");
        goto error;
    }

    /* get output type and format */
    output = (enum disp_output_type)esMODS_MIoctrl(mp_display, MOD_DISP_GET_OUTPUT_TYPE, 0, 0);

    if (output == DISP_OUTPUT_TYPE_LCD)
    {
        switch (level)
        {
            case LION_BRIGHT_LEVEL1:
                lcd_bright = DISP_LCD_BRIGHT_LEVEL0;
                break;

            case LION_BRIGHT_LEVEL2:
                lcd_bright = DISP_LCD_BRIGHT_LEVEL1;
                break;

            case LION_BRIGHT_LEVEL3:
                lcd_bright = DISP_LCD_BRIGHT_LEVEL2;
                break;

            case LION_BRIGHT_LEVEL4:
                lcd_bright = DISP_LCD_BRIGHT_LEVEL3;
                break;

            case LION_BRIGHT_LEVEL5:
                lcd_bright = DISP_LCD_BRIGHT_LEVEL4;
                break;

            case LION_BRIGHT_LEVEL6:
                lcd_bright = DISP_LCD_BRIGHT_LEVEL5;
                break;

            case LION_BRIGHT_LEVEL7:
                lcd_bright = DISP_LCD_BRIGHT_LEVEL6;
                break;

            case LION_BRIGHT_LEVEL8:
                lcd_bright = DISP_LCD_BRIGHT_LEVEL7;
                break;

            case LION_BRIGHT_LEVEL9:
                lcd_bright = DISP_LCD_BRIGHT_LEVEL8;
                break;

            case LION_BRIGHT_LEVEL10:
                lcd_bright = DISP_LCD_BRIGHT_LEVEL9;
                break;

            case LION_BRIGHT_LEVEL11:
                lcd_bright = DISP_LCD_BRIGHT_LEVEL10;
                break;

            case LION_BRIGHT_LEVEL12:
                lcd_bright = DISP_LCD_BRIGHT_LEVEL11;
                break;

            case LION_BRIGHT_LEVEL13:
                lcd_bright = DISP_LCD_BRIGHT_LEVEL12;
                break;

            case LION_BRIGHT_LEVEL14:
                lcd_bright = DISP_LCD_BRIGHT_LEVEL13;
                break;

            case LION_BRIGHT_LEVEL15:
                lcd_bright = DISP_LCD_BRIGHT_LEVEL14;
                break;

            case LION_BRIGHT_LEVEL16:
                lcd_bright = DISP_LCD_BRIGHT_LEVEL15;
                break;

            default:
                lcd_bright = DISP_LCD_BRIGHT_LEVEL11;
                break;
        }

        arg[0]  = lcd_bright;
        arg[1]  = 0;
        arg[2]  = 0;
        __log("setting lcd_bright=%d", lcd_bright);
        esMODS_MIoctrl(mp_display, MOD_DISP_LCD_SET_BRIGHTNESS, 0, (void *)arg);
        save_backlight_to_mixture(lcd_bright);        
    }

    /* close display driver */
    return EPDK_OK;
error:

    return EPDK_FAIL;
}

/*
****************************************************************************************************
*Name        : dsk_display_get_lcd_bright
*Prototype   : __LION_BRIGHT_t dsk_display_get_lcd_bright( void )
*Arguments   : void
*Return      : the bright level of the LCD.
*Description : get the bright level of the LCD.
*Other       :
****************************************************************************************************
*/
__lion_bright_t dsk_display_get_lcd_brightness(void)
{
    enum disp_output_type   output;
	__mp *mp_display        = NULL;
    __lion_bright_t         level;
    __disp_lcd_bright_t     lcd_bright;

    /* open display driver */
	mp_display = esKSRV_Get_Display_Hld();

    if (mp_display == NULL)
    {
        __err("Error in opening display driver.");
        goto error;
    }

    /* get output type and format */
    level   = LION_BRIGHT_LEVEL1;
    output  = (enum disp_output_type)esMODS_MIoctrl(mp_display, MOD_DISP_GET_OUTPUT_TYPE, 0, 0);

    if (output == DISP_OUTPUT_TYPE_LCD)
    {
        lcd_bright = (__disp_lcd_bright_t)esMODS_MIoctrl(mp_display, MOD_DISP_LCD_GET_BRIGHTNESS, 0, 0);

        switch (lcd_bright)
        {
            case DISP_LCD_BRIGHT_LEVEL0:
                level = LION_BRIGHT_LEVEL1;
                break;

            case DISP_LCD_BRIGHT_LEVEL1:
                level = LION_BRIGHT_LEVEL2;
                break;

            case DISP_LCD_BRIGHT_LEVEL2:
                level = LION_BRIGHT_LEVEL3;
                break;

            case DISP_LCD_BRIGHT_LEVEL3:
                level = LION_BRIGHT_LEVEL4;
                break;

            case DISP_LCD_BRIGHT_LEVEL4:
                level = LION_BRIGHT_LEVEL5;
                break;

            case DISP_LCD_BRIGHT_LEVEL5:
                level = LION_BRIGHT_LEVEL6;
                break;

            case DISP_LCD_BRIGHT_LEVEL6:
                level = LION_BRIGHT_LEVEL7;
                break;

            case DISP_LCD_BRIGHT_LEVEL7:
                level = LION_BRIGHT_LEVEL8;
                break;

            case DISP_LCD_BRIGHT_LEVEL8:
                level = LION_BRIGHT_LEVEL9;
                break;

            case DISP_LCD_BRIGHT_LEVEL9:
                level = LION_BRIGHT_LEVEL10;
                break;

            case DISP_LCD_BRIGHT_LEVEL10:
                level = LION_BRIGHT_LEVEL11;
                break;

            case DISP_LCD_BRIGHT_LEVEL11:
                level = LION_BRIGHT_LEVEL12;
                break;

            case DISP_LCD_BRIGHT_LEVEL12:
                level = LION_BRIGHT_LEVEL13;
                break;

            case DISP_LCD_BRIGHT_LEVEL13:
                level = LION_BRIGHT_LEVEL14;
                break;

            case DISP_LCD_BRIGHT_LEVEL14:
                level = LION_BRIGHT_LEVEL15;
                break;

            case DISP_LCD_BRIGHT_LEVEL15:
                level = LION_BRIGHT_LEVEL16;
                break;

            default:
                level = LION_BRIGHT_;
                break;
        }
    }

    /* close display driver */
    return level;
error:
        mp_display = NULL;

    return LION_BRIGHT_;
}

__s32 dsk_display_set_lcd_bright(__s32 bright)
{
    __u64   buf[3];
    __s32   ret;
	__mp    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();
    if (!mp_display)
    {
        __log("Open display driver failed, test program exits.");
        return 0;
    }
    ret = esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_ENABLE, 0, (void *)0);
    buf[0] = 0;     // 关闭proc1 csc
    // 0-normal mode ( disable enhance) ; 1-enhance mode; 2-demo mode (enable half window)
    if (esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_SET_MODE, 0, (void *)buf) < 0)
    {
        __msg("DISP_ENHANCE_SET_MODE fail");
        return EPDK_FAIL;
    }
    else
    {
        __msg("DISP_ENHANCE_SET_MODE  ok");
    }

    buf[0] = bright;       // 驱动参数范围是0-255
    buf[1] = 0;
    buf[2] = 0;
    esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_SET_BRIGHT, 0, (void *)buf);

    mp_display = NULL;
    return ret;
}

__s32  dsk_display_get_lcd_bright(void)
{
    __u64   buf[3];
    __s32   ret = 0;
	__mp    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    ret = esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_GET_BRIGHT, 0, (void *)0);
    mp_display = NULL;
    return (ret);
}

__s32 dsk_display_set_lcd_constract(__s32  value)
{
    __u64   buf[3];
    __s32   ret;
	__mp    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    ret = esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_ENABLE, 0, (void *)0);

    buf[0] = 0;
    // 0-normal mode ( disable enhance) ; 1-enhance mode; 2-demo mode (enable half window)
    if (esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_SET_MODE, 0, (void *)buf) < 0)
    {
        __msg("DISP_ENHANCE_SET_MODE fail");
        return EPDK_FAIL;
    }
    else
    {
        __msg("DISP_ENHANCE_SET_MODE  ok");
    }

    buf[0] = value;
    buf[1] = 0;
    buf[2] = 0;
    esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_SET_CONTRAST, 0, (void *)buf);

    mp_display = NULL;
    return ret;
}

__s32 dsk_display_get_lcd_constract(void)
{
    __u64   buf[3];
    __s32   ret = 0;
	__mp    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    ret = esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_GET_CONTRAST, 0, (void *)0);
    __msg(" second read constract[0x%x][%d]", ret, ret);
    mp_display  = NULL;
    return (ret);
}

__s32 dsk_display_set_lcd_saturation(__s32  value)
{
    __u64   buf[3];
    __s32   ret = 0;
	__mp    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    ret = esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_ENABLE, 0, (void *)0);

    buf[0] = 0;
    // 0-normal mode ( disable enhance) ; 1-enhance mode; 2-demo mode (enable half window)
    if (esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_SET_MODE, 0, (void *)buf) < 0)
    {
        __msg("DISP_ENHANCE_SET_MODE fail");
        return EPDK_FAIL;
    }
    else
    {
        __msg("DISP_ENHANCE_SET_MODE  ok");
    }

    buf[0] = value;
    buf[1] = 0;
    buf[2] = 0;

    esMODS_MIoctrl(mp_display, DISP_ENHANCE_SET_SATURATION, 0, (void *)buf);

    mp_display = NULL;
    return ret;
}

__s32 dsk_display_get_lcd_saturation(void)
{
    __u64   buf[3];
    __s32   ret = 0;
	__mp    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    ret = esMODS_MIoctrl(mp_display, DISP_ENHANCE_SET_SATURATION, 0, (void *)0);

    mp_display = NULL;
    return (ret);
}

__s32 dsk_display_set_lcd_hue(__s32  value)
{
    #if 0
    __u64   buf[3];
    __s32   ret = 0;
	__mp    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    ret = esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_ENABLE, 0, (void *)0);

    buf[0] = 0;
    // 0-normal mode ( disable enhance) ; 1-enhance mode; 2-demo mode (enable half window)
    if (esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_SET_MODE, 0, (void *)buf) < 0)
    {
        __msg("DISP_ENHANCE_SET_MODE fail");
        return EPDK_FAIL;
    }
    else
    {
        __msg("DISP_ENHANCE_SET_MODE  ok");
    }

    buf[0] = value;
    buf[1] = 0;
    buf[2] = 0;
    //esMODS_MIoctrl(g_display_shell, DISP_CMD_SET_HUE, 0, (void *)buf);
    //__msg("write saturation[0x%x],[%d]", value, value);
    //ret = esMODS_MIoctrl(g_display_shell, DISP_CMD_GET_HUE, 0, (void *)0);
    //__msg(" second read saturation[0x%x][%d]", ret, ret);

    mp_display = NULL;
    return ret;
    #endif
    return 0;
}

__s32 dsk_display_get_lcd_hue(void)
{
    #if 0
    __u64   buf[3];
    __s32   ret = 0;
	__mp    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    //ret = esMODS_MIoctrl(g_display_shell, DISP_CMD_GET_HUE, 0, (void *)0);
    __msg(" second read saturation[0x%x][%d]", ret, ret);
    mp_display  = NULL;
    return (ret);
    #endif
    return 0;
}

__s32 dsk_display_set_lcd_edge(__s32  value)
{
    #if 0
    __u64   buf[3];
    __s32   ret = 0;
	__mp    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    ret = esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_ENABLE, 0, (void *)0);

    buf[0] = 0;
    // 0-normal mode ( disable enhance) ; 1-enhance mode; 2-demo mode (enable half window)
    if (esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_SET_MODE, 0, (void *)buf) < 0)
    {
        __msg("DISP_ENHANCE_SET_MODE fail");
        return EPDK_FAIL;
    }
    else
    {
        __msg("DISP_ENHANCE_SET_MODE  ok");
    }

    buf[0] = value;
    buf[1] = 0;
    buf[2] = 0;

    //esMODS_MIoctrl(g_display_shell, DISP_ENHANCE_SET_EDGE, 0, (void *)buf);
    //__msg("write saturation[0x%x],[%d]", value, value);
    //ret = esMODS_MIoctrl(g_display_shell, DISP_ENHANCE_GET_EDGE, 0, (void *)0);
    //__msg(" second read saturation[0x%x][%d]", ret, ret);

    mp_display = NULL;
    return ret;
    #endif
    return 0;
}

__s32 dsk_display_get_lcd_edge(void)
{
    #if 0
    __u64   buf[3];
    __s32   ret = 0;
	__mp    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    //ret = esMODS_MIoctrl(g_display_shell, DISP_ENHANCE_GET_EDGE, 0, (void *)0);
    __msg("second read saturation[0x%x][%d]", ret, ret);
    mp_display = NULL;
    return (ret);
    #endif
    return 0;
}

__s32 dsk_display_set_lcd_detail(__s32  value)
{
    #if 0
    __u64   buf[3];
    __s32   ret;
	__mp    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    ret = esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_ENABLE, 0, (void *)0);

    buf[0] = 0;
    // 0-normal mode ( disable enhance) ; 1-enhance mode; 2-demo mode (enable half window)
    if (esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_SET_MODE, 0, (void *)buf) < 0)
    {
        __msg("DISP_ENHANCE_SET_MODE fail");
        return EPDK_FAIL;
    }
    else
    {
        __msg("DISP_ENHANCE_SET_MODE  ok");
    }

    buf[0] = value;
    buf[1] = 0;
    buf[2] = 0;
    //esMODS_MIoctrl(g_display_shell, DISP_ENHANCE_SET_DETAIL, 0, (void *)buf);
    mp_display = NULL;
    return ret;
    #endif
    return 0;
}

__s32 dsk_display_get_lcd_detail(void)
{
    #if 0
    __u64   buf[3];
    __s32   ret;
	__mp    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    ret = esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_ENABLE, 0, (void *)0);

    buf[0] = 0;
    // 0-normal mode ( disable enhance) ; 1-enhance mode; 2-demo mode (enable half window)
    if (esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_SET_MODE, 0, (void *)buf) < 0)
    {
        __msg("DISP_ENHANCE_SET_MODE fail");
        return EPDK_FAIL;
    }
    else
    {
        __msg("DISP_ENHANCE_SET_MODE  ok");
    }

    //ret = esMODS_MIoctrl(g_display_shell, DISP_ENHANCE_GET_DETAIL, 0, (void *)0);
    mp_display = NULL;
    return (ret);
    #endif
    return 0;
}

__s32 dsk_display_set_lcd_denoise(__s32  value)
{
    #if 0
    __u64   buf[3];
    __s32   ret;
	__mp    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    ret = esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_ENABLE, 0, (void *)0);

    buf[0] = 0;
    // 0-normal mode ( disable enhance) ; 1-enhance mode; 2-demo mode (enable half window)
    if (esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_SET_MODE, 0, (void *)buf) < 0)
    {
        __msg("DISP_ENHANCE_SET_MODE fail");
        return EPDK_FAIL;
    }
    else
    {
        __msg("DISP_ENHANCE_SET_MODE  ok");
    }

    buf[0] = value;
    buf[1] = 0;
    buf[2] = 0;
    //esMODS_MIoctrl(g_display_shell, DISP_ENHANCE_SET_DENOISE, 0, (void *)buf);
    mp_display = NULL;
    return ret;
    #endif
    return 0;
}

__s32  dsk_display_get_lcd_denoise(void)
{
    #if 0
    __u64 buf[3];
    __s32 ret;
	__mp *mp_display = NULL;
	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    ret = esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_ENABLE, 0, (void *)0);

    buf[0] = 0;
    // 0-normal mode ( disable enhance) ; 1-enhance mode; 2-demo mode (enable half window)
    if (esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_SET_MODE, 0, (void *)buf) < 0)
    {
        __msg("DISP_ENHANCE_SET_MODE fail");
        return EPDK_FAIL;
    }
    else
    {
        __msg("DISP_ENHANCE_SET_MODE  ok");
    }

    //ret = esMODS_MIoctrl(g_display_shell, DISP_ENHANCE_GET_DENOISE, 0, (void *)0);
    __msg(" second read saturation[0x%x][%d]", ret, ret);
    mp_display = NULL;
    return (ret);
    #endif
    return 0;
}

__s32  dsk_display_enhance_enable(void)
{
    __u64   buf[3];
    __s32   ret;
	__mp    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    ret = esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_ENABLE, 0, (void *)0);

    buf[0] = 0;
    // 0-normal mode ( disable enhance) ; 1-enhance mode; 2-demo mode (enable half window)
    if (esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_SET_MODE, 0, (void *)buf) < 0)
    {
        __msg("DISP_ENHANCE_SET_MODE fail");
        return EPDK_FAIL;
    }
    else
    {
        __msg("DISP_ENHANCE_SET_MODE  ok");
    }

    mp_display = NULL;
    return ret;
}

__s32 dsk_display_enhance_disable(void)
{
    __u64   arg[3];
    __s32   ret;
	__mp    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    arg[0] = 0;
    // 0-normal mode ( disable enhance) ; 1-enhance mode; 2-demo mode (enable half window)
    if (esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_SET_MODE, 0, (void *)arg) < 0)
    {
        __msg("DISP_ENHANCE_SET_MODE fail");
        return EPDK_FAIL;
    }
    else
    {
        __msg("DISP_ENHANCE_SET_MODE  ok");
    }

    ret = esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_DISABLE, 0, (void *)0);
    mp_display = NULL;
    return ret;
}

__s32 dsk_display_get_enhance_enable(void)
{
    #if 0
    __u64   buf[3];
    __s32   ret = 0;
	__mp    *mp_display = NULL;
	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    //ret = esMODS_MIoctrl(g_display_shell, DISP_ENHANCE_GET_EN, 0, (void *)0);
    mp_display = NULL;
    return ret;
    #endif
    return 0;
}

__s32 dsk_display_enhance_demo_enable(void)
{
    __u64   buf[3];
    __s32   ret;
	__mp    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    ret = esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_DEMO_ENABLE, 0, (void *)0);
    mp_display = NULL;
    return ret;
}

__s32 dsk_display_enhance_demo_disable(void)
{
    __u64   buf[3];
    __s32   ret;
	__mp    *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __msg("Open display driver failed, test program exits.");
        return 0;
    }

    ret = esMODS_MIoctrl(mp_display, MOD_DISP_ENHANCE_DEMO_DISABLE, 0, (void *)0);
    mp_display = NULL;
    return ret;
}

#endif     //  ifndef __DSK_DISPLAY_LCD_C__

/* end of dsk_display_lcd.c */
