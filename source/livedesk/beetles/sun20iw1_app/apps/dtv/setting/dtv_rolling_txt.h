/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : dtv_settings.c
* By        : hot.lee
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  hot.lee  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __DTV_ROLLING_TXT_H__
#define  __DTV_ROLLING_TXT_H__
#include "dtv_setting_ca.h"

void dtv_rolling_txt_create(__gui_msg_t *msg);
void dtv_show_rollingTxt_dialog(__u8 dtvRollingBarType);
void dtv_rolling_txt_frame_draw(__gui_msg_t *msg);

#endif  // __DTV_ROLLING_TXT_H__
