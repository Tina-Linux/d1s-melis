/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : applet_info_manager.h
* By        : Andy.zhang
* Func      : applet info manager
* Version   : v1.0
* ============================================================================================================
* 2010-9-6 19:43:50  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __APPLET_INFO_MANAGER_H__
#define __APPLET_INFO_MANAGER_H__

enum
{
    PERMISSON_AUDIO,
    PERMISSON_
};

typedef struct _Suffix
{
    char(*suffix)[32];
    int nr;
    int alloc_nr;
} Suffix;

typedef struct _AppInfo
{
    char name[64];      // 应用程序名称
    char exec[260];     // 可执行文件路径
    Suffix *loading;    // 能够加载的文件后缀名
    signed long     permission; // 权限
    __bool  standby;    // standby
} AppletInfo;

struct _AppletInfoManager;
typedef struct _AppletInfoManager AppletInfoManager;

typedef __s32(*AppletInfoIter)(AppletInfo *info, void *ctx1, void *ctx2);

AppletInfoManager *applet_info_manager_create(void);

__s32  applet_info_manager_load_file(AppletInfoManager *thiz, const char *filename);

__s32  applet_info_manager_load_dir(AppletInfoManager *thiz, const char *path);

__s32  applet_info_manager_get_count(AppletInfoManager *thiz);

__s32  applet_info_manager_get(AppletInfoManager *thiz, __u8 index, AppletInfo **info);

__s32  applet_info_manager_foreach(AppletInfoManager *thiz, AppletInfoIter iter, void *ctx1, void *ctx2);

void   applet_info_manager_printf(AppletInfoManager *thiz);

void   applet_info_manager_destroy(AppletInfoManager *thiz);

#endif /* __APPLET_INFO_MANAGER_H__ */
