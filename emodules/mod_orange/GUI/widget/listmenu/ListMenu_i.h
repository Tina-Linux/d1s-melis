/*
*******************************************************************************************************************
*                                                           Graphic Library
*                                                   the graphic library support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name:       ListMenu_i.h
*
*Author:          William Wang
*
*Version :        1.1.0
*
*Date:            2008-10-24
*
*Description :    listmenu head file definition
*
*Others :         None at present.
*
* History :
*
* <Author>          <time>              <version>     <description>
*
* William Wang     2008-10-24    1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __LISTMENU_I_H__
#define __LISTMENU_I_H__

#include "Listmenu_e.h"
#include "ListLayer.h"
#include "GlideCurve.h"

#define GUI_MSG_USRDEF          (GUI_MSG_CONTROL + 100)

#define LISTMENU_FRM_NUMBER        64
#define LISTMENU_FRM_TXQ_DEPTH     16
#define LISTMENU_FRM_RXQ_DEPTH     64
#define LISTMENU_OTHER_NUM         2
/**********************************************************************************************************************/
/* listmenu帧格式                                                                                                        */
#define LISTMENU_FRMBUF_FREEFLAG      0xff      /* 通常，若此buffer被使用了，flag不可能为IPIPE_FRMBUF_FREEFLAG           */
#define LISTMENU_FRMBUF_USEDFLAG      0x00
#define LISTMENU_FIXEDEXT_WIDTH       0x00      /*listmenu固定模式下首尾扩展宽度*/

typedef enum LISTMENU_STATUS
{
    LISTMENU_ONRES      = 0,
    LISTMENU_ONDRAW     = 1 << 1,
    LISTMENU_ONRUN      = 1 << 2,              /*运动状态*/
    LISTMENU_ONLOAD     = 1 << 3,              /*加载状态*/
    LISTMENU_PUSHED     = 1 << 8,
    LISTMENU_UNPUSHED   = 1 << 9,
    LISTMENU_FOCUS      = 1 << 10
} LISTMENU_STATUS_E;

/*listmenu控件所在图层的状态信息*/
typedef enum LISTMENU_LYRSTS
{
    LISTMENU_WAKEUP,                    /*listmenu占有framebuffer*/
    LISTMENU_SLEEP                      /*listbufferframebuffer释放*/
} LISTMENU_LYRSTS_E;

typedef enum LISTMENU_REQCMD
{
    LISTMENU_REQSTOP = 1 << 0      /*应用程序申请停止*/
} LISTMENU_REQCMD_e;


/*listmenu消息队列结构*/
typedef struct LISTMENU_RUNMSG
{
    __u32       id;
    __u32       dwAddData;
} __listmenu_runmsg_t;

#define STEP_ARRAY            128

typedef enum tag_glide_state
{
    LSTMGLIDE_IDLE,
    LSTMGLIDE_START,
    LSTMGLIDE_MOVE,
    LSTMGLIDE_STOP
} lstmglide_state_t;


/*listmenu管理信息结构*/
typedef struct tag_LISTMENU_DATA
{
    LIST_LAYER_MAN_t        *listlayer;                 /*图层控制信息结构*/
    __listmenu_runmsg_t     *msgbuff;                   /*消息缓冲结构*/
    __krnl_event_t          *listmenu_RxQ;              /* 所有的通道共用一个输出通道队列                 */
    __u32                    listmenu_RxTID;            /*接收消息线程ID*/
    __u32                    mode;                      /*listmenu模式控制*/
    __krnl_event_t          *run_sem;                   /*控制上下键的输入*/
    __krnl_event_t          *dist_sem;                   /*控制在运动期间是否有其他消息过来*/
    __krnl_event_t          *load_sem;                  /*控制在停止之后的加载情况*/
    H_WIN                    handle;                    /*listmenu所在窗口的句柄*/
    __pGUI_WIN_CB            user_cb;                   /*用户定义的回调函数*/
    __u16                    status;                    /*listmenu状态信息*/
    __u32                    lastmsg;
    __u32                    lastkeyaction;             /*key action added by Derek 2011-1-5 14:27:17*/
    __pos_t                  last_pos;                  /*上次鼠标或者触摸屏的点击点*/
    __u32                    last_touch;
    LISTMENU_LYRSTS_E        lyrsts;                    /*listmenu图层状态信息*/
    H_LYR                    slave_lyr;
    __rect_t                 slyr_rect;                 /* slave layer原始位置 */
    __rect_t                 slyr_crect;                /* slave 当前位置*/

    /* 弹性曲线 */
    int                      step[STEP_ARRAY];           // touch 滑动步长数组
    __krnl_event_t          *move_sem;                  /*控制在停止之后的加载情况*/
    int                      len;                       // 数组内有效数据长度
    int                      index;                     // 图层移动计数
    __pos_t                  win_start;                 // 起始点对应的窗口坐标
    __lstmgcuv_para_t        gcurv;                     // gcurv 参数

    lstmglide_state_t        gstate;                    // tbar 移动状态

    __u32                    request;
} __listmenu_data_t;

typedef __listmenu_data_t    *plistmenudata;

#define LSTM_STATUS(pCtrl)      (((plistmenudata)GUI_CtrlWinGetAddData(pCtrl))->status)

__u32  GUI_LstmOnMessage(__listmenu_data_t *listmenu, __listmenu_runmsg_t *pMsg);
void   GUI_LstmCallBack(__gui_msg_t *message);
__u32  GUI_LstmOnDraw(__listmenu_data_t *listmenu);
__u32  GUI_LstmSetFoucs(__listmenu_data_t *plistmenu);
__u32  GUI_LstmSetCurItemNo(__listmenu_data_t *plistmenu, __u32 no);
void   GUI_LstmStopProc(__listmenu_data_t *plistmenu);
void   GUI_LstmPostRunMsg(__listmenu_data_t *plistmenu, __u32 runid);
void   GUI_LstmStopOnLoad(__listmenu_data_t *plistmenu);
void   GUI_LstmCurProc(__listmenu_data_t *pData);
void   GUI_LstmStopOnLoad(__listmenu_data_t *plistmenu);
#endif
