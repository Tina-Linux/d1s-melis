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

#ifndef __program_preview_frmH_
#define __program_preview_frmH_

#include "program_preview_i.h"

typedef struct tag_program_preview_frmpara
{
    GUI_FONT *font;

    H_LYR layer;
    __s32 focus_id;
    __s32 old_focus;

} program_preview_frmpara_t;




H_WIN program_preview_frmwin_create(H_WIN h_parent, program_preview_frmpara_t *list_para);
__s32 program_preview_frmwin_delete(H_WIN list_win);
#endif
