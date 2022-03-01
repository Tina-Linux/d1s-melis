/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lemoDn
* File    : GUI_DrawPGraph.c
* Purpose : Implementation of GUI_DrawGraph
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 11:42:00
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
*       _DrawGraph
*/
static void _DrawGraph(I16 *pay, int NumPoints, int xOff, int yOff)
{
    int i;
    int y0, y1;
    int yClip0, yClip1;
    /* Perform high level clipping in x */
    yClip0 = GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->y0;
    yClip1 = GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->y1;
    i = 0;

    if (GUI_Context.pClipRect_HL[GUI_Context.SelLayer])
    {
        if (xOff < GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->x0)
        {
            i = GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->x0 - xOff;
        }

        if (xOff + NumPoints - 1 > GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->x1)
        {
            NumPoints = GUI_Context.pClipRect_HL[GUI_Context.SelLayer]->x1 - xOff + 1;
        }
    }

    /* Start drawing if there is something left to do after x-clipping */
    if (i < NumPoints)
    {
        y0 = *(pay + i++) + yOff;

        /* Iterate */
        for (; i < NumPoints; i++)
        {
            /* Perform high level clipping in y */
            y1 = *(pay + i) + yOff;

            if ((y0 >= yClip0) || (y1 >= yClip0))
            {
                if ((y0 <= yClip1) || (y1 <= yClip1))
                {
                    int y01;
                    y01 = (y1 + y0) / 2;

                    if (y0 <= y1)
                    {
                        LCD_DrawVLine(i + xOff - 1, y0, y01);
                        LCD_DrawVLine(i + xOff, y01, y1);
                    }
                    else
                    {
                        LCD_DrawVLine(i + xOff - 1, y01, y0);
                        LCD_DrawVLine(i + xOff, y1, y01);
                    }
                }
            }

            y0 = y1;
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
*       GUI_DrawGraph
*/
void GUI_DrawGraph(I16 *pay, int NumPoints, int x0, int y0)
{
    GUI_LOCK();
#if (GUI_WINSUPPORT)
    WM_ADDORG(x0, y0);
    WM_ITERATE_START(NULL);
    {
#endif
        _DrawGraph(pay, NumPoints, x0, y0);
#if (GUI_WINSUPPORT)
    }
    WM_ITERATE_END();
#endif
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
