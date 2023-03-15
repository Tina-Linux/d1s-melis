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
#ifndef  __DSK_WKM_FSM_CTRL_I_H__
#define  __DSK_WKM_FSM_CTRL_I_H__

#ifdef __cplusplus
extern "C" {
#endif


/*
****************************************************************************************************
*Name        : __dsk_wkm_play_file
*Prototype   : __s32 __dsk_wkm_play_file( const char *file )
*Arguments   : file    input. the file to be played. full path.
*Return      : ==0     succeed
*              !=0     fail
*Description : play a file, and then the walkman is in "playing" status.
*Other       :
****************************************************************************************************
*/
extern __s32 __dsk_wkm_play_file(const char *file);

/*
****************************************************************************************************
*Name        : __dsk_wkm_stop
*Prototype   : __s32 __dsk_wkm_stop( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : stop playing, and then the walkman is in "NOT playing" status.
*Other       :
****************************************************************************************************
*/
extern __s32 __dsk_wkm_stop(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_pause
*Prototype   : __s32 __dsk_wkm_pause( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : pause playing
*Other       :
****************************************************************************************************
*/
extern __s32 __dsk_wkm_pause(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_resume_play
*Prototype   : __s32 __dsk_wkm_resume_play( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : resume playing
*Other       :
****************************************************************************************************
*/
extern __s32 __dsk_wkm_resume_play(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_ff
*Prototype   : __s32 __dsk_wkm_ff( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : start ff
*Other       :
****************************************************************************************************
*/
extern __s32 __dsk_wkm_ff(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_rr
*Prototype   : __s32 __dsk_wkm_rr( void )
*Arguments   : void
*Return      : ==0     succeed
*              !=0     fail
*Description : start rr
*Other       :
****************************************************************************************************
*/
extern __s32 __dsk_wkm_rr(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_jump
*Prototype   : __s32 __dsk_wkm_jump( __u32 time )
*Arguments   : time    input. time, unit:ms.
*Return      : ==0     succeed
*              !=0     fail
*Description : start jump
*Other       :
****************************************************************************************************
*/
extern __s32 __dsk_wkm_jump(__u32 time);

/*
****************************************************************************************************
*Name        : __dsk_wkm_get_fsm_state
*Prototype   : __cedar_status_t __dsk_wkm_get_fsm_state( void )
*Arguments   : void
*Return      : the status of the walkman's finite status machine.
*Description : get the status of the walkman's finite status machine.
*Other       :
****************************************************************************************************
*/
extern __cedar_status_t __dsk_wkm_get_fsm_state(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_is_playing
*Prototype   : __bool __dsk_wkm_is_playing( void )
*Arguments   : void
*Return      : if some file is being played, return EPDK_TRUE; else return EPDK_FALSE.
*Description : check whether the desktop walkman is playing or not.
*Other       :
****************************************************************************************************
*/
extern __bool __dsk_wkm_is_playing(void);

/***************************************************************************************************
*Name        : __dsk_wkm_clear_feedback_msgQ
*Prototype   : void   __dsk_wkm_clear_feedback_msgQ( void )
*Arguments   : void
*Return      : void
*Description : There is a message queue created by FSM, that offers instant error message. This
*              operation can clear the queue.
*Other       :
***************************************************************************************************/
extern void   __dsk_wkm_clear_feedback_msgQ(void);

/***************************************************************************************************
*Name        : __dsk_wkm_get_feedback_msg
*Prototype   : feedback_msg_t  __dsk_wkm_get_feedback_msg( void )
*Arguments   : void
*Return      : error message. 0 means no message at present, while other values are the error
*              messages. The error messages are defined in mod_cedar.h, see __cedar_feedback_msg_t.
*Description : There is a message queue created by FSM, that offers instant error message. This
*              operation can get the instant error message .
*Other       :
***************************************************************************************************/
extern feedback_msg_t  __dsk_wkm_get_feedback_msg(void);



#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_WKM_FSM_CTRL_I_H__

/* end of dsk_wkm_fsm_ctrl_i.h */
