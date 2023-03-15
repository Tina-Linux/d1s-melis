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
#include "palette.h"
#if 0 //__err("not support anymore after d100");
#define A_POS                           24
#define R_POS                            0
#define G_POS                            8
#define B_POS                           16

#define A_PART( x )                     ( (x) << A_POS )
#define R_PART( x )                     ( (x) << R_POS )
#define G_PART( x )                     ( (x) << G_POS )
#define B_PART( x )                     ( (x) << B_POS )

#define GET_A( x )                      ( ( ( ( (__u32 )( (x) & ( 0xFFU << A_POS ) ) ) >> A_POS ) & 0xFF ) )
#define GET_R( x )                      ( ( ( ( (__u32 )( (x) & ( 0xFFU << R_POS ) ) ) >> R_POS ) & 0xFF ) )
#define GET_G( x )                      ( ( ( ( (__u32 )( (x) & ( 0xFFU << G_POS ) ) ) >> G_POS ) & 0xFF ) )
#define GET_B( x )                      ( ( ( ( (__u32 )( (x) & ( 0xFFU << B_POS ) ) ) >> B_POS ) & 0xFF ) )


#define DE_A_POS                        24
#define DE_R_POS                        16
#define DE_G_POS                         8
#define DE_B_POS                         0

#define DE_A_PART( x )                  ( (x) << DE_A_POS )
#define DE_R_PART( x )                  ( (x) << DE_R_POS )
#define DE_G_PART( x )                  ( (x) << DE_G_POS )
#define DE_B_PART( x )                  ( (x) << DE_B_POS )

#define DE_GET_A( x )                   ( ( ( ( (__u32 )( (x) & ( 0xFFU << DE_A_POS ) ) ) >> DE_A_POS ) & 0xFF ) )
#define DE_GET_R( x )                   ( ( ( ( (__u32 )( (x) & ( 0xFFU << DE_R_POS ) ) ) >> DE_R_POS ) & 0xFF ) )
#define DE_GET_G( x )                   ( ( ( ( (__u32 )( (x) & ( 0xFFU << DE_G_POS ) ) ) >> DE_G_POS ) & 0xFF ) )
#define DE_GET_B( x )                   ( ( ( ( (__u32 )( (x) & ( 0xFFU << DE_B_POS ) ) ) >> DE_B_POS ) & 0xFF ) )


#define GUI2DE( GUI_color )             (   ( GET_A( GUI_color ) << DE_A_POS ) \
        | ( GET_R( GUI_color ) << DE_R_POS ) \
        | ( GET_G( GUI_color ) << DE_G_POS ) \
        | ( GET_B( GUI_color ) << DE_B_POS ) )

#define DE2GUI( DE_color )              (   ( DE_GET_A( DE_color ) << A_POS ) \
        | ( DE_GET_R( DE_color ) << R_POS ) \
        | ( DE_GET_G( DE_color ) << G_POS ) \
        | ( DE_GET_B( DE_color ) << B_POS ) )

__s32  com_set_palette_text_color(__u32 *palette, __u32 text_color_index,
                                  __u32 gui_color)
{
    __u32  de_color;
    __u32  i;
    __u32 alpha_table[16] =
    {
        0x10000000, 0x20000000, 0x30000000, 0x40000000, 0x50000000, 0x60000000, 0x70000000, 0x80000000,
        0x90000000, 0xA0000000, 0xB0000000, 0xC0000000, 0xD0000000, 0xE0000000, 0xF0000000, 0xFF000000
    };
    de_color = GUI2DE(gui_color);
    de_color = de_color & 0x00FFFFFF;

    for (i = 0;  i < 16;  i++)
    {
        palette[text_color_index + i] = alpha_table[i] | de_color;
    }

    return 0;
}

__s32 com_set_palette(void *palette_buf)
{
    ES_FILE *hdis;
    __u32  palette[256];
    __u32  arg[3];
    __s32  ret;

    if (palette_buf == NULL)
    {
        return -1;
    }

    hdis = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

    if (hdis == NULL)
    {
        __err("Error in opening disp!");
        return -1;
    }

    eLIBs_memcpy(palette, palette_buf, 256 * 4);
    //set user define text color
    com_set_palette_text_color(palette, APP_COLOR_WHITE
                               , GUI_WHITE);
    com_set_palette_text_color(palette, APP_COLOR_BLACK
                               , GUI_BLACK);
    com_set_palette_text_color(palette, APP_COLOR_GREEN
                               , GUI_GREEN);
    com_set_palette_text_color(palette, APP_COLOR_YELLOW
                               , GUI_YELLOW);
    /* set palette */
    arg[0] = (__u32)palette;
    arg[1] = 0;
    arg[2] = 256 * 4;
    ret = esMODS_MIoctrl(hdis, DISP_CMD_SET_PALETTE_TBL, 0, (void *)arg);

    if (ret != 0)
    {
        __wrn("Fail in setting palette.");
        eLIBs_fclose(hdis);
        return -1;
    }

    eLIBs_fclose(hdis);
    return EPDK_OK;
}

__s32 com_set_palette_by_id(__s32 ID)
{
    __s32 size;
    HTHEME htheme;
    htheme = dsk_theme_open(ID);

    if (NULL == htheme)
    {
        __msg("************id not exist*************");
        return EPDK_FAIL;
    }

    size = dsk_theme_hdl2size(htheme);

    if (size != 1024)
    {
        dsk_theme_close(htheme);
        __msg("wong palette size ...");
        return EPDK_FAIL;
    }

    com_set_palette(dsk_theme_hdl2buf(htheme));
    dsk_theme_close(htheme);
    return EPDK_OK;
}

__s32  com_set_palette_color_by_index(__u32 palette_index, const __u32 *buf, __u32 nr)
{
    ES_FILE *hdis;
    __u32  palette[256];
    __u32  arg[3];
    __s32  ret;

    if (buf == NULL)
    {
        return -1;
    }

    hdis = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

    if (hdis == NULL)
    {
        __err("Error in opening disp!");
        return -1;
    }

    /* get palette */
    arg[0] = (__u32)palette;
    arg[1] = 0;
    arg[2] = 256 * 4;
    ret = esMODS_MIoctrl(hdis, DISP_CMD_GET_PALETTE_TBL, 0, (void *)arg);

    if (ret != 0)
    {
        __wrn("Fail in setting palette.");
        eLIBs_fclose(hdis);
        return -1;
    }

    eLIBs_memcpy(palette + palette_index, buf, sizeof(__u32) * nr);
    /* set palette */
    arg[0] = (__u32)palette;
    arg[1] = 0;
    arg[2] = 256 * 4;
    ret = esMODS_MIoctrl(hdis, DISP_CMD_SET_PALETTE_TBL, 0, (void *)arg);

    if (ret != 0)
    {
        __wrn("Fail in setting palette.");
        eLIBs_fclose(hdis);
        return -1;
    }

    eLIBs_fclose(hdis);
    return EPDK_OK;
}


#else
__s32  com_set_palette_text_color(__u32 *palette, __u32 text_color_index, __u32 gui_color)
{
    return 0;
}

__s32 com_set_palette(void *palette_buf)
{
    return 0;
}

__s32 com_set_palette_by_id(__s32 ID)
{
    return 0;
}

__s32 com_set_palette_color_by_index(__u32 palette_index, const __u32 *buf, __u32 nr)
{
    return 0;
}

H_LYR com_layer_create(RECT *rect, __s32 pipe, __pixel_rgbfmt_t rgb_fmt, __gui_lyrwin_sta_t initsta, char *lyr_name)
{
    H_LYR layer = NULL;
    static __u8 cnt = 0;
    char   lname[64] = {0};
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt       */
    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        0xff,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "com layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };

    if (lyr_name)
    {
        eLIBs_sprintf(lname, "%s%d", lname, cnt++);
        lyrcreate_info.name = lname;
    }

    lyrcreate_info.initsta  = initsta;
    fb.size.width           = rect->width;
    fb.size.height          = rect->height;
    //fb.fmt.fmt.rgb.pixelfmt = rgb_fmt;
    //if (rgb_fmt > PIXEL_MONO_8BPP)
    //{
    //  lstlyr.mode = DISP_LAYER_WORK_MODE_NORMAL;
    //}
    // framebuffer source window
    lstlyr.src_win.x        = 0;
    lstlyr.src_win.y        = 0;
    lstlyr.src_win.width    = rect->width;
    lstlyr.src_win.height   = rect->height;
    //screen region
    lstlyr.scn_win.x        = rect->x;
    lstlyr.scn_win.y        = rect->y;
    lstlyr.scn_win.width    = rect->width;
    lstlyr.scn_win.height   = rect->height;
    lstlyr.pipe             = pipe;
    lstlyr.fb               = &fb;
    lyrcreate_info.lyrpara  = &lstlyr;
    __log("pallete layer");
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("app layer create error !");
    }

    return layer;
}
#endif
