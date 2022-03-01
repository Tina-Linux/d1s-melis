/*
************************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                 (c) Copyright 2007-2010, Jackie, China
*                                       All Rights Reserved
*
* File          : init_uipara.c
* By            : Jackie.chen
* Func      : init uipara
* Version   : v1.0
* ======================================================================================================================
* 2010-11-03  Jackie.chen  create this file, implements the fundemental interface;
************************************************************************************************************************
*/
#include "mod_init_i.h"
#include "scnlock_uipara.h"

__screen_lock_para_t  scnlock_para =
{
    {110, 160, 0, 0, 110, 160, 0, 160, 110, 160, 0},
    {0, 0, 110, 160},
    0,
    {8, 136, 94, 24},
    0,
    0,
    0,
    0,
    0,
    {0, 0, 110, 160},
};

void init_scnlock_uipara(void)
{
    /*scnlock_para.screen_lock_layer                = {110,160,0,0,110,160,0,160,110,160};
    scnlock_para.frm_rect                       = {0,0,110,160};
    scnlock_para.text                           = STRING_SCREENUNLOCK;
    scnlock_para.text_rect                      = {8,136,94,24}
    scnlock_para.screen_lock_icon               = ID_INIT_SCNLOCK_INIT_BMP;
    scnlock_para.screen_unlock_start_icon       = ID_INIT_SCNUNLOCK_START_BMP;
    scnlock_para.screen_unlock1_icon            = ID_INIT_SCNUNLOCK1_BMP;
    scnlock_para.screen_unlock2_icon            = ID_INIT_SCNUNLOCK2_BMP;
    scnlock_para.screen_unlock_end_icon         = ID_INIT_SCNUNLOCK_END_BMP;
    scnlock_para.lock_icon_rect                 = {0,0,110,160};*/
    return;
}
