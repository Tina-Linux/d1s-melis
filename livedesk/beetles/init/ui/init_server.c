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
#include <emodules/mod_update.h>
#include "mod_init_i.h"
#include "init_server.h"
#include "live_init.h"
#include "dialog_scene/dialog_scene.h"
#include "headbar/headbar_com.h"
#include "assistant_scene/assistant_scene.h"
//#include <board/disp_type.h>
#include "dfs_posix.h"

//#define ENABLE_MTP_FUNCTION
#define STANDBY_MODE_EN 0
#define OPEN_CURSOR                         0
#define AUTO_OFF_TIME_ID                    1000
#define CLOSE_SCN_TIME_ID                   1001
#define LOW_POWER_CHECK_TIME_ID             1002
#define CURSOR_TIME_ID                      1003
#define INIT_FW_UPDATE_MAX_ICON_RES_NUM     2

#define PIOC_MEM_BASE                   (0xf1c20800)
#define PIO_REG_CFG(n, i)               ((volatile unsigned int *)(unsigned long)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x00))
#define PIO_REG_DLEVEL(n, i)            ((volatile unsigned int *)(unsigned long)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x14))
#define PIO_REG_PULL(n, i)              ((volatile unsigned int *)(unsigned long)( PIOC_MEM_BASE + ((n)-1)*0x24 + ((i)<<2) + 0x1C))
#define PIO_REG_DATA(n)                 ((volatile unsigned int *)(unsigned long)( PIOC_MEM_BASE + ((n)-1)*0x24 + 0x10))

typedef struct
{
    int32_t     x;
    int32_t     y;
    int32_t     w;
    int32_t     h;
    int32_t     res_id[INIT_FW_UPDATE_MAX_ICON_RES_NUM];
    HTHEME      res_hdl[INIT_FW_UPDATE_MAX_ICON_RES_NUM];
} init_fw_update_rect_t;

typedef struct init_com_s
{
    H_WIN       h_win;
} __init_com_t;

typedef enum
{
    init_fw_update_prog_bg          = 0,
    init_fw_update_prog_left,
    init_fw_update_prog_mid,
    init_fw_update_prog_cursor,
    init_fw_update_prog_icon_num
} init_fw_update_icon_t;

extern GUI_FONT             *SWFFont;
__init_com_t                init_com;
static __bool               g_b_enable_standby  = 1;
static reg_system_para_t    *Init_reg_para      = NULL;
static __bool               is_close_screen     = EPDK_FALSE;

static int32_t init_close_screen(__gui_msg_t *msg, unsigned char enter_type);
static int32_t init_open_screen(__gui_msg_t *msg);

void init_reset_close_scn(void)
{
    __bool          exist;
    __init_ctl_t    *init_ctr;

    init_ctr = (__init_ctl_t *)GUI_WinGetAttr(init_com.h_win);
    exist = GUI_IsTimerInstalled(init_com.h_win, init_ctr->close_scn_time_id);

    if (exist == EPDK_TRUE)
    {
        GUI_ResetTimer(init_com.h_win, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
    }
}

void init_reset_auto_off(void)
{
    __bool exist;
    __init_ctl_t *init_ctr;

    init_ctr = (__init_ctl_t *)GUI_WinGetAttr(init_com.h_win);
    exist = GUI_IsTimerInstalled(init_com.h_win, init_ctr->auto_off_time_id);

    if (exist == EPDK_TRUE)
    {
        GUI_ResetTimer(init_com.h_win, init_ctr->auto_off_time_id, init_ctr->autooff_timeout, NULL);
    }
}

void init_lock(__gui_msg_t *msg)
{
    uint8_t         err;
    __init_ctl_t    *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(init_ctr == NULL)
	{
		__log("init_ctr == NULL");
		return;
	}
    esKRNL_SemPend(init_ctr->state_sem, 0, &err);
    return;
}

void init_unlock(__gui_msg_t *msg)
{
    __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

	if(init_ctr == NULL)
	{
		__log("init_ctr == NULL");
		return;
	}
    esKRNL_SemPost(init_ctr->state_sem);
}

__bool init_is_close_screen(void)
{
    return is_close_screen;
}

static void pull_down_gpio(uint32_t port, uint32_t port_num)
{
    volatile  uint32_t      *tmp_addr;
    uint32_t                tmp_data;
    uint32_t                tmp1;
    uint32_t                tmp2;

    //设置为输出
    tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
    tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x07 << tmp1);
    tmp_data |= (0x01 << tmp1);
    *tmp_addr = tmp_data;
    //设置为下拉
    tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
    tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x03 << tmp2);
    tmp_data |= (0x02 << tmp2);
    *tmp_addr = tmp_data;
    //输出低电平
    tmp_addr = PIO_REG_DATA(port);
    tmp_data = *tmp_addr;
    tmp_data &= ~(1 << port_num);
    //tmp_data |=  (1 << port_num);
    *tmp_addr = tmp_data;
}

static void pull_up_gpio(uint32_t port, uint32_t port_num)
{
    volatile  uint32_t      *tmp_addr;
    uint32_t                tmp_data;
    uint32_t                tmp1;
    uint32_t                tmp2;

    //设置为输出
    tmp1 = (port_num - ((port_num >> 3) << 3)) << 2;
    tmp_addr = PIO_REG_CFG(port, (port_num >> 3));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x07 << tmp1);
    tmp_data |= (0x01 << tmp1);
    *tmp_addr = tmp_data;
    //设置为上拉
    tmp2 = (port_num - ((port_num >> 4) << 4)) << 1;
    tmp_addr = PIO_REG_PULL(port, (port_num >> 4));
    tmp_data = *tmp_addr;
    tmp_data &= ~(0x03 << tmp2);
    tmp_data |= (0x01 << tmp2);
    *tmp_addr = tmp_data;
    //输出高电平
    tmp_addr = PIO_REG_DATA(port);
    tmp_data = *tmp_addr;
    tmp_data &= ~(1 << port_num);
    tmp_data |= (1 << port_num);
    *tmp_addr = tmp_data;
}

static int32_t get_gpio_status(uint32_t port, uint32_t port_num)
{
    volatile  uint32_t      *tmp_addr;
    uint32_t                tmp_data;
    int32_t                 ret;

    tmp_addr = PIO_REG_DATA(port);
    tmp_data = *tmp_addr;
    __wrn("tmp_data=0x%x", tmp_data);
    ret = (tmp_data & (1 << port_num)) >> port_num;
    return ret;
}
//跟init_manwin相关的资源初始化分开处理
static uint32_t init_server_init(__gui_msg_t *msg)
{
    __init_ctl_t    *init_ctr = NULL;

    init_ctr    = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (init_ctr == NULL)
    {
        __wrn("init_ctr == NULL");
        return EPDK_FAIL;
    }

    if (init_ctr->closescn_timeout != 0)
    {
        g_set_close_scn_time(init_ctr->closescn_timeout);
        //GUI_SetTimer(msg->h_deswin, init_ctr->close_scn_time_id , init_ctr->closescn_timeout, NULL);
    }

    if (init_ctr->autooff_timeout != 0)
    {
        dsk_set_auto_off_time(init_ctr->autooff_timeout);
        //GUI_SetTimer(msg->h_deswin, init_ctr->auto_off_time_id, init_ctr->autooff_timeout, NULL);
    }

    /*
    dsk_display_set_lcd_bright(Init_reg_para->lcd_bright * 10 );
    dsk_display_set_lcd_constract(Init_reg_para->lcd_constract * 10);
    dsk_display_set_lcd_saturation(Init_reg_para->lcd_saturation * 10);
    dsk_display_set_lcd_hue(Init_reg_para->lcd_hue * 10);
    */
    //dsk_keytone_set_state((__set_keytone_t)(Init_reg_para->keytone));//desktop模块里面1 表示on,2 表示off
    dsk_display_set_lcd_brightness((__lion_bright_t)(Init_reg_para->backlight + 1)); //背光亮度
    return EPDK_OK;
}

static void init_cursor_create(__init_ctl_t *m_ctl)
{
    HTHEME      h_them_cursor;
    HTHEME      h_them_pal;
    __pos_t     pos;
    int32_t     width, height;

    __err("headbar");
    h_them_cursor               = dsk_theme_open(0);
    m_ctl->cursor_bmp           = dsk_theme_hdl2buf(h_them_cursor);
    m_ctl->cursor_info.pat_mem  = m_ctl->cursor_bmp + 54 + 1024;
    h_them_pal = dsk_theme_open(0);
    m_ctl->cursor_info.palette  = dsk_theme_hdl2buf(h_them_pal);
    GUI_CursorCreate(&(m_ctl->cursor_info));
    dsk_display_get_size(&width, &height);

    pos.x   = width / 2;
    pos.y   = height / 2;
    GUI_CursorSetPos(&pos);
    GUI_CursorHide();
}

static void init_cursor_delete(__init_ctl_t *m_ctl)
{
    eLIBs_free(m_ctl->cursor_info.palette);
    eLIBs_free(m_ctl->cursor_bmp);
    GUI_CursorDelete();
}

static void system_message_handle(void *arg)
{
    init_scene_t *p_scene;
    H_WIN h_wnd = (H_WIN)arg;

    while (1)
    {
        if (esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
        {
            esKRNL_TDel(OS_PRIO_SELF);
            return;
        }

        //orchid update
        p_scene = (init_scene_t *)GUI_WinGetAddData(h_wnd);

        if (p_scene->orchid_update == EPDK_TRUE)
        {
            //dsk_orchid_update();
            //NOTIFY_MSG(DSK_MSG_ORCHID_UPDATE_END, NULL, h_wnd, 0, 0);
        }

        esKRNL_TimeDly(1);
    }
}

static int32_t sys_msg_handle_init(__gui_msg_t *msg)
{
    __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

    /*创建system_message_handle 线程 */
    init_ctr->sys_msg_handle_tid = esKRNL_TCreate(system_message_handle, (void *)msg->h_deswin, 0x800, KRNL_priolevel5);
    esKRNL_TaskNameSet(init_ctr->sys_msg_handle_tid, "initbkhdle");
    return EPDK_OK;
}

static int32_t sys_msg_handle_exit(__gui_msg_t *msg)
{
    __init_ctl_t    *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

    /* 删除system_message_handle 线程 */
    while (esKRNL_TDelReq(init_ctr->sys_msg_handle_tid) != OS_TASK_NOT_EXIST)
    {
        esKRNL_TimeDly(1);
    }

    init_ctr->sys_msg_handle_tid = 0;
    return EPDK_OK;
}

static int32_t init_low_power_proc(__gui_msg_t *msg)
{
    __init_ctl_t    *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (!init_ctr->usb_connect)
    {
        /* 退出应用程序*/
        //activity_return_home(NULL);
        //activity_kill_all_backgrd();
        //__here__;
        activity_finish_all();
        gscene_bgd_set_state(BGD_STATUS_SHOW);
        gscene_hbar_set_state(HBAR_ST_HIDE);
        /* low power dialog */
        {
            init_scene_t *p_scene;
            p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
            p_scene->low_power = EPDK_TRUE;
            scene_on_dialog(msg);
        }
    }

    return EPDK_OK;
}

static int32_t init_restart_close_scn(__gui_msg_t *msg)
{
    __bool          exist;
    __init_ctl_t    *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (init_ctr->closescn_timeout == 0)
    {
        return EPDK_OK;
    }

    exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id);

    if (exist == EPDK_TRUE)
    {
        GUI_ResetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
    }
    else
    {
        GUI_SetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
    }

    __wrn("set close scn timer id ");
    return EPDK_OK;
}
static __s32 init_low_power_off_proc(__gui_msg_t *msg)
{
    __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (!init_ctr->usb_connect)
    {
        init_scene_t *p_scene = NULL;
        //activity_return_home(NULL);
        //activity_kill_all_backgrd();
        activity_finish_all();
        gscene_bgd_set_state(BGD_STATUS_SHOW);
        gscene_hbar_set_state(HBAR_ST_HIDE);
        /* poweroff dialog */
        p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
        p_scene->power_off = EPDK_TRUE;
        //__here__;
        scene_on_dialog(msg);
        NOTIFY_MSG(GUI_MSG_CLOSE, 0, msg->h_deswin, 0, 0);
        //init_close_screen(msg, 2);走standby流程
        //__here__;
        //__here__;
    }

    return EPDK_OK;
}

static __s32 init_power_off_proc(__gui_msg_t *msg)
{
    __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (!init_ctr->usb_connect)
    {
        init_scene_t *p_scene = NULL;
        //activity_return_home(NULL);
        //activity_kill_all_backgrd();
        activity_finish_all();
        gscene_bgd_set_state(BGD_STATUS_SHOW);
        gscene_hbar_set_state(HBAR_ST_HIDE);
        /* poweroff dialog */
        p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
        p_scene->power_off = EPDK_TRUE;
        //__here__;
        scene_on_dialog(msg);

        //__here__;
        if (EPDK_FALSE == p_scene->usb_plug_in) //usb连接到来把关机对话框关闭后，程序不应该往下执行，只有usb未连接才能关机
        {
            __msg("NOTIFY_MSG(GUI_MSG_CLOSE");
            /* 设置闹铃唤醒  */
            standby_startup_alarm();
            //notify "init" to exit
            NOTIFY_MSG(GUI_MSG_CLOSE, 0, msg->h_deswin, 0, 0);
        }

        //__here__;
    }

    return EPDK_OK;
}

static __s32 init_usb_device_plugin_proc(__gui_msg_t *msg)
{
    __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
    /* 通知除主界面外所有应用程序退出 */
    init_ctr->usb_connect = EPDK_TRUE;
    //activity_return_home(NULL);
    //activity_kill_all_backgrd();
    //activity_suspend_top();
    activity_finish_all();

    if (dsk_wkm_is_open())
    {
        dsk_wkm_close();
    }
#ifdef CONFIG_DRIVERS_USB_GADGET_MSC
	{
	   extern void usbd_plugin_all_disk(void);
	   usbd_plugin_all_disk();
	}
#endif

    gscene_bgd_set_state(BGD_STATUS_SHOW);
    gscene_hbar_set_state(HBAR_ST_HIDE);
    /* usb dialog */
    {
        init_scene_t *p_scene = NULL;
        p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
        p_scene->usb_connect = EPDK_TRUE;
        scene_on_dialog(msg);
    }
    return EPDK_OK;
}

static __s32 init_usb_device_plugout_proc(__gui_msg_t *msg)
{
    __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
    init_ctr->usb_connect = EPDK_FALSE;
    {
        init_scene_t *p_scene;
        p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
        p_scene->usb_connect = EPDK_FALSE;
        scene_on_dialog(msg);
        p_scene->usb_plug_in = EPDK_FALSE;
    }
    return EPDK_OK;
}

/* 通知主应用程序 更新orchid开始 */
static __s32 init_orchid_update_start(__gui_msg_t *msg)
{
    {
        init_scene_t *p_scene;
        p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
        p_scene->orchid_update = EPDK_TRUE;
        scene_on_dialog(msg);
    }
    return EPDK_OK;
}

/* 通知主应用程序 更新orchid结束 */
static __s32 init_orchid_update_end(__gui_msg_t *msg)
{
    {
        init_scene_t *p_scene;
        p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
        p_scene->orchid_update = EPDK_FALSE;
    }
    return EPDK_OK;
}

/*set gpio for enter standby*/
/*
static void init_into_standby(void)
{
    (*(volatile unsigned int *)(0xF1C24400 + 0x2c)) &= ~0x5;//x1,y1pull down
    pull_down_gpio(7, 1);
    dsk_amplifier_onoff(0);
    //pull_up_gpio(7,6);
}
*/
/*set gpio for enter standby*/
/*
static void init_out_standby(void)
{
    pull_down_gpio(7, 6);
    pull_up_gpio(7, 1);
    (*(volatile unsigned int *)(0xF1C24400 + 0x2c)) |= 0x5;//x1,y1pull up
}
*/

/*
   关屏由于在待机中可能会按键唤醒机器，这里按power按键关机时
   便采用了进入standby的方法，故添加type区分是普通进入standby还是
   关机，还是低电关机。期中进入standby和普通定时或是遥控关机
   需要有唤醒功能，低电关机这里处理为不允许唤醒，只能硬件
   重启。关机过程中不需要检查当前的应用，充电状态。
   type 0 表示进入standby
   type 1 表示普通关机允许唤醒
   type 2 表示电量低关机
*/
static __s32 init_close_screen(__gui_msg_t *msg, unsigned char enter_type)
{
    __bool is_host_working;
    __u32 output;
	int standby_mode_en = 0;
    //__gui_msg_t msgex;
    __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
    output = dsk_display_get_output_type();

    if ((output == DISP_OUTPUT_TYPE_LCD && !init_ctr->usb_connect) || enter_type) //add by Kingvan
    {
        /* 置关屏状态位 */
        init_lock(msg);

        if (!init_ctr->screen_close_done || enter_type) /* 未关屏 */
        {
            init_ctr->screen_close_done = EPDK_TRUE;
            is_close_screen = EPDK_TRUE;
            init_unlock(msg);
            /* 调用显示驱动, 关屏 */
            dsk_display_lcd_off();
            __wrn("%s %d", __FILE__, __LINE__);

            if (!(EPDK_TRUE == activity_all_able_standby() && !dsk_wkm_is_playing()
                  && !dsk_radio_is_open() && g_is_enable_standby()))
            {
                //__init_decr_freq(msg);//112350
            }

            /* 设置闹铃唤醒  */
            standby_startup_alarm();
            /*{
                ES_FILE                 *p_disp;

                p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
                if(!p_disp)
                {
                    __err("open display device fail!");
                }
                esMODS_MIoctrl(p_disp, DISP_CMD_CLK_OFF, 0, 0);
                eLIBs_fclose(p_disp);
            }*/
            //notify top
            /*msgex.id          = DSK_MSG_SCREEN_CLOSE;
            msgex.h_srcwin      = 0;
            msgex.h_deswin      = 0;
            msgex.dwAddData1    = 0;
            msgex.dwAddData2    = 0;
            activity_notify_top(&msgex);    */
            /* 进入 standby */
            //__here__;
            is_host_working = dsk_usbh_is_working();
            __wrn("g_is_enable_standby:%d", g_is_enable_standby());
            __wrn("is_host_working:%d", is_host_working);
            __wrn("activity_all_able_standby:%d", activity_all_able_standby());
            __wrn("dsk_wkm_is_playing:%d", dsk_wkm_is_playing());
            __wrn("dsk_radio_is_open:%d", dsk_radio_is_open());
            __wrn("dsk_get_charge_state:%d", dsk_get_charge_state());
			if (esCFG_GetKeyValue("app_para", "standby_mode_en", (int *)&standby_mode_en, 1) != EPDK_OK)
			{
				standby_mode_en = 0;
				__err("standby_mode_en err!");
			}
			if(standby_mode_en)
			{

	            if ((/*EPDK_FALSE == is_host_working &&*/ EPDK_TRUE == activity_all_able_standby() && !dsk_wkm_is_playing()
	                    && !dsk_radio_is_open() && g_is_enable_standby()
	                    //&& EPDK_FALSE == dsk_get_charge_state()
	                ) || enter_type)
	            {
	                __s32 ret;
	                user_gpio_set_t gpio_set;
	                __s32 switch_dpdm_io_sta = -1;
#if 1 /*langaojie temp test*/ //remove wrn
	                int pfile = -1;
	                //__here__;
	                //init_into_standby();
	                pfile = open("/dev/audio_play0",O_RDWR);

	                if (!(pfile < 0))
	                {
	                    //system will enter standby, set audio device to standby mode for power save
	                    dsk_speaker_turnoff();
	                    ioctl(pfile, AUDIO_DEV_CMD_ENTER_STANDBY, 0);
	                }
#endif

#if 0
	                {
	                    switch_dpdm_io_sta = -1;
	                    ret = esCFG_GetKeyValue("usbc0", "usb_switch_gpio", (int *)&gpio_set, sizeof(user_gpio_set_t) / 4);

	                    if (EPDK_OK == ret)
	                    {
	                        __wrn("gpio_set.port=%d", gpio_set.port);
	                        __wrn("gpio_set.port_num=%d", gpio_set.port_num);
	                        switch_dpdm_io_sta = get_gpio_status(gpio_set.port, gpio_set.port_num);
	                        //拉高gpio，给大口u盘standby检测用
	                        pull_up_gpio(gpio_set.port, gpio_set.port_num);
	                    }
	                }
#endif
	                __wrn("switch_dpdm_io_sta=%d", switch_dpdm_io_sta);
	                esPWRMAN_EnterStandby(enter_type);
	                //init_out_standby();
#if 0
	                {
	                    //恢复gpio口
	                    if (-1 != switch_dpdm_io_sta)
	                    {
	                        if (0 == switch_dpdm_io_sta)
	                        {
	                            pull_down_gpio(gpio_set.port, gpio_set.port_num);
	                        }
	                        else if (1 == switch_dpdm_io_sta)
	                        {
	                            pull_up_gpio(gpio_set.port, gpio_set.port_num);
	                        }
	                    }
	                }
#endif
	                __wrn("shiql standby test 1");
	                {
	                    //查看是否关机时间已经到了
	                    __sys_pwrman_para_t standby_para;
	                    esPWRMAN_GetStandbyPara(&standby_para);

	                    if (STANDBY_EVENT_POWEROFF & standby_para.EventFlag)
	                    {
	                        dsk_power_off();
	                    }
	                }
#if 1 /*langaojie temp test*/ //remove wrn

	                //__here__;
	                if (!(pfile<0))
	                {
	                    //active audio device
	                    ioctl(pfile, AUDIO_DEV_CMD_EXIT_STANDBY, 0);
	                    esKRNL_TimeDly(10);
	                    dsk_speaker_resume();
	                    close(pfile);
	                    pfile = -1;
	                }
#endif
	                __wrn("WILL ENTER init_open_screen");
	                init_open_screen(msg);
	            }
			}
        }
        else                                    /* 已经关屏 */
        {
            init_unlock(msg);
        }
    }

    return EPDK_OK;
}

static __u32 cpu_clk = 0;
static __u32 sdram_clk = 0;

static __s32 __init_decr_freq(__gui_msg_t *msg)
{
    __s32 ret;
    __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
    init_ctr->bdec_freq = 1;
    ////__here__;
    //cpu_clk = esCLK_GetSrcFreq(CSP_CCM_SYS_CLK_CORE_PLL);
    ////__here__;
    //sdram_clk = esCLK_GetSrcFreq(CSP_CCM_SYS_CLK_SDRAM_PLL);
    //__msg("sdram_clk=%d", sdram_clk);
    ////__here__;
    //esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_CORE_PLL, 120*1000*1000);
    // //__here__;
    // //esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_SDRAM_PLL, 180*1000*1000);
    //ret = esCLK_GetSrcFreq(CSP_CCM_SYS_CLK_SDRAM_PLL);
    //__msg("ret=%d", ret);
    ////__here__;
    //esPWM_LockCpuFreq();
    ////__here__;
    return EPDK_OK;
}

static __s32 __init_incr_freq(__gui_msg_t *msg)
{
    __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (1 == init_ctr->bdec_freq)
    {
        ////__here__;
        //
        //esPWM_UnlockCpuFreq();
        // __msg("sdram_clk=%d", sdram_clk);
        // esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_SDRAM_PLL, sdram_clk);
        //__msg("cpu_clk=%d", cpu_clk);
        //esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_CORE_PLL, cpu_clk);
        ////__here__;
        init_ctr->bdec_freq = 0;
    }

    return EPDK_OK;
}


/**
 * 开屏
 */
static __s32 init_open_screen(__gui_msg_t *msg)
{
    __bool exist;
    __u32 output;
    __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
    init_scene_t *p_scene   = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
    /*{
        ES_FILE                 *p_disp;

        p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
        if(!p_disp)
        {
            __err("open display device fail!");
        }
        esMODS_MIoctrl(p_disp, DISP_CMD_CLK_ON, 0, 0);
        eLIBs_fclose(p_disp);
    }*/
    __wrn("%s", __FUNCTION__);
    output = dsk_display_get_output_type();

    //__here__;
    if ((output == DISP_OUTPUT_TYPE_NONE) || (output == DISP_OUTPUT_TYPE_LCD))
    {
        //__here__;
        init_lock(msg);

        //__here__;
        if (init_ctr->screen_close_done)    /* 屏幕已经关闭 */
        {
            //__here__;
            init_unlock(msg);
            //__here__;
            //__init_incr_freq(msg);//112350
            dsk_display_lcd_on();               /* 调用显示驱动, 开屏*/
            init_lock(msg);
            init_ctr->screen_close_done = EPDK_FALSE;/*     清关屏状态位 */
            is_close_screen = EPDK_FALSE;
            init_unlock(msg);
            //屏幕锁
            //p_scene->scnlock_win = create_screen_lock(msg->h_deswin);
            //GUI_WinSetFocusChild(p_scene->scnlock_win);
        }
        else                                    /* 屏幕还没关闭 */
        {
            init_unlock(msg);
        }

        __wrn("g_is_enable_standby():%d", g_is_enable_standby());
        __wrn("init_ctr->closescn_gate_on:%d", init_ctr->closescn_gate_on);
        __wrn("init_ctr->closescn_timeout:%d", init_ctr->closescn_timeout);

        if (init_ctr->closescn_gate_on == EPDK_TRUE && init_ctr->closescn_timeout != 0)
        {
            GUI_SetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);/* 从新设置定时器*/
        }
    }

    return EPDK_OK;
}

/**
 * 设置自动关屏时间
 */
static __s32 init_set_close_scn(__gui_msg_t *msg)
{
    __bool exist;
    __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
    __wrn("init_set_close_scn 1");

    if (msg->dwAddData1 == 0)
    {
        init_ctr->closescn_timeout = 0;//add by Kingvan
        exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id);

        if (exist == EPDK_TRUE)
        {
            GUI_KillTimer(msg->h_deswin, init_ctr->close_scn_time_id);
        }
    }
    else if (msg->dwAddData1 > 0)
    {
        init_ctr->closescn_timeout = msg->dwAddData1 / 10;
        exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id);

        if (exist == EPDK_TRUE)
        {
            __wrn("init_set_close_scn 2");
            GUI_ResetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
        }
        else
        {
            __wrn("init_set_close_scn 3");
            GUI_SetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
        }
    }
    else
    {
        __wrn(" close screen time < 0, set fail ");
    }

    return EPDK_OK;
}

/**
 * 获取自动关屏时间
 */
static __s32 init_get_close_scn(__gui_msg_t *msg)
{
    __bool exist;
    __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
    return init_ctr->closescn_timeout * 10 / 1000;
    return EPDK_OK;
}


/**
 * 设置自动关机时间
 */
static __s32 init_set_auto_off(__gui_msg_t *msg)
{
    __bool exist;
    __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
    init_ctr->autooff_timeout = 0;
    __wrn("set auto power off timer id ");

    if (msg->dwAddData1 <= 0)
    {
        exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->auto_off_time_id);

        if (exist == EPDK_TRUE)
        {
            GUI_KillTimer(msg->h_deswin, init_ctr->auto_off_time_id);
        }
    }
    else if (msg->dwAddData1 > 0)
    {
        init_ctr->autooff_timeout = msg->dwAddData1 / 10;
        exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->auto_off_time_id);

        if (exist == EPDK_TRUE)
        {
            GUI_ResetTimer(msg->h_deswin, init_ctr->auto_off_time_id, init_ctr->autooff_timeout, NULL);
        }
        else
        {
            GUI_SetTimer(msg->h_deswin, init_ctr->auto_off_time_id, init_ctr->autooff_timeout, NULL);
        }

        __wrn("set auto power off timer id ");
    }

    return EPDK_OK;
}

static int32_t init_scene_create(__gui_msg_t *msg)
{
    init_screen_size();
	
    dialog_init(msg->h_deswin);

    gscene_hbar_create(msg->h_deswin, HBAR_FOARMAT_32BPP);
    return EPDK_OK;
}

static __s32 init_scene_destroy(__gui_msg_t *msg)
{
    init_scene_t *p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
    dialog_deinit(msg->h_deswin);
    //__here__;
    gscene_hbar_delete();
    //__here__;
    //gscene_bgd_deinit();
    return EPDK_OK;
}

static __s32 init_voltage_check(__gui_msg_t *msg)
{
    __gui_msg_t msgex;
    static power_level_e old_vol_level = DSK_POWER_LEVEL_;
    static __bool old_charge_state = 0;
    power_level_e   level;
    __bool charge_state;
    charge_state = dsk_get_charge_state();
    dsk_power_get_voltage_level(&level);

    if (old_vol_level != level || old_charge_state != charge_state)
    {
        msgex.id            = DSK_MSG_VOLTAGE_CHANGE;
        msgex.h_srcwin      = NULL;
        msgex.h_deswin      = msg->h_deswin;
        msgex.dwAddData1    = (__u32)level;
        msgex.dwAddData2    = 0;
        msgex.dwReserved    = 0;
        msgex.p_arg         = NULL;
        activity_notify_top(&msgex);
    }

    __msg("level=%d", level);
    old_vol_level = level;
    old_charge_state = charge_state;
    return EPDK_OK;
}

static __s32 __init_prog_draw_progress(init_fw_update_rect_t *prog_bg, init_fw_update_rect_t *prog_left,
                                       init_fw_update_rect_t *prog_mid, init_fw_update_rect_t *prog_cursor,
                                       __s32 min, __s32 max, __s32 cur)
{
    //画进度条背景
    {
        void *pbmp;

        if (!prog_bg->res_hdl[0])
        {
            __msg("prog_bg->res_hdl[0] is null...");
            return EPDK_FAIL;
        }

        pbmp = dsk_theme_hdl2buf(prog_bg->res_hdl[0]);

        if (!pbmp)
        {
            __msg("pbmp is null...");
            return EPDK_FAIL;
        }

        GUI_BMP_Draw(pbmp, prog_bg->x, prog_bg->y);
    }
    //画进度条左边的图标
    {
        void *pbmp;
        __s32 focus;
        focus = 0;

        if (!prog_left->res_hdl[focus])
        {
            __msg("prog_left->res_hdl[focus] is null...");
            return EPDK_FAIL;
        }

        pbmp = dsk_theme_hdl2buf(prog_left->res_hdl[focus]);

        if (!pbmp)
        {
            __msg("pbmp is null...");
            return EPDK_FAIL;
        }

        GUI_BMP_Draw(pbmp, prog_left->x, prog_left->y);
    }

    //画进度条和cursor
    if (cur > max) //保护一下
    {
        cur = max;
    }

    if (cur < min) //保护一下
    {
        cur = min;
    }

    if (cur >= min
        && cur <= max)
    {
        void *pbmp;

        if (!prog_mid->res_hdl[0])
        {
            __msg("prog_mid->res_hdl[0] is null...");
            return EPDK_FAIL;
        }

        pbmp = dsk_theme_hdl2buf(prog_mid->res_hdl[0]);

        if (!pbmp)
        {
            __msg("pbmp is null...");
            return EPDK_FAIL;
        }

        {
            __s32 i;
            __s32 n;
            __s32 mid_w;
            __s32 max_mid_w;
            __s32 cursor_w;
            __s32 cursor_pos;
            __s32 bg_pos;
            __s32 bg_w;

            if (cur == min)
            {
                n = 0;
            }
            else
            {
                n = (prog_bg->w - prog_cursor->w) * (cur - min)
                    / prog_mid->w / (max - min);
            }

            mid_w = prog_mid->w;
            max_mid_w = prog_bg->w - prog_cursor->w;
            cursor_w = prog_cursor->w;
            bg_pos = prog_bg->x;
            bg_w = prog_bg->w;

            //防止非整数倍时画不满
            if (max_mid_w - n * mid_w < mid_w)
            {
                n++;
                cursor_pos = bg_pos + bg_w - cursor_w;
            }
            else
            {
                cursor_pos = bg_pos + n * mid_w;
            }

            for (i = 1 ; i < n ; i++)
            {
                GUI_BMP_Draw(pbmp, prog_mid->x + i * prog_mid->w
                             , prog_mid->y);
            }

            //画cursor
            if (!prog_cursor->res_hdl[0])
            {
                __msg("prog_cursor->res_hdl[0] is null...");
                return EPDK_FAIL;
            }

            pbmp = dsk_theme_hdl2buf(prog_cursor->res_hdl[0]);

            if (!pbmp)
            {
                __msg("pbmp is null...");
                return EPDK_FAIL;
            }

            GUI_BMP_Draw(pbmp, cursor_pos, prog_cursor->y);
        }
    }

    return EPDK_OK;
}

static H_LYR __prog_32bpp_layer_create(RECT *rect, __s32 pipe)
{
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt       */
    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
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
        "sub menu layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    fb.size.width       = rect->width;
    fb.size.height      = rect->height;
    lstlyr.src_win.x        = 0;
    lstlyr.src_win.y        = 0;
    lstlyr.src_win.width    = rect->width;
    lstlyr.src_win.height   = rect->height;
    lstlyr.scn_win.x        = rect->x;
    lstlyr.scn_win.y        = rect->y;
    lstlyr.scn_win.width    = rect->width;
    lstlyr.scn_win.height   = rect->height;
    lstlyr.pipe = pipe;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("app bar layer create error !");
    }

    return layer;
}

static void fw_update(char *fw_path)
{
    __s32 len;
    __u8    mid_update;
    __mp    *mod_update;
    __s32 progress;
    RECT rect;
    char str_text[256];
    GUI_RECT gui_rect;
    GUI_MEMDEV_Handle memdev_hdl = 0;
    H_LYR hlyr;
    __s32 width, height;
    __u32 status = 0;
    user_gpio_set_t  gpio_set[1];
    __s32 ret;
    __hdle  card_pin_hd[2] = {NULL};
    init_fw_update_rect_t prog_ui[init_fw_update_prog_icon_num];
    __wrn("fw_path=%s", fw_path);
    dsk_display_get_size(&width, &height);
    rect.x = 0;
    rect.y = 0;
    rect.width = width;
    rect.height = height;
    hlyr = __prog_32bpp_layer_create(&rect, 1);

    if (NULL == hlyr)
    {
        __err("create layer fail...");
        return ;
    }

    GUI_LyrWinSetSta(hlyr, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(hlyr);
    GUI_LyrWinSel(hlyr);
    dsk_langres_get_menu_text(STRING_FW_UPDATE, str_text, sizeof(str_text));
    GUI_SetBkColor(GUI_BLUE);
    GUI_Clear();
    gui_rect.x0 = 0;
    gui_rect.y0 = height / 2 - 64;
    gui_rect.x1 = rect.width - 1;
    gui_rect.y1 = gui_rect.y0 + 32;
    GUI_SetFont(SWFFont);
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetColor(GUI_BLACK);
    GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
    GUI_DispStringInRect(str_text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    __wrn("str_text=%s ", str_text);
    prog_ui[init_fw_update_prog_bg].x = (width - 680) / 2;
    prog_ui[init_fw_update_prog_bg].y = height / 2;
    prog_ui[init_fw_update_prog_bg].w = 680;
    prog_ui[init_fw_update_prog_bg].h = 26;
    prog_ui[init_fw_update_prog_bg].res_id[0] = ID_INIT_PROG_BG_BMP;
    prog_ui[init_fw_update_prog_bg].res_id[1] = 0;
    prog_ui[init_fw_update_prog_left].x = (width - 680) / 2;
    prog_ui[init_fw_update_prog_left].y = height / 2;
    prog_ui[init_fw_update_prog_left].w = 12;
    prog_ui[init_fw_update_prog_left].h = 26;
    prog_ui[init_fw_update_prog_left].res_id[0] = ID_INIT_PROG_LEFT_BMP;
    prog_ui[init_fw_update_prog_left].res_id[1] = 0;
    prog_ui[init_fw_update_prog_mid].x = (width - 680) / 2;
    prog_ui[init_fw_update_prog_mid].y = height / 2;
    prog_ui[init_fw_update_prog_mid].w = 12;
    prog_ui[init_fw_update_prog_mid].h = 26;
    prog_ui[init_fw_update_prog_mid].res_id[0] = ID_INIT_PROG_MID_BMP;
    prog_ui[init_fw_update_prog_mid].res_id[1] = 0;
    prog_ui[init_fw_update_prog_cursor].x = (width - 680) / 2;
    prog_ui[init_fw_update_prog_cursor].y = height / 2;
    prog_ui[init_fw_update_prog_cursor].w = 26;
    prog_ui[init_fw_update_prog_cursor].h = 26;
    prog_ui[init_fw_update_prog_cursor].res_id[0] = ID_INIT_PROG_CURSOR_BMP;
    prog_ui[init_fw_update_prog_cursor].res_id[1] = 0;

    while (1)
    {
        ES_FILE *pfile = NULL;
        pfile = eLIBs_fopen(fw_path, "rb");

        if (NULL != pfile)
        {
            eLIBs_fclose(pfile);
            break;
        }
        else
        {
            __log("请在TF卡放置固件:[%s]", fw_path);
        }
    }

    {
        int i;
        int j;

        for (i = 0 ; i < init_fw_update_prog_icon_num ; i++)
        {
            for (j = 0 ; j < INIT_FW_UPDATE_MAX_ICON_RES_NUM ; j++)
            {
                if (prog_ui[i].res_id[j])
                {
                    prog_ui[i].res_hdl[j] = dsk_theme_open(prog_ui[i].res_id[j]);

                    if (!prog_ui[i].res_hdl[j])
                    {
                        __wrn("dsk_theme_open fail...");
                    }
                    else//先读取nor数据占住到内存
                    {
                        dsk_theme_hdl2buf(prog_ui[i].res_hdl[j]);
                    }
                }
                else
                {
                    prog_ui[i].res_hdl[j] = 0;
                }
            }
        }
    }

    __init_prog_draw_progress(&prog_ui[init_fw_update_prog_bg],
                              &prog_ui[init_fw_update_prog_left],
                              &prog_ui[init_fw_update_prog_mid],
                              &prog_ui[init_fw_update_prog_cursor],
                              0, 100, 0);
    mid_update = esMODS_MInstall(BEETLES_APP_ROOT"mod\\update.mod", 0);

    if (mid_update == 0)
    {
        __wrn("update mod install error ");
        goto err1;
    }

    mod_update = esMODS_MOpen(mid_update, 0);

    if (mod_update == NULL)
    {
        __err("open update mode error ");
        esMODS_MUninstall(mid_update);
        goto err1;
    }

    __msg("mod_update = %x,file=%s", mod_update, fw_path);
    esMODS_MIoctrl(mod_update, UPDATE_CMD_START, 0, (void *)fw_path);
    __wrn(" ioctrl mod update complete ");

    while (1)
    {
        esKRNL_TimeDly(50);
        progress = esMODS_MIoctrl(mod_update, UPDATE_CMD_CHECK_PROG, 0, 0);

        if (-1  == progress)
        {
            __wrn("error occur");
            break;
        }

        GUI_LyrWinSel(hlyr);
        // memdev_hdl = GUI_MEMDEV_Create(
        //     prog_ui[init_fw_update_prog_bg].x,
        //     prog_ui[init_fw_update_prog_bg].y,
        //     prog_ui[init_fw_update_prog_bg].w,
        //     prog_ui[init_fw_update_prog_bg].h);
        //if (!memdev_hdl)
        //{
        //     __wrn("GUI_MEMDEV_Create fail...");
        //  continue ;
        //}
        //GUI_MEMDEV_Select(memdev_hdl);
        GUI_SetBkColor(GUI_BLUE);
        GUI_Clear();
        __init_prog_draw_progress(&prog_ui[init_fw_update_prog_bg],
                                  &prog_ui[init_fw_update_prog_left],
                                  &prog_ui[init_fw_update_prog_mid],
                                  &prog_ui[init_fw_update_prog_cursor],
                                  0, 100, progress);
        gui_rect.x0 = 0;
        gui_rect.y0 = height / 2 - 64;
        gui_rect.x1 = rect.width - 1;
        gui_rect.y1 = gui_rect.y0 + 32;
        GUI_SetFont(SWFFont);
        GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
        GUI_SetColor(GUI_BLACK);
        GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
        eLIBs_memset(str_text, 0, sizeof(str_text));
        dsk_langres_get_menu_text(STRING_FW_UPDATE, str_text, sizeof(str_text));
        eLIBs_sprintf(str_text, "%s%d\%", str_text, progress);
        GUI_DispStringInRect(str_text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        __wrn("str_text=%s ", str_text);
        ///GUI_MEMDEV_CopyToLCD(memdev_hdl);
        ///GUI_MEMDEV_Select( NULL );
        ///GUI_MEMDEV_Delete(memdev_hdl);
        __wrn("current progress is %d%%", progress);

        if (100 == progress)
        {
            __wrn("update complete");
            //esMODS_MIoctrl(mod_update, UPDATE_CMD_STOP, 0, 0);
            esKRNL_TimeDly(100);
            break;
        }
    }

    status = esMODS_MIoctrl(mod_update, UPDATE_CMD_CHECK_PROG, 0, 0);
    gui_rect.x0 = 0;
    gui_rect.y0 = height / 2 - 64;
    gui_rect.x1 = rect.width - 1;
    gui_rect.y1 = gui_rect.y0 + 32;
    GUI_Clear();
    GUI_LyrWinSel(hlyr);
    GUI_SetFont(SWFFont);
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetColor(GUI_BLACK);
    GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
    __wrn("status===========%d", status);

    if (status != -1)
    {
        dsk_langres_get_menu_text(STRING_UPDATE_FINISH, str_text, sizeof(str_text));
    }
    else
    {
        dsk_langres_get_menu_text(STRING_UPDATE_FAIL, str_text, sizeof(str_text));
    }

    GUI_DispStringInRect(str_text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    ret = esCFG_GetKeyValue("sddet_para", "detect_pin", (__s32 *)gpio_set, sizeof(user_gpio_set_t) / 4);

    if (!ret)
    {
        card_pin_hd[0] = esPINS_PinGrpReq(gpio_set, 1);

        if (!card_pin_hd[0])
        {
            __wrn("SD Detect : request detect pin fail");
        }

        __wrn("sd dectect pin port = %d, pin_num = %d", gpio_set->port, gpio_set->port_num);
    }

    while (1)
    {
        esKRNL_TimeDly(100);

        if (card_pin_hd[0])
        {
            __s32 data = 0;// 0:in；1:out。
            data = esPINS_ReadPinData(card_pin_hd[0], 0);  //获取电平

            if (1 == data)
            {
                __err("sdcard plugout , reboot now");
            }
        }
    }

    //esMODS_MClose(mod_update);
    //esMODS_MUninstall(mid_update);
err1:
    return ;
}

//#define USBD_HID_FUNC_ENABLE
#ifdef USBD_HID_FUNC_ENABLE

__u8 				thread_id;
__s32 usbd_func_hid_enable = 0;


typedef enum
{
    PANEL_TCON  = 0,
    PANEL_GAMMA_R  = 1,
    PANEL_GAMMA_G    = 2,
    PANEL_GAMMA_B  = 3,
} set_panel_mode;

#define USB_RECV_DATA_LEN   512
__u8 usb_recv_data[USB_RECV_DATA_LEN];
__u8 usb_send_data[USB_RECV_DATA_LEN];

static __u8 r_gamma[256];
static __u8 g_gamma[256];
static __u8 b_gamma[256];

__hdle p_disp_dev;
__u32 recv_data_flag = 0;

static void lcd_get_enhance(__u8 *pbuffer)
{
	disp_enhance_gain_para para;
	__u32 arg[3];
	arg[0] = (__u32)&para;
	arg[1] = 0;
	arg[2] = 0;
	esFSYS_fioctrl(p_disp_dev, DISP_ENHANCE_GET_GAIN, 0, (void *)arg);
    pbuffer[0] = esFSYS_fioctrl(p_disp_dev, DISP_ENHANCE_GET_MODE, 0, (void *)0);
    pbuffer[1] = esFSYS_fioctrl(p_disp_dev, DISP_ENHANCE_GET_BRIGHT, 0, (void *)0);
    pbuffer[2] = esFSYS_fioctrl(p_disp_dev, DISP_ENHANCE_GET_CONTRAST, 0, (void *)0);
    pbuffer[3] = esFSYS_fioctrl(p_disp_dev, DISP_ENHANCE_GET_SATURATION, 0, (void *)0);
    pbuffer[4] = esFSYS_fioctrl(p_disp_dev, DISP_ENHANCE_GET_HUE, 0, (void *)0);
    pbuffer[5] = para.lti_gain;
    pbuffer[6] = para.peak_gain;
    pbuffer[7] = para.peak_hp_ratio;
    pbuffer[8] = para.peak_bp0_ratio;

__log("enhance here = %d %d %d %d %d %d %d %d %d",pbuffer[0],pbuffer[1],pbuffer[2],pbuffer[3],pbuffer[4],pbuffer[5],pbuffer[6],pbuffer[7],pbuffer[8]);
}

static void lcd_set_enhance(__u8 *pbuffer)
{
	__u64 buf[3];
	disp_enhance_gain_para para;

	buf[0] = 0;
    buf[1] = 0;
    buf[2] = 0;
    esMODS_MIoctrl(p_disp_dev, DISP_ENHANCE_ENABLE, 0, (void *)buf);

    buf[0] = pbuffer[0];
    buf[1] = 0;
    buf[2] = 0;
    esMODS_MIoctrl(p_disp_dev, DISP_ENHANCE_SET_MODE, 0, (void *)buf);
    buf[0] = pbuffer[1];
    esMODS_MIoctrl(p_disp_dev, DISP_ENHANCE_SET_BRIGHT, 0, (void *)buf);
    buf[0] = pbuffer[2];
    esMODS_MIoctrl(p_disp_dev, DISP_ENHANCE_SET_CONTRAST, 0, (void *)buf);
    buf[0] = pbuffer[3];
    esMODS_MIoctrl(p_disp_dev, DISP_ENHANCE_SET_SATURATION, 0, (void *)buf);
    buf[0] = pbuffer[4];
    esMODS_MIoctrl(p_disp_dev, DISP_ENHANCE_SET_HUE, 0, (void *)buf);
	para.lti_gain = pbuffer[5];
	para.peak_gain= pbuffer[6];
	para.peak_hp_ratio= pbuffer[7];
	para.peak_bp0_ratio= pbuffer[8];
	buf[0] = &para;
    esMODS_MIoctrl(p_disp_dev, DISP_ENHANCE_SET_GAIN, 0, (void *)buf);
__log("gain = %d peak_gain = %d hp = %d bp0 = %d",para.lti_gain,para.peak_gain,para.peak_hp_ratio,para.peak_bp0_ratio);

}


static __s32 encode_lcd_panel_info(__u8 *pbuffer, set_panel_mode mode)
{
    __u32 buf[3];
	__u32 i;
    __u16 data;
    disp_panel_para panel_para;

    buf[0] = (__u32)(&panel_para);
    buf[1] = 0;
    buf[2] = 0;
    esFSYS_fioctrl(p_disp_dev, DISP_CMD_GET_PANEL_INFO, 0, (void *)buf);
//    __log("lcd_ht=%d, lcd_vt=%d, lcd_rb_swap=%d\n", panel_para.lcd_ht, panel_para.lcd_vt, panel_para.lcd_rb_swap);


	switch(mode)
	{
		case PANEL_TCON:
			
	    pbuffer[0] = 0x07;
	    pbuffer[1] = 0x55;
	    pbuffer[2] = 0x00;
	    pbuffer[3] = 0x00;
	    pbuffer[4] = 0x02;

	    data = 106 - 7;
	    pbuffer[5] = (__u8)(data & 0x00FF);
	    pbuffer[6] = (__u8)((data & 0xFF00) >> 8);

	    data = (__u16)panel_para.lcd_x;
	    pbuffer[7] = (__u8)(data & 0x00FF);
	    pbuffer[8] = (__u8)((data & 0xFF00) >> 8);

	    data = (__u16)panel_para.lcd_y;
	    pbuffer[9] = (__u8)(data & 0x00FF);
	    pbuffer[10] = (__u8)((data & 0xFF00) >> 8);

	    data = (__u16)panel_para.lcd_ht;
	    pbuffer[11] = (__u8)(data & 0x00FF);
	    pbuffer[12] = (__u8)((data & 0xFF00) >> 8);

	    data = (__u16)panel_para.lcd_vt;
	    pbuffer[13] = (__u8)(data & 0x00FF);
	    pbuffer[14] = (__u8)((data & 0xFF00) >> 8);

	    data = (__u16)panel_para.lcd_hbp;
	    pbuffer[15] = (__u8)(data & 0x00FF);
	    pbuffer[16] = (__u8)((data & 0xFF00) >> 8);

	    data = (__u16)panel_para.lcd_vbp;
	    pbuffer[17] = (__u8)(data & 0x00FF);
	    pbuffer[18] = (__u8)((data & 0xFF00) >> 8);

	    data = (__u16)panel_para.lcd_hspw;
	    pbuffer[19] = (__u8)(data & 0x00FF);
	    pbuffer[20] = (__u8)((data & 0xFF00) >> 8);

	    data = (__u16)panel_para.lcd_vspw;
	    pbuffer[21] = (__u8)(data & 0x00FF);
	    pbuffer[22] = (__u8)((data & 0xFF00) >> 8);

	    pbuffer[23] = (__u8)panel_para.lcd_rb_swap;
	    pbuffer[24] = 0x00;
	    pbuffer[25] = 0x00;
	    pbuffer[26] = 0x00;
	//
	    pbuffer[27] = 0x00;
	    pbuffer[28] = 0x00;
	    pbuffer[29] = 0x00;
	    pbuffer[30] = 0x00;
	    pbuffer[31] = 0x00;
	    pbuffer[32] = 0x00;
	    pbuffer[33] = 0x00;
	    pbuffer[34] = 0x00;
	    pbuffer[35] = 0x00;
	    pbuffer[36] = 0x00;
	    pbuffer[37] = 0x00;
	    pbuffer[38] = 0x00;
	    pbuffer[39] = 0x00;
	    pbuffer[40] = 0x00;
	    pbuffer[41] = 0x00;
	    pbuffer[42] = 0x00;
	    pbuffer[43] = 0x00;
	    pbuffer[44] = 0x00;
	    pbuffer[45] = 0x00;
	    pbuffer[46] = 0x00;
	    pbuffer[47] = 0x00;
	    pbuffer[48] = 0x00;
	    pbuffer[49] = 0x00;
	    pbuffer[50] = 0x00;
	    pbuffer[51] = 0x00;
	    pbuffer[52] = 0x00;
	    pbuffer[53] = 0x00;
	    pbuffer[54] = 0x00;
	    pbuffer[55] = 0x00;
	    pbuffer[56] = 0x00;
	    pbuffer[57] = 0x00;
	    pbuffer[58] = 0x00;
	    pbuffer[59] = 0x00;
	    pbuffer[60] = 0x00;
	    pbuffer[61] = 0x00;
	    pbuffer[62] = 0x00;
	//
	    pbuffer[63] = (__u8)panel_para.lcd_if;
	    pbuffer[64] = (__u8)panel_para.lcd_dclk_freq;
	    pbuffer[65] = (__u8)panel_para.lcd_pwm_used;
	    pbuffer[66] = (__u8)panel_para.lcd_pwm_ch;
	    data = (__u16)panel_para.lcd_pwm_freq;
	    pbuffer[67] = (__u8)(data & 0x00FF);
	    pbuffer[68] = (__u8)((data & 0xFF00) >> 8);
	    pbuffer[69] = (__u8)panel_para.lcd_pwm_pol;
	    pbuffer[70] = (__u8)panel_para.lcd_rgb_endian;

	    pbuffer[71] = (__u8)panel_para.lcd_frm;
	    pbuffer[72] = (__u8)panel_para.lcd_gamma_en;
	    pbuffer[73] = (__u8)panel_para.lcd_cmap_en;
	    pbuffer[74] = (__u8)panel_para.lcd_interlace;
	    pbuffer[75] = 0x00;
	    pbuffer[76] = 0x00;
	    pbuffer[77] = 0x00;
	    pbuffer[78] = 0x00;
	    pbuffer[79] = 0x00;
	    pbuffer[80] = 0x00;

	    pbuffer[81] = (__u8)panel_para.lcd_hv_if;
	    pbuffer[82] = (__u8)panel_para.lcd_hv_srgb_seq;
	    pbuffer[83] = (__u8)panel_para.lcd_hv_syuv_seq;
	    pbuffer[84] = (__u8)panel_para.lcd_hv_syuv_fdly;
	    pbuffer[85] = (__u8)panel_para.lcd_hv_clk_phase;
	    pbuffer[86] = (__u8)panel_para.lcd_hv_sync_polarity;

	    pbuffer[87] = (__u8)panel_para.lcd_lvds_if;
	    pbuffer[88] = (__u8)panel_para.lcd_lvds_colordepth;
	    pbuffer[89] = (__u8)panel_para.lcd_lvds_mode;
	    pbuffer[90] = (__u8)panel_para.lcd_lvds_io_polarity;

	    pbuffer[91] = (__u8)panel_para.lcd_cpu_if;
	    pbuffer[92] = (__u8)panel_para.lcd_cpu_te;

	    pbuffer[93] = (__u8)panel_para.lcd_dsi_if;
	    pbuffer[94] = (__u8)panel_para.lcd_dsi_lane;
	    pbuffer[95] = (__u8)panel_para.lcd_dsi_format;
	    pbuffer[96] = (__u8)panel_para.lcd_dsi_eotp;
	    pbuffer[97] = (__u8)panel_para.lcd_dsi_vc;
	    pbuffer[98] = (__u8)panel_para.lcd_dsi_te;

	    pbuffer[99] = (__u8)panel_para.lcd_edp_rate;
	    pbuffer[100] = (__u8)panel_para.lcd_edp_lane;

	    pbuffer[101] = (__u8)panel_para.lcd_edp_colordepth;
	    pbuffer[102] = (__u8)panel_para.lcd_edp_fps;
	    pbuffer[103] = 0x00;
	    pbuffer[104] = 0x00;
	    pbuffer[105] = 0x00;
		break;
		
		case PANEL_GAMMA_R:
		eLIBs_memcpy(pbuffer,&panel_para.lcd_gamma_r,256);
		break;
		
		case PANEL_GAMMA_G:
		eLIBs_memcpy(pbuffer,&panel_para.lcd_gamma_g,256);
		break;
		
		case PANEL_GAMMA_B:
		eLIBs_memcpy(pbuffer,&panel_para.lcd_gamma_b,256);
		/*
		for(i = 0;i<256;i++)
		{
		__log("panle_gamma_b[%d] = %x",i,panel_para.lcd_gamma_b[i]);
		
		}
		*/
		break;
	}
    return 0;
}


static __s32 decode_lcd_panel_info(__u8 *pbuffer, set_panel_mode mode)
{
#if 1
    __u32 buf[3];
    __u16 data;
    disp_panel_para panel_para;

    buf[0] = (__u32)(&panel_para);
    buf[1] = 0;
    buf[2] = 0;
    esFSYS_fioctrl(p_disp_dev, DISP_CMD_GET_PANEL_INFO, 0, (void *)buf);
//    __log("lcd_x=%d, lcd_y=%d, lcd_rb_swap=%d\n", panel_para.lcd_x, panel_para.lcd_y, panel_para.lcd_rb_swap);
switch(mode)
{
	case PANEL_TCON:
		panel_para.lcd_x = (__u32)((pbuffer[8] << 8) | pbuffer[7]);
		panel_para.lcd_y = (__u32)((pbuffer[10] << 8) | pbuffer[9]);
		panel_para.lcd_ht = (__u32)((pbuffer[12] << 8) | pbuffer[11]);
		panel_para.lcd_vt = (__u32)((pbuffer[14] << 8) | pbuffer[13]);
		panel_para.lcd_hbp = (__u32)((pbuffer[16] << 8) | pbuffer[15]);
		panel_para.lcd_vbp = (__u32)((pbuffer[18] << 8) | pbuffer[17]);
	    panel_para.lcd_hspw = (__u32)((pbuffer[20] << 8) | pbuffer[19]);
	    panel_para.lcd_vspw = (__u32)((pbuffer[22] << 8) | pbuffer[21]);
	    panel_para.lcd_rb_swap = pbuffer[23];
	    panel_para.lcd_if = pbuffer[63];
	    panel_para.lcd_dclk_freq = pbuffer[64];
	    panel_para.lcd_pwm_used = pbuffer[65];
	    panel_para.lcd_pwm_ch = pbuffer[66];
	    panel_para.lcd_pwm_freq = (__u32)((pbuffer[68] << 8) | pbuffer[67]);
	    panel_para.lcd_pwm_pol = pbuffer[69];
	    panel_para.lcd_rgb_endian = pbuffer[70];
	    panel_para.lcd_frm = pbuffer[71];
	    panel_para.lcd_gamma_en = pbuffer[72];
	    panel_para.lcd_cmap_en = pbuffer[73];
	    panel_para.lcd_interlace = pbuffer[74];

	    panel_para.lcd_hv_if = pbuffer[81];
	    panel_para.lcd_hv_srgb_seq = pbuffer[82];
	    panel_para.lcd_hv_syuv_seq = pbuffer[83];
	    panel_para.lcd_hv_syuv_fdly = pbuffer[84];
	    panel_para.lcd_hv_clk_phase = pbuffer[85];
	    panel_para.lcd_hv_sync_polarity = pbuffer[86];
	    panel_para.lcd_lvds_if = pbuffer[87];
	    panel_para.lcd_lvds_colordepth = pbuffer[88];
	    panel_para.lcd_lvds_mode = pbuffer[89];
	    panel_para.lcd_lvds_io_polarity = pbuffer[90];

	    panel_para.lcd_cpu_if = pbuffer[91];
	    panel_para.lcd_cpu_te = pbuffer[92];
	    panel_para.lcd_dsi_if = pbuffer[93];
	    panel_para.lcd_dsi_lane = pbuffer[94];
	    panel_para.lcd_dsi_format = pbuffer[95];
	    panel_para.lcd_dsi_eotp = pbuffer[96];
	    panel_para.lcd_dsi_vc = pbuffer[97];
	    panel_para.lcd_dsi_te = pbuffer[98];
	    panel_para.lcd_edp_rate = pbuffer[99];
	    panel_para.lcd_edp_lane = pbuffer[100];
	    panel_para.lcd_edp_colordepth = pbuffer[101];
	    panel_para.lcd_edp_fps = pbuffer[102];
		break;
		
	case PANEL_GAMMA_R:
		
			eLIBs_memcpy(&panel_para.lcd_gamma_r,r_gamma,256);
			break;
			
	case PANEL_GAMMA_G:
			eLIBs_memcpy(&panel_para.lcd_gamma_g,g_gamma,256);
			break;

	case PANEL_GAMMA_B:
			eLIBs_memcpy(&panel_para.lcd_gamma_b,b_gamma,256);
			break;		

}
    panel_para.lcd_tool_en = 1;
//    __log("lcd_x=%d, lcd_y=%d, lcd_rb_swap=%d\n", panel_para.lcd_x, panel_para.lcd_y, panel_para.lcd_rb_swap);
    buf[0] = (__u32)(&panel_para);
    buf[1] = 0;
    buf[2] = 0;
    esFSYS_fioctrl(p_disp_dev, DISP_CMD_SET_PANEL_INFO, 0, (void *)buf);
#endif
    return 0;
}


//#define DBUG_TOOL
void lcd_tool_thread(void *parg)
{
    __u8 cmd;
    int pack_len;
    int data_len;
	__u32 i;
	__u32 connect_flag = 0;
    ES_FILE   *pHwsc;
    ES_FILE   *pHid;
    p_disp_dev = esFSYS_fopen("b:\\DISP\\DISPLAY", "r+");
    if (!p_disp_dev)
    {
        __log("open display device fail!");
        return;
    }

    pHwsc = eLIBs_fopen("b:\\HWSC\\hwsc", "rb+");
    if (pHwsc)
    {
        esMODS_MIoctrl(pHwsc, DEV_IOC_USR_HWSC_ENABLE_MONITOR, 0, NULL);
        eLIBs_fclose(pHwsc);
        __log(".start device monitor .......................................\n");
    }
    else
    {
        __log("try to open b:\\HWSC\\hwsc failed!\n");
        return;
    }

    while (1)
    {
		if(esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
		{     
			eLIBs_fclose(pHid);
			esKRNL_TDel(EXEC_prioself);

			break;
		}
        pHid = eLIBs_fopen("b:\\HID\\usbhid", "rb+");
        if (pHid == NULL)
        {
            __log("open. b:\\HID\\usbhid fail\n");
            esKRNL_TimeDly(100);
        }
        else
        {
            __log("open b:\\HID\\usbhid succeed\n");
            break;
        }

    }

    while (1)
    {
		if(esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
		{     
			eLIBs_fclose(pHid);
			eLIBs_printf("close opend hid handle!\n");
			esKRNL_TDel(EXEC_prioself);

			break;
		}
        pack_len = eLIBs_fread(usb_recv_data, 1, USB_RECV_DATA_LEN, pHid);
		
        if (pack_len == 0)
        {
	        if(connect_flag == 0)
	    	{
		        usb_send_data[0] = 0x07;
		        usb_send_data[1] = 0x55;
		        usb_send_data[2] = 0x00;
		        usb_send_data[3] = 0x00;
		        usb_send_data[4] = 0xff;
		        pack_len = 5;
		        //__log(" wait to connect");
				eLIBs_fwrite(usb_send_data, 1, pack_len, pHid);//win10 need to hold connect
	    	}
	        goto end;
        }
		
		if(usb_recv_data[4] == 0xff)
		{
			connect_flag = 0;
			__log("pc exit!");
		}
		else
		{
			connect_flag = 1;
		}
		
        //__log("usb recv data pack_len=%d\n", pack_len);
        //__log("usb_recv_data:(%x, %x, %x, %x, %x, %x, %x)\n", usb_recv_data[0], usb_recv_data[1],
        //usb_recv_data[2], usb_recv_data[3], usb_recv_data[4], usb_recv_data[5], usb_recv_data[6]);

        if (usb_recv_data[0] != 0x07)
        {
            __log("the hid id is err id=%d\n", usb_recv_data[0]);
            goto end;
        }
        if (usb_recv_data[1] != 0x55)
        {
            __log("the head is err!!!\n");
            goto end;
        }

        if (usb_recv_data[2] != 0)
        {
            __log("the IC is not C800!!!\n");
            goto end;
        }

        if (usb_recv_data[3] != 0)
        {
            __log("the system is not melis!!!\n");
            goto end;
        }

        cmd = usb_recv_data[4];
        __log("cmd=%x\n", cmd);

        switch (cmd)
        {
            // set tcon
            case 0x01:
            {
                data_len = (usb_recv_data[6] << 8) | usb_recv_data[5];
                __log("data_len=%d\n", data_len);
                if (data_len != 99)
                {
                    __log("data_len is err cmd=%d, data_len=%d\n", cmd, data_len);
                    goto end;
                }

                usb_send_data[0] = 0x07;
                usb_send_data[1] = 0x55;
                usb_send_data[2] = 0x00;
                usb_send_data[3] = 0x00;
                usb_send_data[4] = 0x01;
                usb_send_data[5] = (__u8)(data_len & 0x00FF);
                usb_send_data[6] = (__u8)((data_len & 0xFF00) >> 8);
                pack_len = 7;
                eLIBs_fwrite(usb_send_data, 1, pack_len, pHid);

                pack_len = data_len + 7;
				#ifdef DBUG_TOOL
    				for(i=0;i<106;i++)
    				{
    					__log("C800 recv tcon[%d] = %x",i,usb_recv_data[i]);
    				}
				#endif
                decode_lcd_panel_info(usb_recv_data, PANEL_TCON);
                break;
            }

            case 0x04://set gamma_r
            {
                data_len = (usb_recv_data[6] << 8) | usb_recv_data[5];
                __log("data_len=%d\n", data_len);
                if (data_len != 256)
                {
                    __log("data_len is err cmd=%d, data_len=%d\n", cmd, data_len);
                    goto end;
                }

                usb_send_data[0] = 0x07;
                usb_send_data[1] = 0x55;
                usb_send_data[2] = 0x00;
                usb_send_data[3] = 0x00;
                usb_send_data[4] = 0x04;
                usb_send_data[5] = (__u8)(data_len & 0x00FF);
                usb_send_data[6] = (__u8)((data_len & 0xFF00) >> 8);
                pack_len = 7;
                eLIBs_fwrite(usb_send_data, 1, pack_len, pHid);

				eLIBs_memcpy(r_gamma,usb_recv_data + 7,256);
				#ifdef DBUG_TOOL
    				for(i=0;i<256 + 7;i++)
    				{
    					__log("C800 recv gamma_r[%d] = %x",i,usb_recv_data[i]);
    				}
				#endif
                decode_lcd_panel_info(usb_recv_data, PANEL_GAMMA_R);
                break;
            }

            case 0x06://set gamma_g
            {
                data_len = (usb_recv_data[6] << 8) | usb_recv_data[5];
                __log("data_len=%d\n", data_len);
                if (data_len != 256)
                {
                    __log("data_len is err cmd=%d, data_len=%d\n", cmd, data_len);
                    goto end;
                }

                usb_send_data[0] = 0x07;
                usb_send_data[1] = 0x55;
                usb_send_data[2] = 0x00;
                usb_send_data[3] = 0x00;
                usb_send_data[4] = 0x06;
                usb_send_data[5] = (__u8)(data_len & 0x00FF);
                usb_send_data[6] = (__u8)((data_len & 0xFF00) >> 8);
                pack_len = 7;
                eLIBs_fwrite(usb_send_data, 1, pack_len, pHid);

				eLIBs_memcpy(g_gamma,usb_recv_data + 7,256);
				#ifdef DBUG_TOOL
				for(i=0;i<256 +7;i++)
				{
					__log("C800 recv gamma_g[%d] = %x",i,usb_recv_data[i]);
				}
				#endif
                decode_lcd_panel_info(usb_recv_data, PANEL_GAMMA_G);
                break;
            }
			
            case 0x08://set gamma_b
            {
                data_len = (usb_recv_data[6] << 8) | usb_recv_data[5];
                __log("data_len=%d\n", data_len);
                if (data_len != 256)
                {
                    __log("data_len is err cmd=%d, data_len=%d\n", cmd, data_len);
                    goto end;
                }

                usb_send_data[0] = 0x07;
                usb_send_data[1] = 0x55;
                usb_send_data[2] = 0x00;
                usb_send_data[3] = 0x00;
                usb_send_data[4] = 0x08;
                usb_send_data[5] = (__u8)(data_len & 0x00FF);
                usb_send_data[6] = (__u8)((data_len & 0xFF00) >> 8);
                pack_len = 7;
                eLIBs_fwrite(usb_send_data, 1, pack_len, pHid);

                pack_len = data_len + 7;

				eLIBs_memcpy(b_gamma,usb_recv_data + 7,256);
				#ifdef DBUG_TOOL
				for(i=0;i<256 + 7;i++)
				{
					__log("C800 recv gamma_b[%d] = %x",i,usb_recv_data[i]);
				}
				#endif
                decode_lcd_panel_info(usb_recv_data, PANEL_GAMMA_B);

                break;
            }

            case 0x0a://set enchance
            {
                data_len = (usb_recv_data[6] << 8) | usb_recv_data[5];
                __log("data_len=%d\n", data_len);
                if (data_len != 256)
                {
                    __log("data_len is err cmd=%d, data_len=%d\n", cmd, data_len);
                    goto end;
                }

                usb_send_data[0] = 0x07;
                usb_send_data[1] = 0x55;
                usb_send_data[2] = 0x00;
                usb_send_data[3] = 0x00;
                usb_send_data[4] = 0x0a;
                usb_send_data[5] = (__u8)(data_len & 0x00FF);
                usb_send_data[6] = (__u8)((data_len & 0xFF00) >> 8);
                pack_len = 7;
                __log("ret = %d",eLIBs_fwrite(usb_send_data, 1, pack_len, pHid));
                pack_len = data_len + 7;

				//eLIBs_memcpy(b_gamma,usb_recv_data + 7,256);
				#ifdef DBUG_TOOL
				for(i=0;i<256 + 7;i++)
				{
					__log("C800 recv enhance[%d] = %x",i,usb_recv_data[i]);
				}
				#endif
                lcd_set_enhance(usb_recv_data + 7);
                break;
            }
			
            // check gamma_r
            case 0x05:
                data_len = (usb_recv_data[6] << 8) | usb_recv_data[5];
                __log("data_len=%d\n", data_len);
                if (data_len != 256)
                {
                    __log("data_len is err cmd=%d, data_len=%d\n", cmd, data_len);
                    goto end;
                }

                usb_send_data[0] = 0x07;
                usb_send_data[1] = 0x55;
                usb_send_data[2] = 0x00;
                usb_send_data[3] = 0x00;
                usb_send_data[4] = 0x05;
                usb_send_data[5] = (__u8)(data_len & 0x00FF);
                usb_send_data[6] = (__u8)((data_len & 0xFF00) >> 8);
                encode_lcd_panel_info(usb_send_data + 7, PANEL_GAMMA_R);
				pack_len = data_len + 7;
				#ifdef DBUG_TOOL
				for(i=0;i<256 + 7;i++)
				{
					__log("C800 send gamma_r[%d] = %x",i,usb_send_data[i]);
				}
				#endif
                eLIBs_fwrite(usb_send_data, 1, pack_len, pHid);
                break;

            //gamma_g  para from c800 to pc
            case 0x07:
                data_len = (usb_recv_data[6] << 8) | usb_recv_data[5];
                __log("data_len=%d\n", data_len);
                if (data_len != 256)
                {
                    __log("data_len is err cmd=%d, data_len=%d\n", cmd, data_len);
                    goto end;
                }

                usb_send_data[0] = 0x07;
                usb_send_data[1] = 0x55;
                usb_send_data[2] = 0x00;
                usb_send_data[3] = 0x00;
                usb_send_data[4] = 0x07;
                usb_send_data[5] = (__u8)(data_len & 0x00FF);
                usb_send_data[6] = (__u8)((data_len & 0xFF00) >> 8);
                encode_lcd_panel_info(usb_send_data + 7, PANEL_GAMMA_G);
				pack_len = data_len + 7;
				#ifdef DBUG_TOOL
				for(i=0;i<256 + 7;i++)
				{
					__log("C800 send gamma_g[%d] = %x",i,usb_send_data[i]);
				}
				#endif
				eLIBs_fwrite(usb_send_data, 1, pack_len, pHid);
                break;

            //para from c800 to pc
            case 0x09:
                data_len = (usb_recv_data[6] << 8) | usb_recv_data[5];
                __log("data_len=%d\n", data_len);
                if (data_len != 256)
                {
                    __log("data_len is err cmd=%d, data_len=%d\n", cmd, data_len);
                    goto end;
                }

                usb_send_data[0] = 0x07;
                usb_send_data[1] = 0x55;
                usb_send_data[2] = 0x00;
                usb_send_data[3] = 0x00;
                usb_send_data[4] = 0x09;
                usb_send_data[5] = (__u8)(data_len & 0x00FF);
                usb_send_data[6] = (__u8)((data_len & 0xFF00) >> 8);
                encode_lcd_panel_info(usb_send_data + 7, PANEL_GAMMA_B);
				pack_len = data_len + 7;
				#ifdef DBUG_TOOL
				for(i=0;i<256 + 7;i++)
				{
					__log("C800 send gamma_b[%d] = %x",i,usb_send_data[i]);
				}
				#endif
                eLIBs_fwrite(usb_send_data, 1, pack_len, pHid);
                break;

				
            //para from c800 to pc  check_tcon
            case 0x02:
                data_len = (usb_recv_data[6] << 8) | usb_recv_data[5];
                __log("data_len=%d\n", data_len);
                if (data_len != 99)
                {
                    __log("data_len is err cmd=%d, data_len=%d\n", cmd, data_len);
                    goto end;
                }
                encode_lcd_panel_info(usb_send_data, PANEL_TCON);
                pack_len = data_len + 7;
				#ifdef DBUG_TOOL
				for(i=0;i<256;i++)
				{
					__log("C800 send tcon[%d] = %x",i,usb_send_data[i]);
				}
				#endif
                eLIBs_fwrite(usb_send_data, 1, pack_len, pHid);
                break;

				//get enhance
            case 0x0b:
                data_len = (usb_recv_data[6] << 8) | usb_recv_data[5];
                __log("data_len=%d\n", data_len);
                if (data_len != 256)
                {
                    __log("data_len is err cmd=%d, data_len=%d\n", cmd, data_len);
                    goto end;
                }
                usb_send_data[0] = 0x07;
                usb_send_data[1] = 0x55;
                usb_send_data[2] = 0x00;
                usb_send_data[3] = 0x00;
                usb_send_data[4] = 0x0b;
                usb_send_data[5] = (__u8)(data_len & 0x00FF);
                usb_send_data[6] = (__u8)((data_len & 0xFF00) >> 8);
                lcd_get_enhance(usb_send_data + 7);
                pack_len = data_len + 7;
				
				#ifdef DBUG_TOOL
				for(i=0;i<256;i++)
				{
					__log("C800 send enhance[%d] = %x",i,usb_send_data[i]);
				}
				#endif
                eLIBs_fwrite(usb_send_data, 1, pack_len, pHid);
                break;

				
            // usb应答心跳包
            case 0x03:
                usb_send_data[0] = 0x07;
                usb_send_data[1] = 0x55;
                usb_send_data[2] = 0x00;
                usb_send_data[3] = 0x00;
                usb_send_data[4] = 0x03;
                pack_len = 5;
	            eLIBs_fwrite(usb_send_data, 1, pack_len, pHid);
                break;

            default:
                __log("cmd is err, cmd=%x\n", cmd);
                break;
        };

end:
        esKRNL_TimeDly(2);
    }
}
#endif
#ifdef CONFIG_DRIVERS_USB_GADGET_MSC
enum
{
       USB_SLAVE_MSC_PART_PLUGIN = 0,
       USB_SLAVE_MSC_PART_PLUGOUT,
};

typedef struct disk_manager_tag
{
       unsigned char part_path[16];
       unsigned char device_name[16];
       unsigned char used;
       unsigned char plug_status;
} disk_manager_t;
disk_manager_t disk_manager[CONFIG_DRIVERS_USB_GADGET_MSC_MAX_LUNS] = {0};

#if 0
static int device_file_isexist(disk_manager_t *disk_manager)
{
   int ret = -1;
   unsigned char device_name[16] = {0};

   ret = eLIBs_GetPTName(disk_manager->part_path, device_name);
   if (ret == 0 && 0 == strncmp(device_name, disk_manager->device_name, strlen(disk_manager->device_name))) {
	   return 1;
   }
   return 0;
}

static int disk_manager_add_and_plugin(unsigned char *path, __gui_msg_t *msg)
{
	int index = 0, ret = 0, fd = -1;
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
	init_scene_t *p_scene;

	p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
	if (p_scene->usb_plug_in == EPDK_TRUE) {
		fd = open("/dev/msc", O_RDWR);
		if (fd < 0) {
			eLIBs_printf("can not find device:%s\n", "/dev/msc");
			return EPDK_FAIL;
		}
	}
	for (index = 0; index < sizeof(disk_manager)/sizeof(disk_manager[0]); index++) {
		eLIBs_printf("%s %d %s disk_manager[%d].used:%d plug_status:%d\n", __FILE__, __LINE__, __func__, index, disk_manager[index].used,disk_manager[index].plug_status);
		if (disk_manager[index].used == 0) {
			eLIBs_memset(disk_manager[index].part_path, 0x00, sizeof(disk_manager[index].part_path));
			eLIBs_strcpy(disk_manager[index].part_path, path);
			eLIBs_memset(disk_manager[index].device_name, 0x00, sizeof(disk_manager[index].device_name));
			eLIBs_GetPTName(path, disk_manager[index].device_name);
			eLIBs_printf("%s %d %s device_name:%s\n", __FILE__, __LINE__, __func__, disk_manager[index].device_name);
			disk_manager[index].used = 1;
			if (p_scene->usb_plug_in == EPDK_TRUE) {
				ret = ioctl(fd, USB_SLAVE_MSC_PART_PLUGIN, disk_manager[index].part_path);
				disk_manager[index].plug_status = 1;
			}
			break;
		} else if (0 == strncmp(disk_manager[index].part_path, path, strlen(path))) {
			eLIBs_printf("%s %d %s part_path:%s\n", __FILE__, __LINE__, __func__, disk_manager[index].part_path);
			eLIBs_printf("%s %d %s device_name:%s\n", __FILE__, __LINE__, __func__, disk_manager[index].device_name);
			break;
		}
	}
	close(fd);
	if (index == sizeof(disk_manager)/sizeof(disk_manager[0])) {
		eLIBs_printf("%s %d %s fail\n", __FILE__, __LINE__, __func__);
		return -1;
	}
	return 0;
}
static int disk_manager_plugout_del_info(unsigned char *path, __gui_msg_t *msg)
{
	int index = 0, ret = 0, fd = -1;
	__init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
	init_scene_t *p_scene;

	p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
	fd = open("/dev/msc", O_RDWR);
	if (fd < 0) {
		eLIBs_printf("can not find device:%s\n", "/dev/msc");
		return EPDK_FAIL;
	}
	for (index = 0; index < sizeof(disk_manager)/sizeof(disk_manager[0]); index++) {
		if (disk_manager[index].used == 1 && !eLIBs_strcmp(disk_manager[index].part_path, path)) {
			eLIBs_printf("%s %d %s device_name:%s\n", __FILE__, __LINE__, __func__, disk_manager[index].device_name);
			eLIBs_printf("%s %d %s part_path:%s\n", __FILE__, __LINE__, __func__, disk_manager[index].part_path);
			if (device_file_isexist(&disk_manager[index]) == 0) {
				eLIBs_memset(&disk_manager[index], 0x00, sizeof(disk_manager_t));
			}
		}
	}
	close(fd);
	return 0;
}

void usbd_plugout_all_disk(void)
{
   int index = 0;
   int fd = -1, ret = 0;

   fd = open("/dev/msc", O_RDWR);
   if (fd < 0) {
       eLIBs_printf("can not find device:%s\n", "/dev/msc");
       return EPDK_FAIL;
   }

   for (index = 0; index < sizeof(disk_manager)/sizeof(disk_manager[0]); index++) {
       if (disk_manager[index].used == 1 && disk_manager[index].plug_status == 1) {
		   eLIBs_printf("%s %d %s \n", __FILE__, __LINE__, __func__);
           ret = ioctl(fd, USB_SLAVE_MSC_PART_PLUGOUT, disk_manager[index].part_path);
           disk_manager[index].plug_status = 0;
       }
   }
   close(fd);
}

#endif
void usbd_plugin_all_disk(void)
{
	int index = 0;
	int fd = -1, ret = 0;

	fd = open("/dev/msc", O_RDWR);
	if (fd < 0) {
		eLIBs_printf("can not find device:%s\n", "/dev/msc");
		return EPDK_FAIL;
	}
	
	{
		__u8 diskArray[MAX_DISK_LETTER_LEN] = {0};
		int i = 0, index = 0;
		eLIBs_GetDiskArray(diskArray);

		eLIBs_memset(disk_manager, 0x00, sizeof(disk_manager));
#ifdef CONFIG_DRIVERS_USB_GADGET_MSC_MAPP_E_DISK
		for (i = 4, index = 0; i < MAX_DISK_LETTER_LEN && index < sizeof(disk_manager)/sizeof(disk_manager[0]); i++, index++) 
#else
		for (i = 5, index = 0; i < MAX_DISK_LETTER_LEN && index < sizeof(disk_manager)/sizeof(disk_manager[0]); i++, index++)
#endif
		{
			if (diskArray[i] != '\0') {
				//printf("%s %d %s %c:\\\n", __FILE__, __LINE__, __func__, diskArray[i]);
				eLIBs_sprintf(disk_manager[index].part_path, "%c:\\", diskArray[i]);
				eLIBs_GetPTName(disk_manager[index].part_path, disk_manager[index].device_name);
				disk_manager[index].used = 1;
			}
		}
	}
	//eLIBs_printf("%s %d %s \n", __FILE__, __LINE__, __func__);
	for (index = 0; index < sizeof(disk_manager)/sizeof(disk_manager[0]); index++) {
		//eLIBs_printf("%s %d %s disk_manager[%d].used:%d\n", __FILE__, __LINE__, __func__, index, disk_manager[index].used);
		//eLIBs_printf("%s %d %s disk_manager[%d].plug_status:%d\n", __FILE__, __LINE__, __func__, index, disk_manager[index].plug_status);
		if (disk_manager[index].used == 1 && disk_manager[index].plug_status == 0) {
			ret = ioctl(fd, USB_SLAVE_MSC_PART_PLUGIN, disk_manager[index].part_path);
			disk_manager[index].plug_status = 1;
		}
	}
	//eLIBs_printf("%s %d %s \n", __FILE__, __LINE__, __func__);
	close(fd);
}

#endif

static __s32 init_mainwin_cb(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            __init_ctl_t    *init_ctr = NULL;
            init_scene_t    *p_scene = NULL;

            init_ctr        = (__init_ctl_t *)eLIBs_malloc(sizeof(__init_ctl_t));

            if (!init_ctr)
            {
                __err("init_ctr malloc error ");
                return EPDK_FAIL;
            }

            eLIBs_memset(init_ctr, 0, sizeof(__init_ctl_t));
            GUI_WinSetAttr(msg->h_deswin, (void *)init_ctr);

            p_scene = (init_scene_t *)eLIBs_malloc(sizeof(init_scene_t));

            if (NULL == p_scene)
            {
                __err("p_scene malloc error ");
                return EPDK_FAIL;
            }

            eLIBs_memset(p_scene, 0, sizeof(init_scene_t));
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)p_scene);

            p_scene->init_win                   = msg->h_deswin;
            init_com.h_win                      = msg->h_deswin;

            init_ctr->low_power_check_time_id   = LOW_POWER_CHECK_TIME_ID;
            init_ctr->cursor_time_id            = CURSOR_TIME_ID;
            init_ctr->close_scn_time_id         = CLOSE_SCN_TIME_ID;
            init_ctr->auto_off_time_id          = AUTO_OFF_TIME_ID;
            init_ctr->low_power_check_timeout   = 100;          // 1s
            init_ctr->cursor_timeout            = 500;          // 5s

#ifdef USBD_HID_FUNC_ENABLE
			esCFG_GetKeyValue("display", "lcd_adjust_tool_en", &usbd_func_hid_enable, 1);
#endif

            Init_reg_para   = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

            init_ctr->closescn_gate_on      = EPDK_TRUE;
            init_ctr->closescn_timeout      = Init_reg_para->backlightoff;
            init_ctr->autooff_timeout       = Init_reg_para->poweroff;

            __wrn("Init_reg_para->poweroff:%d",     Init_reg_para->poweroff);
            __wrn("init_ctr->autooff_timeout:%d",   init_ctr->autooff_timeout);
            __wrn("Init_reg_para->close_screen:%d", Init_reg_para->backlightoff);
            __wrn("init_ctr->closescn_timeout:%d",  init_ctr->closescn_timeout);
            /* 创建 timer */
            //GUI_SetTimer(msg->h_deswin, init_ctr->low_power_check_time_id, init_ctr->low_power_check_timeout, NULL);
#if OPEN_CURSOR
            GUI_SetTimer(msg->h_deswin, init_ctr->cursor_time_id, init_ctr->cursor_timeout, NULL);
#endif
            /* 创建信号量 */
            init_ctr->state_sem         = esKRNL_SemCreate(1);
            init_ctr->usb_connect       = EPDK_FALSE;
            init_ctr->screen_close_done = EPDK_FALSE;
            init_ctr->tp_msg_enble      = EPDK_TRUE;
            init_ctr->key_msg_enble     = EPDK_TRUE;
            /*initialize Lcd setting Start*/
            /*Lcd initialize End*/
            is_close_screen = EPDK_FALSE;

            init_server_init(msg);
  
#if OPEN_CURSOR
            init_cursor_create(init_ctr);
#endif
            //create hbar,bgd,dialog
            init_scene_create(msg);

            //后台线程处理耗时操作
            sys_msg_handle_init(msg);
            dsk_amplifier_onoff(0);
            dsk_hp_det_pin_det();
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
            break;
        }

        case GUI_MSG_DESTROY:
        {
            uint8_t         err;
            __init_ctl_t    *init_ctr   = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
            init_scene_t    *p_scene    = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);

            if (GUI_IsTimerInstalled(msg->h_deswin, init_ctr->low_power_check_time_id))
            {
                GUI_KillTimer(msg->h_deswin, init_ctr->low_power_check_time_id);
            }

            if (GUI_IsTimerInstalled(msg->h_deswin, init_ctr->cursor_time_id))
            {
                GUI_KillTimer(msg->h_deswin, init_ctr->cursor_time_id);
            }

            if (GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id))
            {
                GUI_KillTimer(msg->h_deswin, init_ctr->close_scn_time_id);
            }

            if (GUI_IsTimerInstalled(msg->h_deswin, init_ctr->auto_off_time_id))
            {
                GUI_KillTimer(msg->h_deswin, init_ctr->auto_off_time_id);
            }

            sys_msg_handle_exit(msg);

            init_scene_destroy(msg);

			esKRNL_SemDel(init_ctr->state_sem, 0, &err);
			init_ctr->state_sem = NULL;
#if OPEN_CURSOR
            init_cursor_delete(init_ctr);
#endif

            live_exit();
            eLIBs_free(init_ctr);
			init_ctr = NULL;
			GUI_WinSetAttr(msg->h_deswin, NULL);
            eLIBs_free(p_scene);
            __msg("init_mainwin destroyed...");
            break;
        }

        case GUI_MSG_CLOSE:
            GUI_ManWinDelete(msg->h_deswin);
            break;

        case GUI_MSG_TIMER:
        {
            init_scene_t    *p_scene = NULL;
            __init_ctl_t    *init_ctr = NULL;

            init_ctr        = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
            p_scene         = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);

            if (msg->dwAddData1 == init_ctr->cursor_time_id) //光标存在时间
            {
                GUI_CursorHide();
            }
            else if (msg->dwAddData1 == init_ctr->low_power_check_time_id) //低电检查
            {
                if (0) //112350
                {
                    static __s32 i = 0;
                    i++;

                    if (0 == (i % 20))
                    {
                        esMEMS_Info();
                    }
                }

                dsk_hp_det_pin_det();

                if (dsk_power_is_low() == EPDK_TRUE) //低电
                {
                    __msg("****************low power...****************");
                    GUI_KillTimer(msg->h_deswin, init_ctr->low_power_check_time_id);
                    SEND_MSG(DSK_MSG_LOW_POWER, NULL, msg->h_deswin, 0, 0);
                }
                else
                {
                    __msg("**************not low power...*********");
                }

                init_voltage_check(msg);
            }
            else if (msg->dwAddData1 == LOW_POWER_DIALOG_TIME_ID) //低电对话框存在时间
            {
                p_scene->low_power = EPDK_FALSE;
                GUI_KillTimer(msg->h_deswin, LOW_POWER_DIALOG_TIME_ID);
                scene_on_dialog(msg);
            }
            else if (msg->dwAddData1 == POWER_OFF_DIALOG_TIME_ID) //关机对话框存在时间
            {
                p_scene->power_off = EPDK_FALSE;
                GUI_KillTimer(msg->h_deswin, POWER_OFF_DIALOG_TIME_ID);
                scene_on_dialog(msg);
            }
            else if (msg->dwAddData1 == ORCHID_UPDATE_DIALOG_TIME_ID) //媒体库升级对话框存在时间
            {
                if (p_scene->orchid_update == EPDK_FALSE)
                {
                    GUI_KillTimer(msg->h_deswin, ORCHID_UPDATE_DIALOG_TIME_ID);
                    scene_on_dialog(msg);
                }
            }
            else if (msg->dwAddData1 == init_ctr->close_scn_time_id) //自动关屏时间
            {
                __wrn("send DSK_MSG_SCREEN_CLOSE msg ");
                __wrn("init_ctr->closescn_timeout:%d", init_ctr->closescn_timeout);
                SEND_MSG(DSK_MSG_SCREEN_CLOSE, NULL, msg->h_deswin, 0, 0); //112350
            }
            else if (msg->dwAddData1 == init_ctr->auto_off_time_id)//自动关机时间
            {
                __wrn("send DSK_MSG_POWER_OFF msg ");
                __wrn("init_ctr->autooff_timeout:%d", init_ctr->autooff_timeout);
                SEND_MSG(DSK_MSG_POWER_OFF, NULL, msg->h_deswin, 0, 0);
            }
            else
            {
                __wrn(" unknown msg timer id = %d ", msg->dwAddData1);
            }

            break;
        }

        case GUI_MSG_KEY:
        {
            __bool          exist;
            __init_ctl_t    *init_ctr = NULL;

            init_ctr        = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (init_ctr == NULL)
            {
                __err("There:%s %d fail", __FILE__, __LINE__);
            }

            //重新设置关屏定时器
            exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id);

            if (exist == EPDK_TRUE)
            {
                GUI_ResetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
            }

            //重新设置关机定时器
            exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->auto_off_time_id);

            if (exist == EPDK_TRUE)
            {
                GUI_ResetTimer(msg->h_deswin, init_ctr->auto_off_time_id, init_ctr->autooff_timeout, NULL);
            }

            switch (msg->dwAddData1)
            {
                case GUI_MSG_KEY_LONGVADD:
                case GUI_MSG_KEY_VADD:
                {
                    if (msg->dwAddData2 != KEY_UP_ACTION)
                    {
                        //dsk_volume_inc();
                    }

                    return GUI_ManWinDefaultProc(msg);
                }

                case GUI_MSG_KEY_LONGVDEC:
                case GUI_MSG_KEY_VDEC:
                {
                    if (msg->dwAddData2 != KEY_UP_ACTION)
                    {
                        //dsk_volume_dec();
                    }

                    return GUI_ManWinDefaultProc(msg);
                }

                default:
                    return GUI_ManWinDefaultProc(msg);
            }
        }

        case GUI_MSG_TOUCH:
        {
            __bool  exist;
            __pos_t pos;
            __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
            __wrn("1");
            //重新设置关屏定时器
            exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id);

            if (exist == EPDK_TRUE)
            {
                __wrn("2");
                GUI_ResetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
            }

            __wrn("3");
            //重新设置关机定时器
            exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->auto_off_time_id);

            if (exist == EPDK_TRUE)
            {
                __wrn("4");
                GUI_ResetTimer(msg->h_deswin, init_ctr->auto_off_time_id, init_ctr->autooff_timeout, NULL);
            }

            __wrn("5");
            pos.x = LOSWORD(msg->dwAddData2);
            pos.y = HISWORD(msg->dwAddData2);
            __msg(" pos.x =%d, pos.y =%d ", pos.x, pos.y);
            __wrn("6");
#if OPEN_CURSOR
            GUI_CursorSetPos(&pos);
            __wrn("11");
            GUI_CursorShow();
            __wrn("7");
#endif
            exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->cursor_time_id);

            if (exist == EPDK_TRUE)
            {
                __wrn("8");
                GUI_ResetTimer(msg->h_deswin, init_ctr->cursor_time_id, init_ctr->cursor_timeout, NULL);
            }

            __wrn("9");
            return GUI_ManWinDefaultProc(msg);
        }

        case HEADBAR_CMD_SET_FORMAT:
        {
            //init_scene_t * p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
            //p_scene->hbar_format   = (__hbar_format_t)msg->dwAddData1;
            //gscene_hbar_delete();
            //gscene_hbar_create(msg->h_deswin, (__hbar_format_t)msg->dwAddData1);
            break;
        }

        /*case HEADBAR_CMD_GET_SCN_POS:
        {
            RECT hbar_rect;
            init_scene_t * p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
            GUI_LyrWinGetScnWindow(p_scene->hbar_layer, &hbar_rect);

            return ((hbar_rect.y<<16) + hbar_rect.x);
        }

        case HEADBAR_CMD_GET_SIZE:
        {
            RECT hbar_rect;
            init_scene_t * p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
            GUI_LyrWinGetScnWindow(p_scene->hbar_layer, &hbar_rect);

            return ((hbar_rect.height<<16) + hbar_rect.width);
        }*/
        case DSK_MSG_USBD_PLUG_IN:              /* usb device plug in */
        {
#ifdef CONFIG_DRIVERS_USB_GADGET_MTP
            __gui_msg_t msgex;
			msgex.id            = DSK_MSG_USBD_PLUG_IN;
            msgex.h_srcwin      = 0;
            msgex.h_deswin      = 0;
            msgex.dwAddData1    = msg->dwAddData1;
            msgex.dwAddData2    = msg->dwAddData2;
            activity_notify_top(&msgex);
#else			
#ifdef USBD_HID_FUNC_ENABLE	
			if (usbd_func_hid_enable) {
				thread_id = esKRNL_TCreate(lcd_tool_thread, (void *)NULL, 0x2000,KRNL_priolevel3);
                esKRNL_TaskNameSet(thread_id, "lcd_tool");
			} else
#endif
			{
	            __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
	            init_scene_t *p_scene;
	            __bool        other_dlg_exist;//连接usb时，是否存在其它模式框
	            p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
	            //提前设置usb连接标志，防止usb连接时，低电或者关机对话框被强制关闭后程序还往下跑，导致应用被结束
	            p_scene->usb_plug_in = EPDK_TRUE;
	            //__here__;
	            other_dlg_exist = EPDK_FALSE;
	            //连接usb的时候禁止自动关屏和关机
	            init_ctr->autooff_timer_exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->auto_off_time_id);

	            if (EPDK_TRUE == init_ctr->autooff_timer_exist)
	            {
	                GUI_KillTimer(msg->h_deswin, init_ctr->auto_off_time_id);
	            }

	            init_ctr->closescn_timer_exist = GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id);

	            if (EPDK_TRUE == init_ctr->closescn_timer_exist)
	            {
	                GUI_KillTimer(msg->h_deswin, init_ctr->close_scn_time_id);
	            }

	            // 禁止消息响应
	            init_ctr->tp_msg_enble = EPDK_FALSE;
	            //__here__;
	            //USB连接到来的时候可能此时提示正在关机或者低电对话框，先结束掉他们
	            //并且设置标记不让它往下跑，从而不关机。
	            __msg("p_scene->power_off=%d", p_scene->power_off);

	            if (p_scene->power_off == EPDK_TRUE)
	            {
	                other_dlg_exist = EPDK_TRUE;
	                //__here__;
	                p_scene->power_off = EPDK_FALSE;

	                if (GUI_IsTimerInstalled(msg->h_deswin, POWER_OFF_DIALOG_TIME_ID))
	                {
	                    //__here__;
	                    GUI_KillTimer(msg->h_deswin, POWER_OFF_DIALOG_TIME_ID);
	                    scene_on_dialog(msg);
	                    //__here__;
	                }
	            }

	            //__here__;
	            if (p_scene->low_power == EPDK_TRUE)
	            {
	                other_dlg_exist = EPDK_TRUE;
	                //__here__;
	                p_scene->low_power = EPDK_FALSE;

	                if (GUI_IsTimerInstalled(msg->h_deswin, LOW_POWER_DIALOG_TIME_ID))
	                {
	                    //__here__;
	                    GUI_KillTimer(msg->h_deswin, LOW_POWER_DIALOG_TIME_ID);
	                    scene_on_dialog(msg);
	                }
	            }

	            //如果连接usb时，存在其它模式框，则必须投递消息的方式通知usb连接，
	            //否则关闭其它对话框后，对话框消息循环未结束，会造成死锁
	            if (EPDK_TRUE == other_dlg_exist)
	            {
	                __gui_msg_t set_msg;
	                eLIBs_memcpy(&set_msg, msg, sizeof(__gui_msg_t));
	                GUI_SendNotifyMessage(&set_msg);
	            }
	            else
	            {
	                init_usb_device_plugin_proc(msg);
	                NOTIFY_MSG(DSK_MSG_ORCHID_UPDATE_START, NULL, msg->h_deswin, 0, 0);
	            }
			}
#endif			
            break;
        }

        case DSK_MSG_USBD_PLUG_OUT:             /* usb devcie plug out */
        {
#ifdef CONFIG_DRIVERS_USB_GADGET_MTP
			__gui_msg_t msgex;
			msgex.id			= DSK_MSG_USBD_PLUG_OUT;
			msgex.h_srcwin		= 0;
			msgex.h_deswin		= 0;
			msgex.dwAddData1	= msg->dwAddData1;
			msgex.dwAddData2	= msg->dwAddData2;
			activity_notify_top(&msgex);
#else
			//eLIBs_printf("%s %d %s DSK_MSG_USBD_PLUG_OUT\n", __FILE__, __LINE__, __func__);
#ifdef USBD_HID_FUNC_ENABLE	
			if (usbd_func_hid_enable) {

				if(thread_id)
				{

					while(1)
					{		
						if(esKRNL_TDelReq(thread_id) == OS_TASK_NOT_EXIST)
						{
							break;
						}
						esKRNL_TimeDly(3);		  
					}
					thread_id = 0;
				}
			} else
#endif			
			{
	            __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
	            //__here__;
	            init_usb_device_plugout_proc(msg);
	            init_ctr->tp_msg_enble = EPDK_TRUE;

	            //__here__;
	            //usb连接拔出后，需重置低电查询定时器，因为可能低电
	            GUI_ResetTimer(msg->h_deswin, init_ctr->low_power_check_time_id, init_ctr->low_power_check_timeout, NULL);
	            //usb连接拔出后，重置自动关机和自动关屏定时器
	            __wrn("init_ctr->autooff_timer_exist=%d", init_ctr->autooff_timer_exist);
	            __wrn("init_ctr->autooff_timeout=%d", init_ctr->autooff_timeout);

	            if (EPDK_TRUE == init_ctr->autooff_timer_exist)
	            {
	                if (GUI_IsTimerInstalled(msg->h_deswin, init_ctr->auto_off_time_id))
	                {
	                    GUI_ResetTimer(msg->h_deswin, init_ctr->auto_off_time_id, init_ctr->autooff_timeout, NULL);
	                }
	                else
	                {
	                    GUI_SetTimer(msg->h_deswin, init_ctr->auto_off_time_id, init_ctr->autooff_timeout, NULL);
	                }
	            }

	            __wrn("init_ctr->closescn_timer_exist=%d", init_ctr->closescn_timer_exist);
	            __wrn("init_ctr->closescn_timeout=%d", init_ctr->closescn_timeout);

	            if (EPDK_TRUE == init_ctr->closescn_timer_exist)
	            {
	                if (GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id))
	                {
	                    GUI_ResetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
	                }
	                else
	                {
	                    GUI_SetTimer(msg->h_deswin, init_ctr->close_scn_time_id, init_ctr->closescn_timeout, NULL);
	                }
	            }

	            //低电提示框弹出后插入usb，重新拔出后
	            //该定时器已被删除，需重置一下
	            if (!GUI_IsTimerInstalled(msg->h_deswin, init_ctr->low_power_check_time_id))
	            {
	                GUI_SetTimer(msg->h_deswin, init_ctr->low_power_check_time_id, init_ctr->low_power_check_timeout, NULL);
	            }
			}
#endif
            break;
        }

        case DSK_MSG_ORCHID_UPDATE_START:
        {
            __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
            //__here__;
            //init_orchid_update_start(msg);
            //__here__;
            {
                __s32 flag;
                char load_para[1024] = {0};
                flag = 2;
                eLIBs_memcpy(load_para, &flag, 4);
                activity_set_load_para("root", "", load_para, sizeof(load_para));
                activity_load_app("application://app_root");
            }
            //__here__;
            break;
        }

        case DSK_MSG_ORCHID_UPDATE_END:
        {
            __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
            //__here__
            init_orchid_update_end(msg);

            //__here__
            if (dsk_display_get_output_type() == DISP_OUTPUT_TYPE_LCD)
            {
                init_ctr->tp_msg_enble = EPDK_TRUE;
            }

            break;
        }

        case DSK_MSG_FS_PART_PLUGIN:
        {
            char diskname[4];
            __gui_msg_t msgex;
            __wrn("msg->dwAddData2=%x", msg->dwAddData2);
            diskname[0] = (__u8)(msg->dwAddData2);
            diskname[1] = '\0';
            __msg("disk %s plug in", diskname);

            if ((diskname[0] == 'D') || (diskname[0] == 'E') || (diskname[0] == 'Z'))
            {
                return 0;
            }

            //notify top
            msgex.id            = DSK_MSG_FS_PART_PLUGIN;
            msgex.h_srcwin      = 0;
            msgex.h_deswin      = 0;
            msgex.dwAddData1    = msg->dwAddData1;
            msgex.dwAddData2    = msg->dwAddData2;
            activity_notify_top(&msgex);
            break;
        }

        case DSK_MSG_FS_PART_PLUGOUT:
        {
            char diskname[4];
            __gui_msg_t msgex;
            diskname[0] = msg->dwAddData2;
            diskname[1] = '\0';
            __msg("disk %s plug out", diskname);

            if ((diskname[0] == 'D') || (diskname[0] == 'E') || (diskname[0] == 'Z'))
            {
                return 0;
            }

            //notify top
            msgex.id            = DSK_MSG_FS_PART_PLUGOUT;
            msgex.h_srcwin      = 0;
            msgex.h_deswin      = 0;
            msgex.dwAddData1    = msg->dwAddData1;
            msgex.dwAddData2    = msg->dwAddData2;
            activity_notify_top(&msgex);

            if (dsk_wkm_is_open())
            {
                char filename[512];
                dsk_wkm_get_now_playing_file(filename);
                __msg("filename = %s", filename);

                if (0 == eLIBs_strnicmp(filename, diskname, 1))
                {
                    //__here__
                    dsk_wkm_close();
                }
            }

            break;
        }

        case DSK_MSG_HANDLE_PLUGOUT:
        {
            //notify top
            __gui_msg_t msgex;
            msgex.id            = DSK_MSG_HANDLE_PLUGOUT;
            msgex.h_srcwin      = 0;
            msgex.h_deswin      = 0;
            msgex.dwAddData1    = msg->dwAddData1;
            msgex.dwAddData2    = msg->dwAddData2;
            activity_notify_top(&msgex);
        }
		break;
        case DSK_MSG_SCREEN_OPEN:               /* 开屏 */
            __wrn("DSK_MSG_SCREEN_OPEN");
            init_open_screen(msg);
            break;

        case DSK_MSG_LOW_POWER:             /* low power */
        {
            __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (init_ctr->power_off)
            {
                break;
            }

            init_ctr->power_off = 1;
            //__here__;
            init_low_power_proc(msg);
            //__here__;
            init_low_power_off_proc(msg);
            /* 调用显示驱动, 关屏 */
            dsk_display_lcd_off();
            init_ctr->power_off = 0;
            //__here__;
            break;
        }

        case DSK_MSG_ENABLE_STANDBY:
            init_lock(msg);
            g_b_enable_standby = 1;
            init_unlock(msg);
            break;

        case DSK_MSG_DISABLE_STANDBY:
            init_lock(msg);
            g_b_enable_standby = 0;
            init_unlock(msg);
            break;

        case DSK_MSG_GET_STANDBY_FLAG:
        {
            __bool flag;
            init_lock(msg);
            flag = g_b_enable_standby;
            init_unlock(msg);
            return flag;
        }

        case DSK_MSG_POWER_OFF:             /* power off */
        {
        	int standby_mode_en = 0;
            __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
            __wrn("DSK_MSG_POWER_OFF");
            __wrn("init_ctr->power_off=%d", init_ctr->power_off);
            GUI_KillTimer(msg->h_deswin, init_ctr->auto_off_time_id);
            __wrn("test power off 1");

            if (msg->dwAddData2 == KEY_UP_ACTION)//&& dsk_get_charge_state()==EPDK_FALSE)//add by Kingvan//remote_power
            {
	            if (esCFG_GetKeyValue("app_para", "standby_mode_en", (int *)&standby_mode_en, 1) != EPDK_OK)
				{
					standby_mode_en = 0;
					__err("standby_mode_en err!");
				}
				if(standby_mode_en)
				{
	                init_ctr->power_off = EPDK_TRUE;
	                scene_on_dialog(msg);
	                init_close_screen(msg, 1);
	                init_ctr->power_off = EPDK_FALSE;

	                if (init_ctr->autooff_timeout != 0)/* 从新设置定时器*/
	                {
	                    dsk_set_auto_off_time(init_ctr->autooff_timeout);
	                }
				}
				else
				{
                    //关屏计时开始
                    init_restart_close_scn(msg);
                    __wrn("test power off 2");
                    init_power_off_proc(msg);
				}
            }

            return EPDK_OK;
        }

        /*case DSK_MSG_HOLD:
        {
            __init_ctl_t * init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
            if(init_ctr->closescn_gate_on == EPDK_TRUE)
            {
                init_close_screen(msg);
            }
            break;
        }*/
        case DSK_MSG_STANDBY_WAKE_UP:
            __wrn("DSK_MSG_STANDBY_WAKE_UP");
            init_open_screen(msg);
            break;

        case DSK_MSG_TVDAC_PLUGIN:
            __msg("************DSK_MSG_TVDAC_PLUGIN*****************");
            break;

        case DSK_MSG_TVDAC_PLUGOUT:
        case DSK_MSG_HDMI_PLUGOUT:
        {
            __msg("************DSK_MSG_TVDAC_PLUGOUT/DSK_MSG_HDMI_PLUGOUT*****************");

            //切屏
            if (dsk_display_get_output_type() != DISP_OUTPUT_TYPE_LCD)
            {
                __gui_msg_t msgex;
                //__here__;
                //notify top
                msgex.id            = msg->id;
                msgex.h_srcwin      = 0;
                msgex.h_deswin      = 0;
                msgex.dwAddData1    = msg->dwAddData1;
                msgex.dwAddData2    = msg->dwAddData2;
                activity_notify_top(&msgex);
            }

            //__here__;
            break;
        }

        case DSK_MSG_SCREEN_SWITCH:
        {
            __init_ctl_t *init_ctr;
            init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (msg->dwAddData1 == LION_DISP_LCD)       // 切换到LCD，打开触摸屏消息
            {
                init_lock(msg);
                init_ctr->tp_msg_enble = EPDK_TRUE;
                init_unlock(msg);
            }
            else                                            // 切换到tv , 关闭触摸屏消息
            {
                init_lock(msg);
                init_ctr->tp_msg_enble = EPDK_FALSE;
                init_unlock(msg);
            }

            //重新创建场景
            //init_scene_destroy(msg);
            //init_scene_create(msg);
            break;
        }

        case DSK_MSG_ASSISTANT_SCENE_CREATE:
        {
            /*init_scene_t *p_scene;
            ES_FILE      *p_disp = NULL;
            p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
            //__here__
            if(!p_scene->assistant_win)
            {
                //创建辅屏场景
                GUI_SetScnNo(1);
                p_scene->assistant_win = assistant_scene_create(msg->h_deswin);

                // open display driver
                p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
                if( p_disp == NULL )
                {
                    __err("Error in opening display driver.");
                    return EPDK_FAIL;
                }
                esMODS_MIoctrl(p_disp, DISP_CMD_LCD_ON, 1, 0);
                eLIBs_fclose(p_disp);
                p_disp = NULL;

                GUI_SetScnNo(0);
            }*/
            break;
        }

        case DSK_MSG_ASSISTANT_SCENE_DELETE:
        {
            /*init_scene_t *p_scene;
            ES_FILE      *p_disp = NULL;
            p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
            //__here__
            if(p_scene->assistant_win)
            {
                //__here__
                GUI_SetScnNo(1);
                assistant_scene_delete(p_scene->assistant_win);
                p_scene->assistant_win = NULL;

                p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
                if( p_disp == NULL )
                {
                    __err("Error in opening display driver.");
                    return EPDK_FAIL;
                }
                esMODS_MIoctrl(p_disp, DISP_CMD_LCD_OFF, 1, 0);
                eLIBs_fclose(p_disp);
                p_disp = NULL;

                GUI_SetScnNo(0);
            }*/
            break;
        }

        case DSK_MSG_SET_SCN_CLOSE:
            init_set_close_scn(msg);
            break;

        case DSK_MSG_GET_SCN_CLOSE:
        {
            return init_get_close_scn(msg);
        }

        case DSK_MSG_SCREEN_CLOSE:
        {
            __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
            __wrn("DSK_MSG_SCREEN_CLOSE");
            GUI_KillTimer(msg->h_deswin, init_ctr->close_scn_time_id);

            if (init_ctr->closescn_gate_on == EPDK_TRUE)
            {
                init_close_screen(msg, 0);
            }

            break;
        }

        case DSK_MSG_CLOSE_SCN_GATE_OFF:
        {
            __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
            init_ctr->closescn_gate_on = EPDK_FALSE;
            break;
        }

        case DSK_MSG_CLOSE_SCN_GATE_ON:
        {
            __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
            init_ctr->closescn_gate_on = EPDK_TRUE;
            break;
        }

        case DSK_MSG_HOME:
        {
            __gui_msg_t msgex;
            //__here__;
            //notify top
            eLIBs_memcpy(&msgex, msg, sizeof(__gui_msg_t));
            activity_notify_top(&msgex);
            break;
        }

        case DSK_MSG_ALARM:
        {
            __gui_msg_t msgex;
            init_open_screen(msg);
            //notify top
            msgex.id            = msg->id;
            msgex.h_srcwin      = 0;
            msgex.h_deswin      = 0;
            msgex.dwAddData1    = msg->dwAddData1;
            msgex.dwAddData2    = msg->dwAddData2;
            activity_notify_top(&msgex);
            break;
        }

        case DSK_MSG_SET_AUTO_OFF:
        {
            init_set_auto_off(msg);
            break;
        }

        case DSK_MSG_FW_UPDATE:
        {
            //__here__;
            /* delay */
            esKRNL_TimeDly(200);
            __msg("********dsk_fw_update*******");
            /* 执行升级过程 */
            dsk_fw_update();
            //__here__;
            esKRNL_TimeDly(200);
            //__here__;
            break;
        }

        case DSK_MSG_FW_START_UPDATE: //升级固件
        {
            __init_ctl_t *init_ctr = (__init_ctl_t *)GUI_WinGetAttr(msg->h_deswin);
            __wrn("msg->dwAddData1=%s", msg->dwAddData1);

            if (GUI_IsTimerInstalled(msg->h_deswin, init_ctr->low_power_check_time_id))
            {
                GUI_KillTimer(msg->h_deswin, init_ctr->low_power_check_time_id);
            }

            if (GUI_IsTimerInstalled(msg->h_deswin, init_ctr->cursor_time_id))
            {
                GUI_KillTimer(msg->h_deswin, init_ctr->cursor_time_id);
            }

            if (GUI_IsTimerInstalled(msg->h_deswin, init_ctr->close_scn_time_id))
            {
                GUI_KillTimer(msg->h_deswin, init_ctr->close_scn_time_id);
            }

            if (GUI_IsTimerInstalled(msg->h_deswin, init_ctr->auto_off_time_id))
            {
                GUI_KillTimer(msg->h_deswin, init_ctr->auto_off_time_id);
            }

            activity_finish_all();
#if 0
            gscene_bgd_set_state(BGD_STATUS_SHOW);
#else
            gscene_bgd_set_flag(EPDK_TRUE);
            gscene_bgd_refresh();
#endif
            gscene_hbar_set_state(HBAR_ST_HIDE);
            fw_update((char *)(msg->dwAddData1));
            break;
        }

        /*case GUI_MSG_COMMAND:
        {
            if(ID_SCREEN_LOCK == LOSWORD(msg->dwAddData1) && SCREEN_LOCK_EXIT == HISWORD(msg->dwAddData1))
            {
                __gui_msg_t msgex;
                init_scene_t * p_scene  = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
                delete_screen_lock(p_scene->scnlock_win);

                msgex.id            = DSK_MSG_SCREEN_OPEN;
                msgex.h_srcwin      = 0;
                msgex.h_deswin      = 0;
                msgex.dwAddData1    = 0;
                msgex.dwAddData2    = 0;
                activity_notify_top(&msgex);
            }
            break;
        }*/
        default:
            GUI_ManWinDefaultProc(msg);
            break;
    }

    return EPDK_OK;
}

H_WIN init_mainwin_create(void)
{
    H_WIN                       hManWin;
    __gui_manwincreate_para_t   create_info;

    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    create_info.attr            = NULL;
    create_info.hParent         = NULL;
    create_info.hHosting        = NULL;
	
    create_info.ManWindowProc   = init_mainwin_cb;
    create_info.name            = "init";
    __wrn("before GUI_ManWinCreate");
    hManWin                     = GUI_ManWinCreate(&create_info);
    __wrn("after GUI_ManWinCreate");

    if (hManWin == NULL)
    {
        __wrn("init: create main windows failed!");
        return NULL;
    }

    return hManWin;
}
