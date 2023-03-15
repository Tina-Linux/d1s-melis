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
