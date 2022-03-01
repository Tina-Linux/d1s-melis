/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º    ListBox.h
*
*Author£º       William Wang
*
*Version :      1.1.0
*
*Date:          2009-7-4
*
*Description :  listbox function definition
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>        <version>     <description>
*
* William Wang     2009-7-4         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __LISTBOX__H____
#define __LISTBOX__H____

#include "config.h"
#include "ListBuffer.h"
#include "MenuList.h"

__s32  ListLayer_HeadToTailCopy(LIST_LAYER_MAN_t *listlayer);
__s32  ListLayer_TailToHeadCopy(LIST_LAYER_MAN_t *listlayer);
__u32  ListLayer_BoxDelCurItem(LIST_LAYER_MAN_t *listlayer);
__u32  ListLayer_BoxAddItemTail(LIST_LAYER_MAN_t *listlayer, __lstm_item_attr_t *NewItem);
__u32  ListLayer_VarBoxOnDraw(LIST_LAYER_MAN_t *listlayer);
__u32  ListLayer_FixedBoxOnDraw(LIST_LAYER_MAN_t *listlayer);
__u32  ListLayer_VarNextPage(LIST_LAYER_MAN_t *listlayer);
__u32  ListLayer_VarPrevPage(LIST_LAYER_MAN_t *listlayer);
#endif //__LIST_LAYER__H____





