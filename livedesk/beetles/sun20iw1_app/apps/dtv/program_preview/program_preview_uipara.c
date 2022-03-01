/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              LiveTouch Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : ebook
* File    : ebook_uipara.c
*
* By      : Hot.lee
* Version : v1.0
* Date    :
**********************************************************************************************************************
*/

#ifndef __PROGRAM_PREVIEW_PARA_H__
#define __PROGRAM_PREVIEW_PARA_H__
#include "program_preview_uipara.h"

//缩略图的大小
//现在播放全部由线程播放
//所以缩略图的坐标(program_pre)请在dtv_uipara.c中修改

program_preview_uipara_t program_pre_480_272 =
{
    {0, 0, 300, 44},               //title
    {300, 0, 180, 44},             //cur_program
    {0, 44, 300, 190},             //program_pre
    {0, 44, 190, 38 * 5},        //36*5=180   //listbar
    {
        {0, 190 + 44, 480, (272 - 190 - 44)}, //cur_next_prog[0]
        {0, 190 + 44 + (272 - 190 - 44) / 2, 480, (272 - 190 - 44)}, //cur_next_prog[1]
    },
    5                             // LISTBAR_ITEM_NUM

};

program_preview_uipara_t program_pre_800_480 =
{
    {0, 0, 311, 35},               //title
    {0, 0, 180, 35},             //cur_program
    {0, 44, 620, 414},             //program_pre
    {0, 44, 180, 38 * 10}, //36*5=180   //listbar
    {
        {0, 414 + 44, 800 / 2, (480 - 414 - 44)}, //cur_next_prog[0]
        {0, 414 + 44, 800 / 2, (480 - 414 - 44)}, //cur_next_prog[1]
    },
    10                             // LISTBAR_ITEM_NUM

};


static program_preview_uipara_t program_pre_720_576 =
{
    {0, 0, 300, 44},               //title
    {300, 0, 180, 44},             //cur_program
    {0, 44, 540, 436},             //program_pre
    {540, 44, 180, 38 * 10}, //36*5=180   //listbar
    {
        {0, 576 - 22, 720 / 2, 22}, //cur_next_prog[0]
        {720 / 2, 576 - 22, 720 / 2, 22}, //cur_next_prog[1]
    },
    10                             // LISTBAR_ITEM_NUM

};


static program_preview_uipara_t program_pre_720_480 =
{
    {0, 0, 300, 44},               //title
    {300, 0, 180, 44},             //cur_program
    {0, 44, 540, 436},             //program_pre
    {540, 44, 180, 38 * 10}, //36*5=180   //listbar
    {
        {0, 414 + 44, 720 / 2, (480 - 414 - 44)}, //cur_next_prog[0]
        {720 / 2, 414 + 44, 720 / 2, (480 - 414 - 44)}, //cur_next_prog[1]
    },
    10                              // LISTBAR_ITEM_NUM

};

program_preview_uipara_t *program_preview_get_uipara(void)
{
    SIZE rect;
    dsk_display_get_size((__s32 *)&rect.width, (__s32 *)&rect.height);
    //  __msg("screen width:%d, height:%d\n",rect.width, rect.height);

    if ((rect.width == 480) && (rect.height == 272))
    {
        return &program_pre_480_272;
    }
    else if ((rect.width == 800) && (rect.height == 480))
    {
        return &program_pre_800_480;
    }
    else if ((rect.width == 720) && (rect.height == 576))
    {
        return &program_pre_720_576;
    }
    else if ((rect.width == 720) && (rect.height == 480))
    {
        return &program_pre_720_480;
    }
    else
    {
        return &program_pre_800_480;
    }
}

#endif


