/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：        txtbox.h
*
*Author：           William Wang
*
*Version :          1.1.0
*
*Date:              2008-12-17
*
*Description :      lion txtbox control function implement
*
*Others :           None at present.
*
* History :
*
* <Author>          <time>          <version>     <description>
*
* William Wang     2008-12-17         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __TXTBOX_I_H__
#define __TXTBOX_I_H__

#include "txtbox_e.h"
#include "txtlayer.h"


#define TXTBOX_FRM_NUMBER        64
#define TXTBOX_FRM_TXQ_DEPTH     16
#define TXTBOX_FRM_RXQ_DEPTH     64

/*txtbox控件所在图层的状态信息*/
typedef enum TXTBOX_LYRSTS
{
    TXTBOX_WAKEUP,                    /*txtbox占有framebuffer*/
    TXTBOX_SLEEP                      /*txtboxframebuffer释放*/
} TXTBOX_LYRSTS_E;

typedef enum TXTBOX_STATUS
{
    TXTBOX_ONRES        = 0,
    TXTBOX_REQDEL       = 1,
    TXTBOX_ONDRAW       = 1 << 1,
    TXTBOX_DRAWFINISH   = 1 << 2,
    TXTBOX_INIT         = 1 << 3
} TXTBOX_STATUS_e;

typedef struct tag_TXTBOX_DATA
{
    txtlayer_man_t          *txtlayer;
    __u32                    mode;
    H_WIN                    handle;
    __pGUI_WIN_CB            user_cb;
    __u32                    status;
    __u32                    item_w;
    TXTBOX_LYRSTS_E          lyrsts;
} __txtbox_data_t;

__u32  GUI_TxtboxOnDraw(H_WIN txt);

#endif
