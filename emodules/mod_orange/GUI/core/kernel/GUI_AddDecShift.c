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
* File    : GUI_AddDecShift.c
* Purpose : Routines to display values as dec
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"
#include "string.h"

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/

const U32 GUI_Pow10[10] =
{
    1, 10, 100, 1000, 10000,
    100000, 1000000, 10000000, 100000000, 1000000000
};

/*********************************************************************
*
*       Static routines
*
**********************************************************************
*/
/*********************************************************************
*
*       _Check_NegLong
*/
static int _Check_NegLong(I32 *pv, char **ps)
{
    if (*pv < 0)
    {
        *(*ps)++ = '-';
        *pv = -*pv;
        return 1;
    }

    return 0;
}

/*********************************************************************
*
*       Module internal routines
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_Long2Len
*/
int GUI_Long2Len(I32 vSign)
{
    int Len = 1;
    I32 v = (vSign > 0) ? vSign : -vSign;

    while ((((U32)v) >= GUI_Pow10[Len]) && (Len < 9))
    {
        Len++;
    }

    if (vSign < 0)
    {
        Len++;
    }

    return Len;
}

/*********************************************************************
*
*       GUI_AddSign
*/
long GUI_AddSign(long v, char **ps)
{
    char c;

    if (v < 0)
    {
        c = '-';
        v = -v;
    }
    else
    {
        c = '+';
    }

    *(*ps)++ = c;
    **ps     = '\0';
    return v;
}

/*********************************************************************
*
*       Public routines
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_AddDecShift
*/
void GUI_AddDecShift(I32 v, U8 Len, U8 Shift, char **ps)
{
    char c;
    long d;
#ifndef _CM16C
    Len -= _Check_NegLong(&v, ps); /* TASKING: Tool internal error S003: asertion failed - please report */
#else

    if (v < 0)
    {
        *(*ps)++ = '-';
        v = -v;
    }

#endif

    if (Shift)
    {
        Len--;
    }

#if GUI_DEBUG_LEVEL >1

    if (Len > 9)
    {
        Len = 9;
        GUI_DEBUG_ERROROUT("Can not display more than 9 dec. digits");
    }

#endif

    if ((U32)v >= GUI_Pow10[Len])
    {
        v = GUI_Pow10[Len] - 1;
    }

    while (Len)
    {
        if (Len-- == Shift)
        {
            *(*ps)++ = GUI_DecChar;
        }

        d = GUI_Pow10[Len];
        c = (char)(v / d);
        v -= c * d;
        *(*ps)++ = c + '0';
    }

    **ps = 0;
}

/*************************** End of file ****************************/
