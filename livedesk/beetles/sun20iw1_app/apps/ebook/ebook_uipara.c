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
#include "ebook_uipara.h"
#define SET_TIPS_W  191
#define SET_TIPS_H  101

static ebook_uipara_t ebook_uipara;

ebook_uipara_t *get_ebook_uipara(void)
{
    ebook_uipara.tips_lyr.x = 0;
    ebook_uipara.tips_lyr.y = 0;
    ebook_uipara.tips_lyr.w = SET_TIPS_W;
    ebook_uipara.tips_lyr.h = SET_TIPS_H;
    ebook_uipara.page_num_progress_lyr.x = 0;
    ebook_uipara.page_num_progress_lyr.y = 30;
    ebook_uipara.page_num_progress_lyr.w = 24;
    ebook_uipara.page_num_progress_lyr.h = 200;
    //显示页码图层
    ebook_uipara.page_lyr.x  = 50;
    ebook_uipara.page_lyr.y  = 448;//..
    ebook_uipara.page_lyr.w  = 700;//..
    ebook_uipara.page_lyr.h  = 27;
    /*ebook_uipara.page_lyr.x  = 0;//..
    ebook_uipara.page_lyr.y  = 0;//..
    ebook_uipara.page_lyr.w  = 800;//..
    ebook_uipara.page_lyr.h  = 480;//..*/
    //显示play pause 图片位置
    ebook_uipara.bmp_play.x = 70;//..
    ebook_uipara.bmp_play.y = 0;//..
    ebook_uipara.bmp_play.w = 25;
    ebook_uipara.bmp_play.h = 27;
    ebook_uipara.page_name_pos.x = 230;//.. //书名显示位置
    ebook_uipara.page_name_pos.y = 0;//..
    ebook_uipara.page_name_pos.w = 260;
    ebook_uipara.page_name_pos.h = 24;
    ebook_uipara.page_no_progress_length.x = 0 ;
    ebook_uipara.page_no_progress_length.y = 0;
    ebook_uipara.page_no_progress_length.w = 12;
    ebook_uipara.page_no_progress_length.h = 196;
    ebook_uipara.page_no_head_length.x = ebook_uipara.page_no_progress_length.x + ebook_uipara.page_no_progress_length.w / 2  ;
    ebook_uipara.page_no_head_length.y = ebook_uipara.page_no_progress_length.y ;
    ebook_uipara.page_no_head_length.w = 12;
    ebook_uipara.page_no_head_length.h = 9;
    ebook_uipara.page_no_dot_length.w = 2;
    ebook_uipara.page_no_dot_length.x = ebook_uipara.page_no_progress_length.x + (ebook_uipara.page_no_progress_length.w - ebook_uipara.page_no_dot_length.w) / 2 ;
    ebook_uipara.page_no_dot_length.y =  ebook_uipara.page_no_progress_length.y + ebook_uipara.page_no_head_length.h ;
    ebook_uipara.page_no_dot_length.h = 1;
    //显示页数
    ebook_uipara.page_no_pos.x = 550;//..300
    ebook_uipara.page_no_pos.y = 0;//..
    ebook_uipara.page_no_pos.w = 140;
    ebook_uipara.page_no_pos.h = 24;
    ebook_uipara.tips_txt_pos.x = 0;    //断点提示文字显示位置
    ebook_uipara.tips_txt_pos.y = 30;
    ebook_uipara.tips_txt_pos.w = 180;
    ebook_uipara.tips_txt_pos.h = 15;
    ebook_uipara.tip_yes_bmp_pos.x = 30;
    ebook_uipara.tip_yes_bmp_pos.y = 59;
    ebook_uipara.tip_yes_bmp_pos.w = 37;
    ebook_uipara.tip_yes_bmp_pos.h = 18;
    ebook_uipara.tip_no_bmp_pos.x = 118;
    ebook_uipara.tip_no_bmp_pos.y = 59;
    ebook_uipara.tip_no_bmp_pos.w = 37;
    ebook_uipara.tip_no_bmp_pos.h = 18;
    //显示背景底图//..
    ebook_uipara.bottom_bg.x = 0;
    ebook_uipara.bottom_bg.y = 0;
    ebook_uipara.bottom_bg.w = 800;
    ebook_uipara.bottom_bg.h = 480;
    //跳转窗口图层//..
    ebook_uipara.goto_lyr.x = 0;
    ebook_uipara.goto_lyr.y = 0;
    ebook_uipara.goto_lyr.w = 800;
    ebook_uipara.goto_lyr.h = 480;
    //跳转窗口 显示跳转 页码
    ebook_uipara.goto_page.x = 286;
    ebook_uipara.goto_page.y = 99;
    ebook_uipara.goto_page.w = 250;
    ebook_uipara.goto_page.h = 50;
    return &ebook_uipara;
}
