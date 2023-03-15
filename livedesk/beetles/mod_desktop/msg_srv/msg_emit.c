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
#include "mod_desktop_i.h"
#include <mod_touchpanel.h>
#include <kconfig.h>

#define     SYS_MSGQ_SIZE           128

/**************************************************************************************************************/
typedef struct _msrv_msg_t
{
    __u32   type;
    __u32   id;
    __u32   data;
    __u32   reserved;
} __msrv_msg_t;

typedef struct _memit_ctr_t
{
    __hdle              p_array_sem;                   // 缓冲区互斥
    __hdle              p_hook_sem;
    __hdle              psys_msg_queue;                // msg 消息队列
    __s32               sys_msg_counter;
    __msrv_msg_t        sys_msg_array[SYS_MSGQ_SIZE];   // msg 消息队列缓冲
    __u32               ksrv_th_id;

    cb_key_hook         key_hook;
    cb_tp_hook          tp_hook;
    cb_init_hook        init_hook;
    __hdle              h_tpGraber;
    __hdle              h_keyGraber;
    cb_tvout_tp_hook    tvout_tp_hook;
    __hdle              p_tvout_tp_hook_sem;
} __memit_ctr_t;

static __memit_ctr_t    emit_ctr = {0};
static __u32            msg_srv_tid;
static __s32            last_touch_action;

/**************************************************************************************************************/
static __msrv_msg_t     *get_msg_buf(__memit_ctr_t *emit);
static __s32            tp_msg_cb(void *msg);
static __s32            key_msg_cb(void *msg);
static void             ksrv_msg_thread(void *arg);
static __s32 msg_srv_init_input_panel(__memit_ctr_t *emit);
/**************************************************************************************************************/
static __s32 msg_srv_init_input_panel(__memit_ctr_t *emit)
{
    /* 创建 desktop_msg_queue */
    emit->sys_msg_counter = 0;
    emit->psys_msg_queue  = esKRNL_QCreate(SYS_MSGQ_SIZE);

    if (!emit->psys_msg_queue)
    {
        __err(" create sys_msg_queue error ");
        return EPDK_FAIL;
    }

    /* 创建 p_array_sem */
    emit->p_array_sem = esKRNL_SemCreate(1);

    if (!emit->p_array_sem)
    {
        __err(" create p_array_sem error ");
        return EPDK_FAIL;
    }

    emit->p_hook_sem = esKRNL_SemCreate(1);

    if (!emit->p_hook_sem)
    {
        __err(" create p_hook_sem error ");
        return EPDK_FAIL;
    }

#ifdef CONFIG_SOC_SUN20IW1P1
    emit->p_tvout_tp_hook_sem = esKRNL_SemCreate(1);

    if (!emit->p_tvout_tp_hook_sem)
    {
        __err(" create p_tvout_tp_hook_sem error ");
        return EPDK_FAIL;
    }

#endif
    return EPDK_OK;

}
/**
 *  初始化 touch panel 消息传输通道
 */
__s32 msg_srv_init_tp_channel(__memit_ctr_t *emit)
{
    /* 装载 touchpanel 驱动 */
    /*  注册回调函数 tp_msg_cb */
    /*{
        ES_FILE *p_tpfile;
        p_tpfile = eLIBs_fopen("b:\\HID\\TP", "r+");
        if( !p_tpfile )
        {
            __err(" open touchpanel driver error ");
            return EPDK_FAIL;
        }

        eLIBs_fioctrl(p_tpfile, DRV_TP_CMD_REG, 0, (void *)tp_msg_cb);
        eLIBs_fclose(p_tpfile);
    }*/
    {
        /* grab logical keyboard, binding event callback to input_subsystem */
        emit_ctr.h_tpGraber = esINPUT_LdevGrab(INPUT_LTS_DEV_NAME, (__pCBK_t)tp_msg_cb, 0, 0);

        if (emit_ctr.h_tpGraber == NULL)
        {
            __log("grab logical touchpannel failed");
            return EPDK_FAIL;
        }
    }
    return EPDK_OK;
}

__s32 msg_srv_init_key_channel(__memit_ctr_t *emit)
{
    __s32  LdevID;

    /* grab logical keyboard, binding event callback to input_subsystem */
    emit_ctr.h_keyGraber = esINPUT_LdevGrab(INPUT_LKEYBOARD_DEV_NAME, (__pCBK_t)key_msg_cb, 0, 0);

    if (emit_ctr.h_keyGraber == NULL)
    {
        __log("grab logical keyboard failed");
        return EPDK_FAIL;
    }

    __log("emit_ctr.h_keyGraber=0x%x", emit_ctr.h_keyGraber);

    //get logical key device id
    LdevID  = esINPUT_GetLdevID(emit_ctr.h_keyGraber);

    if (LdevID == -1)
    {
        __log("get logical id failed");
        return EPDK_FAIL;
    }

    __log("LdevID=%d", LdevID);

    //设置时间间隔单位为1ms，
    if (esINPUT_LdevCtl(LdevID, INPUT_SET_REP_PERIOD, 200 | (4 << 16), NULL) != EPDK_OK)
    {
        __log("logical device ioctl failed");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

__s32 msg_srv_deinit_key_channel(__memit_ctr_t *emit)
{
    esINPUT_LdevRelease(emit_ctr.h_keyGraber);
    return EPDK_OK;
}

/**
 *  释放 touch panel 消息传输通道
 */
__s32 msg_srv_deinit_tp_channel(__memit_ctr_t *emit)
{
    __u8    err;

    /* 删除 psys_msg_queue */
    esKRNL_QDel(emit->psys_msg_queue, 0, &err);

    emit->psys_msg_queue = NULL;

    /* 删除 p_array_sem */
    esKRNL_SemDel(emit->p_array_sem, 0, &err);
    esKRNL_SemDel(emit->p_hook_sem, 0, &err);
    esKRNL_SemDel(emit->p_tvout_tp_hook_sem, 0, &err);

    emit->p_tvout_tp_hook_sem   = NULL;
    emit->p_array_sem           = NULL;
    emit->p_hook_sem            = NULL;
    emit->sys_msg_counter       = 0;

    /* 注销回调函数 */
    /*{
        ES_FILE  *p_tpfile;
        p_tpfile = eLIBs_fopen("b:\\HID\\TP", "r+");
        if( !p_tpfile )
        {
            __err(" open touchpanel driver error ");
            return EPDK_FAIL;
        }
        eLIBs_fioctrl(p_tpfile, DRV_TP_CMD_UNREG, 0, (void *)tp_msg_cb);
        eLIBs_fclose(p_tpfile);
    }*/

    esINPUT_LdevRelease(emit_ctr.h_tpGraber);
    return EPDK_OK;
}

/**
 * 初始化系统消息队列消息传输通道
 */
__s32 msg_srv_init_ksrv_channel(__memit_ctr_t *emit)
{
    /* create ksrv msg thread  */
#ifdef SIM_PC_WIN
    __SAVE_MOD_EVN__
#endif
    emit->ksrv_th_id = esKRNL_TCreate(ksrv_msg_thread, (void *)emit, 0x1000, KRNL_priolevel1);
    esKRNL_TaskNameSet(emit->ksrv_th_id, "appmsg2emitq");
#ifdef SIM_PC_WIN
    __RESTORE_MOD_EVN__
#endif

    if (emit->ksrv_th_id == 0)
    {
        __err(" ksrv_msg_thread create error ");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

/**
 * 释放系统消息队列消息传输通道
 */
__s32 msg_srv_deinit_ksrv_channel(__memit_ctr_t *emit)
{
    while (esKRNL_TDelReq(emit->ksrv_th_id) != OS_TASK_NOT_EXIST)
    {
        esKRNL_TimeDly(1);
    }

    return EPDK_OK;
}

static __msrv_msg_t  *get_msg_buf(__memit_ctr_t *emit)
{
    //__log("sys_msg_counter = %d ", sys_msg_counter);
    emit->sys_msg_counter   = (emit->sys_msg_counter + 1) & (SYS_MSGQ_SIZE - 1);

    return &(emit->sys_msg_array[emit->sys_msg_counter]);
}

/**
 * 通过回调函数的方式取触摸屏消息
 */
static __s32 tp_msg_cb(void *msg)
{
    __msrv_msg_t            *pmsg;
    __u8                    error;
    __u32                   i;
    __input_event_packet_t  *pPacket;
    __input_event_t         *pEventFrame;
    __input_event_t         pEvent_type;
    __input_event_t         pEvent_x;
    __input_event_t         pEvent_y;
    __input_event_t         pEvent_speed_dir;
    __input_event_t         pEvent_speed_val;

    if (msg == NULL)
    {
        __log("invalid argument for call back");
        return EPDK_FAIL;
    }

    /* dump packet events */
    pPacket = (__input_event_packet_t *)msg;

    if (pPacket->event_cnt <= 0 || pPacket->event_cnt > INPUT_MAX_EVENT_NR)
    {
        __log("invalid event count number");
        return EPDK_FAIL;
    }

    //__log("------------------------------------------------");
    for (i = 0; i < pPacket->event_cnt; i++)
    {
        pEventFrame = &(pPacket->events[i]);

        /*__log("Event %d: type = %d, code = %d, value = %d", i + 1, pEventFrame->type, pEventFrame->code, pEventFrame->value);*/

        if (pEventFrame->type == EV_ABS)
        {
            if (pEventFrame->code == ABS_MISC)
            {
                eLIBs_memcpy(&pEvent_type, pEventFrame, sizeof(__input_event_t));
            }
            else if (pEventFrame->code == ABS_X)
            {
                eLIBs_memcpy(&pEvent_x, pEventFrame, sizeof(__input_event_t));
            }
            else if (pEventFrame->code == ABS_Y)
            {
                eLIBs_memcpy(&pEvent_y, pEventFrame, sizeof(__input_event_t));
            }
            else if (pEventFrame->code == ABS_RUDDER)
            {
                eLIBs_memcpy(&pEvent_speed_dir, pEventFrame, sizeof(__input_event_t));
            }
            else if (pEventFrame->code == ABS_BRAKE)
            {
                eLIBs_memcpy(&pEvent_speed_val, pEventFrame, sizeof(__input_event_t));
            }
        }
        else if (pEventFrame->type == EV_SYN)
        {
            break;
        }
    }

    //__log("------------------------------------------------");
    esKRNL_SemPend(emit_ctr.p_array_sem, 0, &error);

    pmsg = get_msg_buf(&emit_ctr);

    esKRNL_SemPost(emit_ctr.p_array_sem);

    pmsg->type  = GUI_MSG_TOUCH;

    switch (pEvent_type.value)
    {
        case EV_TP_PRESS_START:
        {
            pmsg->id            = GUI_MSG_TOUCH_DOWN;
            pmsg->data          = (pEvent_y.value << 16) + pEvent_x.value;
            pmsg->reserved      = (pEvent_speed_dir.value << 16) + pEvent_speed_val.value;
            last_touch_action   = GUI_MSG_TOUCH_DOWN;
            break;
        }

        case EV_TP_PRESS_MOVE:
            pmsg->id            = GUI_MSG_TOUCH_MOVE;
            pmsg->data          = (pEvent_y.value << 16) + pEvent_x.value;
            pmsg->reserved      = (pEvent_speed_dir.value << 16) + pEvent_speed_val.value;
            last_touch_action   = GUI_MSG_TOUCH_MOVE;
            break;

        case EV_TP_PINCH_IN:
            pmsg->id            = GUI_MSG_TOUCH_ZOOMIN;
            pmsg->data          = (pEvent_y.value << 16) + pEvent_x.value;
            pmsg->reserved      = (pEvent_speed_dir.value << 16) + pEvent_speed_val.value;
            last_touch_action   = GUI_MSG_TOUCH_ZOOMIN;
            break;

        case EV_TP_PINCH_OUT:
            pmsg->id            = GUI_MSG_TOUCH_ZOOMOUT;
            pmsg->data          = (pEvent_y.value << 16) + pEvent_x.value;
            pmsg->reserved      = (pEvent_speed_dir.value << 16) + pEvent_speed_val.value;
            last_touch_action   = GUI_MSG_TOUCH_ZOOMOUT;
            break;

        case EV_TP_PRESS_HOLD:
            pmsg->id            = last_touch_action;
            pmsg->data          = (pEvent_y.value << 16) + pEvent_x.value;
            pmsg->reserved      = (pEvent_speed_dir.value << 16) + pEvent_speed_val.value;
            break;

        case EV_TP_ACTION_NULL:
        case EV_TP_ACTION_CLICK:
        case EV_TP_ACTION_DOWN:
        case EV_TP_ACTION_UP:
        case EV_TP_ACTION_LEFT:
        case EV_TP_ACTION_RIGHT:
        case EV_TP_ACTION_ANTICLOCKWISE:
        case EV_TP_ACTION_CLOCKWISE:
        case EV_TP_ACTION_LD2RU:
        case EV_TP_ACTION_RU2LD:
        case EV_TP_ACTION_LU2RD:
        case EV_TP_ACTION_RD2LU:
            pmsg->id            = GUI_MSG_TOUCH_UP;
            pmsg->data          = (pEvent_y.value << 16) + pEvent_x.value;
            pmsg->reserved      = (pEvent_speed_dir.value << 16) + pEvent_speed_val.value;
            break;

        default:
            __log("cannot identify message: %x", pEvent_type.value);
            return EPDK_FALSE;
    }

    __wrn("------------------------------------------------");
    __wrn("pmsg->type = %d, pmsg->id = %d", pmsg->type, pmsg->id);
    __wrn("x = %d, y = %d",                 LOSWORD(pmsg->data), HISWORD(pmsg->data));
    __wrn("------------------------------------------------");

    esKRNL_QPost(emit_ctr.psys_msg_queue, (unsigned long)pmsg);
    return EPDK_TRUE;
}
#if defined(CONFIG_SUNXI_QA_TEST)
static int32_t is_running = 0;
static uint8_t task_id    = 0;

static int msg_sim(uint32_t key_code,uint32_t delay_time)
{
    __u8 err;
	__u8 error;
    __msrv_msg_t            *pmsg;
	esKRNL_SemPend(emit_ctr.p_array_sem, 0, &error);

    pmsg = get_msg_buf(&emit_ctr);

    esKRNL_SemPost(emit_ctr.p_array_sem);

	switch(key_code)
	{
	    case KPAD_TF_USB:
	    {
	        __wrn("+++++++++++++++++ GUI_MSG_KEY_AUDIO");
	        pmsg->type  = GUI_MSG_KEY;
	        pmsg->id    = GUI_MSG_KEY_TF_USB;
	        break;
	    }

	    case KPAD_MUSIC:
	    {
	        __wrn("+++++++++++++++++ GUI_MSG_KEY_AUDIO");
	        pmsg->type  = GUI_MSG_KEY;
	        pmsg->id    = GUI_MSG_KEY_AUDIO;
	        break;
	    }

	    case KPAD_VIDIO:
	    {
	        __wrn("+++++++++++++++++ GUI_MSG_KEY_VIDEO");
	        pmsg->type  = GUI_MSG_KEY;
	        pmsg->id    = GUI_MSG_KEY_VIDEO;
	        break;
	    }

	    case KPAD_PICTURE:
	    {
	        __wrn("+++++++++++++++++ GUI_MSG_KEY_PHOTO");
	        pmsg->type  = GUI_MSG_KEY;
	        pmsg->id    = GUI_MSG_KEY_PHOTO;
	        break;
	    }

	    case KPAD_UP:
	    {
	        __wrn("+++++++++++++++++ GUI_MSG_KEY_UP");
	        pmsg->type  = GUI_MSG_KEY;
	        pmsg->id    = GUI_MSG_KEY_UP;
	        break;
	    }

	    case KPAD_DOWN:
	    {
	        __wrn("+++++++++++++++++ GUI_MSG_KEY_DOWN");
	        pmsg->type  = GUI_MSG_KEY;
	        pmsg->id    = GUI_MSG_KEY_DOWN;
	        break;
	    }


	    case KPAD_NUM0:
	    case IR_KPAD_NUM0:
	    {
	        __wrn("+++++++++++++++ir GUI_MSG_KEY_NUM0");
	        pmsg->type  = GUI_MSG_KEY;
	        pmsg->id    = GUI_MSG_KEY_NUM0;
	        break;
	    }

	    case KPAD_LEFT:
	    case IR_KPAD_LEFT:
	    {
	        __wrn("+++++++++++++++ ir LEFT");
	        pmsg->type  = GUI_MSG_KEY;
	        pmsg->id    = GUI_MSG_KEY_LEFT;
	        break;
	    }

	    case KPAD_RIGHT:
	    case IR_KPAD_RIGHT:
	    {
	        __wrn("+++++++++++++++ ir RIGHT");
	        pmsg->type  = GUI_MSG_KEY;
	        pmsg->id    = GUI_MSG_KEY_RIGHT;
	        break;
	    }

	    case KPAD_ENTER:
	    case IR_KPAD_ENTER:
	    {
	        __wrn("+++++++++++++++ ir ENTER");
	        pmsg->type  = GUI_MSG_KEY;
	        pmsg->id    = GUI_MSG_KEY_ENTER;
	        break;
	    }

	    case KPAD_RETURN:
	    case IR_KPAD_RETURN:
	    {
	        __wrn("+++++++++++++++ ir RETURN");
	        pmsg->type  = GUI_MSG_KEY;
	        pmsg->id    = GUI_MSG_KEY_ESCAPE;
	        break;
	    }

		case KPAD_MENU:
		{
			__wrn("+++++++++++++++++ GUI_MSG_KEY_MENU");
			pmsg->type	= GUI_MSG_KEY;
			pmsg->id	= GUI_MSG_KEY_MENU;
			break;
		}

	    default:
	        __wrn("+++++++++++++++ir no this key msg");

	        break;
	}
    pmsg->data  = 1;

	__log("GUI_MSG_KEY pmsg: id:0x%x  data:0x%x  type:0x%x", pmsg->id,pmsg->data,pmsg->type);

    esKRNL_QPost(emit_ctr.psys_msg_queue, (unsigned long)pmsg);

	esKRNL_TimeDly(2);
	pmsg->data  = 0;
	esKRNL_QPost(emit_ctr.psys_msg_queue, (unsigned long)pmsg);

    if(delay_time<100 || delay_time>1000)
    {
        esKRNL_TimeDly(delay_time*100);//second
    }
    else
    {
        esKRNL_TimeDly(delay_time/10);//ms
    }
    return 0;
}

static H_LYR __prog_32bpp_layer_create(RECT *rect, __s32 pipe)
{
	H_LYR layer = NULL;
	FB	fb =
	{
		{0, 0}, 										/* size 	 */
		{0, 0, 0},										/* buffer	 */
		{FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt		*/
	};
	__disp_layer_para_t lstlyr =
	{
		MOD_DISP_LAYER_WORK_MODE_NORMAL,					/* mode 	 */
		0,												/* ck_mode	 */
		0,												/* alpha_en  */
		0,												/* alpha_val */
		1,												/* pipe 	 */
		9,											 /* prio	  */
		{0, 0, 0, 0},									/* screen	 */
		{0, 0, 0, 0},									/* source	 */
		DISP_LAYER_OUTPUT_CHN_DE_CH1,					/* channel	 */
		NULL											/* fb		 */
	};
	__layerwincreate_para_t lyrcreate_info =
	{
		"sub menu layer",
		NULL,
		GUI_LYRWIN_STA_SUSPEND,
		GUI_LYRWIN_NORMAL
	};
	fb.size.width		= rect->width;
	fb.size.height		= rect->height;
	lstlyr.src_win.x		= 0;
	lstlyr.src_win.y		= 0;
	lstlyr.src_win.width	= rect->width;
	lstlyr.src_win.height	= rect->height;
	lstlyr.scn_win.x		= rect->x;
	lstlyr.scn_win.y		= rect->y;
	lstlyr.scn_win.width	= rect->width;
	lstlyr.scn_win.height	= rect->height;
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

void read_chip_info(__u16 *markid,char *ddr_type)
{
	volatile unsigned int reg;

	reg = (*(volatile unsigned int *)(unsigned long)( 0x03006228));
	//reg = 0xa00;
	*ddr_type = (reg >> 8)&0xf;

	*(__u16 *)markid = (*((volatile __u16 *)(unsigned long)(0x03006200)));
	//*(__u16 *)markid = 0x5C00;

	__log("-----ddr--------%x--------------\n",*ddr_type);
	__log("-----markid-----%x--------------\n",*markid);

}


void read_spec_lock(__u16 * spec_lock)
{
	volatile unsigned int reg;

	reg = (*(volatile unsigned int *)(unsigned long)( 0x03006218));
	*spec_lock  = (reg >> 11)&0x1f;

	__log("-----spec_lock--------%x--------------\n",*spec_lock);


}

static void auto_test(void*arg)
{
	uint8_t err;
	__u16 spec_lock = 0;

    {
		msg_sim(KPAD_VIDIO,6);//电影快捷键,6S后此函数结束，默认播放，先挂载的存储介质
		msg_sim(KPAD_DOWN,6);//下一曲电影
		msg_sim(KPAD_DOWN,6);//下一曲进电影
	}
	{
    	msg_sim(KPAD_MUSIC,6);//音乐快捷键
    }
	msg_sim(KPAD_RETURN,2);
	msg_sim(KPAD_RETURN,3);
	//msg_sim(KPAD_RETURN,3);
	msg_sim(KPAD_TF_USB,1);//切换存储介质

	{

        msg_sim(KPAD_PICTURE,4);//保险进相片
		msg_sim(KPAD_MENU,1);//保险进相片
		msg_sim(KPAD_DOWN,1);
		msg_sim(KPAD_DOWN,1);
		msg_sim(KPAD_ENTER,1);//进入缩放菜单
		msg_sim(KPAD_UP,1);
		msg_sim(KPAD_ENTER,1);//放大
		msg_sim(KPAD_UP,1);
		msg_sim(KPAD_UP,1);
		msg_sim(KPAD_ENTER,1);
		msg_sim(KPAD_ENTER,1);//缩小
		msg_sim(KPAD_MENU,1);
    }
	msg_sim(KPAD_RETURN,3);
	msg_sim(KPAD_RETURN,1);
	msg_sim(KPAD_RETURN,2);//多退一次，保障退到主界面
	msg_sim(KPAD_TF_USB,1);//切换存储介质
	//msg_sim(KPAD_UP,1);
	//msg_sim(KPAD_UP,1);
	//msg_sim(KPAD_UP,1);

	read_spec_lock(&spec_lock);
	__log("auto_test movie complete!\n");
	{
		H_WIN h_win;
		H_LYR hlyr;
		int32_t 	width = 0, height = 0;
		RECT	rect = {0};
		GUI_RECT gui_rect;
		__u16 markid = 0;
		char ddr_type = 0;
		char str[50] = {0};
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
	    GUI_SetBkColor(GUI_BLUE);
	    GUI_Clear();
	    gui_rect.x0 = 0;
	    gui_rect.y0 = height / 2-80 ;
	    gui_rect.x1 = rect.width - 1;
	    gui_rect.y1 = gui_rect.y0 + 160;
	    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
	    GUI_SetColor(GUI_WHITE);
		read_chip_info(&markid, &ddr_type);
		GUI_FONT    *SWFFont = NULL;
		SWFFont     = GUI_SFT_CreateFont(48, BEETLES_APP_ROOT"res\\fonts\\font48.sft");
		GUI_SetFont(SWFFont);
		switch (markid)
	    {
	        case 0x5C00:
	        {
				__log("D1s-A ");
				eLIBs_strcpy(str,"chip type: D1s-A 0x5C00");
				break;
	        }

			case 0x7400:
	        {
				__log("D1s-B ");
				eLIBs_strcpy(str,"chip type: D1s-B 0x7400");
				break;
	        }

			case 0x2C00:
	        {
				__log("D1s-C ");
				eLIBs_strcpy(str,"chip type: D1s-C 0x2C00");
				break;
	        }

			case 0x0:
	        {
				eLIBs_strcpy(str,"Maybe not FT chip id = 0x0");
				break;
	        }

			default:
				__log("unknown chip");
				eLIBs_strcpy(str,"unknown chip");
	            break;
		}
		GUI_DispStringInRect(str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
		__log("str =%s",str);
		eLIBs_memset(str, 0, 50);
		gui_rect.x0 = 0;
	    gui_rect.y0 = height / 2 + 80;
	    gui_rect.x1 = rect.width - 1;
	    gui_rect.y1 = gui_rect.y0 + 160;
		eLIBs_strcpy(str,"ddr_type: ");
		{
		    char stack;
			int len;
			len = eLIBs_strlen(str);
		    {
		        stack = "0123456789ABCDEF"[ddr_type & 0xf];
		    }

		    str[len] = stack;
			str[len+1] = '\0';
		}

	    GUI_DispStringInRect(str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
		//esKRNL_TimeDly(500);
		__log("read_chip_info OK");
		eLIBs_memset(str, 0, 50);

		{
			gui_rect.x0 = 0 ;
		    gui_rect.y0 = 0;
		    gui_rect.x1 = rect.width - 1;
		    gui_rect.y1 = height / 2-80;
			switch (spec_lock)
			{
				case 0x16:
		        {
					eLIBs_strcpy(str,"D1s-A:efuse spec_lock: 0x16");
					break;
		        }

				case 0x6:
		        {
					eLIBs_strcpy(str,"D1s-B:efuse spec_lock: 0x6");
					break;
		        }

				case 0x1:
		        {
					eLIBs_strcpy(str,"T113-S3:efuse spec_lock: 0x1");
					break;
		        }

				default:
					eLIBs_strcpy(str,"unknown chip:efuse spec_lock: 0x");
					{
					    char stack1;
						char stack2;
						int len;
						len = eLIBs_strlen(str);
					    {
					        stack1 = "0123456789ABCDEF"[(spec_lock >> 4) & 0xf];
							stack2 = "0123456789ABCDEF"[spec_lock & 0xf];
					    }

					    str[len] = stack1;
						str[len+1] = stack2; 
						str[len+2] = '\0';
					}
		            break;
			}
			GUI_DispStringInRect(str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
		}

		while(1)
		{
			esKRNL_TimeDly(100);
		}
		GUI_LyrWinDelete(hlyr);
		GUI_SFT_ReleaseFont(SWFFont);

	}


	task_id = 0;
	is_running = 0;
    esKRNL_TDel(EXEC_prioself);
}

static int qa_auto_test(void)
{
    __log("auto_test start!!\n");
    if(is_running == 0)
    {
		task_id = esKRNL_TCreate(auto_test,NULL,0x2000,KRNL_priolevel1);
		is_running = 1;
    }
    else
    {
		__log("auto_test running!!\n");
    }
    return 0;
}
#endif
static __s32 key_msg_cb(void *msg)
{
    __msrv_msg_t            *pmsg;
    __u8                    error;
    __u32                   i;
    __input_event_packet_t  *pPacket;
    __input_event_t         *pEventFrame;

    if (msg == NULL)
    {
        __log("invalid argument for call back");
        return EPDK_FAIL;
    }

    //dump packet events
    pPacket = (__input_event_packet_t *)msg;

    if (pPacket->event_cnt > INPUT_MAX_EVENT_NR)
    {
        __log("invalid event count number");
        return EPDK_FAIL;
    }

    for (i = 0; i < pPacket->event_cnt; i++)
    {
        pEventFrame = &(pPacket->events[i]);

        __inf("Event %d: type = 0x%x, code = 0x%x, value = 0x%x", i, pEventFrame->type, pEventFrame->code, pEventFrame->value);

        if (pEventFrame->type == EV_KEY)
        {			
            esKRNL_SemPend(emit_ctr.p_array_sem, 0, &error);

            pmsg = get_msg_buf(&emit_ctr);

            esKRNL_SemPost(emit_ctr.p_array_sem);

            pmsg->reserved = 0;

            switch (pEventFrame->code)
            {                
                
#if defined(CONFIG_SUNXI_QA_TEST)
				case KEY_RESERVED:
				{
					qa_auto_test();
					break;
				}
#endif
				case KPAD_SCAN:
                {
                    __wrn("+++++++++++++++++ GUI_MSG_KEY_SCAN");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_SCAN;
                    break;
                }

                case KPAD_MENU:
                {
                    __wrn("+++++++++++++++++ GUI_MSG_KEY_MENU");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_MENU;
                    break;
                }

                case KPAD_MUSIC:
                {
                    __wrn("+++++++++++++++++ GUI_MSG_KEY_AUDIO");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_AUDIO;
                    break;
                }

                case KPAD_VIDIO:
                {
                    __wrn("+++++++++++++++++ GUI_MSG_KEY_VIDEO");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_VIDEO;
                    break;
                }

                case KPAD_PICTURE:
                {
                    __wrn("+++++++++++++++++ GUI_MSG_KEY_PHOTO");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_PHOTO;
                    break;
                }

                case KPAD_TV:
                {
                    __wrn("+++++++++++++++++ GUI_MSG_KEY_TV");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_TV;
                    break;
                }

                case KPAD_FM:
                {
                    __wrn("+++++++++++++++++ GUI_MSG_KEY_FM");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_FM;
                    break;
                }

                case KPAD_UP:
                {
                    __wrn("+++++++++++++++++ GUI_MSG_KEY_UP");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_UP;
                    break;
                }

                case KPAD_DOWN:
                {
                    __wrn("+++++++++++++++++ GUI_MSG_KEY_DOWN");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_DOWN;
                    break;
                }

                case KPAD_LCDOFF:
                {
                    __wrn("++++++++++++++++++ GUI_MSG_KEY_LCDOFF");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_LCDOFF;
                    break;
                }

                case KPAD_RESERVED:
                {
                    __wrn("+++++++++++++++ GUI_MSG_KEY_RESERVED");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_RESERVED;
                    break;
                }

                case KPAD_NUM0:
                case IR_KPAD_NUM0:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_NUM0");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_NUM0;
                    break;
                }

                case KPAD_NUM1:
                case IR_KPAD_NUM1:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_NUM1");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_NUM1;
                    break;
                }

                case KPAD_NUM2:
                case IR_KPAD_NUM2:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_NUM2");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_NUM2;
                    break;
                }

                case KPAD_NUM3:
                case IR_KPAD_NUM3:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_NUM3");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_NUM3;
                    break;
                }

                case KPAD_NUM4:
                case IR_KPAD_NUM4:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_NUM4");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_NUM4;
                    break;
                }

                case KPAD_NUM5:
                case IR_KPAD_NUM5:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_NUM5");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_NUM5;
                    break;
                }

                case KPAD_NUM6:
                case IR_KPAD_NUM6:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_NUM6");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_NUM6;
                    break;
                }

                case KPAD_NUM7:
                case IR_KPAD_NUM7:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_NUM7");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_NUM7;
                    break;
                }

                case KPAD_NUM8:
                case IR_KPAD_NUM8:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_NUM8");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_NUM8;
                    break;
                }

                case KPAD_NUM9:
                case IR_KPAD_NUM9:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_NUM9");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_NUM9;
                    break;
                }

                case IR_KPAD_RR:
                {
                    __wrn("+++++++++++++++++ GUI_MSG_KEY_RR");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_RR;
                    break;
                }

                case IR_KPAD_FF:
                {
                    __wrn("+++++++++++++++++ GUI_MSG_KEY_FF");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_FF;
                    break;
                }

                case IR_KPAD_PREV:
                {
                    __wrn("+++++++++++++++++ GUI_MSG_KEY_PREV");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_PREV;
                    break;
                }

                case IR_KPAD_NEXT:
                {
                    __wrn("+++++++++++++++++ GUI_MSG_KEY_NEXT");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_NEXT;
                    break;
                }

                case IR_KPAD_AUX:
                {
                    __wrn("+++++++++++++++++ GUI_MSG_KEY_AUX ");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_AUX;
                    break;
                }

                case IR_KPAD_UP:
                {
                    __wrn("+++++++++++++++ ir UP");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_UP;
                    break;
                }

                case IR_KPAD_DOWN:
                {
                    __wrn("+++++++++++++++ ir DOWN");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_DOWN;
                    break;
                }

                case KPAD_LEFT:
                case IR_KPAD_LEFT:
                {
                    __wrn("+++++++++++++++ ir LEFT");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_LEFT;
                    break;
                }

                case KPAD_RIGHT:
                case IR_KPAD_RIGHT:
                {
                    __wrn("+++++++++++++++ ir RIGHT");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_RIGHT;
                    break;
                }

                case KPAD_ENTER:
                case IR_KPAD_ENTER:
                {
                    __wrn("+++++++++++++++ ir ENTER");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_ENTER;
                    break;
                }

                case KPAD_POWEROFF:
                case IR_KPAD_POWEROFF:
                {
                    //进入standby
                    __wrn("+++++++++++++++ ir GUI_MSG_KEY_POWEROFF");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_POWEROFF;
                    break;
                }

                case IR_KPAD_MENU:
                {
                    __wrn("+++++++++++++++ ir MENU");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_MENU;
                    break;
                }

                case IR_KPAD_MUTE:
                {
                    __wrn("+++++++++++++++ ir MUTE");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_MUTE;
                    break;
                }

                case KPAD_RETURN:
                case IR_KPAD_RETURN:
                {
                    __wrn("+++++++++++++++ ir RETURN");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_ESCAPE;
                    break;
                }

                case KPAD_VOICEUP:
                case IR_KPAD_VOICEUP:
                {
                    __wrn("+++++++++++++++ ir VOICE up");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_VADD;
                    break;
                }

                case KPAD_VOICEDOWN:
                case IR_KPAD_VOICEDOWN:
                {
                    __wrn("+++++++++++++++ ir VOICE down");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_VDEC;
                    break;
                }

                case IR_KPAD_MIC_ONOFF:
                {
                    __wrn("+++++++++++++++ir MIC ONOFF");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_MIC_ONOFF;
                    break;
                }

                case IR_KPAD_REC_ONOFF:
                {
                    __wrn("+++++++++++++++ir REC ONOFF");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_REC_ONOFF;
                    break;
                }

                case IR_KPAD_MIC_ADD:
                {
                    __wrn("+++++++++++++++ir MIC ADD");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_MIC_VOL_ADD;
                    break;
                }

                case IR_KPAD_MIC_DEC:
                {
                    __wrn("+++++++++++++++ir MIC DEC");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_MIC_VOL_DEC;
                    break;
                }

                case IR_KPAD_MODE:
                {
                    __wrn("+++++++++++++++ IR GUI_MSG_KEY_MODE");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_MODE;
                    break;
                }

                case IR_KPAD_MUSIC:
                {
                    __wrn("+++++++++++++++ IR music");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_AUDIO;
                    break;
                }

                case IR_KPAD_MOVIE:
                {
                    __wrn("+++++++++++++++ IR movie");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_VIDEO;
                    break;
                }

                case IR_KPAD_ECHO:
                {
                    __wrn("+++++++++++++++ir ECHO onoff");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_ECHO_ONOFF;
                    break;
                }

                case IR_KAPD_ECHO_SET:
                {
                    __wrn("+++++++++++++++ir ECHO setting");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_ECHO_SETTING;
                    break;
                }

                case IR_KPAD_ACCOMP_ONOFF:
                {
                    __wrn("+++++++++++++++ir accomp onoff");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_ACCOMP_ONOFF;
                    break;
                }

                case IR_KPAD_LYRIC_ADD:
                {
                    __wrn("+++++++++++++++ir lyric add");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_ADD_LYRIC_TIME;
                    break;
                }

                case IR_KPAD_LYRIC_DEC:
                {
                    __wrn("+++++++++++++++ir lyric add");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_DEC_LYRIC_TIME;
                    break;
                }

                case IR_KPAD_REPEAT:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_NUM9");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_REPEATE;
                    break;
                }

                case IR_KPAD_EQ:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_EQ");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_EQ;
                    break;
                }

                case IR_KPAD_SETUP:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_SETUP");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_SETUP;
                    break;
                }

                case IR_KPAD_PLAY_PAUSE:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_PLAY_PAUSE");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_PLAY_PAUSE;
                    pmsg->reserved = 1;//为1代表从ir发过来的按键消息，以区分面板的KPAD_PLAY_PAUSE
                    break;
                }

                case IR_KPAD_SINGER:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_SINGER");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_SINGER;
                    break;
                }

                case IR_KPAD_ALPHABET:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_ALPHABET");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_ALPHABET;
                    break;
                }

                case IR_KPAD_DIGITAL:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_DIGITAL");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_DIGITAL;
                    break;
                }

                case IR_KPAD_FAVOR:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_FAVOR");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_FAVOR;
                    break;
                }

                case IR_KPAD_NEWSONG:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_NEWSONG");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_NEWSONG;
                    break;
                }

                case IR_KPAD_SEL_LIST:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_SEL_LIST");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_SEL_LIST;
                    break;
                }

                case IR_KPAD_DELETE:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_DELETE");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_DELETE;
                    break;
                }

                case IR_KPAD_CUTSONG:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_CUTSONG");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_CUTSONG;
                    break;
                }

                case IR_KPAD_NTFS_PAL:
                {
                    __wrn("+++++++++++++++ir GUI_MSG_KEY_NTFS_PAL");
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_NTFS_PAL;
                    break;
                }

                default:
	                __wrn("+++++++++++++++ir no this key msg");

                    break;
            }

            pmsg->data  = pEventFrame->value;
    
            __inf("GUI_MSG_KEY pmsg: id:0x%x  data:0x%x  type:0x%x", pmsg->id,pmsg->data,pmsg->type);	

            esKRNL_QPost(emit_ctr.psys_msg_queue, (unsigned long)pmsg);
        }
    }

    return EPDK_TRUE;
}

/**
 * 从系统消息队列取消息
 */
static void ksrv_msg_thread(void *arg)
{
    __memit_ctr_t *emit = (__memit_ctr_t *)arg;
    __u8    error;

    /* 清空按键消息队列 */
    while (1)
    {
        __u32           usrmsg;

        usrmsg = esKSRV_GetMsg();

        if (usrmsg == 0)
        {
            break;
        }
    }

    while (1)
    {
        __u32           usrmsg;
        __msrv_msg_t    *pmsg;
        __u32           tmp;

        while (1)
        {
            if (esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
            {
                esKRNL_TDel(OS_PRIO_SELF);
            }

            usrmsg = esKSRV_GetMsg();               // 系统消息队列

            if (usrmsg)
            {
                break;
            }

            esKRNL_TimeDly(2);
        }

        esKRNL_SemPend(emit->p_array_sem, 0, &error);

        pmsg = get_msg_buf(emit);

        //eLIBs_memset(pmsg, 0, sizeof(__msrv_msg_t));

        esKRNL_SemPost(emit->p_array_sem);

        if ((usrmsg & 0xffff0000) == KMSG_USR_CLS_KEY)  // key 按键消息
        {
            tmp = usrmsg & 0x0000ffff;

            __log(" usrmsg = %d ", tmp);

            switch (usrmsg)
            {
                case KMSG_USR_KEY_POWER:                /* standby  */
                    pmsg->type  = DSK_MSG_STANDBY;
                    break;

                case KMSG_USR_KEY_REPEATE:              /* 切换到色差输出 */
                    pmsg->type  = DSK_MSG_SWITCH_YPBPR;
                    break;

                case KMSG_USR_KEY_CLEAR:                /* 切换到cvbs输出 */
                    pmsg->type  = DSK_MSG_SWITCH_CVBS;
                    break;

                case KMSG_USR_KEY_DISPLAY:              /* 切换到hdmi输出 */
                    pmsg->type  = DSK_MSG_SWITCH_HDMI;
                    break;

                case KMSG_USR_KEY_SHIFT:
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_SHIFT;
                    break;

                case KMSG_USR_KEY_SEL:
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_SEL;
                    break;

                case KMSG_USR_KEY_NUM4:
                    pmsg->type  = DSK_MSG_SWITCH_AUDIO_IF;
                    pmsg->data  = AUDIO_DEV_IF_CODEC;
                    break;

                case KMSG_USR_KEY_NUM5:
                    pmsg->type  = DSK_MSG_SWITCH_AUDIO_IF;
                    pmsg->data  = AUDIO_DEV_IF_IIS;
                    break;

                case KMSG_USR_KEY_NUM6:
                    pmsg->type  = DSK_MSG_SWITCH_AUDIO_IF;
                    pmsg->data  = AUDIO_DEV_IF_SPDIF;
                    break;

                case KMSG_USR_KEY_NUM8:                 /* hold 按键 */
                    pmsg->type  = DSK_MSG_HOLD;
                    break;

                case KMSG_USR_KEY_NUM7:                 // 禁音功能
                    pmsg->type  = DSK_MSG_BAN_VOLUME;
                    pmsg->data  = 0;
                    break;

                case KMSG_USR_KEY_GOTO:
                    pmsg->type  = DSK_MSG_APP_EXIT;      /* 一键回主界面 */
                    break;

                case KMSG_USR_KEY_POWEROFF:             // 关机消息
                    pmsg->type  = DSK_MSG_POWER_OFF;
                    break;

                case KMSG_USR_KEY_NUM9:
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_ESCAPE;
                    break;

                case KMSG_USR_KEY_VOICE_UP:
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_VADD;
                    break;

                case KMSG_USR_KEY_VOICE_DOWN:
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_VDEC;
                    break;

                case KMSG_USR_KEY_UP:
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_UP;
                    break;

                case KMSG_USR_KEY_DOWN  :
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_DOWN;
                    break;

                case KMSG_USR_KEY_LEFT  :
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_LEFT;
                    break;

                case KMSG_USR_KEY_RIGHT :
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_RIGHT;
                    break;

                case KMSG_USR_KEY_ENTER:
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_ENTER;
                    break;

                case KMSG_USR_KEY_RISE:
                    pmsg->type  = GUI_MSG_KEY;
                    pmsg->id    = GUI_MSG_KEY_RISE;
                    break;

                default:
                    continue;
            }
        }
        else    /* system 消息 */
        {
            if ((usrmsg & 0x0000ffff) == KMSG_USR_SYSTEM_FS_PLUGIN)
            {
                pmsg->type  = DSK_MSG_FS_PART_PLUGIN;
                pmsg->data  = (usrmsg & KMSG_USR_SYSTEM_FS_PARA_MASK) >> 16;
            }
            else if ((usrmsg & 0x0000ffff) == KMSG_USR_SYSTEM_FS_PLUGOUT)
            {
                pmsg->type  = DSK_MSG_FS_PART_PLUGOUT;
                pmsg->data  = (usrmsg & KMSG_USR_SYSTEM_FS_PARA_MASK) >> 16;
            }
            else
            {
                __log("*************usrmsg = %d***************", usrmsg);

                switch (usrmsg)
                {
                    case KMSG_USR_SYSTEM_USBD_PLUGIN:       // usb device plug in
                        __inf("case KMSG_USR_SYSTEM_USBD_PLUGIN");
                        pmsg->type  = DSK_MSG_USBD_PLUG_IN;
                        break;

                    case KMSG_USR_SYSTEM_USBD_PLUGOUT:      // usb device plug out
                        pmsg->type  = DSK_MSG_USBD_PLUG_OUT;
                        __inf("case KMSG_USR_SYSTEM_USBD_PLUGOUT");
                        break;

                    case KMSG_USR_SYSTEM_SDMMC_PLUGOUT:
                        pmsg->type  = DSK_MSG_APP_EXIT;
                        break;

                    case KMSG_USR_SYSTEM_USBH_PLUGOUT:      // usb host 设备拔出
                        pmsg->type  = DSK_MSG_HANDLE_PLUGOUT;//112350//DSK_MSG_APP_EXIT;
                        break;

                    case KMSG_USR_SYSTEM_TVDAC_PLUGIN:
                        __inf("**********KMSG_USR_SYSTEM_TVDAC_PLUGIN***********");
                        pmsg->type  = DSK_MSG_TVDAC_PLUGIN;
                        break;

                    case KMSG_USR_SYSTEM_TVDAC_PLUGOUT:
                        __inf("***********KMSG_USR_SYSTEM_TVDAC_PLUGOUT***************");
                        pmsg->type  = DSK_MSG_TVDAC_PLUGOUT;
                        break;

                    case KMSG_USR_SYSTEM_HDMI_PLUGIN:
                        __inf("**********KMSG_USR_SYSTEM_HDMI_PLUGIN***********");
                        pmsg->type  = DSK_MSG_HDMI_PLUGIN;
                        break;

                    case KMSG_USR_SYSTEM_HDMI_PLUGOUT:
                        __inf("***********KMSG_USR_SYSTEM_HDMI_PLUGOUT***************");
                        pmsg->type  = DSK_MSG_HDMI_PLUGOUT;
                        break;

                    case KMSG_USR_SYSTEM_WAKEUP:
                        pmsg->type  = DSK_MSG_STANDBY_WAKE_UP;
                        break;

                    default:
                        continue;
                }
            }
        }

        esKRNL_QPost(emit->psys_msg_queue, (unsigned long)pmsg);
    }
}

__s32 msg_srv_get_message(__memit_ctr_t *emit, __msrv_msg_t *p_msg)
{
    __u8            error;
    __msrv_msg_t    *tmp;

    tmp = (__msrv_msg_t *)esKRNL_QAccept(emit->psys_msg_queue, &error);

    if (tmp != NULL)
    {
        memcpy(p_msg, tmp, sizeof(__msrv_msg_t));
        return EPDK_OK;
    }
    else
    {
        return EPDK_FAIL;
    }
}

/**
 * 发送消息到桌面系统消息队列
 */
__s32 msg_srv_dispatch_message(__memit_ctr_t *emit, __msrv_msg_t *p_msg)
{
    H_WIN           h_win;
    __gui_msg_t     msg;
    __s32           x, y;
    __u8            err;

    memset(&msg, 0x00, sizeof(msg));

    if (p_msg->type == GUI_MSG_TOUCH)
    {
        msg.id          = p_msg->type;
        msg.h_srcwin    = NULL;
        msg.dwAddData1  = p_msg->id;
        msg.dwAddData2  = p_msg->data;
        msg.dwReserved  = p_msg->reserved;
        msg.p_arg       = NULL;

        x   = LOSWORD(msg.dwAddData2);
        y   = HISWORD(msg.dwAddData2);

        __bool          btvout_tp_hook;

        btvout_tp_hook = EPDK_FALSE;

        if (emit_ctr.p_tvout_tp_hook_sem)
        {
            esKRNL_SemPend(emit_ctr.p_tvout_tp_hook_sem, 0, &err);
        }

        if (NULL != emit_ctr.tvout_tp_hook)
        {
            __s32 ret;

            ret             = EPDK_FAIL;
            msg.id          = GUI_MSG_USER_DEF;
            btvout_tp_hook  = EPDK_TRUE;
            msg.h_srcwin    = NULL;
            msg.h_deswin    = NULL;

            __wrn("before call back emit_ctr.tvout_tp_hook");
            ret = esKRNL_CallBack((__s32(*)(void *))emit_ctr.tvout_tp_hook, &msg);

            if (EPDK_FAIL == ret)
            {
                __wrn("process fail, esKRNL_CallBack(emit_ctr.tvout_tp_hook, &msg) ret = %d", ret);
            }
            else
            {
                __wrn("process ok, esKRNL_CallBack(emit_ctr.tvout_tp_hook, &msg) ret = %d", ret);
            }
        }

        if (emit_ctr.p_tvout_tp_hook_sem)
        {
            esKRNL_SemPost(emit_ctr.p_tvout_tp_hook_sem);
        }

        if (EPDK_TRUE == btvout_tp_hook)
        {
            return EPDK_OK;
        }

        h_win = GUI_GetTouchFocusWin(x, y); /* 位于窗口范围内, 则发送到窗口 */

        if (h_win)
        {
         //   __log("%d:h_win = %x", __LINE__, h_win);
            msg.h_srcwin    = NULL;
            msg.h_deswin    = h_win;
        }
        else                                /* 不在任何窗口区域, 则发送到焦点窗口 */
        {
            h_win   = GUI_GetActiveManWin();
          //  __log("%d:h_win = %x", __LINE__, h_win);

            if (h_win)
            {
                switch (msg.dwAddData1)
                {
                    case GUI_MSG_TOUCH_DOWN:
                        msg.dwAddData1  = GUI_MSG_TOUCH_OVERDOWN;
                        break;

                    case GUI_MSG_TOUCH_UP:
                        msg.dwAddData1  = GUI_MSG_TOUCH_OVERUP;
                        break;

                    case GUI_MSG_TOUCH_MOVE:
                        msg.dwAddData1  = GUI_MSG_TOUCH_OVERMOVE;
                        break;

                    case GUI_MSG_TOUCH_ZOOMIN:
                        msg.dwAddData1  = GUI_MSG_TOUCH_OVERZOOMIN;
                        break;

                    case GUI_MSG_TOUCH_ZOOMOUT:
                        msg.dwAddData1  = GUI_MSG_TOUCH_OVERZOOMOUT;
                        break;

                    default:
                        break;
                }

                msg.h_srcwin    = NULL;
                msg.h_deswin    = h_win;
            }
            else
            {
                msg.h_deswin = NULL;
            }
        }

        esKRNL_SemPend(emit->p_hook_sem, 0, &err);

        if (emit->tp_hook)
        {
            (emit->tp_hook)(&msg);
        }

        esKRNL_SemPost(emit->p_hook_sem);
    }
    else if (p_msg->type == GUI_MSG_KEY)
    {
        msg.id          = p_msg->type;
        msg.h_srcwin    = NULL;
        msg.h_deswin    = GUI_GetActiveManWin();
        msg.dwAddData1  = p_msg->id;
        msg.dwAddData2  = p_msg->data;
        msg.dwReserved  = p_msg->reserved;
        msg.p_arg       = NULL;

        esKRNL_SemPend(emit->p_hook_sem, 0, &err);

        if (emit->key_hook)
        {
            (emit->key_hook)(&msg);
        }

        esKRNL_SemPost(emit->p_hook_sem);
    }
    else if ((p_msg->type >= DSK_MSG_START) && (p_msg->type <= DSK_MSG_LAST))
    {
        msg.id          = p_msg->type;
        msg.h_srcwin    = NULL;
        msg.h_deswin    = GUI_GetActiveManWin();
        msg.dwAddData1  = 0;
        msg.dwAddData2  = p_msg->data;
        msg.p_arg       = NULL;

        esKRNL_SemPend(emit->p_hook_sem, 0, &err);

        if (emit->init_hook)
        {
            (emit->init_hook)(&msg);
        }

        esKRNL_SemPost(emit->p_hook_sem);
    }
    else
    {
        __err(" msg.id = %d is error ", p_msg->type);
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

/**
 * msg srv thread
 */
void msg_srv_thread(void *arg)
{
    __msrv_msg_t    msg;

    __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
    __inf("&            message server started !!!                 &");
    __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
    msg_srv_init_input_panel(&emit_ctr);
    msg_srv_init_key_channel(&emit_ctr);
#ifdef CONFIG_SUPPORT_TOUCHPANEL
    msg_srv_init_tp_channel(&emit_ctr);
#endif
    msg_srv_init_ksrv_channel(&emit_ctr);
	
    /* 消息分发 */
    while (1)
    {
        if (esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
        {
            /* 释放消息通道 */
            msg_srv_deinit_key_channel(&emit_ctr);
#ifdef CONFIG_SUPPORT_TOUCHPANEL
            msg_srv_deinit_tp_channel(&emit_ctr);
#endif
            msg_srv_deinit_ksrv_channel(&emit_ctr);

            __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
            __inf("&            message server exited !!!                  &");
            __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
            esKRNL_TDel(OS_PRIO_SELF);
            return;
        }

        memset(&msg, 0, sizeof(__msrv_msg_t));

        if (EPDK_OK ==  msg_srv_get_message(&emit_ctr, &msg))
        {
        	//__log("sent msg type:%d  data:%d  id:%d  ",msg.type, msg.data, msg.id);

            msg_srv_dispatch_message(&emit_ctr, &msg);

			//__log("pass msg key to key_hook OK 4");
			
        }
        else
        {
            esKRNL_TimeDly(1);
        }
    }
}


__s32 msg_emit_init(void)
{
    /* 创建消息收集线程 */

    msg_srv_tid = esKRNL_TCreate(msg_srv_thread, NULL, 0x1000, KRNL_priolevel3);
    esKRNL_TaskNameSet(msg_srv_tid, "emitq2orange");
    if (msg_srv_tid != 0)
    {
        return EPDK_OK;
    }
    else
    {
        return EPDK_FAIL;
    }
}

__s32 msg_emit_deinit(void)
{
    /* 删除消息采集线程 */
    while (esKRNL_TDelReq(msg_srv_tid) != OS_TASK_NOT_EXIST)
    {
        esKRNL_TimeDly(1);
    }

    msg_srv_tid = 0;
    return EPDK_OK;
}

__s32 msg_emit_register_hook(__memit_hook_t *hook)
{
    __u8    err;

    emit_ctr.key_hook   = hook->key_hook;
    emit_ctr.tp_hook    = hook->tp_hook;
    emit_ctr.init_hook  = hook->init_hook;    
    return EPDK_OK;
}

__s32 msg_emit_register_tvout_tp_hook(cb_tvout_tp_hook hook)
{
    __u8    err;

    __wrn("msg_emit_register_tvout_tp_hook=0x%x", hook);

    if (emit_ctr.p_tvout_tp_hook_sem)
    {
        esKRNL_SemPend(emit_ctr.p_tvout_tp_hook_sem, 0, &err);
    }

    emit_ctr.tvout_tp_hook = hook;

    if (emit_ctr.p_tvout_tp_hook_sem)
    {
        esKRNL_SemPost(emit_ctr.p_tvout_tp_hook_sem);
    }

    return EPDK_OK;
}
