/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_i.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.24
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
* Gary.Wang      2009.09.24       1.1.0        build the file
*
************************************************************************************************************************
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
