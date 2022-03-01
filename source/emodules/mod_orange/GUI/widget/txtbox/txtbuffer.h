/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：        txtbuffer.h
*
*Author：           William Wang
*
*Version :          1.1.0
*
*Date:              2008-12-18
*
*Description :      txtbox framebuffer definition
*
*Others :           None at present.
*
* History :
*
* <Author>          <time>          <version>     <description>
*
* William Wang     2008-12-18         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __TXTBUFFER_H__
#define __TXTBUFFER_H__

#include "txtbox_e.h"

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
typedef struct tag_TXTBUFFER_MAN
{
    __u8                    mode;
    __u32                   size;
    __rectsz_t              rectsz;
    __fb_format_t           layer_format;
    void                   *buffer;
    void                   *ebuffer;    /*补充的framebuffer地址*/
    __u32                   esize;      /*补充的framebuffer大小*/
    __u32                   vsize;      /*有效的framebuffer大小*/
    __rect_t                vrect;              /*valid rect*/
} txtbuffer_man_t;

txtbuffer_man_t    *TxtBuffer_Create(void);
__u32               TxtBuffer_Destroy(txtbuffer_man_t *txtbuffer);

//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------




#endif
