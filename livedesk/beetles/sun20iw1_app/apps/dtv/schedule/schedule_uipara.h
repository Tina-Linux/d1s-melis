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
#ifndef __SCHEDULE_UIPARA_H_
#define  __SCHEDULE_UIPARA_H_
#include "schedule_i.h"

#if 0
typedef struct schedule_uipara_s
{
    res_pos_t   lyr_win;
    res_pos_t   lyr_scn;
    res_pos_t   item;       //每个item的位置坐标和大小
    res_pos_t   left_button;    //向左方向的三角形的按钮
    res_pos_t   right_button;   //向右方向的三角形按钮
    OFFSET txt_pos;     //通用设置    ，电源管理字符坐标
    res_pos_t   string_detail_pos_de;   //每个选项对应的详细信息说明文字所在坐标
    res_pos_t   string_detail_pos_ex;   //每个选项对应的详细信息说明文字所在坐标
    res_pos_t   item_title_pos;
    res_pos_t   item_content_pos;
    res_pos_t   return_bmp;         //返回图片坐标位置
    res_pos_t   long_string_area ;  // long string scroll area
    tips_uipara_t  schedule_tips_uipara;    //提示框坐标位置
} schedule_uipara_t;

/*
typedef struct adjust_uipara
{
    RECT    adjust_finish;//校准完成提示语
    OFFSET  cross_pos_1;
    OFFSET  cross_pos_2;
    OFFSET  cross_pos_3;
    OFFSET  cross_pos_4;
}adjust_uipara_t;
*/
schedule_uipara_t *schedule_get_uipara(void);
//tips_uipara_t* schedule_get_tips_uipara(void);

#endif
#endif      /*__SCHEDULE_UIPARA_H_*/
