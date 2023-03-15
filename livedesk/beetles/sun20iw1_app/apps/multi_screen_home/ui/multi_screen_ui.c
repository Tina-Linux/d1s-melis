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
#include "multi_screen_ui.h"


static const __s32  multi_screen_uf_icon[HOME_UNFOCUS_ICON_NUM] =
{
    //固定应用
    ID_HOME_NEW_EXPLORER_UF_BMP,
    ID_HOME_NEW_MOVIE_UF_BMP,
    ID_HOME_NEW_MUSIC_UF_BMP,
    ID_HOME_NEW_PICTURE_UF_BMP,
    ID_HOME_NEW_EBOOK_UF_BMP,
    ID_HOME_NEW_FM_UF_BMP,
    ID_HOME_NEW_RECORD_UF_BMP,
    ID_HOME_NEW_SETTING_UF_BMP,
};
static const __s32  multi_screen_f_icon[HOME_FOCUS_ICON_NUM] =
{
    //固定应用选中
    ID_HOME_NEW_EXPLORER_FC_BMP,
    ID_HOME_NEW_MOVIE_FC_BMP,
    ID_HOME_NEW_MUSIC_FC_BMP,
    ID_HOME_NEW_PICTURE_FC_BMP,
    ID_HOME_NEW_EBOOK_FC_BMP,
    ID_HOME_NEW_FM_FC_BMP,
    ID_HOME_NEW_RECORD_FC_BMP,
    ID_HOME_NEW_SETTING_FC_BMP
};

static const int32_t  multi_screen_string[MULTI_SCREEN_STRING_MAX] =
{
    STRING_HOME_EXPLORER,
    STRING_HOME_MOVIE,
    STRING_HOME_MUSIC,
    STRING_HOME_PHOTO,
    STRING_HOME_EBOOK,
    STRING_HOME_FM,
    STRING_HOME_RECORD,
    STRING_HOME_SETTING
};

int32_t init_multi_screen_res(pmulti_screen_ui_t pui, __s16 current_focus)
{
    uint32_t   i;

    __inf("enter %s\r\n", __FUNCTION__);

    //初始化unfocus ICON 资源
    for (i = HOME_UNFOCUS_ICON_START; i < HOME_UNFOCUS_ICON_NUM; i++)
    {
        if (multi_screen_uf_icon[i])
        {
            pui->bmp_uf[i] = dsk_theme_open(multi_screen_uf_icon[i]);

            if (pui->bmp_uf[i] == NULL)
            {
                __wrn("init_multi_screen_res() index:%d open fail\n", i);
            }
        }
        else
        {
            pui->bmp_uf[i] = NULL;
        }
    }

    /*focus icon*/
    pui->bmp_f[current_focus] = dsk_theme_open(multi_screen_f_icon[current_focus]);
/*
    if (pui->bmp_f[i] == NULL)
    {
        __wrn("init_multi_screen_res() index:%d open fail\n", i);
    }
*/
    //初始化String 资源
    for (i = 0; i < MULTI_SCREEN_STRING_MAX; i++)
    {
        dsk_langres_get_menu_text(multi_screen_string[i], pui->lang[i], GUI_NAME_MAX);
    }

    return EPDK_OK;
}
__s32 uninit_multi_screen_res(pmulti_screen_ui_t pui)
{
    __u32   i;

    //释放ICON 资源
    for (i = HOME_UNFOCUS_ICON_START; i < HOME_UNFOCUS_ICON_NUM; i++)
    {
        if (pui->bmp_uf[i])
        {
            __inf("i = %d\n", i);
            dsk_theme_close(pui->bmp_uf[i]);
            pui->bmp_uf[i] = NULL;
        }
    }

    for (i = HOME_FOCUS_ICON_START; i < HOME_FOCUS_ICON_NUM; i++)
    {
        if (pui->bmp_f[i])
        {
            __log("i = %d\n", i);
            dsk_theme_close(pui->bmp_f[i]);
            pui->bmp_f[i] = NULL;
        }
    }

    //释放string 资源
    for (i = 0; i < MULTI_SCREEN_STRING_MAX; i++)
    {
        pui->lang[i][0] = '\0';
    }

    return EPDK_OK;
}



void draw_multi_screen_icon_coor(pmulti_screen_ui_t pui, __s32 index, __s32 X, __s32 Y)
{
    void *pbmp;
#if 1

    if (pui->bmp_uf[index] != NULL)
    {
        pbmp = dsk_theme_hdl2buf(pui->bmp_uf[index]);
        GUI_BMP_Draw(pbmp, X, Y);
    }

#else
    __u8 *pack = pui->pack;
    pbmp = pack + (*(__u32 *)(pack + 16 + multi_screen_icon[index] * 4));
    GUI_BMP_Draw(pbmp, X, Y);
#endif
}

void draw_multi_screen_icon_coor_rect(pmulti_screen_ui_t pui, __s32 index, __s32 bIs_focus, GUI_RECT *rect)
{
    if (-1 == rect->x0)
    {
        __wrn("-1 == rect->x0\n");
        return;
    }

    if (bIs_focus == EPDK_TRUE)
    {
        if (pui->bmp_f[index] == NULL)
        {
            pui->bmp_f[index] = dsk_theme_open(multi_screen_f_icon[index]);
        }

        GUI_ClearRectEx(rect);
        GUI_BMP_Draw(dsk_theme_hdl2buf(pui->bmp_f[index]), rect->x0, rect->y0);
    }
    else
    {
        if (pui->bmp_uf[index] == NULL)
        {
            pui->bmp_uf[index] = dsk_theme_open(multi_screen_uf_icon[index]);
        }

        GUI_ClearRectEx(rect);
        GUI_BMP_Draw(dsk_theme_hdl2buf(pui->bmp_uf[index]), rect->x0, rect->y0);
    }
}


void draw_multi_screen_string_center(pmulti_screen_ui_t pui, __s32 index, __s32 fnt_color, GUI_RECT *rect)
{
    if (pui->lang[index][0] != '\0')
    {
        GUI_SetColor(fnt_color);
        GUI_DispStringInRect(pui->lang[index], rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    }
}


