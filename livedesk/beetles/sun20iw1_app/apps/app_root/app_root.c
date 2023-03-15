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
#include <log.h>
#include "beetles_app.h"
#include "app_root_init.h"
#include "app_root_scene.h"
/**********************************************************************************************************************/


/**********************************************************************************************************************
插件接口注册
**********************************************************************************************************************/
/* 声明 */
static int32_t  app_root_start(Activity  *thiz);
static int32_t  app_root_suspend(Activity *thiz);
static int32_t  app_root_resume(Activity *thiz);
static int32_t  app_root_background(Activity *thiz);
static int32_t  app_root_stop(Activity *thiz);

/* 注册插件接口 */
AppletPlugin APPLET_PLUGIN_INTERFACE =
{
    app_root_start,
    app_root_suspend,
    app_root_resume,
    app_root_background,
    app_root_stop,
    {0},
};

Activity    *root_activity      = NULL; //应用程序框架插件指针
H_WIN       happ_root_manwin    = NULL; //应用程序框架manwin句柄


/**********************************************************************************************************************
插件接口实现
**********************************************************************************************************************/
static int32_t app_root_start(Activity *activity)
{
    __inf("**************app_root plugin start!**************** \n");
    __inf("****************************\n");
    
    app_root_init_res();

    root_activity       = activity;
    happ_root_manwin    = app_root_wincreate(activity);

    if (NULL == happ_root_manwin)
    {
        __inf("	 app_root_wincreate fail! \n");
        return EPDK_FAIL;
    }
    else
    {
        GUI_WinSetFocusChild(happ_root_manwin);
        __inf("	 app_root_wincreate success! \n");
        return EPDK_OK;
    }
}

static int32_t app_root_suspend(Activity *activity)
{
    __log("	 app_root plugin suspend! \n");

    if (happ_root_manwin)
    {
        GUI_ManWinDelete(happ_root_manwin);
        happ_root_manwin = NULL;
        app_root_deinit_res();
    }

    return EPDK_OK;
}

static int32_t app_root_resume(Activity *activity)
{
    __log("	 app_root plugin reseum! \n");
    __log("**************app_root plugin start!**************** \n");

    app_root_init_res();

    root_activity       = activity;
    happ_root_manwin    = app_root_wincreate(activity);

    if (NULL == happ_root_manwin)
    {
        __log("	 app_root_wincreate fail! \n");
        return EPDK_FAIL;
    }
    else
    {
        __log("	 app_root_wincreate success! \n");
        return EPDK_OK;
    }
}

static int32_t app_root_background(Activity *activity)
{
    __log("	 app_root plugin background! \n");

    if (happ_root_manwin)
    {
        GUI_ManWinDelete(happ_root_manwin);
        happ_root_manwin = NULL;
        app_root_deinit_res();
    }

    return EPDK_OK;
}

static int32_t app_root_stop(Activity *activity)
{
    __log("	********************app_root plugin stop!**************** \n");

    if (happ_root_manwin)
    {
        GUI_ManWinDelete(happ_root_manwin);
        happ_root_manwin = NULL;
        app_root_deinit_res();
    }

    return EPDK_OK;
}
