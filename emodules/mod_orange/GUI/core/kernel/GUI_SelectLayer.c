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
