/*
*******************************************************************************************************************
*                                                           Module Orange
*                                                   the embedded Graphical module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    slistbox.h
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
#ifndef __SLISTBOX_H__
#define __SLISTBOX_H__

#define CTRL_SLISTBOX                 ("SLISTBOX")

#define LISTBOX_TYPEMASK              0x0000000FL
#define LISTBOX_PUSHED                (1<<1)

#define LISTBOX_FOCUS                 (1<<2)

#define LISTBOXN_CLICKED              0x70
#define LISTBOXN_PUSHED               0x71
#define LISTBOXN_UNPUSHED             0x72
#define LISTBOXN_DISABLE              0x74
#define LISTBOXN_DOUBLECLICKED        0x75

#define LISTBOXN_DBLCLK               IN_DOUBLECLICKED
#define LISTBOXN_SETFOCUS             0x76
#define LISTBOXN_KILLFOCUS            0x77

#define LISTITEM_DRAWMODE_UNFCS       (0)
#define LISTITEM_DRAWMODE_FCS         (1)
#define LISTITEM_DRAWMODE_DISABLE     (2)
#define LISTITEM_DRAWMODE_FREE        (3)

#define LISTITEM_DRAWCB_ERROR         0xffff
#define LISTITEM_DRAWCB_OK            0

typedef  __hdle h_listboxitem;

typedef struct tagLISTITEMDRAWPARA
{
    __u32                         mode;
    h_listboxitem                 hwin;
    RECT                          rect;
    __bool                        memdev_flags;
    void                          *attr;
} __listitemdraw_para_t;

typedef __s32(*ListboxItem_DrawCB)(__listitemdraw_para_t *drawcb_para);

typedef struct __LSTBOX_ITEM_ATTR
{
    __u32                          item_w;              /* item的宽度                                                 */
    ListboxItem_DrawCB             cb;                  /* item draw回调函数                                          */
    void                          *p_arg;               /* 用户自定义的数据内容                                       */
    __bool                         enable;
} __slistbox_item_attr_t;

typedef struct __LSTBOX_ATTR
{
    __u32                          nitem;              /* item初始数目                                                 */
    __slistbox_item_attr_t        *items;              /* item数组的指针                                               */
    __u32                          bk_color;
    void                          *bk_bmp;
    __bool                         bk_draw;
    __u32                          curitemno;          /*初始化焦点item编号*/
    __u32                          headitemno;
} __slistbox_para_t;

typedef struct tag__SLISTBOX_ATTR
{
    ListboxItem_DrawCB             draw_cb;
    void                          *para;
} __slistbox_attr_t;

__s32               SListbox_UpdateMenulist(H_WIN  hlistbox, __slistbox_para_t *para);
__u32               SListbox_GetItemNo(h_listboxitem H_Item);
unsigned long       SListbox_GetItemAttr(h_listboxitem H_Item);
H_WIN               SListbox_GetHostWin(h_listboxitem H_Item);
H_LYR               SListbox_GetItemLyrWin(h_listboxitem H_Item);
__u32               SListbox_GetItemCount(H_WIN  listbox);
h_listboxitem       SListbox_GetItem(H_WIN  hlistbox, __u32 no);
h_listboxitem       SListbox_GetCurItem(H_WIN  listbox);
__s32               SListbox_PrevPage(H_WIN  hlistbox);
__s32               SListbox_NextPage(H_WIN  hlistbox);
__s32               SListbox_ItemDisable(H_WIN  hlistbox, __u32 index);
__s32               SListbox_ItemEnable(H_WIN  hlistbox, __u32 index);
__s32               SListbox_GetHeadindex(H_WIN  hlistbox);
__s32               SListbox_DelCurItem(H_WIN  hlistbox);
__s32               SListbox_AddTail(H_WIN  hlistbox, __slistbox_item_attr_t *item_para);
__s32               SListbox_SetCurItem(H_WIN  hlistbox, __u32 cur_index);
__u32               SListbox_Disable(H_WIN  hlistbox);
__s32               SListbox_GetItemRect(h_listboxitem hitem, RECT *rect);
__bool              RegisterListBoxControl(void);
__bool              UnRegisterListBoxControl(void);
#endif

