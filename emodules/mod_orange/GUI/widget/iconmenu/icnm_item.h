/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lion
* File    : icnm_item.h
*
* By      : Steven
* Version : v1.0
* Date    : 2008-12-1 9:05:40
**********************************************************************************************************************
*/
#ifndef _LION_ICNM_ICON_H_
#define _LION_ICNM_ICON_H_


typedef struct tag__LION_ICNM_ITEMCREATE_PARA
{
    char                           *name;           /* item name                                                    */
    char                           *title;
    RECT                            rect;
    H_LYR                           h_lyr;
    __pLION_WIN_CB                  cb;
    __lion_icnm_itemcreate_attr_t *attr;
    __u32                           index;

} __lion_icnm_itemcreate_para_t;


typedef struct tag__LION_ICNM_ITEM
{
    char                  name[LION_NAME_MAX];      /* item name                                                    */

    char                  title[LION_TITLE_MAX];

    __lion_painmod_t      mode;

    __u32                 drawflag;

    __u32                 index;

    __s32                 current;                  /* 当前图编号                                                   */
    __s32                 step;                     /* 需要跟随的步数                                               */
    __s32                 start;                    /* 起始步                                                       */
    __s32                 end;                      /* 目标图标号                                                   */

    __lion_icnm_icon_t    icon[8];                  /* 原始图有8个动画                                              */
    __lion_icnm_icon_t    s_icon[16];               /* bmp[0]有16种变化                                             */

} __lion_icnm_item_t;

extern __lion_win_t *ICNM_item_create(__lion_icnm_itemcreate_para_t *p_para, H_WIN h_parent);
extern __s32          ICNM_item_delete(H_WIN h_icon);

extern __s32 ICNM_prescale_icon(__lion_win_t *p_icon, __u8 iconindex);
extern __s32 ICNM_prescale_icons(__lion_win_t *p_icon);

#endif /* #ifndef _LION_ICNM_ICON_H_ */
