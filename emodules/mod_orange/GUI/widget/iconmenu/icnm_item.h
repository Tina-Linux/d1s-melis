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
#ifndef _LION_ICNM_ICON_H_
#define _LION_ICNM_ICON_H_


typedef struct tag__LION_ICNM_ITEMCREATE_PARA
{
    char                           *name;           /* item name                                                    */
    char                           *title;
    RECT                            rect;
    H_LYR                           h_lyr;
    __pLION_WIN_CB                  cb;
    __lion_icnm_itemcreate_attr_t *attr;
    __u32                           index;

} __lion_icnm_itemcreate_para_t;


typedef struct tag__LION_ICNM_ITEM
{
    char                  name[LION_NAME_MAX];      /* item name                                                    */

    char                  title[LION_TITLE_MAX];

    __lion_painmod_t      mode;

    __u32                 drawflag;

    __u32                 index;

    __s32                 current;                  /* 当前图编号                                                   */
    __s32                 step;                     /* 需要跟随的步数                                               */
    __s32                 start;                    /* 起始步                                                       */
    __s32                 end;                      /* 目标图标号                                                   */

    __lion_icnm_icon_t    icon[8];                  /* 原始图有8个动画                                              */
    __lion_icnm_icon_t    s_icon[16];               /* bmp[0]有16种变化                                             */

} __lion_icnm_item_t;

extern __lion_win_t *ICNM_item_create(__lion_icnm_itemcreate_para_t *p_para, H_WIN h_parent);
extern __s32          ICNM_item_delete(H_WIN h_icon);

extern __s32 ICNM_prescale_icon(__lion_win_t *p_icon, __u8 iconindex);
extern __s32 ICNM_prescale_icons(__lion_win_t *p_icon);

#endif /* #ifndef _LION_ICNM_ICON_H_ */
