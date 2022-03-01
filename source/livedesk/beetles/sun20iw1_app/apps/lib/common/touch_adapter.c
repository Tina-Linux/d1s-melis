/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2011, CHIPHD, China
*                                            All Rights Reserved
*
* File      : touch_adapter.c
* By        : CQQ
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011/06/17 13:55  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#include "touch_adapter.h"
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// function --- 判断pos在哪个区域
// rect_sz  --- 区域数组
// count    --- 数组个数
// x        --- x 坐标
// y        --- y 坐标
// return   --- 落点在某个区域内返回对应区域的id值(从0起)，否则返回-1
/************************************************************************/
__s32 touch_check_rect_sz(TOUCH_UI_RECT rect_sz[], __s32 count, TOUCH_UI_INT x, TOUCH_UI_INT y)
{
    __s32 i;

    for (i = count; i--;)
    {
        if (XInRect(x, rect_sz[i]) && YInRect(y, rect_sz[i]))
        {
            break;
        }
    }

    if (0 <= i)
    {
        return i;
    }
    else
    {
        return TOUCH_UI_INVALID_ID;
    }
}


/************************************************************************/
// function --- 判断pos在哪个区域
// rect_sz  --- 区域数组
// id_sz    --- 对应区域的id数组
// count    --- 数组个数
// pos      --- 坐标
// return   --- 落点在某个区域内返回对应区域的id值，否则返回-1
/************************************************************************/
__s32 touch_check_rect_sz_ex(TOUCH_UI_RECT rect_sz[], __s32 id_sz[], __s32 count, TOUCH_UI_POS *pos)
{
    __s32 i;

    for (i = count; i--;)
    {
        if (pPtInRect(pos, rect_sz[i]))
        {
            break;
        }
    }

    if (0 <= i)
    {
        return id_sz[i];
    }
    else
    {
        return TOUCH_UI_INVALID_ID;
    }
}


