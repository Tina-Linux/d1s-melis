/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File          : init.c
* By            : Andy.zhang
* Func      : init thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include <log.h>
#include <emodules/mod_display.h>
#include "mod_init_i.h"
#include "ui/init_server.h"
#include "ui/tp_adjust_scene/scene_adjust.h"
#include "ui/live_init.h"
#include <mod_mixture.h>

#define XML_PATH    BEETLES_APP_ROOT"apps\\desktop"

/* DBMS module path */
#define  DBMS_MOD_PATH      BEETLES_APP_ROOT"mod\\dbms.mod"

#define CHARGE_BMP_NUM      3

typedef struct
{
    uint8_t         play_timer_id;
    H_LYR           hlayer;
    int32_t         bmp_id[CHARGE_BMP_NUM];
    HTHEME          bmp_hdl[CHARGE_BMP_NUM];
} chagre_attr_t;

struct _AppletInfoManager
{
    int32_t         nr;           // 实际节点数目
    int32_t         alloc_nr;     // 分配内存数目
    AppletInfo *info;   // 动态数组
};

static H_WIN                init_mainwin;
static int                  ModDBMSID;
static __mp                 *ModDBMS;
static GUI_MEMDEV_Handle    g_memdev_hdl = 0;
extern GUI_FONT             *SWFFont;

int32_t show_charge_scene(void);

static int32_t __init_memdev_create(H_LYR hlyr, int32_t x, int32_t y, int32_t w, int32_t h)
{
    if (g_memdev_hdl)
    {
        __err("memdev already exist...");
        return EPDK_FAIL;
    }

    if (!hlyr)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(hlyr);
    g_memdev_hdl = GUI_MEMDEV_Create(x, y, w, h);

    if (!g_memdev_hdl)
    {
        return EPDK_FAIL;
    }

    GUI_MEMDEV_Select(g_memdev_hdl);
    return EPDK_OK;
}

int32_t init_memdev_create(H_LYR hlyr)
{
    __s32 ret;
    RECT rect;
    GUI_LyrWinGetScnWindow(hlyr, &rect);
    rect.x = 0;
    rect.y = 0;
    ret = __init_memdev_create(hlyr, rect.x, rect.y, rect.width, rect.height);
    return ret;
}

int32_t init_memdev_delete(void)
{
    if (!g_memdev_hdl)
    {
        __err("g_memdev_hdl is null...");
        return EPDK_FAIL;
    }

    GUI_MEMDEV_CopyToLCD(g_memdev_hdl);
    GUI_MEMDEV_Select(0);
    GUI_MEMDEV_Delete(g_memdev_hdl);
    g_memdev_hdl = 0;
    return EPDK_OK;
}

////////////////////////////////////////////
static int32_t _install_dbms_mod(void)
{
    /* install dbms module */
    ModDBMSID = esMODS_MInstall(DBMS_MOD_PATH, 0);

    if (ModDBMSID == 0)
    {
        __log("Install dbms module failed");
        return EPDK_FAIL;
    }

    __inf("Install dbms module succeeded");
    /* open dbms module */
    ModDBMS = esMODS_MOpen(ModDBMSID, 0);

    if (ModDBMS == (__mp *)0)
    {
        __log("Open dbms module failed");
        return EPDK_FAIL;
    }

    __inf("Open dbms module succeeded");
    return EPDK_OK;
}


static void _uninstall_dbms_mod(void)
{
    /* uninstall dbms module    */
    if (ModDBMS)
    {
        esMODS_MClose(ModDBMS);
    }

    if (ModDBMSID)
    {
        esMODS_MUninstall(ModDBMSID);
    }

    ModDBMS   = NULL;
    ModDBMSID = 0;
}
static void _close_audio_dev(void)
{
	__wrn("need fix");
	return ;
#if 0 /*langaojie temp test*/ //remove wrn
    int32_t p_dac = -1;
    p_dac = eLIBs_fopen("/dev/audio_play0", O_WRONLY);

    if (!p_dac)
    {
        __err("open dac device fail!");
    }

    esMODS_MIoctrl(p_dac, AUDIO_DEV_CMD_CLOSE_DEV, AUDIO_DEV_PLAY, 0);
    eLIBs_fclose(p_dac);
#endif 
}
static void shutdown_logo_show(bool flag)
{
#if CONFIG_MOD_MIXTURE_ENABLE   
	int param = 0;
    static int mid = 0;
    static void  *mp = 0;
    __u64 arg[3];

    if (esCFG_GetKeyValue("mixture_para", "shutdown_logo_en", (int *)&param, 1) != EPDK_OK)
    {
        param = 0;
        __err("get_logo_en err!");
    }
    if(param == 0)
    {
        return;
    }

    if(flag)
    {
       mid = esMODS_MInstall("d:\\mod\\mixture.mod",0);
       mp = esMODS_MOpen(mid, 0);
       arg[0] = SHUTDOWN;
       esMODS_MIoctrl((__mp *)mp, MOD_MIXTURE_START, 0, (void *)arg);
    }
    else
    {
    	while(esMODS_MIoctrl((__mp *)mp, MOD_MIXTURE_QURY_FINISH, 0, 0) != 1)
    	{
    		esKRNL_TimeDly(3);
    	}
        arg[0] = SHUTDOWN;
    	esMODS_MIoctrl((__mp *)mp, MOD_MIXTURE_STOP, 0, (void *)arg);
    	if(mp)
    	{
    		esMODS_MClose((__mp *)mp);
    	}
    	if (mid)
    	{
    		esMODS_MUninstall(mid);
    	}
        mp = NULL;
        mid = 0;
    }

#endif  
}
static void _close_lcd(void)
{
    __mp        *mp_display;
    uint32_t    outputtype;
    unsigned long    arg[3] = { 0 };

	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __err("open display fail");
        return ;
    }

    outputtype = esMODS_MIoctrl(mp_display, MOD_DISP_GET_OUTPUT_TYPE, 0, (void *)arg);

    if (outputtype == DISP_OUTPUT_TYPE_LCD)
    {
        arg[0] = DISP_OUTPUT_TYPE_NONE;
        arg[1] = 0;
        arg[2] = 0;
        esMODS_MIoctrl(mp_display, MOD_DISP_DEVICE_SWITCH, 0, (void *)arg);
    }
}

int32_t key_hook_cb(__gui_msg_t *msg)
{
    if (init_mainwin)
    {
        __init_ctl_t    *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(init_mainwin);

        if (init_ctr == NULL)
        {
            __err("file:%s, line:%d fail\r", __FILE__, __LINE__);
            return EPDK_OK;
        }

        init_lock(msg);

        if (!init_ctr->key_msg_enble)               // 禁止按键
        {
            init_unlock(msg);
            return -1;
        }

        if (init_ctr->screen_close_done)            // 已经关屏
        {
            init_unlock(msg);

            if (/*GUI_MSG_KEY_ENTER == msg->dwAddData1 && */KEY_UP_ACTION == msg->dwAddData2)
            {
                NOTIFY_MSG(DSK_MSG_SCREEN_OPEN, NULL, msg->h_deswin, 0, 0);
                return 0;
            }
            else
            {
                return -1;
            }
        }

        init_unlock(msg);

        esPWRMAN_UsrEventNotify();
    }

    GUI_PostMessage(msg);

#if  0
    {
        //按键enter键down和up键同时到达，导致消息丢失，之间加一个延时
        if (GUI_MSG_KEY_ENTER == msg->dwAddData1 && KEY_DOWN_ACTION == msg->dwAddData2)
        {
            esKRNL_TimeDly(10);
        }
    }
#endif
    return 0;
}

int32_t tp_hook_cb(__gui_msg_t *msg)
{
    if (init_mainwin)
    {
        __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(init_mainwin);
        init_lock(msg);

        if (!init_ctr->tp_msg_enble)            // 禁止 touch
        {
            init_unlock(msg);
            return -1;
        }

        if (init_ctr->screen_close_done)
        {
            init_unlock(msg);
            return -1;
        }

        init_unlock(msg);
        esPWRMAN_UsrEventNotify();
    }

    GUI_PostMessage(msg);
    return 0;
}

/* desktop msg hook */
int32_t init_hook_cb(__gui_msg_t *msg)
{
    if (init_mainwin)
    {
        __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(init_mainwin);

        if (init_ctr == NULL)
        {
            __err("file:%s line:%d fail\r", __FILE__, __LINE__);
            return EPDK_FAIL;
        }

        if (init_ctr->screen_close_done)
        {
            NOTIFY_MSG(DSK_MSG_SCREEN_OPEN, NULL, msg->h_deswin, 0, 0);
        }
    }

    GUI_PostMessage(msg);
    return 0;
}

static void _framework_init(void)
{
    AppletInfoManager   *info_manager = NULL;
    ActivityManager     *activity_manager = NULL;

    info_manager        = applet_info_manager_create();
    #if 1
    applet_info_manager_load_dir(info_manager, XML_PATH);
    #else//liujiaming test
    info_manager->alloc_nr          = 5;
    info_manager->nr                = 1;
    strcpy(info_manager->info->name,"application://app_root");
    strcpy(info_manager->info->exec,"data/mod/app_root.axf");    
    info_manager->info->loading     = NULL;
    info_manager->info->permission  = 1;
    info_manager->info->standby     = 1;
    __inf("info_manager->alloc_nr = %d",info_manager->alloc_nr);
    __inf("info_manager->nr = %d",info_manager->nr);
    __inf("info_manager->info->name = %s",info_manager->info->name);
    __inf("info_manager->info->exec = %s",info_manager->info->exec);
    __inf("info_manager->info->loading = %s",info_manager->info->loading);
    __inf("info_manager->info->permission = %d",info_manager->info->permission);
    __inf("info_manager->info->standby = %d",info_manager->info->standby);
    #endif
    //applet_info_manager_printf(info_manager);     // 打印applet info 信息
    applet_info_manager_set_defualt(info_manager);

    activity_manager    = activity_manager_create();

    activity_manager_set_default(activity_manager);
}

static void _framework_exit(void)
{
    AppletInfoManager   *info_manager       = applet_info_manager_get_default();
    ActivityManager     *activity_manager   = activity_manager_get_default();
    activity_manager_destroy(activity_manager);
    applet_info_manager_destroy(info_manager);
    applet_info_manager_set_defualt(NULL);
    activity_manager_set_default(NULL);
}

static void _process_init(void)
{
    H_WIN                   scene_adjust;
    int32_t flag            = 2;
    char load_para[1024]    = {0};

    _framework_init();

    #if CONFIG_SUPPORT_TOUCHPANEL
    open("/dev/tpadc_rtp", O_WRONLY);
    #endif
    
    eLIBs_memcpy(load_para, &flag, 4);
    __msg("********_process_init***********");
    activity_set_load_para("root", "", load_para, sizeof(load_para));
    activity_load_app("application://app_root");
}

static void _process_exit(void)
{
    bookengine_close();

    _uninstall_dbms_mod();

    msg_emit_deinit();

    _framework_exit();

    _close_audio_dev();
#ifdef CONFIG_DRIVER_ADC_POWER
    dsk_power_dev_uninit();
#endif
}

static H_LYR charge_scene_32bpp_layer_create(RECT *LayerRect)
{
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                                         /* size      */
        {0, 0, 0},                                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},     /* fmt       */

    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        9,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "charge scene",
        NULL,
        GUI_LYRWIN_STA_SLEEP,
        GUI_LYRWIN_NORMAL
    };
    fb.size.width           = LayerRect->width;
    fb.size.height          = LayerRect->height;
    fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
    lstlyr.src_win.x        = 0;
    lstlyr.src_win.y        = 0;
    lstlyr.src_win.width    = LayerRect->width;
    lstlyr.src_win.height   = LayerRect->height;
    lstlyr.scn_win.x        = LayerRect->x;
    lstlyr.scn_win.y        = LayerRect->y;
    lstlyr.scn_win.width    = LayerRect->width;
    lstlyr.scn_win.height   = LayerRect->height;
    lstlyr.pipe             = 1;
    lstlyr.fb               = &fb;
    lyrcreate_info.lyrpara  = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __log("desktop scene layer create error !");
    }

    return layer;
}

static int32_t charge_paint(__gui_msg_t *msg)
{
    static int32_t      index = 0;
    chagre_attr_t       *pattr;
    void                *pbmp;
    HBMP                hbmp;
    int32_t             width;
    int32_t             height;
    int32_t             screen_width;
    int32_t             screen_height;
    GUI_MEMDEV_Handle   hmem_dev;

    pattr = (chagre_attr_t *)GUI_WinGetAttr(msg->h_deswin);

    if (NULL == pattr)
    {
        return EPDK_OK;
    }

    dsk_display_get_size(&screen_width, &screen_height);
    __log("index=%d, pattr->bmp_hdl[index]=%x", index, pattr->bmp_hdl[index]);
    pbmp = dsk_theme_hdl2buf(pattr->bmp_hdl[index]);

    if (NULL == pbmp)
    {
        __log("pbmp is null...");
        return EPDK_OK;
    }

    hbmp = bmp_open(pbmp);

    if ((unsigned long)NULL == hbmp)
    {
        return EPDK_OK;
    }

    width = bmp_get_width(hbmp);
    height = bmp_get_height(hbmp);
    __log("width=%d, height=%d", width, height);

    if (hbmp)
    {
        bmp_close(hbmp);
        hbmp = (unsigned long)NULL;
    }

    GUI_LyrWinSetSta(pattr->hlayer, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSel(pattr->hlayer);
    hmem_dev = GUI_MEMDEV_Create((screen_width - width) / 2, (screen_height - height) / 2, width, height);

    if (!hmem_dev)
    {
        GUI_MEMDEV_Select(0);
    }
    else
    {
        GUI_MEMDEV_Select(hmem_dev);
    }

    GUI_BMP_Draw(pbmp, (screen_width - width) / 2, (screen_height - height) / 2);

    if (hmem_dev)
    {
        GUI_MEMDEV_CopyToLCD(hmem_dev);
        GUI_MEMDEV_Select(0);
        GUI_MEMDEV_Delete(hmem_dev);
        hmem_dev = 0;
    }

    index++;

    if (index >= CHARGE_BMP_NUM) //循环切换
    {
        index = 0;
    }

    if (EPDK_TRUE == dsk_power_is_full()) //如果充满了，就停留在最后一张
    {
        index = CHARGE_BMP_NUM - 1;
    }

    {
        GUI_RECT    rect;
        char        str[64];
        int32_t     w;
        dsk_langres_get_menu_text(STRING_INIT_ENTER_HOME, str, sizeof(str));

        if (SWFFont)
        {
            GUI_SetFont(SWFFont);
        }

        GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
        GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
        GUI_SetColor(GUI_RED);
        GUI_GetTextExtend(&rect, str, eLIBs_strlen(str));
        w = rect.x1 - rect.x0;
        GUI_DispStringAt(str, (screen_width - w) / 2, screen_height - 32);
    }
    return EPDK_OK;
}

static __s32 charge_mainwin_cb(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            int32_t         i;
            chagre_attr_t   *pattr;
            RECT            rect;
            int32_t         screen_width;
            int32_t         screen_height;

            pattr   = (chagre_attr_t *)GUI_WinGetAttr(msg->h_deswin);

            if (NULL == pattr)
            {
                return (signed long)NULL;
            }

            for (i = 0 ; i < CHARGE_BMP_NUM ; i++)
            {
                __log("pattr->bmp_id[i]=%d", pattr->bmp_id[i]);

                if (pattr->bmp_id[i])
                {
                    pattr->bmp_hdl[i] = dsk_theme_open(pattr->bmp_id[i]);
                    __log("i=%d, pattr->bmp_hdl[i]=%x", i, pattr->bmp_hdl[i]);
                }
                else
                {
                    __log("dsk_theme_open fail,bmp_id=%d", pattr->bmp_id[i]);
                    pattr->bmp_hdl[i] = 0;
                }
            }

            dsk_display_get_size(&screen_width, &screen_height);
            rect.x          = 0;
            rect.y          = 0;
            rect.width      = screen_width;
            rect.height     = screen_height;
            __log("screen_width=%d, screen_height=%d", screen_width, screen_height);
            pattr->hlayer = charge_scene_32bpp_layer_create(&rect);

            if (NULL == pattr->hlayer)
            {
                return EPDK_OK;
            }

            GUI_LyrWinSel(pattr->hlayer);
            GUI_SetBkColor(0x0);
            GUI_Clear();
            GUI_SetTimer(msg->h_deswin, pattr->play_timer_id, 100, NULL);
            GUI_LyrWinCacheOff();
            charge_paint(msg);
            return EPDK_OK;
        }

        case GUI_MSG_DESTROY:
        {
            int32_t         i;
            chagre_attr_t   *pattr;

            __log("charge_mainwin_cb, GUI_MSG_DESTROY begin...");
            pattr = (chagre_attr_t *)GUI_WinGetAttr(msg->h_deswin);

            if (NULL == pattr)
            {
                __log("show charge scene alreadly been destroy...");
                return (signed long)NULL;
            }

            if (GUI_IsTimerInstalled(msg->h_deswin, pattr->play_timer_id))
            {
                GUI_KillTimer(msg->h_deswin, pattr->play_timer_id);
            }

            gscene_bgd_set_state(BGD_STATUS_SHOW);

            if (pattr->hlayer)
            {
                GUI_LyrWinDelete(pattr->hlayer);
                pattr->hlayer = NULL;
            }

            for (i = 0 ; i < CHARGE_BMP_NUM ; i++)
            {
                __log("dsk_theme_close, pattr->bmp_hdl[i]=0x%x", pattr->bmp_hdl[i]);

                if (pattr->bmp_hdl[i])
                {
                    dsk_theme_close(pattr->bmp_hdl[i]);
                    pattr->bmp_hdl[i] = 0;
                }
            }

            eLIBs_memset(pattr, 0, sizeof(chagre_attr_t));
            esMEMS_Bfree(pattr, sizeof(chagre_attr_t));
            GUI_WinSetAttr(msg->h_deswin, NULL);
            __log("charge_mainwin_cb, GUI_MSG_DESTROY end...");
            return EPDK_OK;
        }

        case GUI_MSG_CLOSE:
            GUI_ManWinDelete(msg->h_deswin);
            return EPDK_OK;

        case GUI_MSG_TIMER:
        {
            chagre_attr_t *pattr;
            pattr = (chagre_attr_t *)GUI_WinGetAttr(msg->h_deswin);
            if (NULL == pattr)
            {
                return EPDK_OK;
            }

            if (msg->dwAddData1 == pattr->play_timer_id)
            {
                charge_paint(msg);

                if (EPDK_FALSE == dsk_get_charge_state())
                {
                    __gui_msg_t msgex;

                    __log("EPDK_FALSE == dsk_get_charge_state()");
                    eLIBs_memset(&msgex, 0, sizeof(__gui_msg_t));
                    msgex.id = GUI_MSG_CLOSE;
                    msgex.h_deswin = msg->h_deswin;
                    GUI_SendNotifyMessage(&msgex);
                }
            }

            return EPDK_OK;
        }

        case GUI_MSG_KEY:
        {
            if (KEY_DOWN_ACTION == msg->dwAddData2)
            {
                if (GUI_MSG_KEY_MENU == msg->dwAddData1)
                {
                    __gui_msg_t msgex;

                    eLIBs_memset(&msgex, 0, sizeof(__gui_msg_t));
                    msgex.id = GUI_MSG_CLOSE;
                    msgex.h_deswin = msg->h_deswin;
                    GUI_SendNotifyMessage(&msgex);
                }
            }

            return EPDK_OK;
        }

        case GUI_MSG_TOUCH:
        {
            return EPDK_OK;
        }

        default:
        {
            break;
        }
    }

    return GUI_ManWinDefaultProc(msg);
}

static H_WIN show_charege_mainwin_create(void)
{
    H_WIN                       hManWin;
    __gui_manwincreate_para_t   create_info;
    chagre_attr_t               *pattr;

    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    pattr = (chagre_attr_t *)esMEMS_Balloc(sizeof(chagre_attr_t));

    if (NULL == pattr)
    {
        return NULL;
    }

    eLIBs_memset(pattr, 0, sizeof(chagre_attr_t));
    pattr->play_timer_id = 0x43;
    pattr->bmp_id[0] = ID_INIT_CHARGE_BMP0_BMP;
    pattr->bmp_id[1] = ID_INIT_CHARGE_BMP1_BMP;
    pattr->bmp_id[2] = ID_INIT_CHARGE_BMP2_BMP;
    create_info.attr            = pattr;
    create_info.hParent         = NULL;
    create_info.hHosting        = NULL;
    create_info.ManWindowProc   = charge_mainwin_cb;
    create_info.name            = "charge";
    hManWin                     = GUI_ManWinCreate(&create_info);

    if (hManWin == NULL)
    {
        __log("show_charege_mainwin_create failed!");
        return NULL;
    }

    return hManWin;
}


int32_t show_charge_scene(void)
{
    int32_t         ret;
    H_WIN           hManWin;
    __gui_msg_t     msg;

    if (EPDK_FALSE == dsk_get_charge_state())
    {
        __log("not in charge state...");
        return EPDK_OK;
    }

    __log("in charge state...");
    hManWin = show_charege_mainwin_create();

    if (NULL == hManWin)
    {
        __log("show_charege_mainwin_create fail...");
        return EPDK_OK;
    }

    GUI_SetActiveManWin(hManWin);
    __log("hManWin = %x", hManWin);
    /* set message loop win */
    message_loop_win_set_default(hManWin);

    while (GUI_GetMessageEx(&msg, hManWin))     // 消息循环
    {
#ifdef __MSG

        if (msg.id != GUI_MSG_TIMER)
        {
            __msg("msg.h_deswin=%x, msg.id=%d, msg.dwAddData1=%d,msg.dwAddData2=%d"
                  , msg.h_deswin, msg.id, msg.dwAddData1, msg.dwAddData2);
        }

#endif
        ret = GUI_DispatchMessage(&msg);        // 分发消息到回调

        if (msg.p_arg)                          // 同步消息回应
        {
            GUI_SetSyncMsgRetVal(&msg, ret);
            GUI_PostSyncSem(&msg);
        }
    }

    GUI_WinThreadCleanup(hManWin);
    message_loop_win_set_default(NULL);
    GUI_SetActiveManWin(NULL);//置空，防止消息队列无对应窗口
    return EPDK_OK;
}

void application_init_process(void *arg)
{
    __gui_msg_t         msg;
    int32_t             ret = EPDK_FAIL;
    __memit_hook_t      emit_hook;
    __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
    __log("&           application_init_process enter!             &");
    __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");

    __inf("******start to dsk_reg_init_para******");
    ret = dsk_reg_init_para();
    __inf("******finish dsk_reg_init_para******");

    if (EPDK_FAIL == ret)
    {
        __msg("dsk_reg_init_para fail...");
    }
#ifdef CONFIG_DRIVER_ADC_POWER
    ret = dsk_power_dev_init();
    if (EPDK_FAIL == ret)
    {
        __msg("dsk_power_dev_init fail...");
    }
#endif
    __log("%s %d before live_init", __FUNCTION__, __LINE__);

    /* messge server init*/
    emit_hook.key_hook      = key_hook_cb;
    emit_hook.tp_hook       = tp_hook_cb;
    emit_hook.init_hook     = init_hook_cb;
    msg_emit_register_hook(&emit_hook);
    live_init();
    __log("%s %d before gscene_bgd_init", __FUNCTION__, __LINE__);
    {
        SIZE        screen_size;
        int32_t     w, h;

        dsk_display_get_size(&w, &h);
        screen_size.width    = w;
        screen_size.height   = h;
        gscene_bgd_init(&screen_size, BGD_STATUS_HIDE, FB_TYPE_YUV);
    }
#if 1 /*langaojie temp test*/ //remove wrn

    if (1)
	{
		int fd = 0;
		fd = open("/dev/hwsc", O_RDONLY);

		if (fd > 0)
		{
		    ioctl(fd, DEV_IOC_USR_HWSC_ENABLE_MONITOR, 0);
		    close(fd);
		    __log(".start device monitor .......................................");
		}
		else
		{
		    __log("try to open /dev/hwsc failed!");
		}
	}
#endif
    __log("%s %d before init_mainwin_create", __FUNCTION__, __LINE__);
    init_mainwin        = init_mainwin_create();

    GUI_SetActiveManWin(init_mainwin);

    message_loop_win_set_default(init_mainwin);
    __log("%s %d before _process_init", __FUNCTION__, __LINE__);
    _process_init();
#if 0 /*langaojie temp test*/ //remove wrn
    /*add usb mode */
    if (0)
    {
        ES_FILE     *pHwsc;
        pHwsc = eLIBs_fopen("b:\\HWSC\\hwsc", "rb+");

        if (pHwsc)
        {
            esMODS_MIoctrl(pHwsc, DEV_IOC_USR_ADD_USBD_MODE, USBD_UDISK, NULL);
            eLIBs_fclose(pHwsc);
            __log(".start device monitor .......................................");
        }
        else
        {
            __log("try to open b:\\HWSC\\hwsc failed!");
        }
    }
#endif	
    __log("%s %d before GUI_GetMessageEx", __FUNCTION__, __LINE__);
    /* message loop*/
    while (GUI_GetMessageEx(&msg, init_mainwin))    // 消息循环
    {
#ifdef __MSG
        if (msg.id != GUI_MSG_TIMER)
        {
            __msg("msg.h_deswin=%x, msg.id=%d, msg.dwAddData1=%d,msg.dwAddData2=%d", msg.h_deswin, msg.id, msg.dwAddData1, msg.dwAddData2);
        }
#endif
        ret = GUI_DispatchMessage(&msg);        // 分发消息到回调

        if (msg.p_arg)                          // 同步消息回应
        {
            GUI_SetSyncMsgRetVal(&msg, ret);
            GUI_PostSyncSem(&msg);
        }
    }
    
    shutdown_logo_show(1);
    
    GUI_WinThreadCleanup(init_mainwin);
    message_loop_win_set_default(NULL);

    _process_exit();
    __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
    __inf("&           application_init_process exit!              &");
    __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");


    esKRNL_TimeDly(100);
    
    shutdown_logo_show(0);
         
    //_close_lcd();
    //esKSRV_SysInfo();
    //esKSRV_PowerOff();
    __msg("end of application_init_process");
}
