/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º        txtlayer_process.h
*
*Author£º           William Wang
*
*Version :          1.1.0
*
*Date:              2008-12-18
*
*Description :      txtlayer process function definition
*
*Others :           None at present.
*
* History :
*
* <Author>          <time>          <version>     <description>
*
* William Wang     2008-12-18         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __TXTLAYER_PROCESS_H__
#define __TXTLAYER_PROCESS_H__

#include "txtcfg.h"
#include "txtbuffer.h"
#include "txtbox_i.h"
#include "txtlist.h"

__u32 TxtLayer_StampToPos(txtlayer_man_t *txtlayer, __u32 Stamp, __pos_t *pos);
__u32 TxtLayer_StampProcess(txtlayer_man_t *txtlayer, __u32 Stamp);
__u32 TxtLayer_DrawItem(txtlayer_man_t *txtlayer, txtitem_t   *txtitem);
__u32 TxtLayer_DrawMultiItem(txtlayer_man_t *txtlayer);
#endif
