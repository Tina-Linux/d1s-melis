/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º    ListBox_Process.h
*
*Author£º       William Wang
*
*Version :      1.1.0
*
*Date:          2009-7-4
*
*Description :  listbox process mode
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>       <version>     <description>
*
* William Wang     2009-7-4         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __LISTBOX_PROCESS_H____
#define __LISTBOX_PROCESS_H____


#include "config.h"
#include "ListBuffer.h"
#include "ListMenu_i.h"
#include "MenuList.h"

__u32  ListBox_GetBufferStatus(LIST_LAYER_MAN_t    *listlayer);
void   ListBox_LoadItemThread(void *p_arg);
__u32  ListBox_VarKeyUpMsg(LIST_LAYER_MAN_t    *listlayer, __u32 dist, __u32  step);
__u32  ListBox_VarKeyDownMsg(LIST_LAYER_MAN_t    *listlayer, __u32 dist, __u32 step);
__u32  ListBox_FixedKeyUpMsg(LIST_LAYER_MAN_t    *listlayer, __u32 dist, __u32  step);
__u32  ListBox_FixedKeyDownMsg(LIST_LAYER_MAN_t    *listlayer, __u32 dist, __u32 step);
__u32  ListBox_FlatVarMoveDownMsg(LIST_LAYER_MAN_t    *listlayer, __u32 dist);
__u32  ListBox_FlatVarMoveUpMsg(LIST_LAYER_MAN_t    *listlayer, __u32 dist);
__u32  ListBox_FlatFixedMoveUpMsg(LIST_LAYER_MAN_t    *listlayer, __u32 dist);
__u32  ListBox_FlatFixedMoveDownMsg(LIST_LAYER_MAN_t    *listlayer, __u32 dist);
__u32  ListBox_FlatMoveToDist(LIST_LAYER_MAN_t    *listlayer, __s32 dist);
__s32  ListBox_FlatMoveGetDist(LIST_LAYER_MAN_t    *listlayer);
__u32  ListLayer_DrawItemProcess(LIST_LAYER_MAN_t    *listlayer);
__s32  ListBox_GetMoveStatus(LIST_LAYER_MAN_t    *listlayer);
#endif
