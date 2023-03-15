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

#ifndef __SETTING_ALARM_CLOCK_H__
#define __SETTING_ALARM_CLOCK_H__




#include "beetles_app.h"

#define ALARM_CLOCK_MSG_CLOSE           GUI_MSG_USER_DEF+800    //发送关闭消息到父窗口

typedef enum setting_alarm_clock_bmp_e
{
    //背景
    ALARM_CLOCK_BG_BMP,
    ALARM_CLOCK_CLOCK_BG_BMP,
    //页
    ALARM_CLOCK_PAGE_BMP,
    //数字
    ALARM_CLOCK_0_BMP,
    ALARM_CLOCK_1_BMP,
    ALARM_CLOCK_2_BMP,
    ALARM_CLOCK_3_BMP,
    ALARM_CLOCK_4_BMP,
    ALARM_CLOCK_5_BMP,
    ALARM_CLOCK_6_BMP,
    ALARM_CLOCK_7_BMP,
    ALARM_CLOCK_8_BMP,
    ALARM_CLOCK_9_BMP,
    //按钮
    ALARM_CLOCK_DOWN_UNF_BMP,
    ALARM_CLOCK_DOWN_F_BMP,
    ALARM_CLOCK_UP_UNF_BMP,
    ALARM_CLOCK_UP_F_BMP,
    //最大
    ALARM_CLOCK_BMP_MAX
} setting_alarm_clock_bmp;

typedef enum setting_alarm_clock_button_e
{
    //按钮
    ALARM_CLOCK_HOUR_UP,        //小时+
    ALARM_CLOCK_HOUR_DOWN,      //小时-
    ALARM_CLOCK_MINUTE_UP,      //分钟+
    ALARM_CLOCK_MINUTE_DOWN,    //分钟-
    ALARM_CLOCK_OK,             //设置
    ALARM_CLOCK_CANCEL,         //取消
    //最大
    ALARM_CLOCK_BUTTON_MAX
} setting_alarm_clock_button;

typedef struct tag_setting_alarm_clock_attr
{
    H_LYR                       layer;
    reg_alarm_para_t           *reg_alarm;

    __awos_time_t                   clock;
    setting_alarm_clock_button  buttonF;

    HTHEME                      h_bmp_alarm_clock[ALARM_CLOCK_BMP_MAX];

} setting_alarm_clock_attr, *psetting_alarm_clock_attr;






extern H_WIN setting_alarm_clock_frm_create(H_WIN parentWin, __u16 id, reg_alarm_para_t *reg_alarm);


extern __s32 setting_alarm_clock_frm_destory(H_WIN alarmWin);






























#endif//__SETTING_ALARM_CLOCK_H__
