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
