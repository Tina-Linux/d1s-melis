/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º        txtitem.c
*
*Author£º           William Wang
*
*Version :          1.1.0
*
*Date:              2008-12-17
*
*Description :      txtbox item function implement
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
#ifndef __TXTITEM_C__
#define __TXTITEM_C__

#include "txtitem.h"

txtitem_t  *txtitem_Create(__rect_t   *rectsz, __txtbox_item_attr_t *item_attr)
{
    txtitem_t *newitem;

    if (rectsz == NULL || item_attr == NULL)
    {
        return NULL;
    }

    newitem               = (txtitem_t *)TXTBOX_MALLOC(sizeof(txtitem_t));

    if (!newitem)
    {
        return NULL;
    }

    newitem->rect.x       = rectsz->x;
    newitem->rect.y       = rectsz->y;
    newitem->rect.width   = rectsz->width;
    newitem->rect.height  = rectsz->height;
    eLIBs_strncpy(newitem->string, item_attr->buffer, MAX_STRLEN);
    newitem->length       = eLIBs_strlen(item_attr->buffer);
    newitem->Next         = NULL;
    newitem->Prev         = NULL;
    newitem->stamp        = item_attr->pstamp;
    newitem->estamp       = item_attr->estamp;
    /*set menuitem lion handle*/
    newitem->itemno       = 0;
    newitem->status       = 0;
    return newitem;
}


__u32    txtitem_Destroy(txtitem_t *txtitem)
{
    if (txtitem == NULL)
    {
        return TXTBOX_OK;
    }
    else
    {
        TXTBOX_MEMFREE((txtitem_t *)txtitem);
        return TXTBOX_OK;
    }
}
#endif
