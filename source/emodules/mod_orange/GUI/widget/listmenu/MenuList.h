
#ifndef __MENU_LIST__H____
#define __MENU_LIST__H____

#include "config.h"
#include "MenuItem.h"

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
typedef struct tag_MENULIST
{
    __bool             setCurItem;

    /*last current item*/
    MENUITEM_t        *UnCurItem;
    MENUITEM_t        *head;
    MENUITEM_t        *tail;
    MENUITEM_t        *load_head;
    MENUITEM_t        *load_tail;
    MENUITEM_t        *SelItem;
    MENUITEM_t        *UnSelItem;
    __u32              ItemCount;
    __u32              bpp;
    __s32              dist;        //用来判断移动的距离和selItem之间的关系
    __s32              lyrdist;
    __u32              total_dist;
    MENUITEM_t        *CurItem;

    /*only used in listbox mod*/
    MENUITEM_t        *HeadItem;
} MENULIST_t;


MENULIST_t     *MenuList_Create(void);
__u32           MenuList_InsertAfter(MENULIST_t *menulist, MENUITEM_t *after, MENUITEM_t *menuitem);
__u32           MenuList_DelItem(MENULIST_t *menulist, MENUITEM_t *menuitem);
__u32           MenuList_DelAll(MENULIST_t *menulist);
__u32           MenuList_AddTail(MENULIST_t *menulist,  MENUITEM_t *menuitem);
__u32           MenuList_AddHead(MENULIST_t *menulist, MENUITEM_t *menuitem);
MENUITEM_t     *MenuList_GetHead(MENULIST_t *menulist);
H_WIN           MenuList_GetCurItem(MENULIST_t *menulist);
__u32           MenuList_SetCurItem(MENULIST_t *menulist, MENUITEM_t *menuitem);
MENUITEM_t     *MenuList_GetTail(MENULIST_t *menulist);
__u32           MenuList_GetItemCount(MENULIST_t *menulist);
__u32           MenuList_Destroy(MENULIST_t *menulist);
#endif //__MENU_LIST__H____


