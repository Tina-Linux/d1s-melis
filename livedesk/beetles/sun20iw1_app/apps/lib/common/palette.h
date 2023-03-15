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
#ifndef __COM_PALETTE_H_
#define __COM_PALETTE_H_

#include "beetles_app.h"
#include "APP_Mem.h"

#define PALETTE_TEXT_COLOUR_INDEX_(_i)  (0xC0 + (_i) * 0x10)
#define APP_COLOR_WHITE                 GUI_WHITE//PALETTE_TEXT_COLOUR_INDEX_(0)
#define APP_COLOR_BLACK                 GUI_BLACK//PALETTE_TEXT_COLOUR_INDEX_(1)
#define APP_COLOR_YELLOW                GUI_YELLOW//PALETTE_TEXT_COLOUR_INDEX_(2)
#define APP_COLOR_GREEN                 GUI_GREEN//PALETTE_TEXT_COLOUR_INDEX_(3)



__s32  com_set_palette_text_color(__u32 *palette, __u32 text_color_index,
                                  __u32 gui_color);

__s32 com_set_palette(void *palette_buf);

__s32 com_set_palette_by_id(__s32 ID);

__s32 com_set_palette_color_by_index(__u32 palette_index, const __u32 *buf, __u32 nr);

//创建图层, rect is screen region
H_LYR com_layer_create(RECT *rect, __s32 pipe, __pixel_rgbfmt_t rgb_fmt, __gui_lyrwin_sta_t initsta, char *lyr_name);
//销毁图层
#define com_layer_destroy(_layer)   if (_layer){GUI_LyrWinDelete(_layer); _layer = NULL;}

/////////////////////////DEBUG_APP_MEM////////////////////////////////////
#if DEBUG_APP_MEM

#define app_com_layer_create(_layer, _rect, _pipe, _rgb_fmt, _initsta, lyr_name)  do \
    { \
        _layer = com_layer_create(_rect, _pipe, _rgb_fmt, _initsta, lyr_name); \
        AddDebugAPPMEM(_layer, __LINE__, _T(__FILE__)); \
    } while (0)

#define app_com_layer_destroy(_layer)   if (_layer){SubDebugAPPMEM(_layer); GUI_LyrWinDelete(_layer); _layer = NULL;}

#else//no debug

#define app_com_layer_create(_layer, _rect, _pipe, _rgb_fmt, _initsta, lyr_name)  do \
    { \
        _layer = com_layer_create(_rect, _pipe, _rgb_fmt, _initsta, lyr_name); \
    } while (0)

#define app_com_layer_destroy(_layer)   com_layer_destroy(_layer)

#endif //DEBUG_APP_MEM

#define app_com_layer_create_default(_layer, _prt, _name)  \
    app_com_layer_create(_layer, _prt, 1, PIXEL_COLOR_ARGB8888, GUI_LYRWIN_STA_SUSPEND, _name)
#define app_com_calendar_layer_create_default(_layer, _prt, _name)  \
    app_com_layer_create(_layer, _prt, 0, PIXEL_COLOR_ARGB8888, GUI_LYRWIN_STA_SUSPEND, _name)
#define app_fm_layer_create_default(_layer, _prt, _name)  \
    app_com_layer_create(_layer, _prt, 1, PIXEL_COLOR_ARGB8888, GUI_LYRWIN_STA_SUSPEND, _name)  //..
#define app_photo_layer_create_default(_layer, _prt, _name)  \
    app_com_layer_create(_layer, _prt, 1, PIXEL_COLOR_ARGB8888, GUI_LYRWIN_STA_SUSPEND, _name)  //..
//////////////////////////////////////////////////////////////////////////


#endif//__COM_PALETTE_H_
