/*
*******************************************************************************************************************
*                                                           Module Orange
*                                                   the embedded Graphical module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    sicons.h
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2010-4-11
*
*Description :  scene item definition
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>         <version>     <description>
*
* William Wang     2010-4-11         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __SICONS_H__
#define __SICONS_H__

#define CTRL_SICONS                 ("SICONS")

#define ICONS_TYPEMASK              0x0000000FL
#define ICONS_PUSHED                (1<<1)

#define ICONS_FOCUS                 (1<<2)

#define ICONSN_CLICKED              0x70
#define ICONSN_PUSHED               0x71
#define ICONSN_UNPUSHED             0x72
#define ICONSN_DISABLE              0x74
#define ICONSN_DOUBLECLICKED        0x75

#define ICONSN_DBLCLK               IN_DOUBLECLICKED
#define ICONSN_SETFOCUS             0x76
#define ICONSN_KILLFOCUS            0x77

#define LISTITEM_DRAWMODE_UNFCS       (0)
#define LISTITEM_DRAWMODE_FCS         (1)
#define LISTITEM_DRAWMODE_DISABLE     (2)
#define LISTITEM_DRAWMODE_FREE        (3)

#define LISTITEM_DRAWCB_ERROR         0xffff
#define LISTITEM_DRAWCB_OK            0

typedef  __hdle h_iconsitem;

typedef struct tagICONSITEMDRAWPARA
{
    __u32                         mode;
    h_iconsitem                 hwin;
    RECT                          rect;
    __bool                        memdev_flags;
    void                          *attr;
} __iconsitemdraw_para_t;

typedef __s32(*IconsItem_DrawCB)(__iconsitemdraw_para_t *drawcb_para);

typedef struct __ICONS_ITEM_ATTR
{
    __u32                          item_w;              /* item的宽度                                                 */
    IconsItem_DrawCB                cb;                  /* item draw回调函数                                          */
    void                          *p_arg;               /* 用户自定义的数据内容                                       */
    __bool                         enable;
} __sicons_item_attr_t;

typedef struct __ICONS_ATTR
{
    __u32                          nitem;              /* item初始数目                                                 */
    __sicons_item_attr_t          *items;              /* item数组的指针                                               */
    __u32                          bk_color;
    void                          *bk_bmp;
    __bool                         bk_draw;
    __u32                          curitemno;          /*初始化焦点item编号*/
    __u32                          headitemno;
} __sicons_para_t;

typedef struct tag__SICONS_ATTR
{
    IconsItem_DrawCB             draw_cb;
    void                          *para;
} __sicons_attr_t;

__s32               SIcons_UpdateMenulist(H_WIN  hicons, __sicons_para_t *para);
__u32               SIcons_GetItemNo(h_iconsitem H_Item);
__u32               SIcons_GetItemAttr(h_iconsitem H_Item);
H_WIN               SIcons_GetHostWin(h_iconsitem H_Item);
H_LYR               SIcons_GetItemLyrWin(h_iconsitem H_Item);
__u32               SIcons_GetItemCount(H_WIN  icons);
h_iconsitem         SIcons_GetItem(H_WIN  hicons, __u32 no);
h_iconsitem         SIcons_GetCurItem(H_WIN  icons);
__s32               SIcons_PrevPage(H_WIN  hicons);
__s32               SIcons_NextPage(H_WIN  hicons);
__s32               SIcons_ItemDisable(H_WIN  hicons, __u32 index);
__s32               SIcons_ItemEnable(H_WIN  hicons, __u32 index);
__s32               SIcons_GetHeadindex(H_WIN  hicons);
__s32               SIcons_DelCurItem(H_WIN  hicons);
__s32               SIcons_AddTail(H_WIN  hicons, __sicons_item_attr_t *item_para);
__s32               SIcons_SetCurItem(H_WIN  hicons, __u32 cur_index);
__u32               SIcons_Disable(H_WIN  hicons);
__s32               SIcons_GetItemRect(h_iconsitem hitem, RECT *rect);
__bool              RegisterIconsControl(void);
__bool              UnRegisterIconsControl(void);
#endif

