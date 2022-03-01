/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                               (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : app_ebook_i.h
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#ifndef __APP_EBOOK_I_H__
#define  __APP_EBOOK_I_H__

#include "beetles_app.h"
#include "common/common.h"

#define TTS_LIBS_NO_LINK        1

#define app_sprintf                            eLIBs_sprintf

#define  EBOOK_PAGE_ID          (APP_EBOOK_ID+1)
#define  EBOOK_SSET_SCENE_ID    (APP_EBOOK_ID+2)
#define  EBOOK_TIPS_ID          (APP_EBOOK_ID+3)
#define  EBOOK_LOADING_ID       (APP_EBOOK_ID+4)
#define  EBOOK_TTS_ID           (APP_EBOOK_ID+5)
#define  EBOOK_GOTO_ID          (APP_EBOOK_ID+6)
#define  EBOOK_SBMK_ID          (APP_EBOOK_ID+7)
#define  EBOOK_PAGEFULL_ID      (APP_EBOOK_ID+8)


#define  APP_EBOOK_FCS_TXT_COLOR        APP_COLOR_YELLOW
#define  APP_EBOOK_UNFCS_TXT_COLOR  APP_COLOR_WHITE


typedef enum
{
    CMD_PAGE_SET_PREV,
    CMD_PAGE_SET_NEXT,
    CMD_PAGE_SET_PLAY,          /*自动播放和TTS 共用的消息*/
    CMD_PAGE_SET_OUT_STOP_PLAY,     /*自动播放的停止消息*/
    CMD_PAGE_SET_SET,
    CMD_PAGE_SET_CLOSE,
} tag_page_cmd_t;


typedef enum
{
    CMD_TTS_SET_NEXT,
    CMD_TTS_SET_STOP,
} tag_tts_cmd_e;



__s32 Ebook_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);
__s32 Ebook_cmd2parent_syn(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);

#endif

