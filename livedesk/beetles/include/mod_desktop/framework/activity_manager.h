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
#ifndef __ACTIVITY_MANAGER_H__
#define __ACTIVITY_MANAGER_H__

struct _ActivityNode;
typedef struct _ActivityNode ActivityNode;

struct _ActivityManager;
typedef struct _ActivityManager ActivityManager;

typedef __s32(*ActivityInterator)(Activity *plugin, ActivityState state, void *ctx1, void *ctx2);        /* 遍历回调 */

ActivityManager *activity_manager_create(void);                                                         /* create activity manager */

/* suspend and toplevel activity arrange in a stack */
Activity *activity_manager_top(ActivityManager *thiz);                                                  /* 查询 toplevel Activity */
__s32 activity_manager_push(ActivityManager *thiz, Activity *activity);                                 /* push activity to manager */
__s32 activity_manager_pop(ActivityManager *thiz);                                                      /* pop activity from manager */
__s32 activity_manager_top_to_backgrd(ActivityManager *thiz);                                                   /* toplevel activity 转为后台activity */

/* background activity in a link list */
__s32 activity_manager_add_backgrd(ActivityManager *thiz, Activity *activity);                              /* 添加background activity to ActivityManager*/
__s32 activity_manager_delete_backgrd(ActivityManager *thiz, Activity *activity);                           /* 删除background activity */
__s32 activity_manager_backgrd_to_top(ActivityManager *thiz, Activity *activity);                       /* 后台activity 转为toplevel activity*/

/* iterator all activity */
__s32 activity_manager_foreach(ActivityManager *thiz, ActivityInterator iter, void *ctx1, void *ctx2);  /* 遍历所有background activity */
__s32 activity_manager_foreach_suspend(ActivityManager *thiz, ActivityInterator iter, void *ctx1, void *ctx2);
__s32 activity_manager_foreach_backgrd(ActivityManager *thiz, ActivityInterator iter, void *ctx1, void *ctx2);
void  activity_manager_destroy(ActivityManager *thiz);

#endif /* __ACTIVITY_MANAGER_H__*/
