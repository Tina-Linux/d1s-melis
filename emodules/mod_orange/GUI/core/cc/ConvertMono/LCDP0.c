/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : LCDP0.c
* Purpose : Color conversion routines for LCD-drivers
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_cc_private.h"

/*********************************************************************
*
*       Public code, 1/2/4/8 bpp color, LUT
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_Color2Index_0
*/
unsigned LCD_Color2Index_0(LCD_COLOR Color, const LCD_PHYSPALETTE *pPhysPal)
{
    int i;
    int NumEntries = pPhysPal->NumEntries;
    int BestIndex = 0;
    U32 BestDiff = 0xffffff; /* Initialize to worst match */
    const LCD_COLOR *pPalEntry;
    /* Try to find perfect match */
    i = 0;
    pPalEntry = &pPhysPal->pPalEntries[0];

    do
    {
        if (Color == *(pPalEntry + i))
        {
            return i;
        }
    } while (++i < NumEntries);

    /* Find best match */
    i = 0;
    pPalEntry = &pPhysPal->pPalEntries[0];

    do
    {
        U32 Diff = GUI_CalcColorDist(Color, *(pPalEntry + i));

        if (Diff < BestDiff)
        {
            BestDiff  = Diff;
            BestIndex = i;
        }
    } while (++i < NumEntries);

    return BestIndex;
}

/*********************************************************************
*
*       LCD_Index2Color_0
*/
LCD_COLOR LCD_Index2Color_0(int Index, const LCD_PHYSPALETTE *pPhysPal)
{
    if ((unsigned)Index >= (unsigned) pPhysPal->NumEntries)
    {
        return 0;     /* Illegal index */
    }

    return *(pPhysPal->pPalEntries + Index);
}

/*********************************************************************
*
*       LCD_GetIndexMask_0
*/
unsigned LCD_GetIndexMask_0(void)
{
    // return ((1 << LCD_BITSPERPIXEL) - 1);
    return ((1 << 24) - 1);    //mcl
}

/*************************** End of file ****************************/
