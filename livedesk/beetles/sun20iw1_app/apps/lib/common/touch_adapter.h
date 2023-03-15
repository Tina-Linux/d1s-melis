/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __TOUCH_ADAPTER_H__7E1D0283_7AB6_4807_A453_40CAF20877E5__
#define __TOUCH_ADAPTER_H__7E1D0283_7AB6_4807_A453_40CAF20877E5__
//////////////////////////////////////////////////////////////////////////
//#include "epdk.h"
#include <emodules/mod_defs.h>

typedef  __s16        TOUCH_UI_INT;

typedef struct tag_TOUCH_UI_POS
{
    TOUCH_UI_INT x;
    TOUCH_UI_INT y;
} TOUCH_UI_POS;

//distance pos (坐标范围)
typedef struct tag_TOUCH_UI_DPOS
{
    TOUCH_UI_INT d0;
    TOUCH_UI_INT d1;
} TOUCH_UI_DPOS;

typedef struct tag_TOUCH_UI_RTSZ
{
    TOUCH_UI_INT width;
    TOUCH_UI_INT height;
} TOUCH_UI_RTSZ;

typedef struct tag_TOUCH_UI_RECT
{
    TOUCH_UI_INT x;
    TOUCH_UI_INT y;
    TOUCH_UI_INT width;
    TOUCH_UI_INT height;
} TOUCH_UI_RECT;

#define TOUCH_UI_INVALID_ID            -1
#define TouchValidID(_id)              (0 <= (_id))
//////////////////////////////////////////////////////////////////////////
#define ValueInRange(_min, _v, _max)        ((_min) <= (_v) && (_v) < (_max))
#define ValueBetweenRange(_min, _v, _max)   ((_min) <= (_v) && (_v) <= (_max))

#define ValueBetweenRangeEx(_min, _v1, _v2, _max) (((_min) <= (_v1)) && ((_v1) < (_v2)) && ((_v2) <= (_max)))

#define ValueOutOfRange(_min, _v, _max)     ((_v) < (_min) || (_max) < (_v))
///////////////

#define XInRect(_x, _rt)          ValueInRange(_rt.x, _x, _rt.x + _rt.width)
#define YInRect(_y, _rt)          ValueInRange(_rt.y, _y, _rt.y + _rt.height)
#define XYInRect(_x, _y, _rt)     (XInRect(_x, _rt) && YInRect(_y, _rt))

#define PtInRect(_pt, _rt)        (XInRect(_pt.x, _rt) && YInRect(_pt.y, _rt))
#define pPtInRect(_pPt, _rt)      (XInRect(_pPt->x, _rt) && YInRect(_pPt->y, _rt))

/*
#define PtInGuiRect(_pt, _Rect)   \
    ( (_Rect.left <= _pt.x) && (_pt.x < _Rect.right) \
    && (_Rect.top <= _pt.y) && (_pt.y < _Rect.bottom) )
*/

//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// function --- 判断(x, y)在哪个区域
// rect_sz  --- 区域数组
// count    --- 数组个数
// x        --- x 坐标
// y        --- y 坐标
// return   --- 落点在某个区域内返回区域的对应id值(从0起)，否则返回-1
/************************************************************************/
extern __s32 touch_check_rect_sz(TOUCH_UI_RECT rect_sz[], __s32 count,
                                 TOUCH_UI_INT x, TOUCH_UI_INT y);

/************************************************************************/
// function --- 判断pos在哪个区域
// rect_sz  --- 区域数组
// id_sz    --- 对应区域的id数组
// count    --- 数组个数
// pos      --- 坐标
// return   --- 落点在某个区域内返回对应区域的id值，否则返回-1
/************************************************************************/
extern __s32 touch_check_rect_sz_ex(TOUCH_UI_RECT rect_sz[], __s32 id_sz[],
                                    __s32 count, TOUCH_UI_POS *pos);

//////////////////////////////////////////////////////////////////////////

#endif //__TOUCH_ADAPTER_H__7E1D0283_7AB6_4807_A453_40CAF20877E5__
//End of this file
