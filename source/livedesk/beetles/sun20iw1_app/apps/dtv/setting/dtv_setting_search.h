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

#ifndef __dtv_SETTING_search_H_
#define __dtv_SETTING_search_H_

#include "dtv_setting_i.h"

typedef struct tag_dtv_setting_search_para
{
    GUI_FONT *font;

    H_LYR layer;
    __s32 focus_id;
    __s32 old_focus;

} dtv_setting_search_para_t;




H_WIN dtv_setting_search_win_create(H_WIN h_parent, dtv_setting_search_para_t *search_para);
__s32 dtv_setting_search_win_delete(H_WIN search_win);
#endif
