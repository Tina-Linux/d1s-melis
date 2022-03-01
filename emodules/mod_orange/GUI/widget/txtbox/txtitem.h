/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º        txtitem.h
*
*Author£º           William Wang
*
*Version :          1.1.0
*
*Date:              2008-12-17
*
*Description :      txtbox item definition
*
*Others :           None at present.
*
* History :
*
* <Author>          <time>          <version>     <description>
*
* William Wang     2008-12-17         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __TXTITEM_H__
#define __TXTITEM_H__

#include "txtbox_e.h"
#define MAX_STRLEN 100
typedef struct tag_TXTITEM
{
    RECT                       rect;
    struct tag_TXTITEM        *Prev;
    struct tag_TXTITEM        *Next;
    char                       string[MAX_STRLEN];
    __pos_t                    validpos;
    __u32                      length;
    __u32                      font_len;
    __u32                      itemno;
    __u32                      stamp;
    __u32                      estamp;
    __u32                      status;
} txtitem_t;


txtitem_t  *txtitem_Create(__rect_t   *rectsz, __txtbox_item_attr_t *item_attr);
__u32       txtitem_Destroy(txtitem_t *txtitem);

#endif
