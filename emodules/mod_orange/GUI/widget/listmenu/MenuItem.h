/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：        MenuItem.h
*
*Author：           William Wang
*
*Version :          1.1.0
*
*Date:              2008-12-13
*
*Description :      menuitem
*
*Others :           None at present.
*
* History :
*
* <Author>          <time>          <version>     <description>
*
* William Wang     2008-12-13         1.1.0          Create File
*
*******************************************************************************************************************
*/

#ifndef __MENU_ITEM__H____
#define __MENU_ITEM__H____

#include "config.h"
#include "Listmenu_e.h"

#define   LOAD_STS              1
#define   FLOAD_STS             2
#define   FFOUCS_STS            3
#define   FOUCS_STS             4
#define   UNFOUCS_STS           5
#define   FUNFOUCS_STS          6
#define   ACTIVED_STS           7
#define   INACTIVED_STS         8

#define   MENUITEM_MAXWIN       8

#define   GUI_ItemH2P(h_item)       ((MENUITEM_t *)h_item)
#define   GUI_ItemP2H(pitem)        ((H_ITEM)pitem)

typedef struct tag_ITEMCHILD
{
    H_WIN    hWnd;
    RECT     win_rect;
} ITEMCHILD_t;
//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------

typedef struct tag_MEMUITEM
{
    RECT                       rect;
    struct tag_MEMUITEM       *Prev;
    struct tag_MEMUITEM       *Next;
    __u32                      itemno;
    __u32                      foucs_w;
    __pITEM_DRAW_CB            cb;                         /* item draw回调函数                                          */
    __u32                      status;
    H_WIN                      handle;                     /*menuitem所在listmenu的句柄*/
    ITEMCHILD_t                child[MENUITEM_MAXWIN];     /*menuitem的子窗口句柄数组*/
    __u8                       nctrls;                      /*menuitem的子窗口数量*/
    void                      *attr;
} MENUITEM_t;

MENUITEM_t *MenuItem_Create(__rectsz_t *rectsz, __lstm_item_attr_t *item_attr, H_WIN handle);
__u32       MenuItem_Destroy(MENUITEM_t *menuitem);
MENUITEM_t *MenuItem_GetPrev(MENUITEM_t *menuitem);
MENUITEM_t *Menuitem_GetNext(MENUITEM_t *menuitem);

//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------

#endif //__MENU_ITEM__H____


