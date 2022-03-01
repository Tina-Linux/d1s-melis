/*
************************************************************************************************************************
*                                                        CUCKOO
*                                                   the Audio Player
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   :ResDecode.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2008.11.08
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
* Gary.Wang      2008.11.08       1.1.0        build the file
*
*/

#ifndef __COM_MEM_DEV_H_
#define __COM_MEM_DEV_H_

#include "beetles_app.h"

__s32 com_memdev_create(H_LYR hlyr);

__s32 com_memdev_create_ex(H_LYR hlyr, __s32 x, __s32 y, __s32 w, __s32 h);

__s32 com_memdev_delete(void);

#endif//__COM_PALETTE_H_
