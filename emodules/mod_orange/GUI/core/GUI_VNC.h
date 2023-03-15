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
#ifndef  GUI_VNC_H
#define  GUI_VNC_H

#include "GUI.h"
#include "LCD_Private.h"       /* Required because the Driver API depends on Config currently */

#if GUI_SUPPORT_DEVICES


#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif


/*********************************************************************
*
*       defines
*
**********************************************************************
*/
#define GUI_VNC_NO_ERROR            0
#define GUI_VNC_ERROR_MISC          1
#define GUI_VNC_ERROR_WRONGFORMAT   2

/*********************************************************************
*
*       types
*
**********************************************************************
*/
typedef int (*GUI_tSend)(const U8 *pData, int len, void *pConnectInfo);
typedef int (*GUI_tReceive)(U8 *pData, int len, void *pConnectInfo);

typedef struct GUI_VNC_CONTEXT
{
    struct GUI_VNC_CONTEXT *pNext;
    int LayerIndex;
    /* Connection related data */
    GUI_tSend    pfSend;
    GUI_tReceive pfReceive;
    void        *pConnectInfo;
    U16          ServerIndex;
    /* Display related info */
    const tLCDDEV_APIList *pLayerAPIOrig;
    tLCDDEV_APIList       VNC_LayerAPI;
    int x0Dirty, y0Dirty, x1Dirty, y1Dirty;
    int XSize, YSize;
    /* Status */
    char ClientSupportsHextile;
    char IsBigEndian;
} GUI_VNC_CONTEXT;

/*********************************************************************
*
*       functions
*
**********************************************************************
*/
int  GUI_VNC_Process(GUI_VNC_CONTEXT *pContext, GUI_tSend pfSend, GUI_tReceive pfReceive, void *pConnectInfo);
void GUI_VNC_AttachToLayer(GUI_VNC_CONTEXT *pContext, int LayerIndex);
int  GUI_VNC_GetNumConnections(void);

/****  External routine to link the server to the system ... USER defined ! ****/
int  GUI_VNC_X_StartServer(int LayerIndex, int ServerIndex);

#if defined(__cplusplus)
}
#endif

#endif   /* GUI_SUPPORT_DEVICES */
#endif   /* Avoid multiple inclusion */

/*************************** End of file ****************************/
