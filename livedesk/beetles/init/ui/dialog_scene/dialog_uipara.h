/*
************************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                 (c) Copyright 2007-2010, Jackie, China
*                                       All Rights Reserved
*
* File          : dialog_uipara.h
* By            : Jackie.chen
* Func      : dialog uipara
* Version   : v1.0
* ======================================================================================================================
* 2010-11-03  Jackie.chen  create this file, implements the fundemental interface;
************************************************************************************************************************
*/

#ifndef __DIALOG_UIPARA_H__
#define __DIALOG_UIPARA_H__

typedef struct
{
    __s32   fb_w;
    __s32   fb_h;

    __s32   src_x;
    __s32   src_y;
    __s32   src_w;
    __s32   src_h;

    __s32   scn_x;
    __s32   scn_y;
    __s32   scn_w;
    __s32   scn_h;

    __u8    pipe;
} __dialog_layer_t;


typedef struct
{
    /* layer        */
    __dialog_layer_t  dialog_lyr;

} __dialog_uipara_t;


extern __dialog_uipara_t  dialog_uipara;

void init_dialog_uipara(void);

#endif

