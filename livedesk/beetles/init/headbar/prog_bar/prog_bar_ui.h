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
#ifndef __INIT_PROG_BAR_UIPARA_H_
#define  __INIT_PROG_BAR_UIPARA_H_

#define INIT_PROG_BAR_MAX_ICON_RES_NUM 2

typedef struct
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
    __s32 res_id[INIT_PROG_BAR_MAX_ICON_RES_NUM];
    HTHEME res_hdl[INIT_PROG_BAR_MAX_ICON_RES_NUM];
} init_prog_bar_rect_t;

typedef enum
{
    init_prog_bar_icon_prog_bg = 0,
    init_prog_bar_icon_lprog_bar,
    init_prog_bar_icon_mprog_bar,
    init_prog_bar_icon_prog_cursor,
    init_prog_bar_icon_num
} init_prog_bar_icon_t;

typedef struct
{
    init_prog_bar_rect_t uipara_frm;
    init_prog_bar_rect_t uipara_prog_bar_bg;
    init_prog_bar_rect_t uipara_icon[init_prog_bar_icon_num];
    init_prog_bar_rect_t uipara_vol_bg;
    init_prog_bar_rect_t uipara_bright_bg;
} init_prog_bar_uipara_t;

init_prog_bar_uipara_t *init_prog_bar_get_uipara(__s32 rotate);

#endif
