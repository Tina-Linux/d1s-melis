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
#ifndef __RECORD_LIST_VIEW_H__
#define __RECORD_LIST_VIEW_H__

#include "record_ui.h"
#include "record_list_view_doc.h"


typedef enum RECORD_LIST_FOCUSE
{
    RECORD_MUSIC_LIST,
    RECORD_MUSIC_MENU,
    RECORD_MUSIC_DELETE,
    RECORD_MUSIC_VOLUME

} RECORD_LIST_FOCUSE;

typedef enum RECORD_DELETE_STATE
{
    RECORD_DEL_NONE,
    RECORD_DEL_OK,
    RECORD_DEL_CAL
} RECORD_DELETE_STATE;

/*typedef struct tag_recordlist_theme_res_s//....
{
    __s32   res_id;
    HTHEME  h_bmp_handle;
    void*   res_buffer;

}recordlist_theme_res_t;
typedef enum tag_recordlist_nbmp_theme_res_s//..
{
     NMENU_BACK,
     NMENU_PRE,
     NMENU_PLAY,
     NMENU_PAUSE,
     NMENU_NEXT,
     NMENU_DEL,
     NMENU_MAX,
}recordlist_NBMP_theme_res_e;

typedef enum tag_recordlist_nbmp_theme_res_s//..
{
     FMENU_BACK,
     FMENU_PRE,
     FMENU_PLAY,
     FMENU_PAUSE,
     FMENU_NEXT,
     FMENU_DEL,
     FMENU_MAX,
}recordlist_FBMP_theme_res_e;


typedef struct tag_recordlist_menu_ctrl//..
{

  recordlist_theme_res_t   recordlist_nmenu[NMENU_MAX];
  recordlist_theme_res_t   recordlist_fmenu[FMENU_MAX];
}recordlist_menu_ctrl_t;*/


typedef struct tag_record_list_view
{
    precord_ui_t        pui;

    GUI_FONT            *sys_font;
    H_WIN               layer;

    MUSIC_LIST_MENU     curMenu;
    MUSIC_LIST_MENU     playMenu;
    FOCUSE_STATE        curMenuSt;

    RECORD_LIST_FOCUSE  oldPart;
    RECORD_LIST_FOCUSE  curPart;
    __u32               timeCnt;

    RECORD_DELETE_STATE delState;

    precord_list_doc_t  precord_list_doc;

    H_LBAR      listbar_handle;

    __hdle      long_str_handle;
    __show_info_move_t  show_info;
    //..recordlist_menu_ctrl_t   rlmenu_ctrl;//..

} record_list_view_t, *precord_list_view_t;





extern __s32 record_list_view_musicList_init(precord_list_view_t prec_list_view, H_WIN framewin);

extern __s32 record_list_view_musicList_UnInit(precord_list_view_t prec_list_view);

extern void record_list_view_start_show(precord_list_view_t prec_list_view, H_WIN layer);
extern void record_list_view_show(precord_list_view_t prec_list_view, H_WIN layer);
extern void record_list_icon_view_show(precord_list_view_t prec_list_view, H_WIN layer);


















#endif//__RECORD_LIST_VIEW_H__

