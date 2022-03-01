/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                               (c) Copyright 2006-2007, JOHN, China
*                                           All Rights Reserved
*
* File    : sspc_uipara.h
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/

#ifndef _SPCTRM_UIPARA_H_
#define _SPCTRM_UIPARA_H_

#include "beetles_app.h"

typedef struct music_spectrum_item_s
{
    __s32   x;
    __s32   y;
    __u32   width;
    __u32   height;
    //__u32   reserved[32];
} music_spectrum_item_t;

typedef struct SPCTRM_PARA
{
    music_spectrum_item_t   para_frmwin;    //framewin

    music_spectrum_item_t       para_specctr;   //ÆµÆ×¿Ø¼þ´óÐ¡

    __s32   partical_bmp_id;
    __s32   ball_bmp_id;

} __spctrm_uipara_t;

extern __s32 spctrm_get_screen_rect(RECT *screen_rect);
extern __spctrm_uipara_t *spctrm_get_uipara(void);
extern __s32 spctrm_get_screen_rect(RECT *screen_rect);

#endif //_SPCTRM_UIPARA_H_
