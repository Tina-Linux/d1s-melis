#ifndef __LISTLAYER_ITEM_H__
#define __LISTLAYER_ITEM_H__

#include "config.h"
#include "ListBuffer.h"
#include "ListMenu_i.h"
#include "MenuList.h"
#include "ListLayer_Process.h"

__u32  ListLayer_DelCurItem(LIST_LAYER_MAN_t    *listlayer);
__u32  ListLayer_AddItemTail(LIST_LAYER_MAN_t *listlayer, __lstm_item_attr_t *NewItem);
#endif
