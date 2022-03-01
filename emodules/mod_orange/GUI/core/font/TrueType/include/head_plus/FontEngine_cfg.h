/*
************************************************************************************************************************
*                                                      FontEngine
*
*                             Copyright(C), 2006-2008, Allwinner Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : FontEngine_cfg.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2008.06.26
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
* Gary.Wang      2008.06.26       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __FontEngine_cfg_h
#define  __FontEngine_cfg_h


#define EPDK_AC320                   // EPDK_WIN32 or EPDK_AC320
//#define NDEBUG

#ifndef SIM_PC_WIN

#ifndef NDEBUG
#if defined( EPDK_WIN32 )
#define __msg                     printf
#else
//#include "epdk.h"
#endif
#else
#if defined( EPDK_WIN32 )
#define __msg(x)
#else
#define __msg(...)
#endif
#endif
#else
#define __msg(x)
#endif





#endif     //  ifndef __FontEngine_cfg_h

/* end of FontEngine_cfg.h  */
