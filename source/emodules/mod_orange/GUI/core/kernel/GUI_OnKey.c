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
* File    : GUI_OnKey.c
* Purpose : Implementation of GUI_StoreKeyMsg
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

#if GUI_WINSUPPORT

static int _KeyMsgCnt;
static struct
{
    int Key;
    int PressedCnt;
} _KeyMsg;

#endif

static int _Key;

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/

GUI_KEY_MSG_HOOK *GUI_pfKeyMsgHook;

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GetKey
*/
int GUI_GetKey(void)
{
    int r = _Key;
    _Key = 0;
    return r;
}

/*********************************************************************
*
*       GUI_StoreKey
*/
void GUI_StoreKey(int Key)
{
    if (!_Key)
    {
        _Key = Key;
    }

    GUI_X_SIGNAL_EVENT();
}

/*********************************************************************
*
*       GUI_ClearKeyBuffer
*/
void GUI_ClearKeyBuffer(void)
{
    while (GUI_GetKey());
}

/*********************************************************************
*
*       GUI_StoreKeyMsg
*/
void GUI_StoreKeyMsg(int Key, int PressedCnt)
{
#if GUI_WINSUPPORT    /* If 0, WM will not generate any code */
    _KeyMsg.Key = Key;
    _KeyMsg.PressedCnt = PressedCnt;
    _KeyMsgCnt = 1;
    /*remove warning*/
   // _KeyMsgCnt = _KeyMsgCnt;
   // _KeyMsg = _KeyMsg;
    GUI_X_SIGNAL_EVENT();
#else
    GUI_USE_PARA(PressedCnt);
    GUI_StoreKey(Key);
#endif
}

/*********************************************************************
*
*       GUI_PollKeyMsg
*/
#if GUI_WINSUPPORT    /* If 0, WM will not generate any code */
/*
int GUI_PollKeyMsg(void)
{
    int r = 0;
    GUI_LOCK();
    if (_KeyMsgCnt)
    {
        int Key;
        _KeyMsgCnt--;
        Key = _KeyMsg.Key;
        WM_OnKey(Key, _KeyMsg.PressedCnt);
        if (_KeyMsg.PressedCnt == 1)
        {
            GUI_StoreKey(Key);
        }
        r = 1;              // We have done something
    }
    GUI_UNLOCK();
    return r;
}
*/
extern int keyfile;
static __u32 __keydown;
int GUI_PollKeyMsg(void)
{
    int r = 0;
    __u32 key;
    GUI_LOCK();

    if (keyfile)
    {
        key = ioctl(keyfile, DRV_KEY_CMD_QRYKEY,  0);

        if (key)
        {
            /// GUI_StoreKey(key);
            if (key == DRV_KEY_KEYUP)
            {
                if (__keydown)
                {
                    WM_OnKey(__keydown, 0);
                }

                __keydown = 0;
            }
            else
            {
                WM_OnKey(key, 1);
                __keydown = key;
                //GUI_StoreKey(key);
            }

            r = 1;
        }
    }

    GUI_UNLOCK();
    return r;
}
#endif

/*********************************************************************
*
*       GUI_SendKeyMsg
*
* Purpose:
*   Send the key to a window using the window manager (if available).
*   If no window is ready to take the input, we call the store routine
*   and wait for somebody to poll the buffer.
*/
void GUI_SendKeyMsg(int Key, int PressedCnt)
{
#if GUI_WINSUPPORT    /* If 0, WM will not generate any code */

    if (!WM_OnKey(Key, PressedCnt))
    {
        GUI_StoreKeyMsg(Key, PressedCnt);
    }

#else
    GUI_StoreKeyMsg(Key, PressedCnt);
#endif
}

/*************************** End of file ****************************/
