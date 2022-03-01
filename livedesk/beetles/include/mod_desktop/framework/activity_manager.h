/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : activity_manager.h
* By        : Andy.zhang
* Func      : activity manager
* Version   : v1.0
* ============================================================================================================
* 2010-9-6 19:43:50  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
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
