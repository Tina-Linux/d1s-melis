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
#include "headbar_uipara.h"

#define MAX_SIGNAL_LEVEL            6

__headbar_uipara_t  headbar_uipara;
GUI_FONT            *FONT16 = NULL;

static void shbar_palette_adjust(void)
{
    ES_FILE     *p_disp;
    __u32       *palette_buf;
    HTHEME      htheme;
#if 0 // not support anymore    
    htheme      = dsk_theme_open(0);
    palette_buf = dsk_theme_hdl2buf(htheme);

    /*define palette's selfdefine area*/
    *(palette_buf + 0xC0) = 0x00000000;
    *(palette_buf + 0xC1) = 0xC0323232;
    p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

    if (!p_disp)
    {
        __wrn("open display fail\n");
        return ;
    }

    /* get palette */
    {
        __u32   arg[3];
        arg[0] = (__u32)palette_buf;
        arg[1] = 0;
        arg[2] = ROBIN_PALETTE_COLOR_COUNT * 4;
        esMODS_MIoctrl(p_disp, DISP_CMD_SET_PALETTE_TBL, 0, (void *)arg);
    }
    eLIBs_fclose(p_disp);
    dsk_theme_close(htheme);
#else
    __err("not support anymore  in  d100");
#endif
}

void headbar_init_uipara(__hbar_format_t format)
{
    int32_t     width;
    int32_t     height;
    int32_t     screen_type;
    uint8_t     i;

    dsk_display_get_size(&width, &height);

    if ((width == 400) && (height == 240))
    {
        screen_type = SCREEN_TYPE_400_240;
    }
    else if ((width == 480) && (height == 272))
    {
        screen_type = SCREEN_TYPE_480_272;
    }
    else if ((width == 480) && (height == 640))
    {
        screen_type = SCREEN_TYPE_480_640;
    }
	else if ((width == 720) && (height == 480))
    {
        screen_type = SCREEN_TYPE_720_480;
    }
    else if ((width == 720) && (height == 576))
    {
        screen_type = SCREEN_TYPE_720_576;
    }
    else if ((width == 800) && (height == 480))
    {
        screen_type = SCREEN_TYPE_800_480;
    }
    else if ((width == 1024) && (height == 768))
    {
        screen_type = SCREEN_TYPE_1024_768;
    }
    else if ((width == 1280) && (height == 720))
    {
        screen_type = SCREEN_TYPE_1280_720;
    }
    else if ((width == 1920) && (height == 1080))
    {
        screen_type = SCREEN_TYPE_1920_1080;
    }
    else
    {
        screen_type = SCREEN_TYPE_LAST;
    }

    switch (screen_type)
    {
        case SCREEN_TYPE_400_240:
        {
            //layer
            headbar_uipara.hbar_lyr.fb_w  = 480;
            headbar_uipara.hbar_lyr.fb_h  = 24;
            headbar_uipara.hbar_lyr.src_x = 0;
            headbar_uipara.hbar_lyr.src_y = 0;
            headbar_uipara.hbar_lyr.src_w = 480;
            headbar_uipara.hbar_lyr.src_h = 24;
            headbar_uipara.hbar_lyr.scn_x = 0;
            headbar_uipara.hbar_lyr.scn_y = 0;
            headbar_uipara.hbar_lyr.scn_w = 480;
            headbar_uipara.hbar_lyr.scn_h = 24;
            headbar_uipara.hbar_lyr.pipe  = 1;
            //
            headbar_uipara.hbar_frmwin.x = 0;
            headbar_uipara.hbar_frmwin.y = 0;
            headbar_uipara.hbar_frmwin.w = 480;
            headbar_uipara.hbar_frmwin.h = 24;
            headbar_uipara.title.x  = 20;
            headbar_uipara.title.y  = 0;
            headbar_uipara.title.w  = 100;
            headbar_uipara.title.h  = 24;
            headbar_uipara.music.x = 100 + 20;
            headbar_uipara.music.y = 2;
            headbar_uipara.music.w = 22;
            headbar_uipara.music.h = 19;
            headbar_uipara.date.x = 0;
            headbar_uipara.date.y = 0;
            headbar_uipara.date.w = 0;
            headbar_uipara.date.h = 0;
            headbar_uipara.time.x = 213;
            headbar_uipara.time.y = 0;
            headbar_uipara.time.w = 134;
            headbar_uipara.time.h = 24;
            headbar_uipara.singal_icon.x    = 300;
            headbar_uipara.singal_icon.y    = 0;
            headbar_uipara.singal_icon.w    = 46;
            headbar_uipara.singal_icon.h    = 46;
            headbar_uipara.singal_qa.x  = 350;
            headbar_uipara.singal_qa.y  = 0;
            headbar_uipara.singal_qa.w  = 46;
            headbar_uipara.singal_qa.h  = 46;
            headbar_uipara.volume_icon.x    = 357;
            headbar_uipara.volume_icon.y    = 2;
            headbar_uipara.volume_icon.w    = 22;
            headbar_uipara.volume_icon.h    = 19;
            headbar_uipara.volume_txt.x     = headbar_uipara.volume_icon.x + headbar_uipara.volume_icon.w + 2;
            headbar_uipara.volume_txt.y     = 0;
            headbar_uipara.volume_txt.w     = 16;
            headbar_uipara.volume_txt.h     = 24;
            headbar_uipara.bright_icon.x    = 159;
            headbar_uipara.bright_icon.y    = 2;
            headbar_uipara.bright_icon.w    = 22;
            headbar_uipara.bright_icon.h    = 19;
            headbar_uipara.bright_txt.x     = headbar_uipara.bright_icon.x + headbar_uipara.bright_icon.w + 1;
            headbar_uipara.bright_txt.y     = 0;
            headbar_uipara.bright_txt.w     = 16;
            headbar_uipara.bright_txt.h     = 24;
            headbar_uipara.battery.x        = 406;
            headbar_uipara.battery.y        = 2;
            headbar_uipara.battery.w        = 32;
            headbar_uipara.battery.h        = 18;
            headbar_uipara.home.x           = 446;
            headbar_uipara.home.y           = 2;
            headbar_uipara.home.w           = 22;
            headbar_uipara.home.h           = 19;
            headbar_uipara.res_id.vol_id0   = ID_INIT_ENERGY_0_BMP;
            headbar_uipara.res_id.vol_id1   = ID_INIT_ENERGY_1_BMP;
            headbar_uipara.res_id.vol_id2   = ID_INIT_ENERGY_2_BMP;
            headbar_uipara.res_id.vol_id3   = ID_INIT_ENERGY_3_BMP;
            headbar_uipara.res_id.vol_id4   = ID_INIT_ENERGY_4_BMP;
            headbar_uipara.res_id.vol_id5   = ID_INIT_ENERGY_5_BMP;
            headbar_uipara.res_id.charge_vol_id0    = ID_INIT_CHARGE_ENERGY_0_BMP;
            headbar_uipara.res_id.charge_vol_id1    = ID_INIT_CHARGE_ENERGY_1_BMP;
            headbar_uipara.res_id.charge_vol_id2    = ID_INIT_CHARGE_ENERGY_2_BMP;
            headbar_uipara.res_id.charge_vol_id3    = ID_INIT_CHARGE_ENERGY_3_BMP;
            headbar_uipara.res_id.charge_vol_id4    = ID_INIT_CHARGE_ENERGY_4_BMP;
            headbar_uipara.res_id.charge_vol_id5    = ID_INIT_CHARGE_ENERGY_5_BMP;
            headbar_uipara.res_id.no_vol_id         = 0;
            headbar_uipara.res_id.usb_connect_vol_id = 0;
            headbar_uipara.res_id.music_id  = ID_INIT_MUSIC_BMP;
            headbar_uipara.res_id.aux_id    = ID_INIT_LINEIN_BMP;
            headbar_uipara.res_id.pause_id  = ID_INIT_PAUSE_BMP;
            headbar_uipara.res_id.bright_id = ID_INIT_BRIGHT_BMP;
            headbar_uipara.res_id.volume_id = ID_INIT_VOLUME_BMP;
            headbar_uipara.res_id.home_id   = ID_INIT_HOME_BMP;
            headbar_uipara.res_id.bg_id            = ID_INIT_HBAR_BG_BMP;

            for (i = 0; i < MAX_SIGNAL_LEVEL; i++)
            {
                headbar_uipara.res_id.sig_st_id[i] = ID_DTV_ANTENNA_0_BMP + i;
            }

            headbar_uipara.font             = FONT16;
            break;
        }

        case SCREEN_TYPE_480_272:
        {
            //layer
            headbar_uipara.hbar_lyr.fb_w  = 480;
            headbar_uipara.hbar_lyr.fb_h  = 24;
            headbar_uipara.hbar_lyr.src_x = 0;
            headbar_uipara.hbar_lyr.src_y = 0;
            headbar_uipara.hbar_lyr.src_w = 480;
            headbar_uipara.hbar_lyr.src_h = 24;
            headbar_uipara.hbar_lyr.scn_x = 0;
            headbar_uipara.hbar_lyr.scn_y = 0;
            headbar_uipara.hbar_lyr.scn_w = 480;
            headbar_uipara.hbar_lyr.scn_h = 24;
            headbar_uipara.hbar_lyr.pipe  = 1;
            //
            headbar_uipara.hbar_frmwin.x = 0;
            headbar_uipara.hbar_frmwin.y = 0;
            headbar_uipara.hbar_frmwin.w = 480;
            headbar_uipara.hbar_frmwin.h = 24;
            headbar_uipara.title.x  = 10;
            headbar_uipara.title.y  = 0;
            headbar_uipara.title.w  = 80;
            headbar_uipara.title.h  = 24;
            headbar_uipara.music.x = 96;
            headbar_uipara.music.y = 2;
            headbar_uipara.music.w = 22;
            headbar_uipara.music.h = 19;
            headbar_uipara.date.x = 0;
            headbar_uipara.date.y = 0;
            headbar_uipara.date.w = 0;
            headbar_uipara.date.h = 0;
            headbar_uipara.time.x = 193;
            headbar_uipara.time.y = 0;
            headbar_uipara.time.w = 154;
            headbar_uipara.time.h = 24;
            headbar_uipara.singal_icon.x    = 300;
            headbar_uipara.singal_icon.y    = 0;
            headbar_uipara.singal_icon.w    = 46;
            headbar_uipara.singal_icon.h    = 46;
            headbar_uipara.singal_qa.x  = 350;
            headbar_uipara.singal_qa.y  = 0;
            headbar_uipara.singal_qa.w  = 46;
            headbar_uipara.singal_qa.h  = 46;
            headbar_uipara.volume_icon.x    = 357;
            headbar_uipara.volume_icon.y    = 2;
            headbar_uipara.volume_icon.w    = 22;
            headbar_uipara.volume_icon.h    = 19;
            headbar_uipara.volume_txt.x     = headbar_uipara.volume_icon.x + headbar_uipara.volume_icon.w + 2;
            headbar_uipara.volume_txt.y     = 0;
            headbar_uipara.volume_txt.w     = 16;
            headbar_uipara.volume_txt.h     = 24;
            headbar_uipara.bright_icon.x    = 139;
            headbar_uipara.bright_icon.y    = 2;
            headbar_uipara.bright_icon.w    = 22;
            headbar_uipara.bright_icon.h    = 19;
            headbar_uipara.bright_txt.x     = headbar_uipara.bright_icon.x + headbar_uipara.bright_icon.w + 1;
            headbar_uipara.bright_txt.y     = 0;
            headbar_uipara.bright_txt.w     = 16;
            headbar_uipara.bright_txt.h     = 24;
            headbar_uipara.battery.x        = 406;
            headbar_uipara.battery.y        = 2;
            headbar_uipara.battery.w        = 32;
            headbar_uipara.battery.h        = 18;
            headbar_uipara.home.x           = 446;
            headbar_uipara.home.y           = 2;
            headbar_uipara.home.w           = 22;
            headbar_uipara.home.h           = 19;
            headbar_uipara.res_id.vol_id0   = ID_INIT_ENERGY_0_BMP;
            headbar_uipara.res_id.vol_id1   = ID_INIT_ENERGY_1_BMP;
            headbar_uipara.res_id.vol_id2   = ID_INIT_ENERGY_2_BMP;
            headbar_uipara.res_id.vol_id3   = ID_INIT_ENERGY_3_BMP;
            headbar_uipara.res_id.vol_id4   = ID_INIT_ENERGY_4_BMP;
            headbar_uipara.res_id.vol_id5   = ID_INIT_ENERGY_5_BMP;
            headbar_uipara.res_id.charge_vol_id0    = ID_INIT_CHARGE_ENERGY_0_BMP;
            headbar_uipara.res_id.charge_vol_id1    = ID_INIT_CHARGE_ENERGY_1_BMP;
            headbar_uipara.res_id.charge_vol_id2    = ID_INIT_CHARGE_ENERGY_2_BMP;
            headbar_uipara.res_id.charge_vol_id3    = ID_INIT_CHARGE_ENERGY_3_BMP;
            headbar_uipara.res_id.charge_vol_id4    = ID_INIT_CHARGE_ENERGY_4_BMP;
            headbar_uipara.res_id.charge_vol_id5    = ID_INIT_CHARGE_ENERGY_5_BMP;
            headbar_uipara.res_id.no_vol_id         = 0;
            headbar_uipara.res_id.usb_connect_vol_id = 0;
            headbar_uipara.res_id.music_id  = ID_INIT_MUSIC_BMP;
            headbar_uipara.res_id.aux_id    = ID_INIT_LINEIN_BMP;
            headbar_uipara.res_id.pause_id  = ID_INIT_PAUSE_BMP;
            headbar_uipara.res_id.bright_id = ID_INIT_BRIGHT_BMP;
            headbar_uipara.res_id.volume_id = ID_INIT_VOLUME_BMP;
            headbar_uipara.res_id.home_id   = ID_INIT_HOME_BMP;
            headbar_uipara.res_id.bg_id            = ID_INIT_HBAR_BG_BMP;

            for (i = 0; i < 6; i++)
            {
                headbar_uipara.res_id.sig_st_id[i] = ID_DTV_ANTENNA_0_BMP + i;
            }

            headbar_uipara.font             = FONT16;
            break;
        }

        case SCREEN_TYPE_720_480:
        {
            //layer
            //layer
            headbar_uipara.hbar_lyr.fb_w  = 720;
            headbar_uipara.hbar_lyr.fb_h  = 46;
            headbar_uipara.hbar_lyr.src_x = 0;
            headbar_uipara.hbar_lyr.src_y = 0;
            headbar_uipara.hbar_lyr.src_w = 720;
            headbar_uipara.hbar_lyr.src_h = 46;
            headbar_uipara.hbar_lyr.scn_x = 0;//(800-480)/2;
            headbar_uipara.hbar_lyr.scn_y = 10;
            headbar_uipara.hbar_lyr.scn_w = 720;
            headbar_uipara.hbar_lyr.scn_h = 46;
            headbar_uipara.hbar_lyr.pipe  = 1;
            //
            headbar_uipara.hbar_frmwin.x = 0;
            headbar_uipara.hbar_frmwin.y = 0;
            headbar_uipara.hbar_frmwin.w = 720;
            headbar_uipara.hbar_frmwin.h = 46;
            headbar_uipara.title.x  = 20;
            headbar_uipara.title.y  = 0;
            headbar_uipara.title.w  = 100;
            headbar_uipara.title.h  = 46;
            headbar_uipara.music.x = 179 - 80;
            headbar_uipara.music.y = 0;
            headbar_uipara.music.w = 46;
            headbar_uipara.music.h = 46;
            headbar_uipara.date.x = 0;
            headbar_uipara.date.y = 0;
            headbar_uipara.date.w = 0;
            headbar_uipara.date.h = 0;
            headbar_uipara.time.x = 213 - 80;
            headbar_uipara.time.y = 0;
            headbar_uipara.time.w = 134;
            headbar_uipara.time.h = 46;
            headbar_uipara.singal_icon.x    = 300;
            headbar_uipara.singal_icon.y    = 0;
            headbar_uipara.singal_icon.w    = 46;
            headbar_uipara.singal_icon.h    = 46;
            headbar_uipara.singal_qa.x  = 350;
            headbar_uipara.singal_qa.y  = 0;
            headbar_uipara.singal_qa.w  = 46;
            headbar_uipara.singal_qa.h  = 46;
            headbar_uipara.volume_icon.x    = 603 - 80 - 50;
            headbar_uipara.volume_icon.y    = 0;
            headbar_uipara.volume_icon.w    = 46;
            headbar_uipara.volume_icon.h    = 46;
            headbar_uipara.volume_txt.x     = headbar_uipara.volume_icon.x + headbar_uipara.volume_icon.w + 2;
            headbar_uipara.volume_txt.y     = 0;
            headbar_uipara.volume_txt.w     = 16 + 20;
            headbar_uipara.volume_txt.h     = 46;
            headbar_uipara.bright_icon.x    = 507 - 80 - 50;
            headbar_uipara.bright_icon.y    = 0;
            headbar_uipara.bright_icon.w    = 46;
            headbar_uipara.bright_icon.h    = 46;
            headbar_uipara.bright_txt.x     = headbar_uipara.bright_icon.x + headbar_uipara.bright_icon.w + 1;
            headbar_uipara.bright_txt.y     = 0;
            headbar_uipara.bright_txt.w     = 16 + 20;
            headbar_uipara.bright_txt.h     = 46;
            headbar_uipara.battery.x        = 675 - 80 - 30;
            headbar_uipara.battery.y        = 0;
            headbar_uipara.battery.w        = 46;
            headbar_uipara.battery.h        = 46;
            headbar_uipara.home.x           = 730 - 80;
            headbar_uipara.home.y           = 0;
            headbar_uipara.home.w           = 70;
            headbar_uipara.home.h           = 46;
            headbar_uipara.res_id.vol_id0   = ID_INIT_ENERGY_0_BMP;
            headbar_uipara.res_id.vol_id1   = ID_INIT_ENERGY_1_BMP;
            headbar_uipara.res_id.vol_id2   = ID_INIT_ENERGY_2_BMP;
            headbar_uipara.res_id.vol_id3   = ID_INIT_ENERGY_3_BMP;
            headbar_uipara.res_id.vol_id4   = ID_INIT_ENERGY_4_BMP;
            headbar_uipara.res_id.vol_id5   = ID_INIT_ENERGY_5_BMP;
            headbar_uipara.res_id.charge_vol_id0    = ID_INIT_CHARGE_ENERGY_0_BMP;
            headbar_uipara.res_id.charge_vol_id1    = ID_INIT_CHARGE_ENERGY_1_BMP;
            headbar_uipara.res_id.charge_vol_id2    = ID_INIT_CHARGE_ENERGY_2_BMP;
            headbar_uipara.res_id.charge_vol_id3    = ID_INIT_CHARGE_ENERGY_3_BMP;
            headbar_uipara.res_id.charge_vol_id4    = ID_INIT_CHARGE_ENERGY_4_BMP;
            headbar_uipara.res_id.charge_vol_id5    = ID_INIT_CHARGE_ENERGY_5_BMP;
            headbar_uipara.res_id.no_vol_id         = 0;
            headbar_uipara.res_id.usb_connect_vol_id = 0;
            headbar_uipara.res_id.music_id  = ID_INIT_MUSIC_BMP;
            headbar_uipara.res_id.aux_id    = ID_INIT_LINEIN_BMP;
            headbar_uipara.res_id.pause_id  = ID_INIT_PAUSE_BMP;
            headbar_uipara.res_id.bright_id = ID_INIT_BRIGHT_BMP;
            headbar_uipara.res_id.volume_id = ID_INIT_VOLUME_BMP;
            headbar_uipara.res_id.home_id   = ID_INIT_HOME_BMP;
            headbar_uipara.res_id.home_fcs_id = ID_INIT_HOME_FCS_BMP;
            headbar_uipara.res_id.bg_id            = ID_INIT_HBAR_BG_BMP;

            for (i = 0; i < 6; i++)
            {
                headbar_uipara.res_id.sig_st_id[i] = ID_DTV_ANTENNA_0_BMP + i;
            }

            headbar_uipara.font             = FONT16;
            break;
        }
        case SCREEN_TYPE_480_640:
        {
            //layer
            headbar_uipara.hbar_lyr.fb_w  = 800;
            headbar_uipara.hbar_lyr.fb_h  = 46;
            headbar_uipara.hbar_lyr.src_x = 0;
            headbar_uipara.hbar_lyr.src_y = 0;
            headbar_uipara.hbar_lyr.src_w = 800;
            headbar_uipara.hbar_lyr.src_h = 46;
            headbar_uipara.hbar_lyr.scn_x = 0;//(800-480)/2;
            headbar_uipara.hbar_lyr.scn_y = 0;
            headbar_uipara.hbar_lyr.scn_w = 800;
            headbar_uipara.hbar_lyr.scn_h = 46;
            headbar_uipara.hbar_lyr.pipe  = 1;
            //
            headbar_uipara.hbar_frmwin.x = 0;
            headbar_uipara.hbar_frmwin.y = 0;
            headbar_uipara.hbar_frmwin.w = 800;
            headbar_uipara.hbar_frmwin.h = 46;
            headbar_uipara.title.x  = 10;
            headbar_uipara.title.y  = 0;
            headbar_uipara.title.w  = 80;
            headbar_uipara.title.h  = 46;
            headbar_uipara.music.x = 120;
            headbar_uipara.music.y = 0;
            headbar_uipara.music.w = 40;
            headbar_uipara.music.h = 46;
            headbar_uipara.date.x = 170;
            headbar_uipara.date.y = 0;
            headbar_uipara.date.w = 80;
            headbar_uipara.date.h = 0;
            headbar_uipara.time.x = 250;
            headbar_uipara.time.y = 0;
            headbar_uipara.time.w = 80;
            headbar_uipara.time.h = 46;
            headbar_uipara.singal_icon.x    = 330;
            headbar_uipara.singal_icon.y    = 0;
            headbar_uipara.singal_icon.w    = 80;
            headbar_uipara.singal_icon.h    = 46;
            headbar_uipara.singal_qa.x  = headbar_uipara.singal_icon.x + headbar_uipara.singal_icon.w;
            headbar_uipara.singal_qa.y  = 0;
            headbar_uipara.singal_qa.w  = 80;
            headbar_uipara.singal_qa.h  = 46;
            headbar_uipara.bright_icon.x    = 500;
            headbar_uipara.bright_icon.y    = 0;
            headbar_uipara.bright_icon.w    = 40;
            headbar_uipara.bright_icon.h    = 46;
            headbar_uipara.bright_txt.x     = headbar_uipara.bright_icon.x + headbar_uipara.bright_icon.w + 1;
            headbar_uipara.bright_txt.y     = 0;
            headbar_uipara.bright_txt.w     = 40;
            headbar_uipara.bright_txt.h     = 46;
            headbar_uipara.volume_icon.x    = 590;
            headbar_uipara.volume_icon.y    = 0;
            headbar_uipara.volume_icon.w    = 40;
            headbar_uipara.volume_icon.h    = 46;
            headbar_uipara.volume_txt.x     = headbar_uipara.volume_icon.x + headbar_uipara.volume_icon.w + 2;
            headbar_uipara.volume_txt.y     = 0;
            headbar_uipara.volume_txt.w     = 40;
            headbar_uipara.volume_txt.h     = 46;
            headbar_uipara.battery.x        = 680;
            headbar_uipara.battery.y        = 0;
            headbar_uipara.battery.w        = 40;
            headbar_uipara.battery.h        = 46;
            headbar_uipara.home.x           = 730;
            headbar_uipara.home.y           = 0;
            headbar_uipara.home.w           = 70;
            headbar_uipara.home.h           = 46;
            headbar_uipara.res_id.vol_id0   = ID_INIT_ENERGY_0_BMP;
            headbar_uipara.res_id.vol_id1   = ID_INIT_ENERGY_1_BMP;
            headbar_uipara.res_id.vol_id2   = ID_INIT_ENERGY_2_BMP;
            headbar_uipara.res_id.vol_id3   = ID_INIT_ENERGY_3_BMP;
            headbar_uipara.res_id.vol_id4   = ID_INIT_ENERGY_4_BMP;
            headbar_uipara.res_id.vol_id5   = ID_INIT_ENERGY_5_BMP;
            headbar_uipara.res_id.charge_vol_id0    = ID_INIT_CHARGE_ENERGY_0_BMP;
            headbar_uipara.res_id.charge_vol_id1    = ID_INIT_CHARGE_ENERGY_1_BMP;
            headbar_uipara.res_id.charge_vol_id2    = ID_INIT_CHARGE_ENERGY_2_BMP;
            headbar_uipara.res_id.charge_vol_id3    = ID_INIT_CHARGE_ENERGY_3_BMP;
            headbar_uipara.res_id.charge_vol_id4    = ID_INIT_CHARGE_ENERGY_4_BMP;
            headbar_uipara.res_id.charge_vol_id5    = ID_INIT_CHARGE_ENERGY_5_BMP;
            headbar_uipara.res_id.no_vol_id         = 0;
            headbar_uipara.res_id.usb_connect_vol_id = 0;
            headbar_uipara.res_id.music_id  = ID_INIT_MUSIC_BMP;
            headbar_uipara.res_id.aux_id    = ID_INIT_LINEIN_BMP;
            headbar_uipara.res_id.pause_id  = ID_INIT_PAUSE_BMP;
            headbar_uipara.res_id.bright_id = ID_INIT_BRIGHT_BMP;
            headbar_uipara.res_id.volume_id = ID_INIT_VOLUME_BMP;
            headbar_uipara.res_id.home_id   = ID_INIT_HOME_BMP;
            headbar_uipara.res_id.home_fcs_id = ID_INIT_HOME_FCS_BMP;
            headbar_uipara.res_id.bg_id            = ID_INIT_HBAR_BG_BMP;

            for (i = 0; i < 6; i++)
            {
                headbar_uipara.res_id.sig_st_id[i] = ID_DTV_ANTENNA_0_BMP + i;
            }

            headbar_uipara.font             = FONT16;
            break;
        }            
        #if 0
        {
            //layer
            headbar_uipara.hbar_lyr.fb_w  = 640;
            headbar_uipara.hbar_lyr.fb_h  = 46;
            headbar_uipara.hbar_lyr.src_x = 0;
            headbar_uipara.hbar_lyr.src_y = 0;
            headbar_uipara.hbar_lyr.src_w = 640;
            headbar_uipara.hbar_lyr.src_h = 46;
            headbar_uipara.hbar_lyr.scn_x = 0;//(800-480)/2;
            headbar_uipara.hbar_lyr.scn_y = 60;
            headbar_uipara.hbar_lyr.scn_w = 640;
            headbar_uipara.hbar_lyr.scn_h = 46;
            headbar_uipara.hbar_lyr.pipe  = 0;
            //
            headbar_uipara.hbar_frmwin.x = 0;
            headbar_uipara.hbar_frmwin.y = 0;
            headbar_uipara.hbar_frmwin.w = 640;
            headbar_uipara.hbar_frmwin.h = 46;
            headbar_uipara.title.x  = 0;
            headbar_uipara.title.y  = 60;
            headbar_uipara.title.w  = 80;
            headbar_uipara.title.h  = 46;
            headbar_uipara.music.x = 120;
            headbar_uipara.music.y = 0;
            headbar_uipara.music.w = 40;
            headbar_uipara.music.h = 46;
            headbar_uipara.date.x = 170;
            headbar_uipara.date.y = 0;
            headbar_uipara.date.w = 80;
            headbar_uipara.date.h = 0;
            headbar_uipara.time.x = 150;
            headbar_uipara.time.y = 0;
            headbar_uipara.time.w = 80;
            headbar_uipara.time.h = 46;
            headbar_uipara.singal_icon.x    = 130;
            headbar_uipara.singal_icon.y    = 0;
            headbar_uipara.singal_icon.w    = 80;
            headbar_uipara.singal_icon.h    = 46;
            headbar_uipara.singal_qa.x  = headbar_uipara.singal_icon.x + headbar_uipara.singal_icon.w;
            headbar_uipara.singal_qa.y  = 0;
            headbar_uipara.singal_qa.w  = 80;
            headbar_uipara.singal_qa.h  = 46;
            headbar_uipara.bright_icon.x    = 150;
            headbar_uipara.bright_icon.y    = 0;
            headbar_uipara.bright_icon.w    = 40;
            headbar_uipara.bright_icon.h    = 46;
            headbar_uipara.bright_txt.x     = headbar_uipara.bright_icon.x + headbar_uipara.bright_icon.w + 1;
            headbar_uipara.bright_txt.y     = 0;
            headbar_uipara.bright_txt.w     = 40;
            headbar_uipara.bright_txt.h     = 46;
            headbar_uipara.volume_icon.x    = 320;
            headbar_uipara.volume_icon.y    = 0;
            headbar_uipara.volume_icon.w    = 40;
            headbar_uipara.volume_icon.h    = 46;
            headbar_uipara.volume_txt.x     = headbar_uipara.volume_icon.x + headbar_uipara.volume_icon.w + 2;
            headbar_uipara.volume_txt.y     = 0;
            headbar_uipara.volume_txt.w     = 40;
            headbar_uipara.volume_txt.h     = 46;
            headbar_uipara.battery.x        = 640 - 70 - 35 -100;
            headbar_uipara.battery.y        = 0;
            headbar_uipara.battery.w        = 40;
            headbar_uipara.battery.h        = 46;
            headbar_uipara.home.x           = 640 - 70;
            headbar_uipara.home.y           = 0;
            headbar_uipara.home.w           = 70;
            headbar_uipara.home.h           = 46;
            headbar_uipara.res_id.vol_id0   = ID_INIT_ENERGY_0_BMP;
            headbar_uipara.res_id.vol_id1   = ID_INIT_ENERGY_1_BMP;
            headbar_uipara.res_id.vol_id2   = ID_INIT_ENERGY_2_BMP;
            headbar_uipara.res_id.vol_id3   = ID_INIT_ENERGY_3_BMP;
            headbar_uipara.res_id.vol_id4   = ID_INIT_ENERGY_4_BMP;
            headbar_uipara.res_id.vol_id5   = ID_INIT_ENERGY_5_BMP;
            headbar_uipara.res_id.charge_vol_id0    = ID_INIT_CHARGE_ENERGY_0_BMP;
            headbar_uipara.res_id.charge_vol_id1    = ID_INIT_CHARGE_ENERGY_1_BMP;
            headbar_uipara.res_id.charge_vol_id2    = ID_INIT_CHARGE_ENERGY_2_BMP;
            headbar_uipara.res_id.charge_vol_id3    = ID_INIT_CHARGE_ENERGY_3_BMP;
            headbar_uipara.res_id.charge_vol_id4    = ID_INIT_CHARGE_ENERGY_4_BMP;
            headbar_uipara.res_id.charge_vol_id5    = ID_INIT_CHARGE_ENERGY_5_BMP;
            headbar_uipara.res_id.no_vol_id         = 0;
            headbar_uipara.res_id.usb_connect_vol_id = 0;
            headbar_uipara.res_id.music_id  = ID_INIT_MUSIC_BMP;
            headbar_uipara.res_id.aux_id    = ID_INIT_LINEIN_BMP;
            headbar_uipara.res_id.pause_id  = ID_INIT_PAUSE_BMP;
            headbar_uipara.res_id.bright_id = ID_INIT_BRIGHT_BMP;
            headbar_uipara.res_id.volume_id = ID_INIT_VOLUME_BMP;
            headbar_uipara.res_id.home_id   = ID_INIT_HOME_BMP;
            headbar_uipara.res_id.home_fcs_id = ID_INIT_HOME_FCS_BMP;
            headbar_uipara.res_id.bg_id            = ID_INIT_HBAR_BG_BMP;

            for (i = 0; i < 6; i++)
            {
                headbar_uipara.res_id.sig_st_id[i] = ID_DTV_ANTENNA_0_BMP + i;
            }

            headbar_uipara.font             = FONT16;
            break;
        }
        #endif
        case SCREEN_TYPE_720_576:
        {
            //layer
            headbar_uipara.hbar_lyr.fb_w  = 720;
            headbar_uipara.hbar_lyr.fb_h  = 46;
            headbar_uipara.hbar_lyr.src_x = 0;
            headbar_uipara.hbar_lyr.src_y = 0;
            headbar_uipara.hbar_lyr.src_w = 720;
            headbar_uipara.hbar_lyr.src_h = 46;
            headbar_uipara.hbar_lyr.scn_x = 0;//(800-480)/2;
            headbar_uipara.hbar_lyr.scn_y = 10;
            headbar_uipara.hbar_lyr.scn_w = 720;
            headbar_uipara.hbar_lyr.scn_h = 46;
            headbar_uipara.hbar_lyr.pipe  = 1;
            //
            headbar_uipara.hbar_frmwin.x = 0;
            headbar_uipara.hbar_frmwin.y = 0;
            headbar_uipara.hbar_frmwin.w = 720;
            headbar_uipara.hbar_frmwin.h = 46;
            headbar_uipara.title.x  = 20;
            headbar_uipara.title.y  = 0;
            headbar_uipara.title.w  = 100;
            headbar_uipara.title.h  = 46;
            headbar_uipara.music.x = 179 - 80;
            headbar_uipara.music.y = 0;
            headbar_uipara.music.w = 46;
            headbar_uipara.music.h = 46;
            headbar_uipara.date.x = 0;
            headbar_uipara.date.y = 0;
            headbar_uipara.date.w = 0;
            headbar_uipara.date.h = 0;
            headbar_uipara.time.x = 213 - 80;
            headbar_uipara.time.y = 0;
            headbar_uipara.time.w = 134;
            headbar_uipara.time.h = 46;
            headbar_uipara.singal_icon.x    = 300;
            headbar_uipara.singal_icon.y    = 0;
            headbar_uipara.singal_icon.w    = 46;
            headbar_uipara.singal_icon.h    = 46;
            headbar_uipara.singal_qa.x  = 350;
            headbar_uipara.singal_qa.y  = 0;
            headbar_uipara.singal_qa.w  = 46;
            headbar_uipara.singal_qa.h  = 46;
            headbar_uipara.volume_icon.x    = 603 - 80 - 50;
            headbar_uipara.volume_icon.y    = 0;
            headbar_uipara.volume_icon.w    = 46;
            headbar_uipara.volume_icon.h    = 46;
            headbar_uipara.volume_txt.x     = headbar_uipara.volume_icon.x + headbar_uipara.volume_icon.w + 2;
            headbar_uipara.volume_txt.y     = 0;
            headbar_uipara.volume_txt.w     = 16 + 20;
            headbar_uipara.volume_txt.h     = 46;
            headbar_uipara.bright_icon.x    = 507 - 80;
            headbar_uipara.bright_icon.y    = 0;
            headbar_uipara.bright_icon.w    = 46;
            headbar_uipara.bright_icon.h    = 46;
            headbar_uipara.bright_txt.x     = headbar_uipara.bright_icon.x + headbar_uipara.bright_icon.w + 1;
            headbar_uipara.bright_txt.y     = 0;
            headbar_uipara.bright_txt.w     = 16 + 20;
            headbar_uipara.bright_txt.h     = 46;
            headbar_uipara.battery.x        = 675 - 80 - 30;
            headbar_uipara.battery.y        = 0;
            headbar_uipara.battery.w        = 46;
            headbar_uipara.battery.h        = 46;
            headbar_uipara.home.x           = 720 - 60;
            headbar_uipara.home.y           = 0;
            headbar_uipara.home.w           = 70;
            headbar_uipara.home.h           = 46;
            headbar_uipara.res_id.vol_id0   = ID_INIT_ENERGY_0_BMP;
            headbar_uipara.res_id.vol_id1   = ID_INIT_ENERGY_1_BMP;
            headbar_uipara.res_id.vol_id2   = ID_INIT_ENERGY_2_BMP;
            headbar_uipara.res_id.vol_id3   = ID_INIT_ENERGY_3_BMP;
            headbar_uipara.res_id.vol_id4   = ID_INIT_ENERGY_4_BMP;
            headbar_uipara.res_id.vol_id5   = ID_INIT_ENERGY_5_BMP;
            headbar_uipara.res_id.charge_vol_id0    = ID_INIT_CHARGE_ENERGY_0_BMP;
            headbar_uipara.res_id.charge_vol_id1    = ID_INIT_CHARGE_ENERGY_1_BMP;
            headbar_uipara.res_id.charge_vol_id2    = ID_INIT_CHARGE_ENERGY_2_BMP;
            headbar_uipara.res_id.charge_vol_id3    = ID_INIT_CHARGE_ENERGY_3_BMP;
            headbar_uipara.res_id.charge_vol_id4    = ID_INIT_CHARGE_ENERGY_4_BMP;
            headbar_uipara.res_id.charge_vol_id5    = ID_INIT_CHARGE_ENERGY_5_BMP;
            headbar_uipara.res_id.no_vol_id         = 0;
            headbar_uipara.res_id.usb_connect_vol_id = 0;
            headbar_uipara.res_id.music_id  = ID_INIT_MUSIC_BMP;
            headbar_uipara.res_id.aux_id    = ID_INIT_LINEIN_BMP;
            headbar_uipara.res_id.pause_id  = ID_INIT_PAUSE_BMP;
            headbar_uipara.res_id.bright_id = ID_INIT_BRIGHT_BMP;
            headbar_uipara.res_id.volume_id = ID_INIT_VOLUME_BMP;
            headbar_uipara.res_id.home_id   = ID_INIT_HOME_BMP;
            headbar_uipara.res_id.home_fcs_id = ID_INIT_HOME_FCS_BMP;
            headbar_uipara.res_id.bg_id            = ID_INIT_HBAR_BG_BMP;

            for (i = 0; i < 6; i++)
            {
                headbar_uipara.res_id.sig_st_id[i] = ID_DTV_ANTENNA_0_BMP + i;
            }

            headbar_uipara.font             = FONT16;
            break;
        }

        case SCREEN_TYPE_LAST:
        case SCREEN_TYPE_800_480:
        {
            //layer
            headbar_uipara.hbar_lyr.fb_w  = 800;
            headbar_uipara.hbar_lyr.fb_h  = 46;
            headbar_uipara.hbar_lyr.src_x = 0;
            headbar_uipara.hbar_lyr.src_y = 0;
            headbar_uipara.hbar_lyr.src_w = 800;
            headbar_uipara.hbar_lyr.src_h = 46;
            headbar_uipara.hbar_lyr.scn_x = 0;//(800-480)/2;
            headbar_uipara.hbar_lyr.scn_y = 0;
            headbar_uipara.hbar_lyr.scn_w = 800;
            headbar_uipara.hbar_lyr.scn_h = 46;
            headbar_uipara.hbar_lyr.pipe  = 1;
            //
            headbar_uipara.hbar_frmwin.x = 0;
            headbar_uipara.hbar_frmwin.y = 0;
            headbar_uipara.hbar_frmwin.w = 800;
            headbar_uipara.hbar_frmwin.h = 46;
            headbar_uipara.title.x  = 10;
            headbar_uipara.title.y  = 0;
            headbar_uipara.title.w  = 80;
            headbar_uipara.title.h  = 46;
            headbar_uipara.music.x = 120;
            headbar_uipara.music.y = 0;
            headbar_uipara.music.w = 40;
            headbar_uipara.music.h = 46;
            headbar_uipara.date.x = 170;
            headbar_uipara.date.y = 0;
            headbar_uipara.date.w = 80;
            headbar_uipara.date.h = 0;
            headbar_uipara.time.x = 250;
            headbar_uipara.time.y = 0;
            headbar_uipara.time.w = 80;
            headbar_uipara.time.h = 46;
            headbar_uipara.singal_icon.x    = 330;
            headbar_uipara.singal_icon.y    = 0;
            headbar_uipara.singal_icon.w    = 80;
            headbar_uipara.singal_icon.h    = 46;
            headbar_uipara.singal_qa.x  = headbar_uipara.singal_icon.x + headbar_uipara.singal_icon.w;
            headbar_uipara.singal_qa.y  = 0;
            headbar_uipara.singal_qa.w  = 80;
            headbar_uipara.singal_qa.h  = 46;
            headbar_uipara.bright_icon.x    = 500;
            headbar_uipara.bright_icon.y    = 0;
            headbar_uipara.bright_icon.w    = 40;
            headbar_uipara.bright_icon.h    = 46;
            headbar_uipara.bright_txt.x     = headbar_uipara.bright_icon.x + headbar_uipara.bright_icon.w + 1;
            headbar_uipara.bright_txt.y     = 0;
            headbar_uipara.bright_txt.w     = 40;
            headbar_uipara.bright_txt.h     = 46;
            headbar_uipara.volume_icon.x    = 590;
            headbar_uipara.volume_icon.y    = 0;
            headbar_uipara.volume_icon.w    = 40;
            headbar_uipara.volume_icon.h    = 46;
            headbar_uipara.volume_txt.x     = headbar_uipara.volume_icon.x + headbar_uipara.volume_icon.w + 2;
            headbar_uipara.volume_txt.y     = 0;
            headbar_uipara.volume_txt.w     = 40;
            headbar_uipara.volume_txt.h     = 46;
            headbar_uipara.battery.x        = 680;
            headbar_uipara.battery.y        = 0;
            headbar_uipara.battery.w        = 40;
            headbar_uipara.battery.h        = 46;
            headbar_uipara.home.x           = 730;
            headbar_uipara.home.y           = 0;
            headbar_uipara.home.w           = 70;
            headbar_uipara.home.h           = 46;
            headbar_uipara.res_id.vol_id0   = ID_INIT_ENERGY_0_BMP;
            headbar_uipara.res_id.vol_id1   = ID_INIT_ENERGY_1_BMP;
            headbar_uipara.res_id.vol_id2   = ID_INIT_ENERGY_2_BMP;
            headbar_uipara.res_id.vol_id3   = ID_INIT_ENERGY_3_BMP;
            headbar_uipara.res_id.vol_id4   = ID_INIT_ENERGY_4_BMP;
            headbar_uipara.res_id.vol_id5   = ID_INIT_ENERGY_5_BMP;
            headbar_uipara.res_id.charge_vol_id0    = ID_INIT_CHARGE_ENERGY_0_BMP;
            headbar_uipara.res_id.charge_vol_id1    = ID_INIT_CHARGE_ENERGY_1_BMP;
            headbar_uipara.res_id.charge_vol_id2    = ID_INIT_CHARGE_ENERGY_2_BMP;
            headbar_uipara.res_id.charge_vol_id3    = ID_INIT_CHARGE_ENERGY_3_BMP;
            headbar_uipara.res_id.charge_vol_id4    = ID_INIT_CHARGE_ENERGY_4_BMP;
            headbar_uipara.res_id.charge_vol_id5    = ID_INIT_CHARGE_ENERGY_5_BMP;
            headbar_uipara.res_id.no_vol_id         = 0;
            headbar_uipara.res_id.usb_connect_vol_id = 0;
            headbar_uipara.res_id.music_id  = ID_INIT_MUSIC_BMP;
            headbar_uipara.res_id.aux_id    = ID_INIT_LINEIN_BMP;
            headbar_uipara.res_id.pause_id  = ID_INIT_PAUSE_BMP;
            headbar_uipara.res_id.bright_id = ID_INIT_BRIGHT_BMP;
            headbar_uipara.res_id.volume_id = ID_INIT_VOLUME_BMP;
            headbar_uipara.res_id.home_id   = ID_INIT_HOME_BMP;
            headbar_uipara.res_id.home_fcs_id = ID_INIT_HOME_FCS_BMP;
            headbar_uipara.res_id.bg_id            = ID_INIT_HBAR_BG_BMP;

            for (i = 0; i < 6; i++)
            {
                headbar_uipara.res_id.sig_st_id[i] = ID_DTV_ANTENNA_0_BMP + i;
            }

            headbar_uipara.font             = FONT16;
            break;
        }

        case SCREEN_TYPE_1280_720:
        {
            //layer
            headbar_uipara.hbar_lyr.fb_w  = 480;
            headbar_uipara.hbar_lyr.fb_h  = 24;
            headbar_uipara.hbar_lyr.src_x = 0;
            headbar_uipara.hbar_lyr.src_y = 0;
            headbar_uipara.hbar_lyr.src_w = 480;
            headbar_uipara.hbar_lyr.src_h = 24;
            headbar_uipara.hbar_lyr.scn_x = (1280 - 480) / 2;
            headbar_uipara.hbar_lyr.scn_y = 0;
            headbar_uipara.hbar_lyr.scn_w = 480;
            headbar_uipara.hbar_lyr.scn_h = 24;
            headbar_uipara.hbar_lyr.pipe  = 1;
            //
            headbar_uipara.hbar_frmwin.x = 0;
            headbar_uipara.hbar_frmwin.y = 0;
            headbar_uipara.hbar_frmwin.w = 480;
            headbar_uipara.hbar_frmwin.h = 24;
            headbar_uipara.title.x  = 20;
            headbar_uipara.title.y  = 0;
            headbar_uipara.title.w  = 100;
            headbar_uipara.title.h  = 24;
            headbar_uipara.music.x = 100 + 20;
            headbar_uipara.music.y = 2;
            headbar_uipara.music.w = 22;
            headbar_uipara.music.h = 19;
            headbar_uipara.date.x = 0;
            headbar_uipara.date.y = 0;
            headbar_uipara.date.w = 0;
            headbar_uipara.date.h = 0;
            headbar_uipara.time.x = 213;
            headbar_uipara.time.y = 0;
            headbar_uipara.time.w = 134;
            headbar_uipara.time.h = 24;
            headbar_uipara.singal_icon.x    = 300;
            headbar_uipara.singal_icon.y    = 0;
            headbar_uipara.singal_icon.w    = 46;
            headbar_uipara.singal_icon.h    = 46;
            headbar_uipara.singal_qa.x  = 350;
            headbar_uipara.singal_qa.y  = 0;
            headbar_uipara.singal_qa.w  = 46;
            headbar_uipara.singal_qa.h  = 46;
            headbar_uipara.volume_icon.x    = 357;
            headbar_uipara.volume_icon.y    = 2;
            headbar_uipara.volume_icon.w    = 22;
            headbar_uipara.volume_icon.h    = 19;
            headbar_uipara.volume_txt.x     = headbar_uipara.volume_icon.x + headbar_uipara.volume_icon.w + 2;
            headbar_uipara.volume_txt.y     = 0;
            headbar_uipara.volume_txt.w     = 16;
            headbar_uipara.volume_txt.h     = 24;
            headbar_uipara.bright_icon.x    = 159;
            headbar_uipara.bright_icon.y    = 2;
            headbar_uipara.bright_icon.w    = 22;
            headbar_uipara.bright_icon.h    = 19;
            headbar_uipara.bright_txt.x     = headbar_uipara.bright_icon.x + headbar_uipara.bright_icon.w + 1;
            headbar_uipara.bright_txt.y     = 0;
            headbar_uipara.bright_txt.w     = 16;
            headbar_uipara.bright_txt.h     = 24;
            headbar_uipara.battery.x        = 406;
            headbar_uipara.battery.y        = 2;
            headbar_uipara.battery.w        = 32;
            headbar_uipara.battery.h        = 18;
            headbar_uipara.home.x           = 446;
            headbar_uipara.home.y           = 2;
            headbar_uipara.home.w           = 22;
            headbar_uipara.home.h           = 19;
            headbar_uipara.res_id.vol_id0   = ID_INIT_ENERGY_0_BMP;
            headbar_uipara.res_id.vol_id1   = ID_INIT_ENERGY_1_BMP;
            headbar_uipara.res_id.vol_id2   = ID_INIT_ENERGY_2_BMP;
            headbar_uipara.res_id.vol_id3   = ID_INIT_ENERGY_3_BMP;
            headbar_uipara.res_id.vol_id4   = ID_INIT_ENERGY_4_BMP;
            headbar_uipara.res_id.vol_id5   = ID_INIT_ENERGY_5_BMP;
            headbar_uipara.res_id.charge_vol_id0    = ID_INIT_CHARGE_ENERGY_0_BMP;
            headbar_uipara.res_id.charge_vol_id1    = ID_INIT_CHARGE_ENERGY_1_BMP;
            headbar_uipara.res_id.charge_vol_id2    = ID_INIT_CHARGE_ENERGY_2_BMP;
            headbar_uipara.res_id.charge_vol_id3    = ID_INIT_CHARGE_ENERGY_3_BMP;
            headbar_uipara.res_id.charge_vol_id4    = ID_INIT_CHARGE_ENERGY_4_BMP;
            headbar_uipara.res_id.charge_vol_id5    = ID_INIT_CHARGE_ENERGY_5_BMP;
            headbar_uipara.res_id.no_vol_id         = 0;
            headbar_uipara.res_id.usb_connect_vol_id = 0;
            headbar_uipara.res_id.music_id  = ID_INIT_MUSIC_BMP;
            headbar_uipara.res_id.aux_id    = ID_INIT_LINEIN_BMP;
            headbar_uipara.res_id.pause_id  = ID_INIT_PAUSE_BMP;
            headbar_uipara.res_id.bright_id = ID_INIT_BRIGHT_BMP;
            headbar_uipara.res_id.volume_id = ID_INIT_VOLUME_BMP;
            headbar_uipara.res_id.home_id   = ID_INIT_HOME_BMP;
            headbar_uipara.res_id.bg_id            = ID_INIT_HBAR_BG_BMP;

            for (i = 0; i < 6; i++)
            {
                headbar_uipara.res_id.sig_st_id[i] = ID_DTV_ANTENNA_0_BMP + i;
            }

            headbar_uipara.font             = FONT16;
            break;
        }

        case SCREEN_TYPE_1920_1080:
        {
            //layer
            headbar_uipara.hbar_lyr.fb_w  = 480;
            headbar_uipara.hbar_lyr.fb_h  = 24;
            headbar_uipara.hbar_lyr.src_x = 0;
            headbar_uipara.hbar_lyr.src_y = 0;
            headbar_uipara.hbar_lyr.src_w = 480;
            headbar_uipara.hbar_lyr.src_h = 24;
            headbar_uipara.hbar_lyr.scn_x = (1920 - 480) / 2;
            headbar_uipara.hbar_lyr.scn_y = 0;
            headbar_uipara.hbar_lyr.scn_w = 480;
            headbar_uipara.hbar_lyr.scn_h = 24;
            headbar_uipara.hbar_lyr.pipe  = 1;
            //
            headbar_uipara.hbar_frmwin.x = 0;
            headbar_uipara.hbar_frmwin.y = 0;
            headbar_uipara.hbar_frmwin.w = 480;
            headbar_uipara.hbar_frmwin.h = 24;
            headbar_uipara.title.x  = 20;
            headbar_uipara.title.y  = 0;
            headbar_uipara.title.w  = 100;
            headbar_uipara.title.h  = 24;
            headbar_uipara.music.x = 100 + 20;
            headbar_uipara.music.y = 2;
            headbar_uipara.music.w = 22;
            headbar_uipara.music.h = 19;
            headbar_uipara.date.x = 0;
            headbar_uipara.date.y = 0;
            headbar_uipara.date.w = 0;
            headbar_uipara.date.h = 0;
            headbar_uipara.time.x = 213;
            headbar_uipara.time.y = 0;
            headbar_uipara.time.w = 134;
            headbar_uipara.time.h = 24;
            headbar_uipara.singal_icon.x    = 300;
            headbar_uipara.singal_icon.y    = 0;
            headbar_uipara.singal_icon.w    = 46;
            headbar_uipara.singal_icon.h    = 46;
            headbar_uipara.singal_qa.x  = 350;
            headbar_uipara.singal_qa.y  = 0;
            headbar_uipara.singal_qa.w  = 46;
            headbar_uipara.singal_qa.h  = 46;
            headbar_uipara.volume_icon.x    = 357;
            headbar_uipara.volume_icon.y    = 2;
            headbar_uipara.volume_icon.w    = 22;
            headbar_uipara.volume_icon.h    = 19;
            headbar_uipara.volume_txt.x     = headbar_uipara.volume_icon.x + headbar_uipara.volume_icon.w + 2;
            headbar_uipara.volume_txt.y     = 0;
            headbar_uipara.volume_txt.w     = 16;
            headbar_uipara.volume_txt.h     = 24;
            headbar_uipara.bright_icon.x    = 159;
            headbar_uipara.bright_icon.y    = 2;
            headbar_uipara.bright_icon.w    = 22;
            headbar_uipara.bright_icon.h    = 19;
            headbar_uipara.bright_txt.x     = headbar_uipara.bright_icon.x + headbar_uipara.bright_icon.w + 1;
            headbar_uipara.bright_txt.y     = 0;
            headbar_uipara.bright_txt.w     = 16;
            headbar_uipara.bright_txt.h     = 24;
            headbar_uipara.battery.x        = 406;
            headbar_uipara.battery.y        = 2;
            headbar_uipara.battery.w        = 32;
            headbar_uipara.battery.h        = 18;
            headbar_uipara.home.x           = 446;
            headbar_uipara.home.y           = 2;
            headbar_uipara.home.w           = 22;
            headbar_uipara.home.h           = 19;
            headbar_uipara.res_id.vol_id0   = ID_INIT_ENERGY_0_BMP;
            headbar_uipara.res_id.vol_id1   = ID_INIT_ENERGY_1_BMP;
            headbar_uipara.res_id.vol_id2   = ID_INIT_ENERGY_2_BMP;
            headbar_uipara.res_id.vol_id3   = ID_INIT_ENERGY_3_BMP;
            headbar_uipara.res_id.vol_id4   = ID_INIT_ENERGY_4_BMP;
            headbar_uipara.res_id.vol_id5   = ID_INIT_ENERGY_5_BMP;
            headbar_uipara.res_id.charge_vol_id0    = ID_INIT_CHARGE_ENERGY_0_BMP;
            headbar_uipara.res_id.charge_vol_id1    = ID_INIT_CHARGE_ENERGY_1_BMP;
            headbar_uipara.res_id.charge_vol_id2    = ID_INIT_CHARGE_ENERGY_2_BMP;
            headbar_uipara.res_id.charge_vol_id3    = ID_INIT_CHARGE_ENERGY_3_BMP;
            headbar_uipara.res_id.charge_vol_id4    = ID_INIT_CHARGE_ENERGY_4_BMP;
            headbar_uipara.res_id.charge_vol_id5    = ID_INIT_CHARGE_ENERGY_5_BMP;
            headbar_uipara.res_id.no_vol_id         = 0;
            headbar_uipara.res_id.usb_connect_vol_id = 0;
            headbar_uipara.res_id.music_id  = ID_INIT_MUSIC_BMP;
            headbar_uipara.res_id.aux_id    = ID_INIT_LINEIN_BMP;
            headbar_uipara.res_id.pause_id  = ID_INIT_PAUSE_BMP;
            headbar_uipara.res_id.bright_id = ID_INIT_BRIGHT_BMP;
            headbar_uipara.res_id.volume_id = ID_INIT_VOLUME_BMP;
            headbar_uipara.res_id.home_id   = ID_INIT_HOME_BMP;
            headbar_uipara.res_id.bg_id            = ID_INIT_HBAR_BG_BMP;

            for (i = 0; i < 6; i++)
            {
                headbar_uipara.res_id.sig_st_id[i] = ID_DTV_ANTENNA_0_BMP + i;
            }

            headbar_uipara.font             = FONT16;
            break;
        }

        default:
            __err(" unknown screen type \n");
            break;
    }
}
