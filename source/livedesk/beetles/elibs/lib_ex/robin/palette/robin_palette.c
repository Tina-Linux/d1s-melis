/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_palette.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.10.10
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.10.10       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_palette_c
#define  __robin_palette_c


#include "robin_i.h"
#include <kconfig.h>
#include <emodules/mod_display.h>


/***************************************************************************************************
*Name        : robin_set_palette
*Prototype   : __s32 robin_set_palette( const __u32 *palette_buf )
*Arguments   : palette_buf   input. the palette data buffer, whose size must be
*                            ( ROBIN_PALETTE_COLOR_COUNT * 4 ).
*Return      : ==0       succeed
*              !=0       fail
*Description : set the whole palette.
*Other       :
***************************************************************************************************/
__s32 robin_set_palette(const __u32 *palette_buf)
{
    __err("not support anymore after d100");

    return 0;
}



/***************************************************************************************************
*Name        : robin_get_palette
*Prototype   : __s32 robin_get_palette( __u32 *palette_buf )
*Arguments   : palette_buf   output. the buffer to store palette data, whose size must be
*                            ( ROBIN_PALETTE_COLOR_COUNT * 4 ).
*Return      : ==0       succeed
*              !=0       fail
*Description : get the whole palette.
*Other       :
***************************************************************************************************/
__s32 robin_get_palette(__u32 *palette_buf)
{
    __err("not support anymore after d100");
    return 0;
}



/***************************************************************************************************
*Name        : robin_modify_palette
*Prototype   : __s32 robin_modify_palette( __u32 first_index, const __u32 *color_array, __u32 nr )
*Arguments   : first_index   input. the index of the first color to be modified in the palette, it
*                            rangs 0~~~~(ROBIN_PALETTE_COLOR_COUNT-1).
*              color_array   input. the new color list, it is used to replace partial colors in
*                            palette.
*              nr            input. the color count of the color array.
*Return      : ==0       succeed
*              !=0       fail
*Description : modify the partial colors of the palette.
*Other       :
***************************************************************************************************/
__s32 robin_modify_palette(__u32 first_index, const __u32 *color_array, __u32 nr)
{
    __err("not support anymore after d100");
    return 0;
}

#endif     //  ifndef __robin_palette_c
