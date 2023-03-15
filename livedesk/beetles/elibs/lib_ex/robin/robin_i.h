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
#ifndef  __robin_i_h
#define  __robin_i_h

#include "apps.h"

#include "lib/robin_lib.h"
#include "foundation/foundation.h"
#include "lib_ex.h"



#define AUDIO_DRV_FILE_PATH             "b:\\AUDIO\\CTRL"
#define DISPLAY_DRV_FILE_PATH           "b:\\DISP\\DISPLAY"
//#define ORCHID_FILE_PATH                "b:\\module\\orchid"
#define CEDAR_FILE_PATH                 BEETLES_APP_ROOT"mod\\cedar.mod"
#define WILLOW_FILE_PATH                BEETLES_APP_ROOT"mod\\willow\\willow.mod"


//#define KARAOKE_CEDAR_FILE_PATH         "f:\\rootfs\\mod\\cedar.mod"  //chengf  add  for  karaoke

//extern G_FILE  *volatile  robin_horc;
//extern HNPL     volatile  robin_hnpl;
extern HRATNPL    volatile  rat_hnpl;
extern __mp    *volatile  robin_hced;
extern g_Queue  volatile  robin_cedar_msgQ;
extern __mp    *volatile  robin_hdis;
extern __mp    *volatile  robin_hwlo;

extern __bool   volatile  to_quit_robin;


/***************************************************************************************************
*Name        : orchid_open
*Prototype   : __s32 orchid_open( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : open orchid
*Other       :
***************************************************************************************************/
extern __s32 orchid_open(void);

/***************************************************************************************************
*Name        : orchid_close
*Prototype   : void  orchid_close( void )
*Arguments   : void
*Return      : void
*Description : close orchid
*Other       :
***************************************************************************************************/
extern void  orchid_close(void);

/***************************************************************************************************
*Name        : npl_create
*Prototype   : __s32 npl_create( orc_media_type_e media_type, orc_npl_work_mode_e  work_mode )
*Arguments   : media_type     input. media type, such as audio, video and so on
*              work_mode      input. work mode.
*Return      : ==0    succeed
*              !=0    fail
*Description : create now playlist
*Other       :
***************************************************************************************************/
extern __s32 npl_create(rat_media_type_t  media_type);

/***************************************************************************************************
*Name        : npl_delete
*Prototype   : void  npl_delete( void )
*Arguments   : void
*Return      : void
*Description : delete now playlist
*Other       :
***************************************************************************************************/
extern void  npl_delete(void);


/***************************************************************************************************
*Name        : cedar_open
*Prototype   : __s32 cedar_open( void )
*Arguments   : reserve_mem_size memory size that cedar reserve for app
*Return      : ==0    succeed
*              !=0    fail
*Description : open cedar
*Other       :
***************************************************************************************************/
extern __s32 cedar_open(robin_open_arg_t *arg_p);

/***************************************************************************************************
*Name        : cedar_close
*Prototype   : void  cedar_close( void )
*Arguments   : void
*Return      : void
*Description : close cedar
*Other       :
***************************************************************************************************/
extern void  cedar_close(void);

/***************************************************************************************************
*Name        : get_feedback_msgQ
*Prototype   : __s32  get_feedback_msgQ( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : get the feedback message queue handle.
*Other       :
***************************************************************************************************/
extern __s32 get_feedback_msgQ(void);

/***************************************************************************************************
*Name        : disp_open
*Prototype   : __s32 disp_open( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : open display driver
*Other       :
***************************************************************************************************/
extern __s32 disp_open(void);

/***************************************************************************************************
*Name        : disp_close
*Prototype   : void  disp_close( void )
*Arguments   : void
*Return      : void
*Description : close display driver
*Other       :
***************************************************************************************************/
extern void  disp_close(void);

/***************************************************************************************************
*Name        : willow_open
*Prototype   : __s32 willow_open( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : open willow
*Other       :
***************************************************************************************************/
extern __s32 willow_open(void);

/***************************************************************************************************
*Name        : willow_close
*Prototype   : void  willow_close( void )
*Arguments   : void
*Return      : void
*Description : close willow
*Other       :
***************************************************************************************************/
extern void  willow_close(void);




#endif     //  ifndef __robin_i_h

/* end of robin_i.h */
