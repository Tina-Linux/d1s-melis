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

#ifndef __dtv_SETTING_antenna_H_
#define __dtv_SETTING_antenna_H_

#include "dtv_setting_i.h"

typedef struct tag_dtv_setting_antenna_para
{
    GUI_FONT *font;

    H_LYR layer;
    __s32 focus_id;
    __s32 old_focus;

} dtv_setting_antenna_para_t;




H_WIN dtv_setting_antenna_win_create(H_WIN h_parent, dtv_setting_antenna_para_t *antenna_para);
__s32 dtv_setting_antenna_win_delete(H_WIN antenna_win);
#endif
