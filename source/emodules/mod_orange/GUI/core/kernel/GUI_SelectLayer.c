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
* File    : GUI_SelectLayer.c
* Purpose : select current  layer  to draw
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 9:51:50
**********************************************************************************************************************
*/

#include <stddef.h>           /* needed for definition of NULL */
#include "gui_core_kernel_private.h"

int GUI_SelectLayer(unsigned int Index);


/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_SelectLayer
        unsigned int GUI_SelectLayer(unsigned int Index) {
  unsigned int OldIndex;
  GUI_LOCK();
  OldIndex = GUI_Context.SelLayer;
  if (Index < GUI_NUM_LAYERS) {
    GUI_Context.SelLayer = Index;
    GUI_SelectLCD();
  }
  GUI_UNLOCK();
  return OldIndex;
}
*/
#if  0  // mcl del for new layer module
int GUI_OpenLayer(void *pFBAddr, unsigned int width, unsigned int height, int colorMode)
{
    int i;
    int r = 0;
    GUI_LOCK();
    __msg("here!");

    for (i = 1; i < GUI_NUM_LAYERS; i++) // search right layer
    {
        if (GUI_LayerInfo.layerState[i] == 0 && GUI_LayerInfo.colorMode[i] == colorMode)
        {
            r = 1;
            break;
        }
    }

    if (!r)
    {
        __inf(" No Layer Fit ");
        GUI_UNLOCK();
        return -1;
    }
    else
    {
        orange.fbaddr[i] = pFBAddr;
        Screen_Context.LCDXSize[i] =  width;
        Screen_Context.LCDYSize[i] =  height;
        Screen_Context.LCDDirection[i] = 0;
        GUI_SelectLayer(i);
        GUI_Init_layer();
        GUI_LayerInfo.layerState[i] = 1;
    }

    GUI_UNLOCK();
    return i;
}
#endif

#if  1
int GUI_CreateLayer(void *pFBAddr, unsigned int width, unsigned int height, int colorMode)
{
    int i;
    int r = 0;
    GUI_LOCK();

    for (i = 0; i < GUI_NUM_LAYERS; i++)
    {
        __inf("layerState %d: %d", i, GUI_LayerInfo.layerState[i]);

        if (GUI_LayerInfo.layerState[i] == 0) //  search for free layer
        {
            r = 1;
            break;
        }
    }

    if (!r)
    {
        __err(" No free layer ");
        GUI_UNLOCK();
        return -1;
    }
    else
    {
        __inf("layer %d, fb addr is 0x%p, w:%d, h:%d.", i, pFBAddr, width, height);
        orange.fbaddr[i] = pFBAddr;
        Screen_Context.LCDXSize[i] =  width;
        Screen_Context.LCDYSize[i] =  height;
        Screen_Context.LCDDirection[i] = gui_scndir;
        GUI_LayerInfo.layerState[i] = 1;
        GUI_LayerInfo.colorMode[i] = colorMode;
        GUI_SelectLayer(i);
        GUI_Init_layer();
    }

    GUI_UNLOCK();
    return i;
}
#endif

int GUI_ReleaseLayer(int index)
{
    GUI_LayerInfo.layerState[index] = 0;
    //esMEMS_Mfree(0,(void*)lemon.fbaddr[index]);
    return 0;
}

int GUI_SelectLayer(unsigned int Index)
{
    unsigned int OldIndex;
    GUI_LOCK();
    OldIndex = GUI_Context.SelLayer;

    if (Index < GUI_NUM_LAYERS)
    {
        GUI_Context.SelLayer = Index;
        GUI_SelectLCD();
    }
    else
    {
        __inf("not enough layers!");
        GUI_UNLOCK();
        return -1;
    }

    GUI_UNLOCK();
    return OldIndex;
}

/*************************** End of file ****************************/
