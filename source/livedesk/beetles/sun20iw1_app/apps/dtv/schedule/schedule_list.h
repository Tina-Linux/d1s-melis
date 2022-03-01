/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      :
* By        :
* Func      :
* Version   : v1.0
* ============================================================================================================

**************************************************************************************************************
*/

#ifndef __SCHEDULE_LIST_H_
#define __SCHEDULE_LIST_H_

#include "schedule_i.h"

typedef struct tag_schedule_list_para
{
    GUI_FONT *font;

    H_LYR layer;
    __s32 focus_id;
    __s32 old_focus;

} schedule_list_para_t;




H_WIN schedule_list_win_create(H_WIN h_parent, schedule_list_para_t *list_para);
__s32 schedule_list_win_delete(H_WIN list_win);
#endif
