/*
************************************************************************************************************************
*                                                       walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_fsm_ctrl.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.06
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
* Gary.Wang      2010.10.06        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_WKM_FSM_CTRL_H__
#define  __DSK_WKM_FSM_CTRL_H__

#ifdef __cplusplus
extern "C" {
#endif



/*
****************************************************************************************************
*Name        : dsk_wkm_play_file
*Prototype   : __s32 dsk_wkm_play_file( const char *file )
*Arguments   : file    input. the file to be played. full path.
*Return      : ==0     succeed
*              !=0     fail
*Description : play a file, and then the walkman is in "playing" status.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_wkm_play_file(const char *file);

extern __s32 dsk_wkm_play_next(void);

extern __s32 dsk_wkm_play_prev(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_stop
*Prototype   : __s32 dsk_wkm_stop( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : stop playing, and then the walkman is in "NOT playing" status.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_wkm_stop(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_pause
*Prototype   : __s32 dsk_wkm_pause( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : pause playing
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_wkm_pause(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_resume_play
*Prototype   : __s32 dsk_wkm_resume_play( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : resume playing
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_wkm_resume_play(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_ff
*Prototype   : __s32 dsk_wkm_ff( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : start ff
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_wkm_ff(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_rr
*Prototype   : __s32 dsk_wkm_rr( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : start rr
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_wkm_rr(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_jump
*Prototype   : __s32 dsk_wkm_jump( __u32 time )
*Arguments   : time    input. time, unit:ms.
*Return      : ==0     succeed
*              !=0     fail
*Description : start jump
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_wkm_jump(__u32 time);

/*
****************************************************************************************************
*Name        : dsk_wkm_get_fsm_state
*Prototype   : __cedar_status_t dsk_wkm_get_fsm_state( void )
*Arguments   : void
*Return      : the status of the walkman's finite status machine.
*Description : get the status of the walkman's finite status machine.
*Other       :
****************************************************************************************************
*/
extern __cedar_status_t dsk_wkm_get_fsm_state(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_is_playing
*Prototype   : __bool dsk_wkm_is_playing( void )
*Arguments   : void
*Return      : if some file is being played, return EPDK_TRUE; else return EPDK_FALSE.
*Description : check whether the desktop walkman is playing or not.
*Other       :
****************************************************************************************************
*/
extern __bool dsk_wkm_is_playing(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_is_playing
*Prototype   : __bool dsk_wkm_get_pause_state( void )
*Arguments   : void
*Return      : if some file is being pause, return EPDK_TRUE; else return EPDK_FALSE.
*Description :
*Other       :
****************************************************************************************************
*/
extern __bool dsk_wkm_get_pause_state(void);
/***************************************************************************************************
*Name        : dsk_wkm_clear_feedback_msgQ
*Prototype   : void   dsk_wkm_clear_feedback_msgQ( void )
*Arguments   : void
*Return      : void
*Description : There is a message queue created by FSM, that offers instant error message. This
*              operation can clear the queue.
*Other       :
***************************************************************************************************/
extern void   dsk_wkm_clear_feedback_msgQ(void);

/***************************************************************************************************
*Name        : dsk_wkm_get_feedback_msg
*Prototype   : feedback_msg_t  dsk_wkm_get_feedback_msg( void )
*Arguments   : void
*Return      : error message. 0 means no message at present, while other values are the error
*              messages. The error messages are defined in mod_cedar.h, see __cedar_feedback_msg_t.
*Description : There is a message queue created by FSM, that offers instant error message. This
*              operation can get the instant error message .
*Other       :
***************************************************************************************************/
extern feedback_msg_t  dsk_wkm_get_feedback_msg(void);



#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_WKM_FSM_CTRL_H__

/* end of dsk_wkm_fsm_ctrl.h */
