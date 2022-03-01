/*
************************************************************************************************************************
*                                                       RESOURCE
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : res_cfg.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2008.05.19
*
* Description :
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2008.05.19       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __resdec_cfg_h
#define  __resdec_cfg_h


#define EPOS_PLATFORM                   // WIN32_PLATFORM or EPOS_PLATFORM
#define RES_IN_ELF_FILE                 // RES_IN_ELF_FILE  or  RES_IN_RES_FILE
//#define NDEBUG

#ifndef SIM_PC_WIN

#ifndef NDEBUG
#if defined( WIN32_PLATFORM )
#define __msg                     printf
#else
//#include "epdk.h"
#endif
#else
#define __msg(...)
#endif
#else
#define __msg
#endif

#if defined( WIN32_PLATFORM ) && defined( RES_IN_ELF_FILE )
#error this config " WIN32_PLATFORM && RES_IN_ELF_FILE " are illegal.
#endif

#endif     //  ifndef __res_cfg_h

/* end of res_cfg.h  */
