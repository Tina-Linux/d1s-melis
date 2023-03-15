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
#ifndef __ACTIVITY__
#define __ACTIVITY__

#include "applet_info_manager.h"

struct _Activity;
typedef struct _Activity Activity;

typedef __s32(*AppletStart)(Activity *thiz);
typedef __s32(*AppletSuspend)(Activity *thiz);
typedef __s32(*AppletResume)(Activity *thiz);
typedef __s32(*AppletBackground)(Activity *thiz);
typedef __s32(*AppletStop)(Activity *thiz);

typedef struct _AppletPlugin
{
    AppletStart         start;
    AppletSuspend       suspend;
    AppletResume        resume;
    AppletBackground    background;
    AppletStop          stop;
    char                priv[1];
} AppletPlugin;

typedef enum _ActivityState
{
    ACTIVITY_ST_STOP,
    ACTIVITY_ST_TOPLEVEL,
    ACTIVITY_ST_SUSPEND,
    ACTIVITY_ST_BACKGROUND
} ActivityState;

enum
{
    APPLET_CMD_START,
    APPLET_CMD_SUSPEND,
    APPLET_CMD_RESUME,
    APPLET_CMD_BACKGROUND,
    APPLET_CMD_STOP,
    APPLET_CMD_
};

//Activity* activity_create(AppletInfo *info);                  /* 创建一个 Activity */

AppletPlugin *activity_get_plugin(Activity *thiz);              /* 获取AppletPlugin接口*/

__s32 activity_set_attr(Activity *thiz, void *attr);            /* 设置priv属性      */

void *activity_get_attr(Activity *thiz);                        /* 获取priv属性      */

int32_t activity_set_load_para(char *app_name, char *src_name, void *p_arg, __u32 size);

__s32 activity_get_load_para(char *app_name, char *src_name, void *p_arg, __u32 size);

__s32 activity_init(void);

__s32 activity_deinit(void);

__s32 activity_load_file(char *filename);       /* 运行一个文件      */

__s32 activity_load_app(char *appname);             /* 运行一个应用程序  */

H_WIN activity_get_root_win(Activity *thiz);                    /* 链接窗口          */

__s32 activity_background_top(void);                        /* 使程序进入后台状态*/

__s32 activity_return_home(Activity *thiz);                     /* 返回主界面        */

void  activity_finish(Activity *thiz);                          /* 杀掉Activity      */

//void activity_kill_backgrd_except_home(void);         /* 杀掉除home外的所有Activity    */

//void activity_kill_all_backgrd(void);
void  activity_finish_all(void);

__s32 activity_suspend_top(void);           /* 将toplevel的activity置为suspend  */

__s32 activity_resume_top(void);                    /* 恢复toplevel的activity */

__s32 activity_notify_top(__gui_msg_t *pmsg);

__s32 activity_all_able_standby(void);      /* 查询是否所有运行的activity均允许standby*/




#endif /* __ACTIVITY__*/
