/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : msgbox.h
* By        : Andy.zhang
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __DTV_SETTING_CA_H_
#define  __DTV_SETTING_CA_H_

#include "dtv_setting_i.h"

#define DTV_CA_NONE             (0)//?T ca 
#define SHU_SHI_TONG_CAS            (1)//¨ºy¨º¨®¨ª¡§
#define TONG_FANG_LING_XUN_CAS  (2)//¨ª?¡¤? 
#define MEDIAGATE_CAS               (3)
#define DVN_JET_CAS             (4)

typedef struct tag_dtv_setting_ca_para
{
    __s32     focus_id;
    H_LYR     layer;
} setting_ca_para_t;

typedef struct tag_dtv_setting_ca_sub_para
{
    H_LYR       layer;
    __s32       Current_item;

} setting_ca_sub_para_t;

H_WIN dtv_setting_ca_win_create(H_WIN h_parent, setting_ca_para_t *para);
void  dtv_setting_ca_win_delete(H_WIN ca_win);

void  dtv_setting_ca_sub_win_delete(H_WIN ca_sub_win);
H_WIN dtv_setting_ca_sub_win_create(H_WIN h_parent, setting_ca_sub_para_t *para);
__s32 dtv_ca_msg_callback(void *arg);
__s32 dtv_ca_msg_is_exist(void);
void dtv_ca_msg_off(void);
void dtv_ca_msg_clear(void);
void dtv_ca_msg_resume(void);
void dtv_ca_msg_display(__u32 ca_msg_type);

#endif
