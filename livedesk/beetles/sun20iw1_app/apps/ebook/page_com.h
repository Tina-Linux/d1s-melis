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
#ifndef __PAGE_COM_H__
#define __PAGE_COM_H__

#include "app_ebook_i.h"

typedef enum tag_PAGE_NC_MSG
{
    COM_NC_PAGE_NEXT,
    COM_NC_PAGE_PREV,
    COM_NC_PAGE_JUMP,

    COM_NC_PAGE_EXIT,
    COM_NC_SET,

} __page_nc_msg_e;


typedef enum tag_PAGE_CMD_MSG
{
    COM_CMD_UPDATE_PAGE = GUI_MSG_,
    COM_CMD_SET_PLAY_STATUS,
    COM_CMD_SHOW_PROGRESS,
    COM_CMD_SEEKPAGE,       //电子书跳页
    COM_CMD_LONGSTRING_STOP,    // 停止长字符串滚动
    COM_CMD_LONGSTRING_START,   //开始长字符串滚动
    CLOSE_LONG_STRING,
    OPEN_LONG_STRING,
} __page_cmd_msg_e;

typedef struct tag_page_create_para
{
    H_LYR                       page_layer;
    H_LYR                       page_num_progress_lyr ; //页数进度图层
    GUI_FONT                    *font;

    char            name[128];
    __s32       play_status;
    __s32       total_page;
    __s32       cur_page;
} __page_create_para_t;

typedef struct goto_create_para//..
{
    H_LYR                       goto_layer;
    H_WIN                       goto_hwin;


} __goto_create_para_t;




H_WIN page_frmwin_create(H_WIN hwin, __page_create_para_t *para);
__s32 page_frmwin_delete(H_WIN h_win);

__s32 goto_frmwin_delete(H_WIN h_win);
H_WIN goto_frmwin_create(H_WIN parent, __goto_create_para_t *para);



#endif /* __PAGE_COM_H__ */
