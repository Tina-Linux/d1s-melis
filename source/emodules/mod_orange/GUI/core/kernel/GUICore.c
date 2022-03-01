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
* File    : GUICore.c
* Purpose : Core routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 15:54:04
**********************************************************************************************************************
*/

#include <stddef.h>           /* needed for definition of NULL */
#include <kconfig.h>

#define  GL_CORE_C

#include "gui_core_kernel_private.h"
/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
#if GUI_SUPPORT_AA
static const tLCD_HL_APIList _HL_APIList =
{
    LCD_DrawHLine,
    LCD_DrawPixel
};
#endif

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _InitContext
*
* Purpose
*   Initializes the given context.
*
* Special considerations
*   Make sure that no GUI functions are called, because locking is
*   not allowed here (GUITASK_INIT not yet called).
*/
static void _InitContext(GUI_CONTEXT *pContext)
{
    __u8 layer;
    layer = GUI_Context.SelLayer >= GUI_NUM_LAYERS ? GUI_NUM_LAYERS - 1 : GUI_Context.SelLayer;
    /* memset(..,0,..) is not required, as this function is called only at startup of the GUI when data is 0 */
    // #if GUI_WINSUPPORT    // mcl add
    //pContext->WM__pUserClipRect = NULL;
    //#endif
    //pContext->pClipRect_HL[layer] = NULL;  //mcl del 08.11.25
#if GUI_SUPPORT_DEVICES
    pContext->pDeviceAPI   = LCD_aAPI[1]; /* &LCD_L0_APIList; */  // layer  mcl  aRGB 08.11.25
#endif
    pContext->pClipRect_HL[layer] = &GUI_Context.ClipRect[layer];
    LCD_L0_GetRect(&pContext->ClipRect[layer]);
#if GUI_SUPPORT_AA
cc
    pContext->pLCD_HL      = &_HL_APIList;
#endif
    pContext->pAFont       = GUI_DEFAULT_FONT;
    pContext->pClipRect_HL[layer] = &GUI_Context.ClipRect[layer];
    pContext->PenSize      = 1;
    /* Variables in WM module */
#if GUI_WINSUPPORT
    pContext->hAWin  = WM_GetDesktopWindow();
#endif
    /* Variables in GUI_AA module */
#if GUI_SUPPORT_AA
    pContext->AA_Factor = 3;
#endif
    pContext->Color   = GUI_INVALID_COLOR;
    pContext->BkColor = GUI_INVALID_COLOR;
    //LCD_SetBkColor(GUI_DEFAULT_BKCOLOR);
    LCD_SetBkColor(0xff000000);    // mcl for aRGB
    LCD_SetColor(GUI_DEFAULT_COLOR);
    pContext->pUC_API = &GUI__API_TableNone;
    pContext->BlendFlag = 0;
}
/*********************************************************************
*
*       _InitScreenContext
*
* Purpose
*   Initializes the given screen context.
*
* Special considerations
*   Make sure that no GUI functions are called, because locking is
*   not allowed here (GUITASK_INIT not yet called).
*/
static void _InitScreenContext(SCREEN_CONTEXT *pSContext)
{
    pSContext->LCDXSize[0] = LCD_XSIZE_0;
    pSContext->LCDYSize[0] = LCD_YSIZE_0;
	__inf("LCDXSize = %d  LCDYSize = %d",pSContext->LCDXSize[0],pSContext->LCDYSize[0]);
    pSContext->LCDDirection[0] = 1;
}



/*********************************************************************
*
*       _InitGUILayerInfo
*
* Purpose
*   Initializes the given layer information.
*
* Special considerations
*   Make sure that no GUI functions are called, because locking is
*   not allowed here (GUITASK_INIT not yet called).
*/
static void _InitGUILayerInfo(GUI_LAYER_INFO *pGUI_LayerInfo)
{
    int i;
    pGUI_LayerInfo->layerState[0] = 0;  // mcl  08.12.11
    pGUI_LayerInfo->colorMode[0] =  PIXEL_COLOR_ARGB8888;

    for (i = 1; i < GUI_NUM_LAYERS; i++)
    {
        pGUI_LayerInfo->layerState[i] = 0;
        pGUI_LayerInfo->colorMode[i]  = PIXEL_COLOR_ARGB8888;// all layers are inited to argb8888
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
*       GUI_ClearRect
*/
void GUI_ClearRect(int x0, int y0, int x1, int y1)
{
    GUI_DRAWMODE PrevDraw;
#if GUI_WINSUPPORT
    GUI_RECT r;
#endif
    GUI_LOCK();
    PrevDraw = LCD_SetDrawMode(GUI_DRAWMODE_REV);
#if GUI_WINSUPPORT
    WM_ADDORG(x0, y0);
    WM_ADDORG(x1, y1);
    r.x0 = x0;
    r.x1 = x1;
    r.y0 = y0;
    r.y1 = y1;
    WM_ITERATE_START(&r)
    {
#endif
        LCD_FillRect(x0, y0, x1, y1);
#if GUI_WINSUPPORT
    }
    WM_ITERATE_END();
#endif
    LCD_SetDrawMode(PrevDraw);
    GUI_UNLOCK();
}

/*********************************************************************
*
*       GUI_Clear
*/
void GUI_Clear(void)
{
    GUI_GotoXY(0, 0);    /* Reset text cursor to upper left */
    GUI_ClearRect(GUI_XMIN, GUI_YMIN, GUI_XMAX, GUI_YMAX);
}
/*
**********************************************************************************************************************
*                                               GUI_Kernel_Init
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int GUI_Kernel_Init(void)
{
    int r;
    GUI_Context.SelLayer = 0xFF; //mcl add   sel layer 0
    /* Init system wide globals first */
    GUI_DecChar = '.';     // global  variables     mcl
    //GUI_X_Init();           //  KeySem = esKRNL_SemCreate(0);   mcl
    /* Init context */
    _InitScreenContext(&Screen_Context); //layer 0
    _InitGUILayerInfo(&GUI_LayerInfo);
    _InitContext(&GUI_Context);
    GUITASK_INIT();
    r = LCD_Init();
#if GUI_WINSUPPORT
    WM_Init();
#endif
    GUITASK_COPY_CONTEXT();
    return r;
}
/*
**********************************************************************************************************************
*                                               GUI_Kernel_Exit
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int GUI_Kernel_Exit(void)
{
    return EPDK_OK;
}

/*********************************************************************
*
*       GUI_Init_layer   //init layer >0   Miracle
*
* Purpose:
*   Init of GUI internal data structures & variables
*/
int GUI_Init_layer(void)
{
    int r;
    GUI_DEBUG_LOG("\nGUI_Init()");
    /* Init system wide globals first */
    GUI_DecChar = '.';
    //_InitContext(&GUI_Context);  // only init context for layer index>0
    // GUITASK_INIT();            // mcl del
    r = LCD_Init_layer();
#if GUI_WINSUPPORT
    // WM_Init();     //   mcl del  for layer index>0    08.11.25
#endif
    //GUITASK_COPY_CONTEXT();   // multi task  mcl  08.11.25
    return r;
}

/*************************** End of file ****************************/
