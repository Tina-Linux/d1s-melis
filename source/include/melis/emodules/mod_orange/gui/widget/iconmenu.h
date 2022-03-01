/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              Apps Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : gui widget
* File    : iconmenu.h
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-4 11:28:56
**********************************************************************************************************************
*/
#ifndef __GUI_ICONMENU_H__
#define __GUI_ICONMENU_H__

/* icon(argb)                                       */
typedef struct tag__GUI_ICNM_ICON
{
    SIZE            size;                           /* icon 的大小                                                  */
    void           *argbbuf;                        /* icon 的argb buffer的地址                                     */
} __gui_icnm_icon_t;

/* attr for create a item during icnm creating      */
typedef struct tag__GUI_ICNM_ITEMCREATE_ATTR
{
    char                *name;                      /* item name                                                    */
    char                 title[GUI_TITLE_MAX];
    __gui_icnm_icon_t   icon[8];                   /* 原始图有8个动画                                              */
} __gui_icnm_itemcreate_attr_t;

/* attr for create a icon menu                      */
typedef struct tag__GUI_ICNM_ATTR
{
    __u32                            nitem;         /* item初始数目                                                 */
    __u32                            curitem;
    __gui_icnm_itemcreate_attr_t   *items;         /* item数组的指针                                               */

} __gui_icnm_attr_t;

/* para for create a icon menu                      */
typedef struct tag__GUI_ICNM_CREATE_PARA
{
    char                *name;

    RECT                 rect;
    H_LYR                h_lyr;
    __pGUI_WIN_CB       cb;

    __gui_icnm_attr_t *attr;

} __gui_icnm_create_para_t;


//extern H_WIN GUI_IcnmCreate  (__gui_icnm_create_para_t * p_para, H_WIN h_parent);
//extern __u8   GUI_IcnmGetCurItemIndex(H_WIN h_icnm);

#endif /* #ifndef __GUI_ICONMENU_H__ */

