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
#ifndef __PHOTO_UIPARA_H__6E2A6309_5D59_44E8_8741_E78774A3EB7C__
#define __PHOTO_UIPARA_H__6E2A6309_5D59_44E8_8741_E78774A3EB7C__

#include "beetles_app.h"
#include "photo_adapter.h"

// main menu id
enum
{
    ID_MMENU_EXIT = 0,
    ID_MMENU_BG_MUSIC,
#if 0
    ID_MMENU_ZOOM,

    ID_MMENU_SLIDESHOW,
#endif
    ID_MMENU_FILEINFO,
    ID_MMENU_AUTOPLAY,
    ID_MMENU_INTERVAL,
    ID_MMENU_SAVE_BG,
    ID_MMENU_ROTATE,//..
    //  ID_MMENU_SCALE,
    ID_MAX_MMENU,
};

#define PHOTO_MMENU_COUNT              ID_MAX_MMENU // main menu total item count
#define PHOTO_MMENU_PAGE_NUM           6 // (main menu) item count of a page
#define PHOTO_SMENU_PAGE_NUM           6 // (sub menu) item count of a page

//////////////////////////////////////////////////
// typedef struct
// {
//
// }PHOTO_SCALE_UI_T;

//file info ui para
typedef struct
{
    TOUCH_UI_RECT        rt;           // 位置/大小
    TOUCH_UI_POS         icon;         // icon 位置
    TOUCH_UI_POS         txt;          // text 位置
    TOUCH_UI_INT         filename_sp;  // text file name space
    TOUCH_UI_INT         txt_sp;       // text line space
    TOUCH_UI_INT         txt_line_w;   // text line width
} PHOTO_INFO_UI_T;

//play state icon ui para
typedef struct
{
    TOUCH_UI_RECT        rt;           // 位置/大小
} PHOTO_STATE_UI_T;

//背景ui参数
typedef struct
{
    PHOTO_INFO_UI_T      file_info;
    PHOTO_STATE_UI_T     state;
} PHOTO_BG_UI_T;

//////////////////////////////////////////
#define COLOUR_BG_8BPP                  BEETLES_BLACK_COLOR
#define COLOUR_EDIT_BG_8BPP             BEETLES_GRAY_COLOR

#define COLOUR_8BPP_INDEX_0             0xF0
#define COLOUR_8BPP_INDEX_(_i)          (COLOUR_8BPP_INDEX_0 + (_i))

//ui颜色参数
typedef struct
{
    __s32 txt_n;
    __s32 txt_f;
} PHOTO_UI_COLOUR_T;

//////////////////////////////////////////
//ui参数
typedef struct
{
    RECT                    rt;
    PHOTO_UI_COLOUR_T       colour;
    PHOTO_BG_UI_T           bg;
} PHOTO_UI_PARA_T;

//////////////////////////////////////////////////////////////////////////
extern PHOTO_UI_PARA_T *get_photo_uipara(void);


#endif //__PHOTO_UIPARA_H__6E2A6309_5D59_44E8_8741_E78774A3EB7C__
//End of this file
