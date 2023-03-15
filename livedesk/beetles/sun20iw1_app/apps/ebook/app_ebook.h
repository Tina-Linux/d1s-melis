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
#ifndef __APP_EBOOK_H__
#define  __APP_EBOOK_H__

#include "app_ebook_i.h"
#define BMK_MAX_NUM 6   //..子书书签的个数
#define BMK_PAGE_NO_LENGTH 32 //..电子书书签页码长度

typedef enum tag_AUTO_TIME
{
    AUTO_TIME_FAST = 0,
    AUTO_TIME_MIDDLE,
    AUTO_TIME_SLOW,
    AUTO_TIME_OFF,
} __auto_time_e;

typedef enum tag_BG_MUSIC
{
    BG_MUSIC_OFF = 0,
    BG_MUSIC_ON,
} __bg_music_e;

typedef enum tag_BOOK_COLOR
{
    BOOK_COLOR_BLACK = 0,
    BOOK_COLOR_WHITE,
    BOOK_COLOR_YELLOW,
    BOOK_COLOR_
} __book_color_e;
typedef struct tag_ebook_sset_para//..电子书签的内容属性
{
    /*GUI_FONT *sset_font;
    H_WIN      layer;
    __s32     main_id;
    __s32     value;
    __s32     total_page;*/

    char bmk_page_no[BMK_MAX_NUM][BMK_PAGE_NO_LENGTH] ; // 电子书书签页码

} ebook_sset_para_t; //..


typedef struct __ebook_bkpoint_inf
{
    __u32   page_no;                /* 上次退出电子书的页码        */
    char    ebookname[RAT_MAX_FULL_PATH_LEN + 1]; /* 上次退出电子书时的电子书的名字  */
    //..char    bmk_page_no[BMK_MAX_NUM][BMK_PAGE_NO_LENGTH];//..电子书签
    __u32   bmk_numth;//..第几个书签

} __ebook_bkpoint_t;

H_WIN app_ebook_create(root_para_t *para);
__s32 app_ebook_notify_delete_sub_scene(H_WIN hmanwin);

#endif
