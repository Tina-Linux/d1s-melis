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
#include <string.h>
#include <log.h>
#include "page_com.h"
#include "ebook_uipara.h"
#include "common/misc.h"

//#define g_memset  memset

void *My_Malloc(void *heap, __u32 size);

#undef SCREEN_HEIGHT
#define SCREEN_HEIGHT       272

static __u8 paly_flag = 0;

typedef struct tag_ebook_page_ctrl
{
    __page_create_para_t *page_para ;

    __hdle  LongString_hdl ;        //长字符串句柄
    __s32   page_sta ;          // 当前电子书播放状态 , 1 表示正在播放, 0  表示手动播放
    __s32   txt_color ;
    __pos_t win_pos ;
    __u32   key_cnt ;
    __bool  page_no_progress ;  //进度条显示标志 , 1 表示正在显示
    ebook_theme_res_t       page_res_bmp[PAGE_BMP_MAX];



} ebook_page_ctrl_t;

typedef struct tag_ebook_goto_ctrl//..
{

    ebook_theme_res_t       goto_res_nbmp[GOTO_NBMP_MAX];
    ebook_theme_res_t       goto_res_fbmp[GOTO_FBMP_MAX];
    ebook_theme_res_t       goto_bottom_bmp;
    ebook_theme_res_t       goto_bg_bmp;
    ebook_theme_res_t       goto_page_bottom;
    __u32      cur_key;
    __u32      goto_page;

} ebook_goto_ctrl_t;


__s32 ebook_page_com_long_string_init(ebook_page_ctrl_t *page_ctrl)
{
    if (page_ctrl->LongString_hdl == NULL)
    {
        __show_info_move_t show_info;
        ebook_uipara_t *ebook_uipara = NULL;
        ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();
        eLIBs_memset(&show_info, 0, sizeof(__show_info_move_t));
        show_info.hlayer = page_ctrl->page_para->page_layer ;
        show_info.alpha_en = EPDK_FALSE;
        show_info.pFont = NULL;
        show_info.string = "";
        show_info.fontColor = APP_EBOOK_FCS_TXT_COLOR ;
        show_info.align = GUI_TA_LEFT | GUI_TA_VCENTER;
        show_info.bmp = page_ctrl->page_res_bmp[PAGE_NAME_ROLL].res_buffer;
        show_info.bmp_pos.x = ebook_uipara->page_name_pos.x;
        show_info.bmp_pos.y = ebook_uipara->page_name_pos.y;
        page_ctrl->LongString_hdl = GUI_LongStringCreate(&show_info);

        if (page_ctrl->LongString_hdl == NULL)
        {
            __wrn("create long string failed!\n");
        }
    }
    else
    {
        __msg("long string scroll has already started!\n");
    }

    return EPDK_OK;
}

__s32 ebook_page_com_long_string_uninit(ebook_page_ctrl_t *page_ctrl)
{
    if (page_ctrl->LongString_hdl != NULL)
    {
        GUI_LongStringDelete(page_ctrl->LongString_hdl);
        page_ctrl->LongString_hdl = NULL;
    }

    return EPDK_OK;
}

__s32 ebook_page_com_long_string_stop(ebook_page_ctrl_t *page_ctrl)
{
    if (page_ctrl->LongString_hdl != NULL)
    {
        GUI_LongStringStop(page_ctrl->LongString_hdl);
    }

    return EPDK_OK;
}

__s32 ebook_page_com_long_string_start(ebook_page_ctrl_t *page_ctrl)
{
    if (page_ctrl->LongString_hdl != NULL)
    {
        GUI_LongStringStart(page_ctrl->LongString_hdl);
    }

    return EPDK_OK;
}

static void ebook_page_init(ebook_page_ctrl_t *page_ctrl)
{
    __inf("Enter:%s\r\n", __FUNCTION__);
    eLIBs_memset((void *)page_ctrl->page_res_bmp,  0, sizeof(page_ctrl->page_res_bmp));
    // page_ctrl->page_res_bmp[PAGE_BOTTOM_BMP].res_id = ID_EBOOK_PAGE_BOTTOM_BMP ;
    page_ctrl->page_res_bmp[PAGE_BOTTOM_BMP].res_id = ID_EBOOK_PAGE_BOTTOM_BMP ;
    page_ctrl->page_res_bmp[PAGE_BOTTOM_BMP].h_bmp_handle = theme_open(page_ctrl->page_res_bmp[PAGE_BOTTOM_BMP].res_id);
    page_ctrl->page_res_bmp[PAGE_BOTTOM_BMP].res_buffer = theme_hdl2buf(page_ctrl->page_res_bmp[PAGE_BOTTOM_BMP].h_bmp_handle);
    page_ctrl->page_res_bmp[PAGE_PLAY_BUTTON_BMP].res_id = ID_EBOOK_PLAY_BMP ;
    page_ctrl->page_res_bmp[PAGE_PLAY_BUTTON_BMP].h_bmp_handle = theme_open(page_ctrl->page_res_bmp[PAGE_PLAY_BUTTON_BMP].res_id);
    page_ctrl->page_res_bmp[PAGE_PLAY_BUTTON_BMP].res_buffer = theme_hdl2buf(page_ctrl->page_res_bmp[PAGE_PLAY_BUTTON_BMP].h_bmp_handle);
    page_ctrl->page_res_bmp[PAGE_PAUSE_BUTTON_BMP].res_id = ID_EBOOK_PAUSE_BMP;
    page_ctrl->page_res_bmp[PAGE_PAUSE_BUTTON_BMP].h_bmp_handle = theme_open(page_ctrl->page_res_bmp[PAGE_PAUSE_BUTTON_BMP].res_id);
    page_ctrl->page_res_bmp[PAGE_PAUSE_BUTTON_BMP].res_buffer = theme_hdl2buf(page_ctrl->page_res_bmp[PAGE_PAUSE_BUTTON_BMP].h_bmp_handle);
    page_ctrl->page_res_bmp[PAGE_PROGRESS_BODY].res_id = ID_EBOOK_BODY_BMP;
    page_ctrl->page_res_bmp[PAGE_PROGRESS_BODY].h_bmp_handle = theme_open(page_ctrl->page_res_bmp[PAGE_PROGRESS_BODY].res_id);
    page_ctrl->page_res_bmp[PAGE_PROGRESS_BODY].res_buffer = theme_hdl2buf(page_ctrl->page_res_bmp[PAGE_PROGRESS_BODY].h_bmp_handle);
    page_ctrl->page_res_bmp[PAGE_PROGRESS_HEAD].res_id = ID_EBOOK_HEAD_BMP;
    page_ctrl->page_res_bmp[PAGE_PROGRESS_HEAD].h_bmp_handle = theme_open(page_ctrl->page_res_bmp[PAGE_PROGRESS_HEAD].res_id);
    page_ctrl->page_res_bmp[PAGE_PROGRESS_HEAD].res_buffer = theme_hdl2buf(page_ctrl->page_res_bmp[PAGE_PROGRESS_HEAD].h_bmp_handle);
    page_ctrl->page_res_bmp[PAGE_PROGRESS_DOT].res_id = ID_EBOOK_DOT_BMP;
    page_ctrl->page_res_bmp[PAGE_PROGRESS_DOT].h_bmp_handle = theme_open(page_ctrl->page_res_bmp[PAGE_PROGRESS_DOT].res_id);
    page_ctrl->page_res_bmp[PAGE_PROGRESS_DOT].res_buffer = theme_hdl2buf(page_ctrl->page_res_bmp[PAGE_PROGRESS_DOT].h_bmp_handle);
    page_ctrl->page_res_bmp[PAGE_NAME_ROLL].res_id = ID_EBOOK_NAME_ROLL_BMP;
    page_ctrl->page_res_bmp[PAGE_NAME_ROLL].h_bmp_handle = theme_open(page_ctrl->page_res_bmp[PAGE_NAME_ROLL].res_id);
    page_ctrl->page_res_bmp[PAGE_NAME_ROLL].res_buffer = theme_hdl2buf(page_ctrl->page_res_bmp[PAGE_NAME_ROLL].h_bmp_handle);
}

static void ebook_goto_init(ebook_goto_ctrl_t *goto_ctrl)//..初始化资源
{
    eLIBs_memset((void *)&goto_ctrl->goto_bottom_bmp,  0, sizeof(goto_ctrl->goto_bottom_bmp));
    eLIBs_memset((void *)goto_ctrl->goto_res_nbmp,  0, sizeof(goto_ctrl->goto_res_nbmp));
    eLIBs_memset((void *)goto_ctrl->goto_res_fbmp,  0, sizeof(goto_ctrl->goto_res_fbmp));
    eLIBs_memset((void *)&goto_ctrl->goto_bg_bmp,  0, sizeof(goto_ctrl->goto_bg_bmp));
    goto_ctrl->goto_bg_bmp.res_id = ID_EBOOK_GOTO_BG_BMP ;
    goto_ctrl->goto_bg_bmp.h_bmp_handle = theme_open(goto_ctrl->goto_bg_bmp.res_id);
    goto_ctrl->goto_bg_bmp.res_buffer = theme_hdl2buf(goto_ctrl->goto_bg_bmp.h_bmp_handle);
    goto_ctrl->goto_bottom_bmp.res_id = ID_EBOOK_GOTO_BOTTOM_BMP ;
    goto_ctrl->goto_bottom_bmp.h_bmp_handle = theme_open(goto_ctrl->goto_bottom_bmp.res_id);
    goto_ctrl->goto_bottom_bmp.res_buffer = theme_hdl2buf(goto_ctrl->goto_bottom_bmp.h_bmp_handle);
    goto_ctrl->goto_page_bottom.res_id = ID_EBOOK_SHOW_GOTO_PAGE_BMP;
    goto_ctrl->goto_page_bottom.h_bmp_handle = theme_open(goto_ctrl->goto_page_bottom.res_id);
    goto_ctrl->goto_page_bottom.res_buffer = theme_hdl2buf(goto_ctrl->goto_page_bottom.h_bmp_handle);
    goto_ctrl->goto_res_nbmp[GOTO_0N].res_id = ID_EBOOK_0N_BMP ;
    goto_ctrl->goto_res_nbmp[GOTO_0N].h_bmp_handle = theme_open(goto_ctrl->goto_res_nbmp[GOTO_0N].res_id);
    goto_ctrl->goto_res_nbmp[GOTO_0N].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_nbmp[GOTO_0N].h_bmp_handle);
    goto_ctrl->goto_res_nbmp[GOTO_1N].res_id = ID_EBOOK_1N_BMP ;
    goto_ctrl->goto_res_nbmp[GOTO_1N].h_bmp_handle = theme_open(goto_ctrl->goto_res_nbmp[GOTO_1N].res_id);
    goto_ctrl->goto_res_nbmp[GOTO_1N].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_nbmp[GOTO_1N].h_bmp_handle);
    goto_ctrl->goto_res_nbmp[GOTO_2N].res_id = ID_EBOOK_2N_BMP ;
    goto_ctrl->goto_res_nbmp[GOTO_2N].h_bmp_handle = theme_open(goto_ctrl->goto_res_nbmp[GOTO_2N].res_id);
    goto_ctrl->goto_res_nbmp[GOTO_2N].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_nbmp[GOTO_2N].h_bmp_handle);
    goto_ctrl->goto_res_nbmp[GOTO_3N].res_id = ID_EBOOK_3N_BMP;
    goto_ctrl->goto_res_nbmp[GOTO_3N].h_bmp_handle = theme_open(goto_ctrl->goto_res_nbmp[GOTO_3N].res_id);
    goto_ctrl->goto_res_nbmp[GOTO_3N].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_nbmp[GOTO_3N].h_bmp_handle);
    goto_ctrl->goto_res_nbmp[GOTO_4N].res_id = ID_EBOOK_4N_BMP;
    goto_ctrl->goto_res_nbmp[GOTO_4N].h_bmp_handle = theme_open(goto_ctrl->goto_res_nbmp[GOTO_4N].res_id);
    goto_ctrl->goto_res_nbmp[GOTO_4N].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_nbmp[GOTO_4N].h_bmp_handle);
    goto_ctrl->goto_res_nbmp[GOTO_5N].res_id = ID_EBOOK_5N_BMP;
    goto_ctrl->goto_res_nbmp[GOTO_5N].h_bmp_handle = theme_open(goto_ctrl->goto_res_nbmp[GOTO_5N].res_id);
    goto_ctrl->goto_res_nbmp[GOTO_5N].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_nbmp[GOTO_5N].h_bmp_handle);
    goto_ctrl->goto_res_nbmp[GOTO_6N].res_id = ID_EBOOK_6N_BMP;
    goto_ctrl->goto_res_nbmp[GOTO_6N].h_bmp_handle = theme_open(goto_ctrl->goto_res_nbmp[GOTO_6N].res_id);
    goto_ctrl->goto_res_nbmp[GOTO_6N].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_nbmp[GOTO_6N].h_bmp_handle);
    goto_ctrl->goto_res_nbmp[GOTO_7N].res_id = ID_EBOOK_7N_BMP ;
    goto_ctrl->goto_res_nbmp[GOTO_7N].h_bmp_handle = theme_open(goto_ctrl->goto_res_nbmp[GOTO_7N].res_id);
    goto_ctrl->goto_res_nbmp[GOTO_7N].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_nbmp[GOTO_7N].h_bmp_handle);
    goto_ctrl->goto_res_nbmp[GOTO_8N].res_id = ID_EBOOK_8N_BMP ;
    goto_ctrl->goto_res_nbmp[GOTO_8N].h_bmp_handle = theme_open(goto_ctrl->goto_res_nbmp[GOTO_8N].res_id);
    goto_ctrl->goto_res_nbmp[GOTO_8N].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_nbmp[GOTO_8N].h_bmp_handle);
    goto_ctrl->goto_res_nbmp[GOTO_9N].res_id = ID_EBOOK_9N_BMP ;
    goto_ctrl->goto_res_nbmp[GOTO_9N].h_bmp_handle = theme_open(goto_ctrl->goto_res_nbmp[GOTO_9N].res_id);
    goto_ctrl->goto_res_nbmp[GOTO_9N].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_nbmp[GOTO_9N].h_bmp_handle);
    goto_ctrl->goto_res_nbmp[GOTO_DELN].res_id = ID_EBOOK_DEL_N_BMP ;
    goto_ctrl->goto_res_nbmp[GOTO_DELN].h_bmp_handle = theme_open(goto_ctrl->goto_res_nbmp[GOTO_DELN].res_id);
    goto_ctrl->goto_res_nbmp[GOTO_DELN].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_nbmp[GOTO_DELN].h_bmp_handle);
    goto_ctrl->goto_res_nbmp[GOTO_ENTERN].res_id = ID_EBOOK_ENTER_NF_BMP ;
    goto_ctrl->goto_res_nbmp[GOTO_ENTERN].h_bmp_handle = theme_open(goto_ctrl->goto_res_nbmp[GOTO_ENTERN].res_id);
    goto_ctrl->goto_res_nbmp[GOTO_ENTERN].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_nbmp[GOTO_ENTERN].h_bmp_handle);
    goto_ctrl->goto_res_nbmp[GOTO_QUITN].res_id = ID_EBOOK_QUIT_NF_BMP ;
    goto_ctrl->goto_res_nbmp[GOTO_QUITN].h_bmp_handle = theme_open(goto_ctrl->goto_res_nbmp[GOTO_QUITN].res_id);
    goto_ctrl->goto_res_nbmp[GOTO_QUITN].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_nbmp[GOTO_QUITN].h_bmp_handle);
    //..goto_res_fbmp
    goto_ctrl->goto_res_fbmp[GOTO_0F].res_id = ID_EBOOK_0F_BMP ;
    goto_ctrl->goto_res_fbmp[GOTO_0F].h_bmp_handle = theme_open(goto_ctrl->goto_res_fbmp[GOTO_0F].res_id);
    goto_ctrl->goto_res_fbmp[GOTO_0F].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_fbmp[GOTO_0F].h_bmp_handle);
    goto_ctrl->goto_res_fbmp[GOTO_1F].res_id = ID_EBOOK_1F_BMP ;
    goto_ctrl->goto_res_fbmp[GOTO_1F].h_bmp_handle = theme_open(goto_ctrl->goto_res_fbmp[GOTO_1F].res_id);
    goto_ctrl->goto_res_fbmp[GOTO_1F].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_fbmp[GOTO_1F].h_bmp_handle);
    goto_ctrl->goto_res_fbmp[GOTO_2F].res_id = ID_EBOOK_2F_BMP ;
    goto_ctrl->goto_res_fbmp[GOTO_2F].h_bmp_handle = theme_open(goto_ctrl->goto_res_fbmp[GOTO_2F].res_id);
    goto_ctrl->goto_res_fbmp[GOTO_2F].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_fbmp[GOTO_2F].h_bmp_handle);
    goto_ctrl->goto_res_fbmp[GOTO_3F].res_id = ID_EBOOK_3F_BMP ;
    goto_ctrl->goto_res_fbmp[GOTO_3F].h_bmp_handle = theme_open(goto_ctrl->goto_res_fbmp[GOTO_3F].res_id);
    goto_ctrl->goto_res_fbmp[GOTO_3F].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_fbmp[GOTO_3F].h_bmp_handle);
    goto_ctrl->goto_res_fbmp[GOTO_4F].res_id = ID_EBOOK_4F_BMP ;
    goto_ctrl->goto_res_fbmp[GOTO_4F].h_bmp_handle = theme_open(goto_ctrl->goto_res_fbmp[GOTO_4F].res_id);
    goto_ctrl->goto_res_fbmp[GOTO_4F].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_fbmp[GOTO_4F].h_bmp_handle);
    goto_ctrl->goto_res_fbmp[GOTO_5F].res_id = ID_EBOOK_5F_BMP ;
    goto_ctrl->goto_res_fbmp[GOTO_5F].h_bmp_handle = theme_open(goto_ctrl->goto_res_fbmp[GOTO_5F].res_id);
    goto_ctrl->goto_res_fbmp[GOTO_5F].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_fbmp[GOTO_5F].h_bmp_handle);
    goto_ctrl->goto_res_fbmp[GOTO_6F].res_id = ID_EBOOK_6F_BMP ;
    goto_ctrl->goto_res_fbmp[GOTO_6F].h_bmp_handle = theme_open(goto_ctrl->goto_res_fbmp[GOTO_6F].res_id);
    goto_ctrl->goto_res_fbmp[GOTO_6F].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_fbmp[GOTO_6F].h_bmp_handle);
    goto_ctrl->goto_res_fbmp[GOTO_7F].res_id = ID_EBOOK_7F_BMP ;
    goto_ctrl->goto_res_fbmp[GOTO_7F].h_bmp_handle = theme_open(goto_ctrl->goto_res_fbmp[GOTO_7F].res_id);
    goto_ctrl->goto_res_fbmp[GOTO_7F].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_fbmp[GOTO_7F].h_bmp_handle);
    goto_ctrl->goto_res_fbmp[GOTO_8F].res_id = ID_EBOOK_8F_BMP ;
    goto_ctrl->goto_res_fbmp[GOTO_8F].h_bmp_handle = theme_open(goto_ctrl->goto_res_fbmp[GOTO_8F].res_id);
    goto_ctrl->goto_res_fbmp[GOTO_8F].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_fbmp[GOTO_8F].h_bmp_handle);
    goto_ctrl->goto_res_fbmp[GOTO_9F].res_id = ID_EBOOK_9F_BMP ;
    goto_ctrl->goto_res_fbmp[GOTO_9F].h_bmp_handle = theme_open(goto_ctrl->goto_res_fbmp[GOTO_9F].res_id);
    goto_ctrl->goto_res_fbmp[GOTO_9F].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_fbmp[GOTO_9F].h_bmp_handle);
    goto_ctrl->goto_res_fbmp[GOTO_DELF].res_id = ID_EBOOK_DEL_F_BMP ;
    goto_ctrl->goto_res_fbmp[GOTO_DELF].h_bmp_handle = theme_open(goto_ctrl->goto_res_fbmp[GOTO_DELF].res_id);
    goto_ctrl->goto_res_fbmp[GOTO_DELF].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_fbmp[GOTO_DELF].h_bmp_handle);
    goto_ctrl->goto_res_fbmp[GOTO_ENTERN].res_id = ID_EBOOK_ENTER_F_BMP ;
    goto_ctrl->goto_res_fbmp[GOTO_ENTERN].h_bmp_handle = theme_open(goto_ctrl->goto_res_fbmp[GOTO_ENTERN].res_id);
    goto_ctrl->goto_res_fbmp[GOTO_ENTERN].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_fbmp[GOTO_ENTERN].h_bmp_handle);
    goto_ctrl->goto_res_fbmp[GOTO_QUITN].res_id = ID_EBOOK_QUIT_F_BMP;
    goto_ctrl->goto_res_fbmp[GOTO_QUITN].h_bmp_handle = theme_open(goto_ctrl->goto_res_fbmp[GOTO_QUITN].res_id);
    goto_ctrl->goto_res_fbmp[GOTO_QUITN].res_buffer = theme_hdl2buf(goto_ctrl->goto_res_fbmp[GOTO_QUITN].h_bmp_handle);
}

static void ebook_goto_uninit(ebook_goto_ctrl_t *goto_ctrl)//..关闭资源
{
    theme_close(goto_ctrl->goto_bg_bmp.h_bmp_handle);
    theme_close(goto_ctrl->goto_bottom_bmp.h_bmp_handle);
    theme_close(goto_ctrl->goto_page_bottom.h_bmp_handle);
    //..goto_res_nbmp
    theme_close(goto_ctrl->goto_res_nbmp[GOTO_0N].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_nbmp[GOTO_1N].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_nbmp[GOTO_2N].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_nbmp[GOTO_3N].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_nbmp[GOTO_4N].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_nbmp[GOTO_5N].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_nbmp[GOTO_6N].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_nbmp[GOTO_7N].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_nbmp[GOTO_8N].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_nbmp[GOTO_9N].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_nbmp[GOTO_DELN].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_nbmp[GOTO_ENTERN].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_nbmp[GOTO_QUITN].h_bmp_handle);
    //..goto_res_fbmp
    theme_close(goto_ctrl->goto_res_fbmp[GOTO_0F].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_fbmp[GOTO_1F].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_fbmp[GOTO_2F].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_fbmp[GOTO_3F].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_fbmp[GOTO_4F].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_fbmp[GOTO_5F].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_fbmp[GOTO_6F].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_fbmp[GOTO_7F].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_fbmp[GOTO_8F].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_fbmp[GOTO_9F].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_fbmp[GOTO_DELF].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_fbmp[GOTO_ENTERF].h_bmp_handle);
    theme_close(goto_ctrl->goto_res_fbmp[GOTO_QUITF].h_bmp_handle);
}


static void ebook_page_uninit(ebook_page_ctrl_t *page_ctrl)
{
    theme_close(page_ctrl->page_res_bmp[PAGE_BOTTOM_BMP].h_bmp_handle);
    theme_close(page_ctrl->page_res_bmp[PAGE_PLAY_BUTTON_BMP].h_bmp_handle);
    theme_close(page_ctrl->page_res_bmp[PAGE_PAUSE_BUTTON_BMP].h_bmp_handle);
    theme_close(page_ctrl->page_res_bmp[PAGE_PROGRESS_BODY].h_bmp_handle);
    theme_close(page_ctrl->page_res_bmp[PAGE_PROGRESS_DOT].h_bmp_handle);
    theme_close(page_ctrl->page_res_bmp[PAGE_PROGRESS_HEAD].h_bmp_handle);
    theme_close(page_ctrl->page_res_bmp[PAGE_NAME_ROLL].h_bmp_handle);
    eLIBs_memset((void *)page_ctrl->page_res_bmp,  0, sizeof(page_ctrl->page_res_bmp));
}

/*
************************************************************************************************************************
*
*Description: paint current play book name
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static __s32 page_paint_ebook_name(__gui_msg_t *msg)
{
    ebook_page_ctrl_t *page_ctrl;
    ebook_uipara_t *ebook_uipara;
    __show_info_move_t     show_info;
    GUI_RECT    gui_rect;
    __inf("Enter %s\r\n", __FUNCTION__);

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
    {
        __wrn("LyrWin GUI_LYRWIN_STA_SUSPEND\r\n");
        return EPDK_FAIL;
    }

    page_ctrl = (ebook_page_ctrl_t *) GUI_WinGetAddData(msg->h_deswin) ;
    GUI_LyrWinSel(page_ctrl->page_para->page_layer);
    GUI_SetFont(page_ctrl->page_para->font);
    GUI_SetColor(APP_COLOR_YELLOW);
    GUI_SetBkColor(0xF0);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();
    GUI_LyrWinCacheOn();
    gui_rect.x0 = ebook_uipara->page_name_pos.x ;
    gui_rect.y0 = ebook_uipara->page_name_pos.y ;
    gui_rect.x1 = gui_rect.x0 + ebook_uipara->page_name_pos.w ;
    gui_rect.y1 = gui_rect.y0 + ebook_uipara->page_name_pos.h ;
    GUI_DispStringInRect(page_ctrl->page_para->name, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    show_info.hlayer        = page_ctrl->page_para->page_layer;
    show_info.region.x      = ebook_uipara->page_name_pos.x;
    show_info.region.y      = ebook_uipara->page_name_pos.y;
    show_info.region.width  = ebook_uipara->page_name_pos.w;
    show_info.region.height = ebook_uipara->page_name_pos.h + 1;
    show_info.encode_id     = EPDK_CHARSET_ENM_UTF8;
    show_info.pFont         = page_ctrl->page_para->font;
    show_info.string        = page_ctrl->page_para->name;
    show_info.bkColor       = 0xF0;
    show_info.fontColor     = APP_COLOR_YELLOW;
    show_info.align         = GUI_TA_LEFT | GUI_TA_VCENTER;
    show_info.bmp           = page_ctrl->page_res_bmp[PAGE_NAME_ROLL].res_buffer;
    show_info.bmp_pos.x     = ebook_uipara->page_name_pos.x;
    show_info.bmp_pos.y  = ebook_uipara->page_name_pos.y;
    GUI_LongStringStop(page_ctrl->LongString_hdl);
    GUI_LongStringReset(page_ctrl->LongString_hdl, &show_info);
    GUI_LongStringStart(page_ctrl->LongString_hdl);
    GUI_LyrWinCacheOff();
    return EPDK_OK;
}

//paint 电子书页数的进度条
static __s32 page_paint_page_no_progress(__gui_msg_t *msg)
{
    ebook_page_ctrl_t *page_ctrl = NULL ;
    ebook_uipara_t *ebook_uipara = NULL ;
    GUI_RECT    gui_rect;
    GUI_MEMDEV_Handle   draw_mem;
    __s32   i  = 0  ;
    __msg("page_paint_page_no_progress........\n");
    page_ctrl = (ebook_page_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (page_ctrl == NULL)
    {
        __msg(" page_ctrl is null, return \n ");
        return EPDK_FAIL ;
    }

    if (GUI_LyrWinGetSta(page_ctrl->page_para->page_num_progress_lyr) != GUI_LYRWIN_STA_ON)
    {
        __msg("page_ctrl->page_para->page_num_progress_lyr is not in on status\n ");
        return EPDK_FAIL;
    }

    ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();
    //__here__;
    GUI_LyrWinSel(page_ctrl->page_para->page_num_progress_lyr);
    draw_mem = GUI_MEMDEV_Create(0, 0, ebook_uipara->page_num_progress_lyr.w, ebook_uipara->page_num_progress_lyr.h);

    if (draw_mem == NULL)
    {
        __wrn("GUI_MEMDEV_Create fail\r\n");
    }

    GUI_MEMDEV_Select(draw_mem);
    GUI_SetFont(page_ctrl->page_para->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetColor(page_ctrl->txt_color);
    GUI_SetBkColor(0xF0);
    GUI_BMP_Draw(page_ctrl->page_res_bmp[PAGE_PROGRESS_BODY].res_buffer, ebook_uipara->page_no_progress_length.x, ebook_uipara->page_no_progress_length.y) ;

    for (i = 0 ; i < page_ctrl->page_para->cur_page * ebook_uipara->page_no_progress_length.h / (ebook_uipara->page_no_dot_length.h * page_ctrl->page_para->total_page) ; i++)
    {
        if (0 == i)
        {
            GUI_BMP_Draw(page_ctrl->page_res_bmp[PAGE_PROGRESS_HEAD].res_buffer, ebook_uipara->page_no_head_length.x, ebook_uipara->page_no_head_length.y) ;
            __msg("ebook_uipara->page_no_head_length.x =%d , ebook_uipara->page_no_head_length.y =%d \n", ebook_uipara->page_no_head_length.x, ebook_uipara->page_no_head_length.y);
        }
        else
        {
            GUI_BMP_Draw(page_ctrl->page_res_bmp[PAGE_PROGRESS_DOT].res_buffer, ebook_uipara->page_no_dot_length.x,  i * ebook_uipara->page_no_dot_length.h + ebook_uipara->page_no_head_length.y) ;
            __msg("pos.x =%d  , pos.y =%d\n", ebook_uipara->page_no_dot_length.x, i * ebook_uipara->page_no_dot_length.h + ebook_uipara->page_no_head_length.y);
        }
    }

    //__here__;
    GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
    return EPDK_OK;
}

static __s32 page_paint_all(__gui_msg_t *msg)
{
    ebook_page_ctrl_t *page_ctrl;
    ebook_uipara_t *ebook_uipara;
    __inf("Enter %s\r\n", __FUNCTION__);

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
    {
        __wrn("LyrWin GUI_LYRWIN_STA_SUSPEND\r\n");
        return EPDK_FAIL;
    }

    page_ctrl = (ebook_page_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (page_ctrl == NULL)
    {
        __wrn("page_ctrl null \r\n");
        return EPDK_FAIL;
    }

    ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();
    {
        GUI_RECT    gui_rect;
        char        buf[128] = {0}, buf1[128] = {0};
        GUI_MEMDEV_Handle   draw_mem;
        GUI_LyrWinSel(page_ctrl->page_para->page_layer);
        //GUI_LyrWinSetBottom(page_ctrl->page_para->page_layer);//..
        draw_mem = GUI_MEMDEV_Create(0, 0, ebook_uipara->page_lyr.w, ebook_uipara->page_lyr.h);

        if (NULL == draw_mem)
        {
            __wrn("GUI_MEMDEV_Create fail!\r\n");
            return EPDK_FAIL;
        }

        GUI_MEMDEV_Select(draw_mem);
        GUI_SetFont(page_ctrl->page_para->font);
        GUI_UC_SetEncodeUTF8();
        GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
        GUI_SetColor(page_ctrl->txt_color);
        GUI_SetBkColor(GUI_BLACK);
        // GUI_BMP_Draw(page_ctrl->page_res_bmp[PAGE_BOTTOM_BMP].res_buffer, 0 , 0 );
        __wrn("GUI_BMP_Draw PAGE_BG_BMP!\r\n");
        GUI_BMP_Draw(page_ctrl->page_res_bmp[PAGE_BOTTOM_BMP].res_buffer, 0, 0);  //..

        /*paint play pause
            gui_rect.x0 = ebook_uipara->bmp_play.x;
            gui_rect.y0 = ebook_uipara->bmp_play.y;
            gui_rect.x1 = gui_rect.x0 + ebook_uipara->bmp_play.w-1;
            gui_rect.y1 = gui_rect.y0 + ebook_uipara->bmp_play.h-1;*/

        if (page_ctrl->page_para->play_status == 0)
        {
            GUI_BMP_Draw(page_ctrl->page_res_bmp[PAGE_PLAY_BUTTON_BMP].res_buffer, ebook_uipara->bmp_play.x, ebook_uipara->bmp_play.y);
        }
        else
        {
            GUI_BMP_Draw(page_ctrl->page_res_bmp[PAGE_PAUSE_BUTTON_BMP].res_buffer, ebook_uipara->bmp_play.x, ebook_uipara->bmp_play.y);
        }

        //page no
        gui_rect.x0 = ebook_uipara->page_no_pos.x;
        gui_rect.y0 = ebook_uipara->page_no_pos.y;
        gui_rect.x1 = gui_rect.x0 + ebook_uipara->page_no_pos.w - 1;
        gui_rect.y1 = gui_rect.y0 + ebook_uipara->page_no_pos.h - 1;
        //GUI_ClearRect(gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
        app_sprintf(buf, "[%d/%d]", page_ctrl->page_para->cur_page, page_ctrl->page_para->total_page);
        GUI_DispStringInRect(buf, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
        GUI_MEMDEV_CopyToLCD(draw_mem);
        GUI_MEMDEV_Select(NULL);
        GUI_MEMDEV_Delete(draw_mem);
        draw_mem = NULL;
        page_paint_ebook_name(msg);
    }
    return EPDK_OK;
}


/*
************************************************************************************************************************
*                                       _epdf_on_touch
*
*Description:
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
#if 0
static __s32 page_on_touch(__gui_msg_t *msg)
{
    ebook_page_ctrl_t *page_ctrl = NULL;
    ebook_uipara_t *ebook_uipara;
    //static __u32 ebook_touch_flag = 0;
    ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();
    page_ctrl = (ebook_page_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    __wrn("page com touch message\n");
    __wrn("msg->dwAddData1 = %d, page_ctrl->page_sta = %d\n", msg->dwAddData1, page_ctrl->page_sta);
    __wrn("pos.x %d  pos.y  = %d\n", LOSWORD(msg->dwAddData2), HISWORD(msg->dwAddData2));
    __wrn("**msg->dwAddData1=0x%x\n", msg->dwAddData1);

    if (msg->dwAddData1 == GUI_MSG_TOUCH_DOWN
        || msg->dwAddData1 == GUI_MSG_TOUCH_OVERDOWN)
    {
        if (GUI_WinGetCaptureWin() != msg->h_deswin)
        {
            GUI_WinSetCaptureWin(msg->h_deswin);
        }
    }
    else if (msg->dwAddData1 == GUI_MSG_TOUCH_UP
             || msg->dwAddData1 == GUI_MSG_TOUCH_OVERUP)
    {
        if (GUI_WinGetCaptureWin() == msg->h_deswin)
        {
            GUI_WinReleaseCapture();
        }
    }

    switch (msg->dwAddData1)
    {
        case GUI_MSG_TOUCH_DOWN:
        case GUI_MSG_TOUCH_MOVE:
        case GUI_MSG_TOUCH_LONGDOWN:
        case GUI_MSG_TOUCH_OVERDOWN:
        case GUI_MSG_TOUCH_OVERLONGDOWN:
        {
            //ebook_touch_flag++;
            {
                //ebook_touch_flag = ebook_touch_flag%4;
                if (page_ctrl->page_sta == 0)
                {
                    page_ctrl->win_pos.x = LOSWORD(msg->dwAddData2);
                    page_ctrl->win_pos.y = HISWORD(msg->dwAddData2);

                    if (msg->h_srcwin != GUI_WinGetParent(msg->h_deswin))
                    {
                        page_ctrl->win_pos.x += ebook_uipara->page_lyr.x;
                        page_ctrl->win_pos.y += ebook_uipara->page_lyr.y;
                    }

                    __msg("-------------------------------------page_ctrl->win_pos.y = %d\n", page_ctrl->win_pos.y);

                    if (page_ctrl->win_pos.y < SCREEN_HEIGHT * 3 / 10)
                    {
                        page_ctrl->page_sta = 1;

                        if (1 == page_ctrl->page_para->play_status)
                        {
                            page_ctrl->page_para->play_status = 1;
                            __msg("-------------------------------------1\n");
                            //if(ebook_touch_flag == 4)
                            {
                                Ebook_cmd2parent_syn(msg->h_deswin, CMD_PAGE_SET_PLAY, page_ctrl->page_para->play_status, 0);   //停止自动播放
                            }
                        }
                    }
                    else if (page_ctrl->win_pos.y > SCREEN_HEIGHT * 7 / 10)
                    {
                        if (page_ctrl->win_pos.y > ebook_uipara->page_lyr.y)
                        {
                            if (page_ctrl->win_pos.x < 50)
                            {
                                __msg("-------------------------------------2\n");
                                page_ctrl->page_sta = 4;
                            }

                            return EPDK_OK;
                        }
                        else
                        {
                            page_ctrl->page_sta = 2;

                            if (1 == page_ctrl->page_para->play_status)
                            {
                                page_ctrl->page_para->play_status = 1;
                                __msg("-------------------------------------3\n");
                                //if(ebook_touch_flag == 4)
                                {
                                    Ebook_cmd2parent_syn(msg->h_deswin, CMD_PAGE_SET_PLAY, page_ctrl->page_para->play_status, 0);   //停止自动播放
                                }
                            }
                        }
                    }
                    else
                    {
                        page_ctrl->page_sta = 3;

                        if (1 == page_ctrl->page_para->play_status)
                        {
                            page_ctrl->page_para->play_status = 1;
                            __msg("-------------------------------------4\n");
                            Ebook_cmd2parent_syn(msg->h_deswin, CMD_PAGE_SET_PLAY, page_ctrl->page_para->play_status, 0);   //停止自动播放
                        }
                    }

                    return EPDK_OK;
                }

                if (1 == page_ctrl->page_sta)
                {
                    page_ctrl->key_cnt++;

                    if (page_ctrl->page_para->cur_page > 0)
                    {
                        if (page_ctrl->page_para->total_page < 50)      //总页数在50页以内, 每按5次按键就减少一页
                        {
                            page_ctrl->page_para->cur_page = page_ctrl->page_para->cur_page - (page_ctrl->key_cnt / 5) ;

                            if (page_ctrl->page_para->cur_page < 1)
                            {
                                page_ctrl->page_para->cur_page = 1 ;
                            }
                        }
                        else if (page_ctrl->page_para->total_page < 1000)       //总页数在1000 页以内, 每按2 次按键就减少一页
                        {
                            page_ctrl->page_para->cur_page = page_ctrl->page_para->cur_page - (page_ctrl->key_cnt / 2) ;

                            if (page_ctrl->page_para->cur_page < 1)
                            {
                                page_ctrl->page_para->cur_page = 1;
                            }
                        }
                        else        //总页数大于 1000 页时 ，没按下一次按键跳转一页
                        {
                            page_ctrl->page_para->cur_page = page_ctrl->page_para->cur_page - page_ctrl->key_cnt ;

                            if (page_ctrl->page_para->cur_page < 1)
                            {
                                page_ctrl->page_para->cur_page = 1 ;
                            }
                        }
                    }

                    //if(ebook_touch_flag == 4)
                    Ebook_cmd2parent_syn(msg->h_deswin, CMD_PAGE_SET_PREV, 0, msg->dwAddData1);// show prev page
                }
                else if (2 == page_ctrl->page_sta)
                {
                    page_ctrl->key_cnt++;

                    if (page_ctrl->page_para->cur_page  < page_ctrl->page_para->total_page)
                    {
                        if (page_ctrl->page_para->total_page < 50)      //总页数在50页以内, 每按5次按键就增加一页
                        {
                            page_ctrl->page_para->cur_page = page_ctrl->page_para->cur_page +  page_ctrl->key_cnt / 5 ;

                            if (page_ctrl->page_para->cur_page > page_ctrl->page_para->total_page)
                            {
                                page_ctrl->page_para->cur_page = page_ctrl->page_para->total_page ;
                            }
                        }
                        else if (page_ctrl->page_para->total_page < 1000)       //总页数在1000 页以内, 每按2 次按键就增加一页
                        {
                            page_ctrl->page_para->cur_page = page_ctrl->page_para->cur_page + (page_ctrl->key_cnt / 2) ;

                            if (page_ctrl->page_para->cur_page > page_ctrl->page_para->total_page)
                            {
                                page_ctrl->page_para->cur_page = page_ctrl->page_para->total_page ;
                            }
                        }
                        else        //总页数大于 1000 页时 ，每按下一次按键跳转一页
                        {
                            page_ctrl->page_para->cur_page = page_ctrl->page_para->cur_page + page_ctrl->key_cnt ;

                            if (page_ctrl->page_para->cur_page > page_ctrl->page_para->total_page)
                            {
                                page_ctrl->page_para->cur_page = page_ctrl->page_para->total_page ;
                            }
                        }
                    }

                    //if(ebook_touch_flag == 4)
                    Ebook_cmd2parent_syn(msg->h_deswin, CMD_PAGE_SET_NEXT, 0, msg->dwAddData1);// show next page
                }

                /*  更新页码*/
                //if(ebook_touch_flag == 4)
                {
                    __gui_msg_t setmsg;
                    setmsg.id = COM_CMD_UPDATE_PAGE ;
                    setmsg.h_deswin = msg->h_deswin ;
                    setmsg.dwAddData1 = page_ctrl->page_para->cur_page ;
                    GUI_SendMessage(&setmsg);
                }

                if (page_ctrl->page_no_progress == 1)
                {
                    __gui_msg_t setmsg;
                    setmsg.id = COM_CMD_SHOW_PROGRESS ;
                    setmsg.h_deswin = msg->h_deswin ;
                    setmsg.dwAddData1 = page_ctrl->page_para->cur_page ;
                    GUI_SendMessage(&setmsg);
                }
            }
        }

        return EPDK_OK;

        case GUI_MSG_TOUCH_UP:
        case GUI_MSG_TOUCH_OVERUP:
        {
            if (page_ctrl->page_sta == 1)
            {
                page_ctrl->key_cnt = 0;
                page_ctrl->page_sta = 0;

                if (1 == page_ctrl->page_no_progress)
                {
                    GUI_LyrWinSetSta(page_ctrl->page_para->page_num_progress_lyr, GUI_LYRWIN_STA_SUSPEND) ;
                    page_ctrl->page_no_progress = 0 ;
                    GUI_LyrWinSetSta(page_ctrl->page_para->page_layer, GUI_LYRWIN_STA_ON);
                    /* 发送消息到主窗口，通知电子书跳转到当前页*/
                    {
                        __gui_msg_t setmsg;
                        setmsg.id = COM_CMD_SEEKPAGE ;
                        setmsg.h_deswin = GUI_WinGetParent(msg->h_deswin) ;
                        setmsg.dwAddData1 = page_ctrl->page_para->cur_page ;
                        GUI_SendMessage(&setmsg);
                    }
                }
                else
                {
                    Ebook_cmd2parent_syn(msg->h_deswin, CMD_PAGE_SET_PREV, 0, msg->dwAddData1);// show prev page
                }
            }
            else if (page_ctrl->page_sta == 2)
            {
                page_ctrl->key_cnt = 0;
                page_ctrl->page_sta = 0;

                if (1 == page_ctrl->page_no_progress)
                {
                    GUI_LyrWinSetSta(page_ctrl->page_para->page_num_progress_lyr, GUI_LYRWIN_STA_SUSPEND) ;
                    page_ctrl->page_no_progress = 0 ;
                    GUI_LyrWinSetSta(page_ctrl->page_para->page_layer, GUI_LYRWIN_STA_ON);
                    /* 发送消息到主窗口，通知电子书跳转到当前页*/
                    {
                        __gui_msg_t setmsg;
                        setmsg.id = COM_CMD_SEEKPAGE ;
                        setmsg.h_deswin = GUI_WinGetParent(msg->h_deswin) ;
                        setmsg.dwAddData1 = page_ctrl->page_para->cur_page ;
                        GUI_SendMessage(&setmsg);
                    }
                }
                else
                {
                    Ebook_cmd2parent_syn(msg->h_deswin, CMD_PAGE_SET_NEXT, 0, msg->dwAddData1);// show next  page
                }
            }
            else if (page_ctrl->page_sta == 3)
            {
                page_ctrl->key_cnt = 0;
                page_ctrl->page_sta = 0;
                Ebook_cmd2parent_syn(msg->h_deswin, CMD_PAGE_SET_SET, 0, 0);
            }
            else if (page_ctrl->page_sta == 4)
            {
                page_ctrl->key_cnt = 0;
                page_ctrl->page_sta = 0;

                if (page_ctrl->page_para->play_status == 0)
                {
                    page_ctrl->page_para->play_status = 1;
                }
                else
                {
                    page_ctrl->page_para->play_status = 0;
                }

                __msg("-------------------------------------\n");
                Ebook_cmd2parent_syn(msg->h_deswin, CMD_PAGE_SET_PLAY, page_ctrl->page_para->play_status, 0);
                page_paint_all(msg) ;
            }
            else
            {
                page_ctrl->key_cnt = 0;
                page_ctrl->page_sta = 0;
            }
        }

        return EPDK_OK;

        default:
            break;
    }

    return EPDK_OK;
}
#endif

static __s32 page_key_proc(__gui_msg_t *msg)
{
    ebook_page_ctrl_t *page_ctrl = NULL;
    static __s32 last_key = -1, key_cnt = 0;
    static __s32 menu_show_flag = 0;// 1 时表示菜单在显示，0 时表示隐藏
    __wrn("aaaaaa menulist longdown 10\n");
    page_ctrl = (ebook_page_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        if ((last_key == GUI_MSG_KEY_ENTER) || (last_key == GUI_MSG_KEY_LONGENTER) || (last_key == GUI_MSG_KEY_PLAY_PAUSE))
        {
            if (key_cnt < 5)
            {
                if (page_ctrl->page_para->play_status == 0)
                {
                    page_ctrl->page_para->play_status = 1;
                }
                else
                {
                    page_ctrl->page_para->play_status = 0;
                }

                Ebook_cmd2parent(msg->h_deswin, CMD_PAGE_SET_PLAY, page_ctrl->page_para->play_status, 0);   //更新播放状态
                page_paint_all(msg) ;
            }
            else
            {
                Ebook_cmd2parent(msg->h_deswin, CMD_PAGE_SET_SET, 0, 0);//连续按enter键时进入设置场景中
            }

            key_cnt = 0;
        }
        else if (/*(last_key == GUI_MSG_KEY_MENU)||*/(last_key == GUI_MSG_KEY_ESCAPE))  //短按menu键，退出应用程序
        {
            Ebook_cmd2parent(msg->h_deswin, CMD_PAGE_SET_CLOSE, 0, 0);
        }
        else if (last_key == GUI_MSG_KEY_MENU) //按menu键弹出菜单
        {
            Ebook_cmd2parent(msg->h_deswin, CMD_PAGE_SET_SET, 0, 0);
        }
    }
    else
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
            case GUI_MSG_KEY_PREV:
            case GUI_MSG_KEY_LONGPREV:
            {
                Ebook_cmd2parent(msg->h_deswin, CMD_PAGE_SET_PREV, 0, 0);//show previous page
            }
            break;

            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_NEXT:
            case GUI_MSG_KEY_LONGNEXT:
            {
                Ebook_cmd2parent(msg->h_deswin, CMD_PAGE_SET_NEXT, 0, 0);// show next page
            }
            break;

            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_LONGLEFT:
            {
                Ebook_cmd2parent(msg->h_deswin, CMD_PAGE_SET_PREV, 0, 0);//show previous page
            }
            break;

            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT://volume add
            {
                Ebook_cmd2parent(msg->h_deswin, CMD_PAGE_SET_NEXT, 0, 0);//show next page
            }
            break;

            case GUI_MSG_KEY_ENTER:
            case GUI_MSG_KEY_LONGENTER:
            {
                key_cnt++;
            }
            break;

            default:
                break;
        }
    }

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        last_key = -1;
    }
    else
    {
        last_key = msg->dwAddData1;
    }

    return EPDK_OK;
}
/*
************************************************************************************************************************
*                                       __cb_page_frmwin
*
*Description:
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static __s32 __cb_page_frmwin(__gui_msg_t *msg)
{
    __inf("Enter %s msg->id:%d\r\n", __FUNCTION__, msg->id);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            ebook_page_ctrl_t *page_ctrl = NULL;
            __page_create_para_t  *page_para = NULL;
            __wrn("+++++++GUI_MSG_CREATE+++++++\r\n");
            page_para = (__page_create_para_t *)GUI_WinGetAttr(msg->h_deswin);
            page_ctrl = (ebook_page_ctrl_t *)My_Malloc(0, sizeof(ebook_page_ctrl_t));

            if (NULL == page_ctrl)
            {
                __err("memory alloc fail......\n");
                return EPDK_FAIL;
            }

            eLIBs_memset(page_ctrl, 0, sizeof(ebook_page_ctrl_t));
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)page_ctrl);
            ebook_page_init(page_ctrl);
            page_ctrl->page_para = page_para;
            page_ctrl->txt_color = APP_EBOOK_FCS_TXT_COLOR;
            ebook_page_com_long_string_init(page_ctrl);
            return EPDK_OK;
        }

        case GUI_MSG_DESTROY:
        {
            ebook_page_ctrl_t *page_ctrl;
            page_ctrl = (ebook_page_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
            __msg("*****page com destroy begin********\n");

            if (!page_ctrl)
            {
                __err("page_ctrl is null ......\n");
                return EPDK_FAIL;
            }

            //__here__;
            if (page_ctrl->page_para->page_num_progress_lyr)
            {
                //__here__;
                GUI_LyrWinDelete(page_ctrl->page_para->page_num_progress_lyr);
                //__here__;
                page_ctrl->page_para->page_num_progress_lyr = NULL  ;
            }

            //__here__;
            ebook_page_com_long_string_uninit(page_ctrl);
            //__here__;
            ebook_page_uninit(page_ctrl);

            //__here__;

            if (page_ctrl->page_para)
            {
                My_Mfree(0, page_ctrl->page_para);
                page_ctrl->page_para = NULL ;
            }

            //__here__;
            if (page_ctrl)
            {
                My_Mfree(0, page_ctrl);
                page_ctrl = NULL ;
            }

            //__here__;
            __msg("*****page com destroy end********\n");
            return EPDK_OK;
        }

        case GUI_MSG_CLOSE:
        {
            Ebook_cmd2parent(msg->h_deswin, CMD_PAGE_SET_CLOSE, 0, 0);
        }

        return EPDK_OK;

        case GUI_MSG_PAINT:
            __wrn("+++++++GUI_MSG_PAINT+++++++\r\n");
            page_paint_all(msg);
            return EPDK_OK;

        case DSK_MSG_ALARM:
        {
            ebook_page_ctrl_t *page_ctrl = NULL;
            return EPDK_OK;
            page_ctrl = (ebook_page_ctrl_t *)My_Malloc(0, sizeof(ebook_page_ctrl_t));

            if (page_ctrl->page_para->play_status == 1)
            {
                paly_flag = 1;
                page_ctrl->page_para->play_status = 0;
                Ebook_cmd2parent(msg->h_deswin, CMD_PAGE_SET_PLAY, page_ctrl->page_para->play_status, 0);
                page_paint_all(msg) ;
            }
            else
            {
                paly_flag = 0;
            }
        }

        return EPDK_OK;

        case ALARM_MSG_CLOSE:
        {
            ebook_page_ctrl_t *page_ctrl = NULL;
            return EPDK_OK;
            page_ctrl = (ebook_page_ctrl_t *)My_Malloc(0, sizeof(ebook_page_ctrl_t));

            if (paly_flag == 1)
            {
                page_ctrl->page_para->play_status = 1;
                Ebook_cmd2parent(msg->h_deswin, CMD_PAGE_SET_PLAY, page_ctrl->page_para->play_status, 0);
                page_paint_all(msg);
            }
        }

        return EPDK_OK;
#if 0

        case GUI_MSG_TOUCH:
            page_on_touch(msg);
            return EPDK_OK;
#endif

        case GUI_MSG_KEY:
            page_key_proc(msg);
            return EPDK_OK;

        case GUI_MSG_TIMER:
        {
            break;
        }

        return EPDK_OK;

        case COM_CMD_UPDATE_PAGE:
        {
            ebook_page_ctrl_t *page_ctrl;
            page_ctrl = (ebook_page_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
            page_ctrl->page_para->cur_page = msg->dwAddData1;
            page_paint_all(msg);
        }

        return EPDK_OK;

        case COM_CMD_SET_PLAY_STATUS:
        {
            ebook_page_ctrl_t *page_ctrl;
            page_ctrl = (ebook_page_ctrl_t *)GUI_WinGetAddData(msg->h_deswin) ;
            page_ctrl->page_para->play_status = msg->dwAddData1;
            page_paint_all(msg);
        }

        return EPDK_OK;

        case COM_CMD_SHOW_PROGRESS:
        {
            ebook_page_ctrl_t *page_ctrl;
            page_ctrl = (ebook_page_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
            page_ctrl->page_para->cur_page = msg->dwAddData1;
            page_ctrl->page_no_progress = 1 ;
            //__here__ ;
            GUI_LyrWinSetSta(page_ctrl->page_para->page_num_progress_lyr, GUI_LYRWIN_STA_ON);
            GUI_LyrWinSetTop(page_ctrl->page_para->page_num_progress_lyr);
            //__here__ ;
            page_paint_page_no_progress(msg);
            //__here__;
            return EPDK_OK ;
        }

        case COM_CMD_LONGSTRING_STOP:
        {
            ebook_page_ctrl_t *page_ctrl;
            page_ctrl = (ebook_page_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
            ebook_page_com_long_string_stop(page_ctrl);
            return EPDK_OK ;
        }

        case COM_CMD_LONGSTRING_START:
        {
            ebook_page_ctrl_t *page_ctrl;
            page_ctrl = (ebook_page_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
            ebook_page_com_long_string_start(page_ctrl);
            return EPDK_OK ;
        }

        case GUI_MSG_COMMAND:
            break;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}


H_WIN page_frmwin_create(H_WIN parent, __page_create_para_t *para)
{
    H_WIN                           h_win;
    __page_create_para_t            *p_para = NULL ;
    __gui_framewincreate_para_t     frm_create_para;
    FB  fb;
    p_para = (__page_create_para_t *)My_Malloc(0, sizeof(__page_create_para_t));

    if (!p_para)
    {
        __err("alloc error \n");
        return 0;
    }

    eLIBs_memset(p_para, 0, sizeof(__page_create_para_t));
    eLIBs_memcpy((void *)p_para, (const void *)para, sizeof(__page_create_para_t));
    GUI_LyrWinGetFB(para->page_layer, &fb);
    g_memset(&frm_create_para, 0, sizeof(__gui_framewincreate_para_t));
    frm_create_para.dwStyle          = WS_VISIBLE;  //设为disabled 主要是不接收触摸消息
    frm_create_para.dwExStyle        = 0;
    frm_create_para.name             = NULL;
    frm_create_para.hOwner           = 0;
    frm_create_para.hHosting         = parent;
    frm_create_para.FrameWinProc     = __cb_page_frmwin;
    frm_create_para.id               = EBOOK_PAGE_ID;
    frm_create_para.hLayer           = p_para->page_layer;
    frm_create_para.rect.x            = 0;
    frm_create_para.rect.y            = 0;
    frm_create_para.rect.width        = fb.size.width;
    frm_create_para.rect.height       = fb.size.height;
    frm_create_para.attr              = (void *)p_para;
    h_win = GUI_FrmWinCreate(&frm_create_para);

    if (!h_win)
    {
        __err("app bar fwin create error ! \n");
    }

    return h_win;
}

__s32 page_frmwin_delete(H_WIN h_win)
{
    if (h_win)
    {
        GUI_FrmWinDelete(h_win);
        h_win = 0;
    }

    return EPDK_OK;
}



/*********************************************************************************
*********************************************************************************
                              跳转窗口



**********************************************************************************
**********************************************************************************/
void show_goto_page_num(__s32       goto_page)
{
    ebook_uipara_t *ebook_uipara;
    GUI_RECT   gui_rect;
    char       buf[128];
    GUI_SetColor(APP_COLOR_BLACK);
    ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();
    gui_rect.x0 = ebook_uipara->goto_page.x;
    gui_rect.y0 = ebook_uipara->goto_page.y;
    gui_rect.x1 = gui_rect.x0 + ebook_uipara->goto_page.w - 1;
    gui_rect.y1 = gui_rect.y0 + ebook_uipara->goto_page.h - 1;
    GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
    GUI_SetFont(SWFFont);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetColor(GUI_RED);
    app_sprintf(buf, "%d", goto_page);
    __wrn("show page num = %d  [%d  %d  %d  %d]",goto_page,gui_rect.x0,gui_rect.y0,gui_rect.x1,gui_rect.y1);
    GUI_DispStringInRect(buf, &gui_rect, GUI_TA_RIGHT | GUI_TA_BOTTOM);
}


__s32 goto_key_pro(__gui_msg_t *msg)
{
    ebook_goto_ctrl_t *goto_ctrl = NULL;
    static __s32 last_key = -1;
    __goto_create_para_t            *g_para = NULL ;
    goto_ctrl = (ebook_goto_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    g_para = (__goto_create_para_t *)GUI_WinGetAttr(msg->h_deswin);
    GUI_LyrWinSetSta(g_para->goto_layer, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSel(g_para->goto_layer);
    __wrn("goto_ctrl->cur_key=%d\n", goto_ctrl->cur_key);
    __wrn("msg->dwAddData2=%d\n", msg->dwAddData2);
#if 0

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        __wrn("last_key=%d\n", last_key);

        switch (last_key)
        {
            case GUI_MSG_KEY_LONGRIGHT:
            case GUI_MSG_KEY_RIGHT:
                //case GUI_MSG_KEY_UP:
            {
                goto_ctrl->cur_key ++;
                __wrn("goto_ctrl->cur_key 1 =%d\n", goto_ctrl->cur_key);

                if (goto_ctrl->cur_key == 14)
                {
                    goto_ctrl->cur_key = GOTO_1F;
                }

                switch (goto_ctrl->cur_key)
                {
                    case GOTO_1F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_QUITN].res_buffer, 402, 358);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_1F].res_buffer, 286, 151);
                        break;
                    }

                    case GOTO_2F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_1N].res_buffer, 286, 151);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_2F].res_buffer, 370, 151);
                        break;
                    }

                    case GOTO_3F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_2N].res_buffer, 370, 151);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_3F].res_buffer, 450,  151);
                        break;
                    }

                    case GOTO_4F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_3N].res_buffer, 450,  151);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_4F].res_buffer, 286, 202);
                        break;
                    }

                    case GOTO_5F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_4N].res_buffer, 286, 202);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_5F].res_buffer, 370, 202);
                        break;
                    }

                    case GOTO_6F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_5N].res_buffer, 370, 202);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_6F].res_buffer, 450, 202);
                        break;
                    }

                    case GOTO_7F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_6N].res_buffer, 450, 202);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_7F].res_buffer, 286, 254);
                        break;
                    }

                    case GOTO_8F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_7N].res_buffer, 286, 254);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_8F].res_buffer, 370, 254);
                        break;
                    }

                    case GOTO_9F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_8N].res_buffer, 370, 254);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_9F].res_buffer, 450, 254);
                        break;
                    }

                    case (GOTO_0F):
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_9N].res_buffer, 450, 254);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_0F].res_buffer, 286, 306);
                        break;
                    }

                    case GOTO_DELF:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_0N].res_buffer, 286, 306);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_DELF].res_buffer, 448, 305);
                        break;
                    }

                    case GOTO_ENTERF:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_DELN].res_buffer, 448, 305);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_ENTERF].res_buffer, 251, 358);
                        break;
                    }

                    case GOTO_QUITF:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_ENTERN].res_buffer, 251, 358);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_QUITF].res_buffer, 402, 358);
                        break;
                    }
                }

                last_key = -1;
                return EPDK_OK;
                break;
            }

            case GUI_MSG_KEY_LONGLEFT:
            case GUI_MSG_KEY_LEFT:
                // case GUI_MSG_KEY_DOWN:
            {
                if (goto_ctrl->cur_key > 1)
                {
                    goto_ctrl->cur_key--;
                }
                else
                {
                    goto_ctrl->cur_key = GOTO_QUITF;
                }

                switch (goto_ctrl->cur_key)
                {
                    case GOTO_1F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_2N].res_buffer, 370, 151);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_1F].res_buffer, 286, 151);
                        break;
                    }

                    case GOTO_2F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_3N].res_buffer, 450,  151);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_2F].res_buffer, 370, 151);
                        break;
                    }

                    case GOTO_3F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_4N].res_buffer, 286, 202);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_3F].res_buffer, 450,  151);
                        break;
                    }

                    case GOTO_4F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_5N].res_buffer, 370, 202);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_4F].res_buffer, 286, 202);
                        break;
                    }

                    case GOTO_5F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_6N].res_buffer, 450, 202);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_5F].res_buffer, 370, 202);
                        break;
                    }

                    case GOTO_6F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_7N].res_buffer, 286, 254);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_6F].res_buffer, 450, 202);
                        break;
                    }

                    case GOTO_7F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_8N].res_buffer, 370, 254);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_7F].res_buffer, 286, 254);
                        break;
                    }

                    case GOTO_8F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_9N].res_buffer, 450, 254);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_8F].res_buffer, 370, 254);
                        break;
                    }

                    case GOTO_9F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_0N].res_buffer, 286, 306);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_9F].res_buffer, 450, 254);
                        break;
                    }

                    case GOTO_0F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_DELN].res_buffer, 448, 305);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_0F].res_buffer, 286, 306);
                        break;
                    }

                    case GOTO_DELF:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_ENTERN].res_buffer, 251, 358);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_DELF].res_buffer, 448, 305);
                        break;
                    }

                    case GOTO_ENTERF:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_QUITN].res_buffer, 402, 358);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_ENTERF].res_buffer, 251, 358);
                        break;
                    }

                    case GOTO_QUITF:
                    {
                        __wrn("GOTO_QUITF =%d\n", goto_ctrl->cur_key);
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_1N].res_buffer, 286, 151);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_QUITF].res_buffer, 402, 358);
                        break;
                    }
                }

                last_key = -1;
                return EPDK_OK;
            }

            case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_DOWN:
            {
                if (goto_ctrl->cur_key >= 12 && goto_ctrl->cur_key <= 13)
                {
                    goto_ctrl->cur_key = 1;
                    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_ENTERN].res_buffer, 251, 358);
                    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_QUITN].res_buffer, 402, 358);
                    GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_1F].res_buffer, 286, 151);
                }
                else if (goto_ctrl->cur_key >= 10 && goto_ctrl->cur_key <= 11)
                {
                    goto_ctrl->cur_key += 2;

                    if (12 == goto_ctrl->cur_key)
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_0N].res_buffer, 286, 306);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_ENTERF].res_buffer, 251, 358);
                    }
                    else
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_DELN].res_buffer, 448, 305);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_QUITF].res_buffer, 402, 358);
                    }
                }
                else if (goto_ctrl->cur_key >= 8 && goto_ctrl->cur_key <= 9)
                {
                    goto_ctrl->cur_key += 2;

                    if (10 == goto_ctrl->cur_key)
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_8N].res_buffer, 370, 254);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_0F].res_buffer, 286, 306);
                    }
                    else
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_9N].res_buffer, 450, 254);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_DELF].res_buffer, 448, 305);
                    }
                }
                else if (goto_ctrl->cur_key >= 1 && goto_ctrl->cur_key <= 7)
                {
                    goto_ctrl->cur_key += 3;

                    switch (goto_ctrl->cur_key)
                    {
                        case GOTO_4F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_1N].res_buffer, 286, 151);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_4F].res_buffer, 286, 202);
                            break;
                        }

                        case GOTO_5F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_2N].res_buffer, 370, 151);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_5F].res_buffer, 370, 202);
                            break;
                        }

                        case GOTO_6F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_3N].res_buffer, 450,  151);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_6F].res_buffer, 450, 202);
                            break;
                        }

                        case GOTO_7F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_4N].res_buffer, 286, 202);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_7F].res_buffer, 286, 254);
                            break;
                        }

                        case GOTO_8F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_5N].res_buffer, 370, 202);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_8F].res_buffer, 370, 254);
                            break;
                        }

                        case GOTO_9F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_6N].res_buffer, 450, 202);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_9F].res_buffer, 450, 254);
                            break;
                        }

                        case GOTO_0F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_7N].res_buffer, 286, 254);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_0F].res_buffer, 286, 306);
                            break;
                        }
                    }
                }

                last_key = -1;
                return EPDK_OK;
            }

            case GUI_MSG_KEY_LONGUP:
            case GUI_MSG_KEY_UP:
            {
                __wrn("1 goto_ctrl->cur_key=%d\n", goto_ctrl->cur_key);

                if (goto_ctrl->cur_key >= 11 && goto_ctrl->cur_key <= 13)
                {
                    goto_ctrl->cur_key -= 2;

                    if (10 == goto_ctrl->cur_key)
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_ENTERN].res_buffer, 251, 358);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_0F].res_buffer, 286, 306);
                    }
                    else if (11 == goto_ctrl->cur_key)
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_QUITN].res_buffer, 402, 358);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_DELF].res_buffer, 448, 305);
                    }
                    else if (goto_ctrl->cur_key == 9)
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_DELN].res_buffer, 448, 305);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_9F].res_buffer, 450, 254);
                    }

                    __wrn("2 goto_ctrl->cur_key=%d\n", goto_ctrl->cur_key);
                }
                else if (goto_ctrl->cur_key >= 4 && goto_ctrl->cur_key <= 10)
                {
                    goto_ctrl->cur_key -= 3;

                    switch (goto_ctrl->cur_key)
                    {
                        case GOTO_7F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_0N].res_buffer, 286, 306);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_7F].res_buffer, 286, 254);
                            break;
                        }

                        case GOTO_4F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_7N].res_buffer, 286, 254);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_4F].res_buffer, 286, 202);
                            break;
                        }

                        case GOTO_5F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_8N].res_buffer, 370, 254);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_5F].res_buffer, 370, 202);
                            break;
                        }

                        case GOTO_6F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_9N].res_buffer, 450, 254);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_6F].res_buffer, 450, 202);
                            break;
                        }

                        case GOTO_1F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_4N].res_buffer, 286, 202);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_1F].res_buffer, 286, 151);
                            break;
                        }

                        case GOTO_2F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_5N].res_buffer, 370, 202);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_2F].res_buffer, 370, 151);
                            break;
                        }

                        case GOTO_3F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_6N].res_buffer, 450, 202);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_3F].res_buffer, 450,  151);
                            break;
                        }
                    }

                    __wrn("3 goto_ctrl->cur_key=%d\n", goto_ctrl->cur_key);
                }
                else if (goto_ctrl->cur_key >= 1 && goto_ctrl->cur_key <= 3)
                {
                    goto_ctrl->cur_key = 12;
                    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_1N].res_buffer, 286, 151);
                    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_2N].res_buffer, 370, 151);
                    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_3N].res_buffer, 450,  151);
                    GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_ENTERF].res_buffer, 251, 358);
                    __wrn("4 goto_ctrl->cur_key=%d\n", goto_ctrl->cur_key);
                }

                last_key = -1;
                return EPDK_OK;
            }

            case GUI_MSG_KEY_ESCAPE:
            case GUI_MSG_KEY_LONGESCAPE:
            {
                NOTIFY_MSG(GUI_MSG_COMMAND, g_para->goto_hwin, GUI_WinGetParent(g_para->goto_hwin), MAKELONG(EBOOK_GOTO_ID, GOTO_QUITF), 0);
                last_key = -1;
                return EPDK_OK;
            }

            case GUI_MSG_KEY_ENTER:
            case GUI_MSG_KEY_LONGENTER:
            {
                SEND_MSG(GUI_MSG_COMMAND, g_para->goto_hwin, g_para->goto_hwin, goto_ctrl->cur_key, 0);
                last_key = -1;
                return EPDK_OK;
            }
        }

        last_key = -1;
    }
    else
    {
        switch (msg->dwAddData1)
        {
                __wrn("msg->dwAddData1=%d", msg->dwAddData1);

            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT:
            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_LONGLEFT:
            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_ESCAPE:
            case GUI_MSG_KEY_LONGESCAPE:
            case GUI_MSG_KEY_ENTER:
            case GUI_MSG_KEY_LONGENTER:
                //..case GUI_MSG_KEY_VDEC:
                last_key = msg->dwAddData1;
                return EPDK_OK;
        }
    }

#endif

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        if (last_key == GUI_MSG_KEY_ENTER)
        {
            SEND_MSG(GUI_MSG_COMMAND, g_para->goto_hwin, g_para->goto_hwin, goto_ctrl->cur_key, 0);
        }
        else if (last_key == GUI_MSG_KEY_ESCAPE)
        {
            NOTIFY_MSG(GUI_MSG_COMMAND, g_para->goto_hwin, GUI_WinGetParent(g_para->goto_hwin), MAKELONG(EBOOK_GOTO_ID, GOTO_QUITF), 0);
        }
    }
    else
    {
        __wrn("last_key=%d\n", last_key);

        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_LONGRIGHT:
            case GUI_MSG_KEY_RIGHT:
                //case GUI_MSG_KEY_UP:
            {
                goto_ctrl->cur_key ++;
                __wrn("goto_ctrl->cur_key 1 =%d\n", goto_ctrl->cur_key);

                if (goto_ctrl->cur_key == 14)
                {
                    goto_ctrl->cur_key = GOTO_1F;
                }

                switch (goto_ctrl->cur_key)
                {
                    case GOTO_1F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_QUITN].res_buffer, 402, 358);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_1F].res_buffer, 286, 151);
                        break;
                    }

                    case GOTO_2F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_1N].res_buffer, 286, 151);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_2F].res_buffer, 370, 151);
                        break;
                    }

                    case GOTO_3F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_2N].res_buffer, 370, 151);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_3F].res_buffer, 450,  151);
                        break;
                    }

                    case GOTO_4F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_3N].res_buffer, 450,  151);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_4F].res_buffer, 286, 202);
                        break;
                    }

                    case GOTO_5F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_4N].res_buffer, 286, 202);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_5F].res_buffer, 370, 202);
                        break;
                    }

                    case GOTO_6F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_5N].res_buffer, 370, 202);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_6F].res_buffer, 450, 202);
                        break;
                    }

                    case GOTO_7F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_6N].res_buffer, 450, 202);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_7F].res_buffer, 286, 254);
                        break;
                    }

                    case GOTO_8F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_7N].res_buffer, 286, 254);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_8F].res_buffer, 370, 254);
                        break;
                    }

                    case GOTO_9F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_8N].res_buffer, 370, 254);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_9F].res_buffer, 450, 254);
                        break;
                    }

                    case (GOTO_0F):
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_9N].res_buffer, 450, 254);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_0F].res_buffer, 286, 306);
                        break;
                    }

                    case GOTO_DELF:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_0N].res_buffer, 286, 306);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_DELF].res_buffer, 448, 305);
                        break;
                    }

                    case GOTO_ENTERF:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_DELN].res_buffer, 448, 305);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_ENTERF].res_buffer, 251, 358);
                        break;
                    }

                    case GOTO_QUITF:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_ENTERN].res_buffer, 251, 358);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_QUITF].res_buffer, 402, 358);
                        break;
                    }
                }
            }
            break;

            case GUI_MSG_KEY_LONGLEFT:
            case GUI_MSG_KEY_LEFT:
                // case GUI_MSG_KEY_DOWN:
            {
                if (goto_ctrl->cur_key > 1)
                {
                    goto_ctrl->cur_key--;
                }
                else
                {
                    goto_ctrl->cur_key = GOTO_QUITF;
                }

                switch (goto_ctrl->cur_key)
                {
                    case GOTO_1F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_2N].res_buffer, 370, 151);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_1F].res_buffer, 286, 151);
                        break;
                    }

                    case GOTO_2F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_3N].res_buffer, 450,  151);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_2F].res_buffer, 370, 151);
                        break;
                    }

                    case GOTO_3F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_4N].res_buffer, 286, 202);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_3F].res_buffer, 450,  151);
                        break;
                    }

                    case GOTO_4F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_5N].res_buffer, 370, 202);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_4F].res_buffer, 286, 202);
                        break;
                    }

                    case GOTO_5F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_6N].res_buffer, 450, 202);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_5F].res_buffer, 370, 202);
                        break;
                    }

                    case GOTO_6F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_7N].res_buffer, 286, 254);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_6F].res_buffer, 450, 202);
                        break;
                    }

                    case GOTO_7F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_8N].res_buffer, 370, 254);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_7F].res_buffer, 286, 254);
                        break;
                    }

                    case GOTO_8F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_9N].res_buffer, 450, 254);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_8F].res_buffer, 370, 254);
                        break;
                    }

                    case GOTO_9F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_0N].res_buffer, 286, 306);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_9F].res_buffer, 450, 254);
                        break;
                    }

                    case GOTO_0F:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_DELN].res_buffer, 448, 305);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_0F].res_buffer, 286, 306);
                        break;
                    }

                    case GOTO_DELF:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_ENTERN].res_buffer, 251, 358);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_DELF].res_buffer, 448, 305);
                        break;
                    }

                    case GOTO_ENTERF:
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_QUITN].res_buffer, 402, 358);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_ENTERF].res_buffer, 251, 358);
                        break;
                    }

                    case GOTO_QUITF:
                    {
                        __wrn("GOTO_QUITF =%d\n", goto_ctrl->cur_key);
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_1N].res_buffer, 286, 151);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_QUITF].res_buffer, 402, 358);
                        break;
                    }
                }
            }
            break;

            case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_DOWN:
            {
                if (goto_ctrl->cur_key >= 12 && goto_ctrl->cur_key <= 13)
                {
                    goto_ctrl->cur_key = 1;
                    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_ENTERN].res_buffer, 251, 358);
                    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_QUITN].res_buffer, 402, 358);
                    GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_1F].res_buffer, 286, 151);
                }
                else if (goto_ctrl->cur_key >= 10 && goto_ctrl->cur_key <= 11)
                {
                    goto_ctrl->cur_key += 2;

                    if (12 == goto_ctrl->cur_key)
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_0N].res_buffer, 286, 306);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_ENTERF].res_buffer, 251, 358);
                    }
                    else
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_DELN].res_buffer, 448, 305);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_QUITF].res_buffer, 402, 358);
                    }
                }
                else if (goto_ctrl->cur_key >= 8 && goto_ctrl->cur_key <= 9)
                {
                    goto_ctrl->cur_key += 2;

                    if (10 == goto_ctrl->cur_key)
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_8N].res_buffer, 370, 254);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_0F].res_buffer, 286, 306);
                    }
                    else
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_9N].res_buffer, 450, 254);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_DELF].res_buffer, 448, 305);
                    }
                }
                else if (goto_ctrl->cur_key >= 1 && goto_ctrl->cur_key <= 7)
                {
                    goto_ctrl->cur_key += 3;

                    switch (goto_ctrl->cur_key)
                    {
                        case GOTO_4F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_1N].res_buffer, 286, 151);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_4F].res_buffer, 286, 202);
                            break;
                        }

                        case GOTO_5F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_2N].res_buffer, 370, 151);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_5F].res_buffer, 370, 202);
                            break;
                        }

                        case GOTO_6F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_3N].res_buffer, 450,  151);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_6F].res_buffer, 450, 202);
                            break;
                        }

                        case GOTO_7F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_4N].res_buffer, 286, 202);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_7F].res_buffer, 286, 254);
                            break;
                        }

                        case GOTO_8F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_5N].res_buffer, 370, 202);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_8F].res_buffer, 370, 254);
                            break;
                        }

                        case GOTO_9F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_6N].res_buffer, 450, 202);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_9F].res_buffer, 450, 254);
                            break;
                        }

                        case GOTO_0F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_7N].res_buffer, 286, 254);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_0F].res_buffer, 286, 306);
                            break;
                        }
                    }
                }
            }
            break;

            case GUI_MSG_KEY_LONGUP:
            case GUI_MSG_KEY_UP:
            {
                __wrn("1 goto_ctrl->cur_key=%d\n", goto_ctrl->cur_key);

                if (goto_ctrl->cur_key >= 11 && goto_ctrl->cur_key <= 13)
                {
                    goto_ctrl->cur_key -= 2;

                    if (10 == goto_ctrl->cur_key)
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_ENTERN].res_buffer, 251, 358);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_0F].res_buffer, 286, 306);
                    }
                    else if (11 == goto_ctrl->cur_key)
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_QUITN].res_buffer, 402, 358);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_DELF].res_buffer, 448, 305);
                    }
                    else if (goto_ctrl->cur_key == 9)
                    {
                        GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_DELN].res_buffer, 448, 305);
                        GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_9F].res_buffer, 450, 254);
                    }

                    __wrn("2 goto_ctrl->cur_key=%d\n", goto_ctrl->cur_key);
                }
                else if (goto_ctrl->cur_key >= 4 && goto_ctrl->cur_key <= 10)
                {
                    goto_ctrl->cur_key -= 3;

                    switch (goto_ctrl->cur_key)
                    {
                        case GOTO_7F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_0N].res_buffer, 286, 306);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_7F].res_buffer, 286, 254);
                            break;
                        }

                        case GOTO_4F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_7N].res_buffer, 286, 254);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_4F].res_buffer, 286, 202);
                            break;
                        }

                        case GOTO_5F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_8N].res_buffer, 370, 254);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_5F].res_buffer, 370, 202);
                            break;
                        }

                        case GOTO_6F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_9N].res_buffer, 450, 254);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_6F].res_buffer, 450, 202);
                            break;
                        }

                        case GOTO_1F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_4N].res_buffer, 286, 202);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_1F].res_buffer, 286, 151);
                            break;
                        }

                        case GOTO_2F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_5N].res_buffer, 370, 202);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_2F].res_buffer, 370, 151);
                            break;
                        }

                        case GOTO_3F:
                        {
                            GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_6N].res_buffer, 450, 202);
                            GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_3F].res_buffer, 450,  151);
                            break;
                        }
                    }

                    __wrn("3 goto_ctrl->cur_key=%d\n", goto_ctrl->cur_key);
                }
                else if (goto_ctrl->cur_key >= 1 && goto_ctrl->cur_key <= 3)
                {
                    goto_ctrl->cur_key = 12;
                    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_1N].res_buffer, 286, 151);
                    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_2N].res_buffer, 370, 151);
                    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_3N].res_buffer, 450,  151);
                    GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_ENTERF].res_buffer, 251, 358);
                    __wrn("4 goto_ctrl->cur_key=%d\n", goto_ctrl->cur_key);
                }
            }
            break;

            default:
                break;
        }
    }

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        last_key = -1;
    }
    else
    {
        last_key = msg->dwAddData1;
    }

    return EPDK_OK;
}

__s32 goto_init_draw(ebook_goto_ctrl_t *goto_ctrl)
{
    //GUI_BMP_Draw(goto_ctrl->goto_bg_bmp.res_buffer, 0 , 0 );
    GUI_BMP_Draw(goto_ctrl->goto_bottom_bmp.res_buffer, 246, 57);
    GUI_BMP_Draw(goto_ctrl->goto_page_bottom.res_buffer, 268, 77);
    GUI_BMP_Draw(goto_ctrl->goto_res_fbmp[GOTO_1F].res_buffer, 286, 151);
    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_2N].res_buffer, 370, 151);
    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_3N].res_buffer, 450,  151);
    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_4N].res_buffer, 286, 202);
    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_5N].res_buffer, 370, 202);
    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_6N].res_buffer, 450, 202);
    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_7N].res_buffer, 286, 254);
    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_8N].res_buffer, 370, 254);
    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_9N].res_buffer, 450, 254);
    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_0N].res_buffer, 286, 306);
    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_DELN].res_buffer, 448, 305);
    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_ENTERN].res_buffer, 251, 358);
    GUI_BMP_Draw(goto_ctrl->goto_res_nbmp[GOTO_QUITN].res_buffer, 402, 358);
    return EPDK_OK;
}

__s32 goto_page_draw(H_LYR      layer, ebook_goto_ctrl_t *goto_ctrl)

{
    GUI_MEMDEV_Handle   draw_mem;
    //__s16 x1, y1;
    __s16 x0 = 246, y0 = 57, w = 307, h = 368;

    if (GUI_LyrWinGetSta(layer) != GUI_LYRWIN_STA_ON)
    {
        GUI_LyrWinSetSta(layer, GUI_LYRWIN_STA_ON);
    }

    //x1 = w+x0;
    //y1 = h+y0;
    GUI_LyrWinSetTop(layer);
    GUI_LyrWinSel(layer);
    draw_mem = GUI_MEMDEV_Create(x0, y0, w,  h);

    if (draw_mem == 0)
    {
        __msg("&&&&&&&&&&&&&&&&&&&   Create Memory drever fail! &&&&&&&&&&&&&&&&&&& \n");
        return EPDK_FAIL;
    }

    GUI_MEMDEV_Select(draw_mem);
    GUI_SetBkColor(0x00);
    //GUI_ClearRect(x0, y0, x1,  y1);
    goto_init_draw(goto_ctrl);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
    return EPDK_OK;
}





static __s32 __cb_goto_frmwin(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            ebook_goto_ctrl_t *goto_ctrl = NULL;
            __goto_create_para_t            *g_para = NULL ;
            g_para = (__goto_create_para_t *)GUI_WinGetAttr(msg->h_deswin);
            goto_ctrl = (ebook_goto_ctrl_t *)My_Malloc(0, sizeof(ebook_goto_ctrl_t));

            if (!goto_ctrl)
            {
                __err("memory alloc fail......\n");
                return EPDK_FAIL;
            }

            __wrn("1 no problem\n");
            eLIBs_memset(goto_ctrl, 0, sizeof(ebook_goto_ctrl_t));
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)goto_ctrl);
            ebook_goto_init(goto_ctrl);
            goto_ctrl->cur_key = GOTO_1F;
            goto_ctrl->goto_page = 0;
            //GUI_LyrWinSetSta(g_para->goto_layer, GUI_LYRWIN_STA_ON);
            //GUI_LyrWinSetTop(g_para->goto_layer);
            //GUI_LyrWinSel(g_para->goto_layer);
            goto_page_draw(g_para->goto_layer, goto_ctrl);
            __wrn("3 no problem\n");
            return EPDK_OK;
        }

        case GUI_MSG_DESTROY:
        {
            ebook_goto_ctrl_t *goto_ctrl;
            __goto_create_para_t            *g_para = NULL ;
            __wrn("*****goto com destroy begin********\n");
            g_para = (__goto_create_para_t *)GUI_WinGetAttr(msg->h_deswin);
            goto_ctrl = (ebook_goto_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
            __wrn("*****goto com destroy begin********\n");

            if (!goto_ctrl)
            {
                __err("page_ctrl is null ......\n");
                return EPDK_FAIL;
            }

            __wrn("*****delete layer  begin********\n");
            __wrn("g_para->goto_layer=%x\n", g_para->goto_layer);
            __wrn("*****ebook_goto_uninit  begin********\n");
            ebook_goto_uninit(goto_ctrl);
            __wrn("*****goto_ctrl free  begin********\n");

            if (goto_ctrl)
            {
                //__here__;
                My_Mfree(0, goto_ctrl);
                goto_ctrl = NULL ;
            }

            if (g_para)
            {
                My_Mfree(0, g_para);
                g_para = NULL ;
            }

            //__here__;
            __wrn("*****goto com destroy end********\n");
            return EPDK_OK;
        }

        case GUI_MSG_CLOSE:
        {
            __wrn("arrive close\n");
            return EPDK_OK;
        }

        case GUI_MSG_PAINT:
        {
            __wrn("arrive paint\n");
            return EPDK_OK;
        }

        case GUI_MSG_KEY:
        {
            __wrn("arrive key\n");
            goto_key_pro(msg);
            return EPDK_OK;
        }

        case COM_CMD_UPDATE_PAGE:
        {
            __wrn("arrive update page\n");
            return EPDK_OK;
        }

        case GUI_MSG_COMMAND:
        {
            ebook_goto_ctrl_t *goto_ctrl = NULL;
            __goto_create_para_t            *g_para = NULL ;
            __u32 pagenum;
            g_para = (__goto_create_para_t *)GUI_WinGetAttr(msg->h_deswin);
            __wrn("arrive command\n");
            goto_ctrl = (ebook_goto_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
            GUI_LyrWinSetSta(g_para->goto_layer, GUI_LYRWIN_STA_ON);
            GUI_LyrWinSel(g_para->goto_layer);

            switch (msg->dwAddData1)
            {
                case GOTO_0F:
                {
                    msg->dwAddData1 -= 10;
#if 1/*langaojie temp test*/
                    GUI_BMP_Draw(goto_ctrl->goto_page_bottom.res_buffer, 268, 77);
                    pagenum = goto_ctrl->goto_page;
                    goto_ctrl->goto_page = goto_ctrl->goto_page * 10 + msg->dwAddData1;

                    if (goto_ctrl->goto_page > 999999999)
                    {
                        goto_ctrl->goto_page = pagenum;
                    }

                    show_goto_page_num(goto_ctrl->goto_page);
                    break;
#endif
                }

                case GOTO_1F:
                case GOTO_2F:
                case GOTO_3F:
                case GOTO_4F:
                case GOTO_5F:
                case GOTO_6F:
                case GOTO_7F:
                case GOTO_8F:
                case GOTO_9F:
                {
                    GUI_BMP_Draw(goto_ctrl->goto_page_bottom.res_buffer, 268, 77);
                    pagenum = goto_ctrl->goto_page;
                    goto_ctrl->goto_page = goto_ctrl->goto_page * 10 + msg->dwAddData1;

                    if (goto_ctrl->goto_page > 99999999)
                    {
                        goto_ctrl->goto_page = pagenum;
                    }

                    show_goto_page_num(goto_ctrl->goto_page);
                    break;
                }

                case GOTO_DELF:
                {
                    GUI_BMP_Draw(goto_ctrl->goto_page_bottom.res_buffer, 268, 77);
                    goto_ctrl->goto_page /= 10;
                    show_goto_page_num(goto_ctrl->goto_page);
                    break;
                }

                case GOTO_ENTERF:
                {
                    __wrn("goto_ctrl->goto_page=%d\n", goto_ctrl->goto_page);
                    __wrn("g_para->goto_hwin=%x\n", g_para->goto_hwin);
                    __wrn("Parent(g_para->goto_hwin)=%x\n", GUI_WinGetParent(g_para->goto_hwin));
                    NOTIFY_MSG(GUI_MSG_COMMAND, g_para->goto_hwin, GUI_WinGetParent(g_para->goto_hwin), MAKELONG(EBOOK_GOTO_ID, GOTO_ENTERF), goto_ctrl->goto_page);
                    break;
                }

                case GOTO_QUITF:
                {
                    NOTIFY_MSG(GUI_MSG_COMMAND, g_para->goto_hwin, GUI_WinGetParent(g_para->goto_hwin), MAKELONG(EBOOK_GOTO_ID, GOTO_QUITF), 0);
                    break;
                }

                default:
                {
                    __wrn("send message is wrong\n");
                    break;
                }
            }
            GUI_LyrWinFlushFB(g_para->goto_layer);
            return EPDK_OK;
        }

        default:
        {
            __wrn("arrive default\n");
            break;
        }
    }

    __wrn("arrive here\n");
    return GUI_FrmWinDefaultProc(msg);
}



H_WIN goto_frmwin_create(H_WIN parent, __goto_create_para_t *para)
{
    __goto_create_para_t            *g_para = NULL ;
    __gui_framewincreate_para_t     frm_create_para;
    FB  fb;
    g_para = (__goto_create_para_t *)My_Malloc(0, sizeof(__goto_create_para_t));

    if (!g_para)
    {
        __err("alloc error \n");
        return 0;
    }

    eLIBs_memset(g_para, 0, sizeof(__goto_create_para_t));
    eLIBs_memcpy((void *)g_para, (const void *)para, sizeof(__goto_create_para_t));
    GUI_LyrWinGetFB(para->goto_layer, &fb);
    g_memset(&frm_create_para, 0, sizeof(__gui_framewincreate_para_t));
    frm_create_para.dwStyle          = WS_VISIBLE;//..  //设为disabled 主要是不接收触摸消息
    frm_create_para.dwExStyle        = 0;
    frm_create_para.name             = NULL;
    frm_create_para.hOwner           = 0;
    frm_create_para.hHosting         = parent;
    frm_create_para.FrameWinProc     = __cb_goto_frmwin;
    frm_create_para.id               = EBOOK_GOTO_ID;
    frm_create_para.hLayer           = g_para->goto_layer;
    frm_create_para.rect.x            = 0;
    frm_create_para.rect.y            = 0;
    frm_create_para.rect.width        = fb.size.width;
    frm_create_para.rect.height       = fb.size.height;
    frm_create_para.BkColor.alpha     = 0;
    frm_create_para.BkColor.red       = 0;
    frm_create_para.BkColor.green     = 0;
    frm_create_para.BkColor.blue      = 0;
    frm_create_para.attr              = (void *)g_para;
    g_para->goto_hwin = GUI_FrmWinCreate(&frm_create_para);
    __wrn("goto h_win1=%x\n", g_para->goto_hwin);

    if (!g_para->goto_hwin)
    {
        __err("app bar fwin create error ! \n");
    }

    __wrn("goto h_win2=%x\n", g_para->goto_hwin);
    GUI_WinSetFocusChild(g_para->goto_hwin);
    __wrn("goto h_win3=%x\n", g_para->goto_hwin);
    return g_para->goto_hwin;
}

__s32 goto_frmwin_delete(H_WIN h_win)
{
    H_LYR h_layer;
    h_layer = GUI_WinGetLyrWin(h_win);

    if (h_layer != NULL)
    {
        GUI_LyrWinDelete(h_layer);
        h_layer = NULL;
    }

    return EPDK_OK;
}
