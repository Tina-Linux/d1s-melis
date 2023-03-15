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
#include "gui_core_kernel_private.h"


/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define FIXEDPALETTE LCD_FIXEDPALETTE

#if   FIXEDPALETTE == -1     /* User defined color conversion routines */
#define COLOR2INDEX(Color) LCD_Color2Index_User(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_User(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_User()
#elif FIXEDPALETTE == 0      /* User defined colors (LCD_PHYSCOLORS) */
#define COLOR2INDEX(Color) LCD_Color2Index_0(Color, &LCD_PhysPal)
#define INDEX2COLOR(Index) LCD_Index2Color_0(Index, &LCD_PhysPal)
#define GETINDEXMASK()     LCD_GetIndexMask_0()
#elif FIXEDPALETTE == 1
#define COLOR2INDEX(Color) LCD_Color2Index_1(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_1(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_1()
#elif FIXEDPALETTE == 2
#define COLOR2INDEX(Color) LCD_Color2Index_2(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_2(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_2()
#elif FIXEDPALETTE == 4
#define COLOR2INDEX(Color) LCD_Color2Index_4(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_4(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_4()
#elif (FIXEDPALETTE == 111) && (LCD_SWAP_RB==0)
#define COLOR2INDEX(Color) LCD_Color2Index_111(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_111(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_111()
#elif (FIXEDPALETTE == 111) && (LCD_SWAP_RB)
#define COLOR2INDEX(Color) LCD_Color2Index_M111(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_M111(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_M111()
#elif (FIXEDPALETTE == 222) && (LCD_SWAP_RB==0)
#define COLOR2INDEX(Color) LCD_Color2Index_222(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_222(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_222()
#elif (FIXEDPALETTE == 222) && (LCD_SWAP_RB==1)
#define COLOR2INDEX(Color) LCD_Color2Index_M222(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_M222(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_M222()
#elif (FIXEDPALETTE == 233) && (LCD_SWAP_RB==0)
#define COLOR2INDEX(Color) LCD_Color2Index_233(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_233(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_233()
#elif (FIXEDPALETTE == 233) && (LCD_SWAP_RB==1)
#define COLOR2INDEX(Color) LCD_Color2Index_M233(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_M233(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_M233()
#elif (FIXEDPALETTE == 323) && (LCD_SWAP_RB==0)
#define COLOR2INDEX(Color) LCD_Color2Index_323(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_323(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_323()
#elif (FIXEDPALETTE == 323) && (LCD_SWAP_RB==1)
#define COLOR2INDEX(Color) LCD_Color2Index_M323(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_M323(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_M323()
#elif (FIXEDPALETTE == 332) && (LCD_SWAP_RB==0)
#define COLOR2INDEX(Color) LCD_Color2Index_332(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_332(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_332()
#elif (FIXEDPALETTE == 332) && (LCD_SWAP_RB==1)
#define COLOR2INDEX(Color) LCD_Color2Index_M332(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_M332(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_M332()
#elif (FIXEDPALETTE == 444121)
#define COLOR2INDEX(Color) LCD_Color2Index_444_12_1(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_444_12_1(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_444_12_1()
#elif (FIXEDPALETTE == 44412) && (LCD_SWAP_RB==0)
#define COLOR2INDEX(Color) LCD_Color2Index_444_12(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_444_12(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_444_12()
#elif (FIXEDPALETTE == 44416)  && (LCD_SWAP_RB==0)
#define COLOR2INDEX(Color) LCD_Color2Index_444_16(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_444_16(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_444_16()
#elif (FIXEDPALETTE == 44412)  && (LCD_SWAP_RB==1)
#define COLOR2INDEX(Color) LCD_Color2Index_M444_12(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_M444_12(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_M444_12()
#elif (FIXEDPALETTE == 44416)  && (LCD_SWAP_RB==1)
#define COLOR2INDEX(Color) LCD_Color2Index_M444_16(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_M444_16(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_M444_16()
#elif (FIXEDPALETTE == 555)  && (LCD_SWAP_RB==0)
#define COLOR2INDEX(Color) LCD_Color2Index_555(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_555(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_555()
#elif (FIXEDPALETTE == 555)  && (LCD_SWAP_RB)
#define COLOR2INDEX(Color) LCD_Color2Index_M555(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_M555(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_M555()
#elif (FIXEDPALETTE == 556) && (LCD_SWAP_RB==0)
#define COLOR2INDEX(Color) LCD_Color2Index_556(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_556(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_556()
#elif (FIXEDPALETTE == 556) && (LCD_SWAP_RB)
#define COLOR2INDEX(Color) LCD_Color2Index_M556(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_M556(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_M556()
#elif (FIXEDPALETTE == 565) && (LCD_SWAP_RB==0)
#define COLOR2INDEX(Color) LCD_Color2Index_565(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_565(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_565()
#elif (FIXEDPALETTE == 565) && (LCD_SWAP_RB)
#define COLOR2INDEX(Color) LCD_Color2Index_M565(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_M565(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_M565()
#elif (FIXEDPALETTE == 655) && (LCD_SWAP_RB==0)
#define COLOR2INDEX(Color) LCD_Color2Index_655(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_655(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_655()
#elif (FIXEDPALETTE == 655) && (LCD_SWAP_RB)
#define COLOR2INDEX(Color) LCD_Color2Index_M655(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_M655(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_M655()
#elif FIXEDPALETTE == 8666
#define COLOR2INDEX(Color) LCD_Color2Index_8666(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_8666(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_8666()
#elif FIXEDPALETTE == 86661
#define COLOR2INDEX(Color) LCD_Color2Index_8666_1(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_8666_1(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_8666_1()
#elif (FIXEDPALETTE == 8888)&& (LCD_SWAP_RB==0)
#define COLOR2INDEX(Color) LCD_Color2Index_8888(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_8888(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_8888()
#elif (FIXEDPALETTE == 8888)&& (LCD_SWAP_RB==1)
#define COLOR2INDEX(Color) LCD_Color2Index_M8888(Color)
#define INDEX2COLOR(Index) LCD_Index2Color_M8888(Index)
#define GETINDEXMASK()     LCD_GetIndexMask_M8888()

#else
#error Unsupported color mode
#endif

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

#if (LCD_FIXEDPALETTE==0)
#if !LCD_PHYSCOLORS_IN_RAM
const
#endif
LCD_COLOR       LCD_PhysColors[]   = { LCD_PHYSCOLORS };
const LCD_PHYSPALETTE LCD_PhysPal  = { LCD_NUM_COLORS, &LCD_PhysColors[0] };
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*
*           LCD_L0_Color2Index    16bpp
*/
unsigned int LCD_L0_Color2Index(LCD_COLOR Color)
{
    unsigned int Index;
    Index = LCD_Color2Index_M565(Color);
#if LCD_REVERSE
    Index ^= ((1 << 16) - 1);
#endif
    return Index;
}

/*********************************************************************
*
*           LCD_L0_1_Color2Index    added by mcl   32 bpp
*/

unsigned int LCD_L0_1_Color2Index(LCD_COLOR Color)
{
    unsigned int Index;
    Index = LCD_Color2Index_M8888(Color);
#if LCD_REVERSE
    Index ^= ((1 << 24) - 1);
#endif
    return Index;
}

/*********************************************************************
*
*           LCD_L0_2_Color2Index    added by mcl  8bpp
*/

unsigned int LCD_L0_2_Color2Index(LCD_COLOR Color)
{
    unsigned int Index;
#if  GUI_SW_LAYER
    Index = Color;
#else
    Index = LCD_Color2Index_M233(Color);
#if LCD_REVERSE
    Index ^= ((1 << 8) - 1);
#endif
#endif
    return Index;
}

unsigned int LCD_L0_3_Color2Index(LCD_COLOR Color)
{
    unsigned int Index;
#if  GUI_SW_LAYER
    Index = Color;
#else
    Index = LCD_Color2Index_2(Color);
#if LCD_REVERSE
    Index ^= ((1 << 8) - 1);
#endif
#endif
    return Index;
}

/*********************************************************************
*
*       LCD_L0_Index2Color   16bpp
*/
LCD_COLOR LCD_L0_Index2Color(int Index)
{
#if LCD_REVERSE
    Index ^= ((1 << 16) - 1);
#endif
    return LCD_Index2Color_M565(Index);
}

/*********************************************************************
*
*       LCD_L0_1_Index2Color   added by mcl   32bpp
*/
LCD_COLOR LCD_L0_1_Index2Color(int Index)
{
#if LCD_REVERSE
    Index ^= ((1 << 24) - 1);
#endif
    return LCD_Index2Color_M8888(Index);
}

/*********************************************************************
*
*       LCD_L0_2_Index2Color   added by mcl   8bpp
*/
LCD_COLOR LCD_L0_2_Index2Color(int Index)
{
#if  GUI_SW_LAYER
    LCD_COLOR  r;
    r = Index;
    return r;
#else
#if LCD_REVERSE
    Index ^= ((1 << 2) - 1);
#endif
    return LCD_Index2Color_M233(Index);
#endif
}


/*********************************************************************
*
*       LCD_L0_2_Index2Color   added by mcl    2bpp
*/
LCD_COLOR LCD_L0_3_Index2Color(int Index)
{
#if  GUI_SW_LAYER
    LCD_COLOR  r;
    r = Index;
    return r;
#else
#if LCD_REVERSE
    Index ^= ((1 << 2) - 1);
#endif
    return LCD_Index2Color_2(Index);
#endif
}

/*********************************************************************
*
*       LCD_L0_GetIndexMask   //16bpp
*/
unsigned int LCD_L0_GetIndexMask(void)
{
    return LCD_GetIndexMask_M565();
}


/*********************************************************************
*
*       LCD_L0_1_GetIndexMask  added by mcl      32bpp
*/
unsigned int LCD_L0_1_GetIndexMask(void)
{
    return LCD_GetIndexMask_M8888();
}

/*********************************************************************
*
*       LCD_L0_2_GetIndexMask  added by mcl   8bpp
*/
unsigned int LCD_L0_2_GetIndexMask(void)
{
    return LCD_GetIndexMask_M233();
}

/*********************************************************************
*
*       LCD_L0_2_GetIndexMask  added by mcl     2bpp
*/
unsigned int LCD_L0_3_GetIndexMask(void)
{
    return LCD_GetIndexMask_2();
}


/*********************************************************************
*
*       LCD_L0_GetRect
*/
void LCD_L0_GetRect(LCD_RECT *pRect)
{
    pRect->x0 = 0;
    pRect->y0 = 0;
    pRect->x1 = Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1;
    pRect->y1 = Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1;
}



/*********************************************************************
*
*       LCD_L0_1_GetRect   added by mcl
*/
void LCD_L0_1_GetRect(LCD_RECT *pRect)
{
    pRect->x0 = 0;
    pRect->y0 = 0;
    pRect->x1 = Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1;
    pRect->y1 = Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1;
}

/*********************************************************************
*
*       LCD_L0_2_GetRect   added by mcl
*/
void LCD_L0_2_GetRect(LCD_RECT *pRect)
{
    pRect->x0 = 0;
    pRect->y0 = 0;
    pRect->x1 = Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1;
    pRect->y1 = Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1;
}

/*********************************************************************
*
*       LCD_L0_2_GetRect   added by mcl
*/
void LCD_L0_3_GetRect(LCD_RECT *pRect)
{
    pRect->x0 = 0;
    pRect->y0 = 0;
    pRect->x1 = Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1;
    pRect->y1 = Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1;
}

#if 0     // mcl del 
/*********************************************************************
*
*           LCD_L0_Color2Index
*/
unsigned int LCD_L0_Color2Index(LCD_COLOR Color)
{
    unsigned int Index;
    Index = COLOR2INDEX(Color);
#if LCD_REVERSE
    Index ^= ((1 << LCD_BITSPERPIXEL) - 1);
#endif
    return Index;
}

/*********************************************************************
*
*           LCD_L0_1_Color2Index    added by mcl
*/

unsigned int LCD_L0_1_Color2Index(LCD_COLOR Color)
{
    unsigned int Index;
    Index = COLOR2INDEX(Color);
#if LCD_REVERSE
    Index ^= ((1 << LCD_BITSPERPIXEL) - 1);
#endif
    return Index;
}

/*********************************************************************
*
*           LCD_L0_2_Color2Index    added by mcl
*/

unsigned int LCD_L0_2_Color2Index(LCD_COLOR Color)
{
    unsigned int Index;
    Index = COLOR2INDEX(Color);
#if LCD_REVERSE
    Index ^= ((1 << LCD_BITSPERPIXEL) - 1);
#endif
    return Index;
}

/*********************************************************************
*
*           LCD_L0_3_Color2Index    added by mcl
*/

unsigned int LCD_L0_3_Color2Index(LCD_COLOR Color)
{
    unsigned int Index;
    Index = COLOR2INDEX(Color);
#if LCD_REVERSE
    Index ^= ((1 << LCD_BITSPERPIXEL) - 1);
#endif
    return Index;
}



/*********************************************************************
*
*       LCD_L0_Index2Color
*/
LCD_COLOR LCD_L0_Index2Color(int Index)
{
#if LCD_REVERSE
    Index ^= ((1 << LCD_BITSPERPIXEL) - 1);
#endif
    return INDEX2COLOR(Index);
}

/*********************************************************************
*
*       LCD_L0_1_Index2Color   added by mcl
*/
LCD_COLOR LCD_L0_1_Index2Color(int Index)
{
#if LCD_REVERSE
    Index ^= ((1 << LCD_BITSPERPIXEL) - 1);
#endif
    return INDEX2COLOR(Index);
}

/*********************************************************************
*
*       LCD_L0_2_Index2Color   added by mcl
*/
LCD_COLOR LCD_L0_2_Index2Color(int Index)
{
#if LCD_REVERSE
    Index ^= ((1 << LCD_BITSPERPIXEL) - 1);
#endif
    return INDEX2COLOR(Index);
}

/*********************************************************************
*
*       LCD_L0_3_Index2Color   added by mcl
*/
LCD_COLOR LCD_L0_3_Index2Color(int Index)
{
#if LCD_REVERSE
    Index ^= ((1 << LCD_BITSPERPIXEL) - 1);
#endif
    return INDEX2COLOR(Index);
}


/*********************************************************************
*
*       LCD_L0_GetIndexMask
*/
unsigned int LCD_L0_GetIndexMask(void)
{
    return GETINDEXMASK();
}


/*********************************************************************
*
*       LCD_L0_1_GetIndexMask  added by mcl
*/
unsigned int LCD_L0_1_GetIndexMask(void)
{
    return GETINDEXMASK();
}

/*********************************************************************
*
*       LCD_L0_2_GetIndexMask  added by mcl
*/
unsigned int LCD_L0_2_GetIndexMask(void)
{
    return GETINDEXMASK();
}

/*********************************************************************
*
*       LCD_L0_3_GetIndexMask  added by mcl
*/
unsigned int LCD_L0_3_GetIndexMask(void)
{
    return GETINDEXMASK();
}


/*********************************************************************
*
*       LCD_L0_GetRect
*/
void LCD_L0_GetRect(LCD_RECT *pRect)
{
    pRect->x0 = 0;
    pRect->y0 = 0;
    pRect->x1 = Screen_Context.LCDXSize[0] - 1;
    pRect->y1 = Screen_Context.LCDYSize[0] - 1;
}



/*********************************************************************
*
*       LCD_L0_1_GetRect   added by mcl
*/
void LCD_L0_1_GetRect(LCD_RECT *pRect)
{
    pRect->x0 = 0;
    pRect->y0 = 0;
    pRect->x1 = Screen_Context.LCDXSize[1] - 1;
    pRect->y1 = Screen_Context.LCDYSize[1] - 1;
}

/*********************************************************************
*
*       LCD_L0_2_GetRect   added by mcl
*/
void LCD_L0_2_GetRect(LCD_RECT *pRect)
{
    pRect->x0 = 0;
    pRect->y0 = 0;
    pRect->x1 = Screen_Context.LCDXSize[2] - 1;
    pRect->y1 = Screen_Context.LCDYSize[2] - 1;
}

/*********************************************************************
*
*       LCD_L0_3_GetRect   added by mcl
*/
void LCD_L0_3_GetRect(LCD_RECT *pRect)
{
    pRect->x0 = 0;
    pRect->y0 = 0;
    pRect->x1 = Screen_Context.LCDXSize[3] - 1;
    pRect->y1 = Screen_Context.LCDYSize[3] - 1;
}

#endif
/*************************** End of file ****************************/
