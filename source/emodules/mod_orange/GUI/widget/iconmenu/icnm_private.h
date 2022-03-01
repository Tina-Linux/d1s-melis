/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lion
* File    : imenu_private.h
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-4 11:28:56
**********************************************************************************************************************
*/
#ifndef __LION_ICNM_PRIVATE_H__
#define __LION_ICNM_PRIVATE_H__

#include "lion_i.h"
#include "icnm_item.h"
#include "icnm_scaler.h"

typedef struct tag__LION_ICNM
{
    char                  name[LION_NAME_MAX];
    H_WIN                 h_win;
    char                  title[LION_TITLE_MAX];    /* title                                                        */
    __u8                  curitem;                  /* µ±Ç°item                                                     */
    __u32                 nitem;
    __lion_win_t        **items;
    __u32                 tid_show;
    __u32                 tid_prescale;
} __lion_icnm_t;

extern SIZE  ICNM_icontbl[];
extern __u32 ICNM_steptbl[];
extern __u32 ICNM_move[];
extern __u32 ICNM_pos[];
#endif /* #ifndef __LION_ICNM_PRIVATE_H__ */
