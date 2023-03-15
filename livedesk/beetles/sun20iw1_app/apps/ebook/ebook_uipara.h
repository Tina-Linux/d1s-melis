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
#ifndef __EBOOK_UIPARA_H__
#define  __EBOOK_UIPARA_H__

#include "app_ebook_i.h"

typedef struct tag_ebook_res
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
} ebook_res_t;


typedef struct tag_ebook_uipara
{
    ebook_res_t     tips_lyr;
    ebook_res_t     page_lyr;
    ebook_res_t page_num_progress_lyr ;
    ebook_res_t     bmp_play;

    ebook_res_t     page_no_pos;
    ebook_res_t     page_name_pos;
    ebook_res_t     page_volume_value_pos;
    ebook_res_t page_no_progress_length ;       //电子书页数进度条长度
    ebook_res_t    page_no_dot_length ;     //电子书页数进度条每个点的长度
    ebook_res_t    page_no_head_length ;    //电子书页数进度条顶部
    ebook_res_t tips_txt_pos;
    ebook_res_t tip_no_bmp_pos;
    ebook_res_t tip_yes_bmp_pos;
    ebook_res_t bottom_bg;//..
    ebook_res_t  goto_lyr;//..
    ebook_res_t     goto_page;//..

} ebook_uipara_t;

typedef struct tag_ebook_theme_res_s
{
    __s32   res_id;
    HTHEME  h_bmp_handle;
    void   *res_buffer;
} ebook_theme_res_t;

typedef enum tag_page_com_theme_res_s
{
    PAGE_BOTTOM_BMP = 0,
    PAGE_PLAY_BUTTON_BMP,
    PAGE_PAUSE_BUTTON_BMP,
    PAGE_PROGRESS_HEAD,
    PAGE_PROGRESS_BODY,
    PAGE_PROGRESS_DOT,
    PAGE_NAME_ROLL,
    PAGE_BMP_MAX,
} page_theme_res_e;

typedef enum tag_goto_nbmp_theme_res_s//..
{


    GOTO_1N = 1,
    GOTO_2N,
    GOTO_3N,
    GOTO_4N,
    GOTO_5N,
    GOTO_6N,
    GOTO_7N,
    GOTO_8N,
    GOTO_9N,
    GOTO_0N,
    GOTO_DELN,
    GOTO_ENTERN,
    GOTO_QUITN,
    GOTO_NBMP_MAX,
} GOTO_NBMP_theme_res_e;
typedef enum tag_goto_fbmp_theme_res_s//..
{


    GOTO_1F = 1,
    GOTO_2F,
    GOTO_3F,
    GOTO_4F,
    GOTO_5F,
    GOTO_6F,
    GOTO_7F,
    GOTO_8F,
    GOTO_9F,
    GOTO_0F,
    GOTO_DELF,
    GOTO_ENTERF,
    GOTO_QUITF,
    GOTO_FBMP_MAX,
} GOTO_FBMP_theme_res_e;



ebook_uipara_t *get_ebook_uipara(void);

#endif
