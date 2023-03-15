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
#include <log.h>
#include "mod_init_i.h"
#include "live_init.h"
#include <kconfig.h>

/**************************************************************************************************************/
#define LANG_DIR            BEETLES_APP_ROOT"apps\\lang.bin"
#define THEME_DIR           BEETLES_APP_ROOT"apps\\theme.bin"
#define AUDIO_DEV "/dev/audio_play0"
/**************************************************************************************************************/
GUI_FONT    *SWFFont    = NULL;    // 主界面字体
int32_t     LCD_WIDTH;
int32_t     LCD_HEIGHT;
/**************************************************************************************************************/

/**
 * 初始化 系统资源
 */
int32_t live_init(void)
{
    __msg("~~~~~~~~~~~~~~file system char set~~~~~~~~~~~");
    __msg("~~~~~~~~~~~~~~before dsk_set_fs_charset~~~~~~~~~~~");
    __log("~~~~~~~~~~~~~~before dsk_set_isn_charset~~~~~~~~~~~");
    __msg("~~~~~~~~~~~~~~before GUI_SFT_CreateFont~~~~~~~~~~~");

    SWFFont = GUI_SFT_CreateFont(BEETLES_FONT_SIZE_INT, BEETLES_APP_ROOT"res\\fonts\\font16.sft");

    if (!SWFFont)
    {
        __log("GUI_SFT_CreateFont fail, set default font....");
        SWFFont = GUI_GetDefaultFont();
    }

    //initialize walkman/keytone
    //dsk_wkm_init(DSK_WKM_MODE_AUDIO_MIN);
	
    __msg("~~~~~~~~~~~~~~before dsk_keytone_init~~~~~~~~~~~");
    //dsk_keytone_init(BEETLES_APP_ROOT"res\\sounds\\chord.wav");
    //dsk_keytone_init("E:\\apps\\chord.wav");
	
    //initialize system settings
    //dsk_gamma_on();
    //dsk_display_enhance_on();
    //initialize lang/theme
    __msg("~~~~~~~~~~~~~~before dsk_langres_init..s  ~~~~~~~~~~~");

    dsk_langres_init(LANG_DIR);
    __msg("~~~~~~~~~~~~~~before dsk_theme_init~~~~~~~~~~~");
    dsk_theme_init(THEME_DIR);
    open("/dev/audio_play0" ,O_RDWR);

    //dsk_orchid_load_last_npl();
    {
        reg_system_para_t   *para = NULL;

        __inf("~~~~~~~~~~~~~~before dsk_reg_get_para_by_app~~~~~~~~~~~");
        para    = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

        if (para != NULL)
        {
            //dsk_set_gamma(lion_reg_gamma_get());
            dsk_display_set_lcd_brightness(para->backlight);
            __msg("para->backlight=%d", para->backlight);
            //dsk_volume_set(para->volume);
            __msg("para->volume=%d", para->volume);
            //g_set_close_scn_time(para->backlightoff);
            //__msg("para->backlightoff=%d", para->backlightoff);
            //dsk_set_auto_off_time(para->poweroff);
            //__msg("para->poweroff=%d", para->poweroff);
            dsk_langres_set_type(para->language);
            __msg("para->language=%x", para->language);
            para->output = LION_DISP_LCD;

            if (EPDK_TRUE == para->keytone)
            {            
                dsk_keytone_set_state(SET_KEYTONE_ON);
            }
            else
            {
               // dsk_keytone_set_state(SET_KEYTONE_OFF);
            }
        }
        else
        {
            __msg("~~~~~~~~~~~~~~before dsk_display_set_lcd_bright~~~~~~~~~~~");
            dsk_display_set_lcd_brightness(LION_BRIGHT_LEVEL12);
            __msg("~~~~~~~~~~~~~~before dsk_volume_set~~~~~~~~~~~");
            dsk_volume_set(20);
            //dsk_set_auto_off_time(0);
            //g_set_close_scn_time(0);
            __msg("~~~~~~~~~~~~~~before dsk_langres_set_type~~~~~~~~~~~");
            dsk_langres_set_type(EPDK_LANGUAGE_ENM_CHINESES);
            __msg("~~~~~~~~~~~~~~before dsk_keytone_set_state~~~~~~~~~~~");
            dsk_keytone_set_state(SET_KEYTONE_OFF);
        }
    }
    return EPDK_OK;
}


/**
 * 释放 系统资源
 */
int32_t live_exit(void)
{
    /* 释放字体 */
    GUI_SetFont(GUI_GetDefaultFont());

    if (SWFFont && (GUI_GetDefaultFont() != SWFFont))
    {
        GUI_SFT_ReleaseFont(SWFFont);
        SWFFont = NULL;
    }

    //save system settings
    dsk_reg_flush();
    dsk_reg_deinit_para();
    //exit walkman/keytone
    dsk_keytone_exit();
    dsk_wkm_exit();
    //dsk_orchid_save_npl();
    //释放语言图片资源
    dsk_theme_exit();
    dsk_langres_exit();
    return EPDK_OK;
}

void init_screen_size(void)
{
    dsk_display_get_size(&LCD_WIDTH, &LCD_HEIGHT);
}
