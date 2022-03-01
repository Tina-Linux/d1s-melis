/*
************************************************************************************************************************
*                                                       walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_walkman_i.h
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
#ifndef  __DSK_WALKMAN_I_H__
#define  __DSK_WALKMAN_I_H__



#include "engine_i.h"
#include "lib/dsk_wkm_lib.h"
#include "foundation/foundation.h"
#include "dsk_wkm_monitor.h"


#define FS_FILE_NAME_LEN_MAX            512
#define FS_FULL_PATH_LEN_MAX            768


#define ORCHID_FILE_PATH                "b:\\module\\orchid"
#define CEDAR_FILE_PATH                 BEETLES_APP_ROOT"mod\\cedar.mod"
#define WILLOW_FILE_PATH                BEETLES_APP_ROOT"mod\\willow\\willow.mod"


extern __mp       *volatile   dsk_wkm_horc;
extern HNPL         volatile    dsk_wkm_hnpl;
extern __mp         *volatile   dsk_wkm_hced;
extern g_Queue      volatile    dsk_wkm_cedar_msgQ;
extern g_mutex_t    volatile    dsk_wkm_res_mutex;

extern __pCBK_t     volatile    cb_auto_play_next;
extern void         *volatile   cb_ctx_auto_play_next;
extern __pCBK_t     volatile    cb_no_item_to_play;
extern void         *volatile   cb_ctx_no_item_to_play;


extern __bool       volatile    to_quit_dsk_wkm;

extern char                     now_playing_file[FS_FULL_PATH_LEN_MAX];

/*
****************************************************************************************************
*Name        : __dsk_wkm_orchid_open
*Prototype   : __s32 __dsk_wkm_orchid_open( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : open orchid
*Other       :
****************************************************************************************************
*/
extern __s32 __dsk_wkm_orchid_open(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_orchid_close
*Prototype   : void  __dsk_wkm_orchid_close( void )
*Arguments   : void
*Return      : void
*Description : close orchid
*Other       :
****************************************************************************************************
*/
extern void  __dsk_wkm_orchid_close(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_create
*Prototype   : __s32 __dsk_wkm_npl_create( orc_media_type_e media_type, orc_npl_work_mode_e  work_mode )
*Arguments   : media_type     input. media type, such as audio, video and so on
*              work_mode      input. work mode.
*Return      : ==0    succeed
*              !=0    fail
*Description : create now playlist
*Other       :
****************************************************************************************************
*/
extern __s32 __dsk_wkm_npl_create(orc_media_type_e  media_type, orc_npl_work_mode_e  work_mode);

/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_delete
*Prototype   : void  __dsk_wkm_npl_delete( void )
*Arguments   : void
*Return      : void
*Description : delete now playlist
*Other       :
****************************************************************************************************
*/
extern void  __dsk_wkm_npl_delete(void);


/*
****************************************************************************************************
*Name        : __dsk_wkm_cedar_open
*Prototype   : __s32 __dsk_wkm_cedar_open( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : open cedar
*Other       :
****************************************************************************************************
*/
extern __s32 __dsk_wkm_cedar_open(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_cedar_close
*Prototype   : void  __dsk_wkm_cedar_close( void )
*Arguments   : void
*Return      : void
*Description : close cedar
*Other       :
****************************************************************************************************
*/
extern void  __dsk_wkm_cedar_close(void);

/***************************************************************************************************
*Name        : __dsk_wkm_get_feedback_msgQ
*Prototype   : __s32  __dsk_wkm_get_feedback_msgQ( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : get the feedback message queue handle.
*Other       :
***************************************************************************************************/
extern __s32 __dsk_wkm_get_feedback_msgQ(void);

/***************************************************************************************************
*Name        : __dsk_wkm_res_mutex_create
*Prototype   : __s32 __dsk_wkm_res_mutex_create( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : create cedar mutex
*Other       :
***************************************************************************************************/
extern __s32 __dsk_wkm_res_mutex_create(void);

/***************************************************************************************************
*Name        : __dsk_wkm_res_mutex_delete
*Prototype   : void  __dsk_wkm_res_mutex_delete( void )
*Arguments   : void
*Return      : void
*Description : delete cedar mutex
*Other       :
***************************************************************************************************/
extern void  __dsk_wkm_res_mutex_delete(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_set_open_state
*Prototype   : void  __dsk_wkm_set_open_state( __bool is_open )
*Arguments   : is_open  input. the walkman's open state.
*Return      : void
*Description : record the open state.
*Other       :
****************************************************************************************************
*/
extern void  __dsk_wkm_set_open_state(__bool is_open);

/*
****************************************************************************************************
*Name        : __dsk_wkm_get_open_state
*Prototype   : __bool  __dsk_wkm_get_open_state( void )
*Arguments   : void
*Return      : the walkman's open state
*Description : get the open state.
*Other       :
****************************************************************************************************
*/
extern __bool  __dsk_wkm_get_open_state(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_set_playing_state
*Prototype   : void  __dsk_wkm_set_playing_state( __bool is_playing )
*Arguments   : is_playing  input. the walkman's play state.
*Return      : void
*Description : record the play state.
*Other       :
****************************************************************************************************
*/
extern void  __dsk_wkm_set_playing_state(__bool is_playing);

/*
****************************************************************************************************
*Name        : __dsk_wkm_set_playing_state
*Prototype   : void  __dsk_wkm_set_pause_state( __bool is_pause )
*Arguments   : is_playing  input. the walkman's play state.
*Return      : void
*Description : record the pause state.
*Other       :
****************************************************************************************************
*/
extern void  __dsk_wkm_set_pause_state(__bool is_pause);

/*
****************************************************************************************************
*Name        : __dsk_wkm_set_playing_state
*Prototype   : void  __dsk_wkm_get_pause_state( __bool is_pause )
*Arguments   :
*Return      : void
*Description : get the pause state.
*Other       :
****************************************************************************************************
*/
extern __bool  __dsk_wkm_get_pause_state(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_get_playing_state
*Prototype   : __bool  __dsk_wkm_get_playing_state( void )
*Arguments   : void
*Return      : the walkman's play state
*Description : get the play state.
*Other       :
****************************************************************************************************
*/
extern __bool  __dsk_wkm_get_playing_state(void);

extern __s32 __dsk_wkm_set_attr(void *attr);

extern void *__dsk_wkm_get_attr(void);

void  __dsk_wkm_set_npl_dirty_flag(__bool is_dirty);

__bool  __dsk_wkm_get_npl_dirty_flag(void);
#endif     //  ifndef __DSK_WALKMAN_I_H__

/* end of dsk_walkman_i.h */
