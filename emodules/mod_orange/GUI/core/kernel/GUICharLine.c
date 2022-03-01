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
* File    : GUICharLine.C
* Purpose : Implementation of character and string services
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 15:54:04
**********************************************************************************************************************
*/

#include <stddef.h>           /* needed for definition of NULL */
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _DispLine
*/
static void _DispLine(const char GUI_UNI_PTR *s, int MaxNumChars, const GUI_RECT *pRect)
{
    /* Check if we have anything to do at all ... */
    if (GUI_Context.pClipRect_HL[GUI_Context.SelLayer])
    {
        if (GUI_RectsIntersect(GUI_Context.pClipRect_HL[GUI_Context.SelLayer], pRect) == 0)
        {
            return;
        }
    }

#if GUI_COMPILER_SUPPORTS_FP

    if (GUI_Context.pAFont->pafEncode)
    {
        GUI_Context.pAFont->pafEncode->pfDispLine(s, MaxNumChars);
    }
    else
    {
#else
    {
#endif
        U16 Char;

        while (--MaxNumChars >= 0)
        {
            Char = GUI_UC__GetCharCodeInc(&s);
            GUI_Context.pAFont->pfDispChar(Char);
            //if (GUI_pfDispCharStyle) {
            //GUI_pfDispCharStyle(Char);
            //}
        }
    }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI__GetLineNumChars
*/
int GUI__GetLineNumChars(const char GUI_UNI_PTR *s, int MaxNumChars)
{
    int NumChars = 0;

    if (s)
    {
#if GUI_COMPILER_SUPPORTS_FP

        if (GUI_Context.pAFont->pafEncode)
        {
            //2008/07/22 9:08:22
            if (GUI_Context.pAFont->pafEncode->pfGetLineLen) //mcl add  to avoid  Error by NULL pfGetLineLen
                //2008/07/22 9:08:33
            {
                return GUI_Context.pAFont->pafEncode->pfGetLineLen(s, MaxNumChars);
            }
        }

#endif

        for (; NumChars < MaxNumChars; NumChars++)
        {
            U16 Data = GUI_UC__GetCharCodeInc(&s);

            if ((Data == 0) || (Data == '\n') || (Data == '\r')) //modified ba Derek 2010.12.02.11:00
            {
                break;
            }
        }
    }

    return NumChars;
}

/*********************************************************************
*
*       GUI_GetLineDistX
*
*  This routine is used to calculate the length of a line in pixels.
*/
int GUI__GetLineDistX(const char GUI_UNI_PTR *s, int MaxNumChars)
{
    int Dist = 0;

    if (s)
    {
        U16 Char;
#if GUI_COMPILER_SUPPORTS_FP

        if (GUI_Context.pAFont->pafEncode)
        {
            //2008/07/22 9:11:08
            if (GUI_Context.pAFont->pafEncode->pfGetLineDistX) //mcl add  to avoid  Error by NULL pfGetLineDistX
                //2008/07/22 9:11:18
            {
                return GUI_Context.pAFont->pafEncode->pfGetLineDistX(s, MaxNumChars);
            }
        }

#endif

        while (--MaxNumChars >= 0)
        {
            Char  = GUI_UC__GetCharCodeInc(&s);
            Dist += GUI_GetCharDistX(Char);
        }
    }

    return Dist;
}

/*********************************************************************
*
*       GUI__DispLine
*/
void GUI__DispLine(const char GUI_UNI_PTR *s, int MaxNumChars, const GUI_RECT *pr)
{
    GUI_RECT r;
#if GUI_SUPPORT_ROTATION

    if (GUI_pLCD_APIList)
    {
#if GUI_WINSUPPORT
        WM_ITERATE_START(NULL)
        {
#endif
            /* Do the actual drawing via routine call. */
            _DispLine(s, MaxNumChars, &r);
#if GUI_WINSUPPORT
        }
        WM_ITERATE_END();
#endif
    }
    else
#endif
    {
        r = *pr;
#if GUI_WINSUPPORT
        WM_ADDORG(r.x0, r.y0);
        WM_ADDORG(r.x1, r.y1);
        WM_ITERATE_START(&r)
        {
#endif
            GUI_Context.DispPosX[GUI_Context.SelLayer] = r.x0;
            GUI_Context.DispPosY[GUI_Context.SelLayer] = r.y0;
            /* Do the actual drawing via routine call. */
            _DispLine(s, MaxNumChars, &r);
#if GUI_WINSUPPORT
        }
        WM_ITERATE_END();
        WM_SUBORG(GUI_Context.DispPosX[GUI_Context.SelLayer], GUI_Context.DispPosY[GUI_Context.SelLayer]);
#endif
    }
}

/*************************** End of file ****************************/
