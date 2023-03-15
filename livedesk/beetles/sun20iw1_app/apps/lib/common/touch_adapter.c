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
