/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_fsm_ctrl.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.25
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
* Gary.Wang      2009.09.25       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_fsm_ctrl_h
#define  __robin_fsm_ctrl_h
#include <kconfig.h>

typedef  long  feedback_msg_t;


/***************************************************************************************************
*Name        : robin_set_cmd_play_file
*Prototype   : __s32  robin_set_cmd_play_file( const char *file, const __cedar_tag_inf_t *tag_info_p )
*Arguments   : file           input. the file to be played.
*              tag_info_p     input. the tag info of the file. If there is no tag info, input should
*                             be NULL.
*Return      : ==0       succeed
*              !=0       fail
*Description : set play a file cmd. This operation will send the "play file" command to FSM but can't
*              enable FSM to be in "play" status immediately, in other words, this is an asynchronous
*              operation.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_cmd_play_file(const char *file, const __cedar_tag_inf_t *tag_info_p);

/***************************************************************************************************
*Name        : robin_play_file
*Prototype   : __s32  robin_play_file( const char *file, const __cedar_tag_inf_t *tag_info_p )
*Arguments   : file           input. the file to be played.
*              tag_info_p     input. the tag info of the file. If there is no tag info, input should
*                             be NULL.
*Return      : ==0       succeed
*              !=0       fail
*Description :
*              this is an synchronous
*              operation.
*Other       :
***************************************************************************************************/
extern __s32 robin_play_file(const char *file, const __cedar_tag_inf_t *tag_info_p);

/***************************************************************************************************
*Name        : robin_set_cmd_play
*Prototype   : __s32  robin_set_cmd_play( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set play cmd. This operation will send the "play" command to FSM and wait the FSM
*              to be in "play" status.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_cmd_play(void);

/***************************************************************************************************
*Name        : robin_set_cmd_stop
*Prototype   : __s32  robin_set_cmd_stop( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set stop cmd. This operation will send the "stop" command to FSM and wait the FSM
*              to be in "stop" status.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_cmd_stop(void);

/***************************************************************************************************
*Name        : robin_set_cmd_pause
*Prototype   : __s32  robin_set_cmd_pause( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set pause cmd. This operation will send the "pause" command to FSM and wait the FSM
*              to be in "pause" status.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_cmd_pause(void);

/***************************************************************************************************
*Name        : robin_set_cmd_ff
*Prototype   : __s32  robin_set_cmd_ff( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set ff cmd. This operation will send the "ff" command to FSM and wait the FSM
*              to be in "ff" status.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_cmd_ff(void);

/***************************************************************************************************
*Name        : robin_set_cmd_rr
*Prototype   : __s32  robin_set_cmd_rr( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set rr cmd. This operation will send the "rr" command to FSM and wait the FSM
*              to be in "rr" status.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_cmd_rr(void);

/***************************************************************************************************
*Name        : robin_set_cmd_jump
*Prototype   : __s32  robin_set_cmd_jump( __u32 time )
*Arguments   : time      time. unit : ms.
*Return      : ==0       succeed
*              !=0       fail
*Description : set jump cmd. This operation will send the "jump" command to FSM.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_cmd_jump(__u32 time);

/***************************************************************************************************
*Name        : robin_get_fsm_status
*Prototype   : __cedar_status_t  robin_get_fsm_status( void )
*Arguments   : void
*Return      : the current status of the fsm, such as "play", "pause" and so on. The status are
*              defined in mod_cedar.h, see __cedar_status_t.
*Description : get the current status of the fsm.
*Other       :
***************************************************************************************************/
extern __cedar_status_t  robin_get_fsm_status(void);

/***************************************************************************************************
*Name        : robin_clear_feedback_msgQ
*Prototype   : void   robin_clear_feedback_msgQ( void )
*Arguments   : void
*Return      : void
*Description : There is a message queue created by FSM, that offers instant error message. This
*              operation can clear the queue.
*Other       :
***************************************************************************************************/
extern void   robin_clear_feedback_msgQ(void);

/***************************************************************************************************
*Name        : robin_get_feedback_msg
*Prototype   : feedback_msg_t  robin_get_feedback_msg( void )
*Arguments   : void
*Return      : error message. 0 means no message at present, while other values are the error
*              messages. The error messages are defined in mod_cedar.h, see __cedar_feedback_msg_t.
*Description : There is a message queue created by FSM, that offers instant error message. This
*              operation can get the instant error message .
*Other       :
***************************************************************************************************/
extern feedback_msg_t  robin_get_feedback_msg(void);

/***************************************************************************************************
*Name        : robin_get_fsm_cur_file
*Prototype   : __s32  robin_get_fsm_cur_file( char *file, __u32 size )
*Arguments   : file      output. a buffer to store the fsm's current file.
*              size      input. The size of the buffer.
*Return      : ==0       succeed.
*              !=0       no file.
*Description : get the fsm's current file.
*Other       :
***************************************************************************************************/
extern __s32  robin_get_fsm_cur_file(char *file, __u32 size);

/***************************************************************************************************
*Name        : robin_fsm_is_busy
*Prototype   : __bool robin_fsm_is_busy( void );
*Arguments   : void
*Return      : EPDK_TRUE   the fsm is busy.
*              EPDK_FALSE  the fsm is not busy.
*Description : check whether the fsm is busy or not.
*Other       :
***************************************************************************************************/
extern __bool robin_fsm_is_busy(void);

extern __s32 robin_play_aux_file(const char *file, __cedar_play_aux_wav_mode_t mode);

#endif     //  ifndef __robin_fsm_ctrl_h

/* end of robin_fsm_ctrl.h */
