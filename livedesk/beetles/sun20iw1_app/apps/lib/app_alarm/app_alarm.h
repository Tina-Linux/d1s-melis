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
#ifndef __APP_ALARM_H__
#define __APP_ALARM_H__



#include "beetles_app.h"

#define ALARM_TIMER_TIMES               10                              //0.5s

#define ALARM_MSG_CLOSE         GUI_MSG_USER_DEF+1000   //发送关闭消息到父窗口

typedef enum app_alarm_bmp_e
{
    ALARM_ALARM_BG_BMP,     //背景

    ALARM_ALARM_ANI1_BMP,   //动画1
    ALARM_ALARM_ANI2_BMP,   //动画2
    ALARM_ALARM_ANI3_BMP,   //动画3

    ALARM_BUTTON_UNF_BMP,   //无焦点
    ALARM_BUTTON_F_BMP,     //焦点

    ALARM_0_BMP,            //数字
    ALARM_1_BMP,
    ALARM_2_BMP,
    ALARM_3_BMP,
    ALARM_4_BMP,
    ALARM_5_BMP,
    ALARM_6_BMP,
    ALARM_7_BMP,
    ALARM_8_BMP,
    ALARM_9_BMP,
    ALARM_10_BMP,

    ALARM_BMP_MAX
} app_alarm_bmp;

typedef enum app_alarm_button_e
{
    ALARM_DELAY_BUTTON,     //贪睡按键
    ALARM_EXIT_BUTTON       //退出按键

} app_alarm_button;

typedef struct tag_app_alarm_attr
{
    H_LYR               layer;
    reg_alarm_para_t   *reg_alarm;

    __u16               alarmTimeID;
    __u32               alarmTimeStart;

    __bool              isDelay;

    __u32               ani_fp;

    app_alarm_button    buttonF;

    __bool              extRobinOpen;
    __s32               sysVolume;

    HTHEME              h_bmp_alarm[ALARM_BMP_MAX];

    __s32               close_time_before_alarm;

} app_alarm_attr, *papp_alarm_attr;






extern H_WIN app_alarm_frm_create(H_WIN parentWin);


extern __s32 app_alarm_frm_destory(H_WIN alarmWin);














#endif//__APP_ALARM_H__
