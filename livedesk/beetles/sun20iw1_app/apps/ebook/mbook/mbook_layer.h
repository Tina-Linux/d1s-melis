/*
**************************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              FM app sample
*
*                                   (c) Copyright 2006-2007, ANDY, China
*                                            All Rights Reserved
*
* File    : mbook_layer.h
* By      : lyn
* Version : V1.00
* time    : 2009-8-14
**************************************************************************************************************
*/
#ifndef __MBOOK_LAYER_H__
#define __MBOOK_LAYER_H__

H_WIN   MBOOK_Layer_CreatLayer(__u32 w, __u32 h, __s32 pipe);

__s32   MBOOK_Layer_DeleteLayer(H_WIN h_lyr);

__s32   MBOOK_Layer_OpenShow(H_WIN h_lyr);

__s32   MBOOK_Layer_CloseShow(H_WIN h_lyr);

__s32   MBOOK_Layer_IsOnShow(H_WIN h_lyr);

__s32   MBOOK_Layer_GetSrcY(H_WIN h_lyr);

__s32   MBOOK_Layer_GetScnY(H_WIN h_lyr);

__s32   MBOOK_Layer_GetSrcH(H_WIN h_lyr);

void    MBOOK_Layer_SetScn(H_WIN h_lyr, __s32 y);

void    MBOOK_Layer_SetSrc(H_WIN h_lyr, __s32 y);

void    MBOOK_Layer_ResetWin(H_WIN h_lyr);

void    MBOOK_Layer_Rotate(H_WIN h_lyr, __u32 w, __u32 h, __u32 rotate);

#endif /* __MBOOK_LAYER_H__ */
