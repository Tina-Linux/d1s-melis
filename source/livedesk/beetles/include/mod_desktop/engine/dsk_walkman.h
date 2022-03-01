/*
************************************************************************************************************************
*                                                       walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_walkman.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.06.14
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2010.06.14        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_WALKMAN_H__
#define  __DSK_WALKMAN_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "walkman//dsk_wkm_npl.h"
#include "walkman//dsk_wkm_play_mode.h"
#include "walkman//dsk_wkm_fsm_ctrl.h"
#include "walkman//dsk_wkm_ff_rr_speed.h"
#include "walkman//dsk_wkm_eq.h"
#include "walkman//dsk_wkm_ab.h"
#include "walkman//dsk_wkm_lyric.h"
#include "walkman//dsk_wkm_spectrum.h"
#include "walkman//dsk_wkm_media_info.h"
#include "walkman//dsk_wkm_play_speed.h"
#include "walkman//dsk_wkm_star.h"
#include "walkman//dsk_wkm_monitor_gate.h"


typedef enum
{
    DSK_WKM_MODE_AUDIO_MIN          = 0,

    DSK_WKM_MODE_UNKNOWN            = -1
}
dsk_wkm_mode_e;


typedef enum
{
    DSK_WKM_EVENT_AUTO_PLAY_NEXT    = 0,    // walkman has played the next item autoly. The argument is NULL.
    DSK_WKM_EVENT_NO_ITEM_TO_PLAY,          // no item to play in the current repeat mode. The argument is NULL.

    DSK_WKM_EVENT_UNKNOWN           = -1
} dsk_wkm_event_e;


/*
****************************************************************************************************
*Name        : dsk_wkm_init
*Prototype   : __s32 dsk_wkm_init( dsk_wkm_mode_e work_mode )
*Arguments   : work_mode  input. work mode
*Return      : ==0     succeed
*              !=0     fail
*Description : init walkman
*Other       :
****************************************************************************************************
*/
__s32 dsk_wkm_init(dsk_wkm_mode_e work_mode);

/*
****************************************************************************************************
*Name        : dsk_wkm_exit
*Prototype   : void dsk_wkm_exit( void )
*Arguments   : void
*Return      : void
*Description : exit walkman
*Other       :
****************************************************************************************************
*/
extern void dsk_wkm_exit(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_open
*Prototype   : __s32  dsk_wkm_open( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : open walkman and then you can play file.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_open(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_close
*Prototype   : void  dsk_wkm_close( void )
*Arguments   : void
*Return      : void
*Description : close walkman.
*Other       :
****************************************************************************************************
*/
extern void  dsk_wkm_close(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_is_open
*Prototype   : __bool  dsk_wkm_is_open( void )
*Arguments   : void
*Return      : if walkman is open, return EPDK_TRUE; else return EPDK_FALSE.
*Description : check whether walkman is open or not.
*Other       :
****************************************************************************************************
*/
extern __bool  dsk_wkm_is_open(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_set_cb
*Prototype   : __pCBK_t dsk_wkm_set_cb( dsk_wkm_event_e event, __pCBK_t cb )
*Arguments   : event   input. event type.
*              cb      input. the callback function. NULL means unregister the callback function. 回
*                      调函数里面不能调用“dsk_wkm_..."相关的接口，以免死锁。
*Return      : the old callback function.
*Description : set callback functions for events.
*Other       :
****************************************************************************************************
*/
extern __pCBK_t dsk_wkm_set_cb(dsk_wkm_event_e event, __pCBK_t cb, void *ctx);

/*
****************************************************************************************************
*Name        : dsk_wkm_get_cb
*Prototype   : __pCBK_t dsk_wkm_get_cb( dsk_wkm_event_e event )
*Arguments   : event   input. event type.
*Return      : the callback function of a event.
*Description : get the callback function of a event.
*Other       :
****************************************************************************************************
*/
extern __pCBK_t dsk_wkm_get_cb(dsk_wkm_event_e event);

extern __s32 dsk_wkm_set_attr(void *attr);

extern void *dsk_wkm_get_attr(void);


#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_WALKMAN_H__
