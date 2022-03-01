/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : program_previews_uipara.h
* By        : Hot.lee
* Func  : ui parameter
* Version   : v1.0
* ============================================================================================================
**************************************************************************************************************
*/

#ifndef __PROGRAM_PREVIEW_UIPARA_H_
#define  __PROGRAM_PREVIEW_UIPARA_H_
#include "program_preview_i.h"


typedef struct program_preview_uipara_s
{
    RECT   title;            //标题
    RECT   cur_program;      // 1/10
    RECT   program_pre;      //预览
    RECT   listbar;          //listbar坐标
    RECT   cur_next_prog[2];    //当前、下一个播放节目
    __s32  listbar_item;     // listbar 总数

} program_preview_uipara_t;


program_preview_uipara_t *program_preview_get_uipara(void);
//tips_uipara_t* program_preview_get_tips_uipara(void);


#endif      /*__PROGRAM_PREVIEW_UIPARA_H_*/

