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
* File    : GUITOUCH.c
* Purpose : Touch screen manager
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static int  _ScreenX              = 0;    /* x-pos              */
static int  _ScreenY              = 0;    /* y-pos              */
static int  _NumBytesInBuffer     = 0;    /* bytes in rx buffer */
static U8   _Buttons              = 0;    /* button status      */
static U8   _abInBuffer[3];               /* mouse rx buffer    */

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _EvaPacket
*
* Purpose:
*   Process data packet from mouse:
*
*             | D7    D6    D5    D4    D3    D2    D1    D0
*   ----------+----------------------------------------------
*   1st byte  | --    --    Y-    X-     1    --    LB    RB
*   2nd byte  | X7    X6    X5    X4    X3    X2    X1    X0
*   3rd byte  | Y7    Y6    Y5    Y4    Y3    Y2    Y1    Y0
*/
static void _EvaPacket(void)
{
    char a;
    GUI_PID_STATE State;
    _Buttons = _abInBuffer[0] & 0x03;
    a = _abInBuffer[1];

    /* test x move sign. */
    if (_abInBuffer[0] & 0x10)
    {
        a = -a;
        _ScreenX  -= a;
    }        /* direction is negative, move left */
    else
    {
        _ScreenX  += a;
    }

    a = _abInBuffer[2];

    /* test y move sign. */
    if (_abInBuffer[0] & 0x20)
    {
        a = -a;
        _ScreenY  += a;
    }  /* direction is negative, move down */ else

    {
        _ScreenY  -= a;
    }

    /* check min/max positions */
    if (_ScreenX < 0)
    {
        _ScreenX = 0;
    }
    else if (_ScreenX > Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1)
    {
        _ScreenX = Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1;
    }

    if (_ScreenY < 0)
    {
        _ScreenY = 0;
    }
    else if (_ScreenY > Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1)
    {
        _ScreenY = Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1;
    }

    /* signal new mouse data */
    State.x       = _ScreenX;
    State.y       = _ScreenY;
    State.Pressed = _Buttons;
    GUI_MOUSE_StoreState(&State);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_MOUSE_DRIVER_PS2_OnRx
*
* Purpose:
*   Mouse receive interrupt handler. The PS2 mouse interrupt gets
*   in three bytes from the mouse, then wakes up the mouse LSR.
*/
void GUI_MOUSE_DRIVER_PS2_OnRx(unsigned char Data)
{
    if (!_NumBytesInBuffer)
    {
        /* check for start frame */
        if ((Data & 0x0c) == 0x08)
        {
            _abInBuffer[0] = Data;
            _NumBytesInBuffer++;
        }
    }
    else
    {
        _abInBuffer[_NumBytesInBuffer] = Data;
        _NumBytesInBuffer++;

        if (_NumBytesInBuffer >= 3)
        {
            _EvaPacket();
            _NumBytesInBuffer = 0;
        }
    }
}

/*********************************************************************
*
*       GUI_MOUSE_DRIVER_PS2_Init
*/
void GUI_MOUSE_DRIVER_PS2_Init(void)
{
    _NumBytesInBuffer = 0;
}

/*************************** End of file ****************************/
