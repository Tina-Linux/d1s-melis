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
