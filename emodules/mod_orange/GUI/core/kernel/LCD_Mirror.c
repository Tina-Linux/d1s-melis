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
//#include "GUI.h"
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/
GUI_CONST_STORAGE U8 LCD_aMirror[256] =
{
    ________, X_______, _X______, XX______, __X_____, X_X_____, _XX_____, XXX_____,
    ___X____, X__X____, _X_X____, XX_X____, __XX____, X_XX____, _XXX____, XXXX____,
    ____X___, X___X___, _X__X___, XX__X___, __X_X___, X_X_X___, _XX_X___, XXX_X___,
    ___XX___, X__XX___, _X_XX___, XX_XX___, __XXX___, X_XXX___, _XXXX___, XXXXX___,
    _____X__, X____X__, _X___X__, XX___X__, __X__X__, X_X__X__, _XX__X__, XXX__X__,
    ___X_X__, X__X_X__, _X_X_X__, XX_X_X__, __XX_X__, X_XX_X__, _XXX_X__, XXXX_X__,
    ____XX__, X___XX__, _X__XX__, XX__XX__, __X_XX__, X_X_XX__, _XX_XX__, XXX_XX__,
    ___XXX__, X__XXX__, _X_XXX__, XX_XXX__, __XXXX__, X_XXXX__, _XXXXX__, XXXXXX__,
    ______X_, X_____X_, _X____X_, XX____X_, __X___X_, X_X___X_, _XX___X_, XXX___X_,
    ___X__X_, X__X__X_, _X_X__X_, XX_X__X_, __XX__X_, X_XX__X_, _XXX__X_, XXXX__X_,
    ____X_X_, X___X_X_, _X__X_X_, XX__X_X_, __X_X_X_, X_X_X_X_, _XX_X_X_, XXX_X_X_,
    ___XX_X_, X__XX_X_, _X_XX_X_, XX_XX_X_, __XXX_X_, X_XXX_X_, _XXXX_X_, XXXXX_X_,
    _____XX_, X____XX_, _X___XX_, XX___XX_, __X__XX_, X_X__XX_, _XX__XX_, XXX__XX_,
    ___X_XX_, X__X_XX_, _X_X_XX_, XX_X_XX_, __XX_XX_, X_XX_XX_, _XXX_XX_, XXXX_XX_,
    ____XXX_, X___XXX_, _X__XXX_, XX__XXX_, __X_XXX_, X_X_XXX_, _XX_XXX_, XXX_XXX_,
    ___XXXX_, X__XXXX_, _X_XXXX_, XX_XXXX_, __XXXXX_, X_XXXXX_, _XXXXXX_, XXXXXXX_,
    _______X, X______X, _X_____X, XX_____X, __X____X, X_X____X, _XX____X, XXX____X,
    ___X___X, X__X___X, _X_X___X, XX_X___X, __XX___X, X_XX___X, _XXX___X, XXXX___X,
    ____X__X, X___X__X, _X__X__X, XX__X__X, __X_X__X, X_X_X__X, _XX_X__X, XXX_X__X,
    ___XX__X, X__XX__X, _X_XX__X, XX_XX__X, __XXX__X, X_XXX__X, _XXXX__X, XXXXX__X,
    _____X_X, X____X_X, _X___X_X, XX___X_X, __X__X_X, X_X__X_X, _XX__X_X, XXX__X_X,
    ___X_X_X, X__X_X_X, _X_X_X_X, XX_X_X_X, __XX_X_X, X_XX_X_X, _XXX_X_X, XXXX_X_X,
    ____XX_X, X___XX_X, _X__XX_X, XX__XX_X, __X_XX_X, X_X_XX_X, _XX_XX_X, XXX_XX_X,
    ___XXX_X, X__XXX_X, _X_XXX_X, XX_XXX_X, __XXXX_X, X_XXXX_X, _XXXXX_X, XXXXXX_X,
    ______XX, X_____XX, _X____XX, XX____XX, __X___XX, X_X___XX, _XX___XX, XXX___XX,
    ___X__XX, X__X__XX, _X_X__XX, XX_X__XX, __XX__XX, X_XX__XX, _XXX__XX, XXXX__XX,
    ____X_XX, X___X_XX, _X__X_XX, XX__X_XX, __X_X_XX, X_X_X_XX, _XX_X_XX, XXX_X_XX,
    ___XX_XX, X__XX_XX, _X_XX_XX, XX_XX_XX, __XXX_XX, X_XXX_XX, _XXXX_XX, XXXXX_XX,
    _____XXX, X____XXX, _X___XXX, XX___XXX, __X__XXX, X_X__XXX, _XX__XXX, XXX__XXX,
    ___X_XXX, X__X_XXX, _X_X_XXX, XX_X_XXX, __XX_XXX, X_XX_XXX, _XXX_XXX, XXXX_XXX,
    ____XXXX, X___XXXX, _X__XXXX, XX__XXXX, __X_XXXX, X_X_XXXX, _XX_XXXX, XXX_XXXX,
    ___XXXXX, X__XXXXX, _X_XXXXX, XX_XXXXX, __XXXXXX, X_XXXXXX, _XXXXXXX, XXXXXXXX
};

/*************************** End of file ****************************/
