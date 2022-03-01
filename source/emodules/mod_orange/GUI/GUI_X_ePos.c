/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              µC/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
---Author-Explanation
*
* 1.00.00 020519 JJL    First release of uC/GUI to uC/OS-II interface
*
*
* Known problems or limitations with current version
*
*    None.
*
*
* Open issues
*
*    None
*********************************************************************************************************
*/

#include "GUI_Private.h"


/*
*********************************************************************************************************
*                                         GLOBAL VARIABLES
*********************************************************************************************************
*/

static  __krnl_event_t  *DispSem;
static  __krnl_event_t  *EventMbox;

static  __krnl_event_t  *KeySem;
static  int             KeyPressed;
static  char            KeyIsInited;


/*
*********************************************************************************************************
*                                        TIMING FUNCTIONS
*
* Notes: Some timing dependent routines of uC/GUI require a GetTime and delay funtion.
*        Default time unit (tick), normally is 1 ms.
*********************************************************************************************************
*/

int  GUI_X_GetTime(void)
{
    return ((int)esKRNL_TimeGet());
}


void  GUI_X_Delay(int period)
{
    INT32U  ticks;
    ticks = (period * OS_TICKS_PER_SEC) / 1000;
    esKRNL_TimeDly((INT16U)ticks);
}


/*
*********************************************************************************************************
*                                          GUI_X_ExecIdle()
*********************************************************************************************************
*/
void GUI_X_ExecIdle(void)
{
    esKRNL_TimeDly(1);
}


/*
*********************************************************************************************************
*                                    MULTITASKING INTERFACE FUNCTIONS
*
* Note(1): 1) The following routines are required only if uC/GUI is used in a true multi task environment,
*             which means you have more than one thread using the uC/GUI API.  In this case the #define
*             GUI_OS 1   needs to be in GUIConf.h
*********************************************************************************************************
*/

void  GUI_X_InitOS(void)
{
    DispSem   = esKRNL_SemCreate(1);
    EventMbox = esKRNL_MboxCreate(0);
}




U32  GUI_X_GetTaskId(void)
{
    __u32 tid;
    tid = esKRNL_GetTIDCur();
    return tid & 0xff;
}
/*
**********************************************************************************************************************
*                                               GUI_X_Lock
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

void  GUI_X_Lock(void)
{
    INT8U  err;
    esKRNL_SemPend(DispSem, 0, &err);
}

/*
**********************************************************************************************************************
*                                               GUI_X_Unlock
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
void  GUI_X_Unlock(void)
{
    esKRNL_SemPost(DispSem);
}
/*
**********************************************************************************************************************
*                                               GUI_X_LockAccept
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
__u16  GUI_X_LockAccept(void)
{
    return esKRNL_SemAccept(DispSem);
}

/*
*********************************************************************************************************
*                                        GUI_X_WaitEvent()
*                                        GUI_X_SignalEvent()
*********************************************************************************************************
*/


void GUI_X_WaitEvent(void)
{
    INT8U  err;
    (void)esKRNL_MboxPend(EventMbox, 0, &err);
}


void GUI_X_SignalEvent(void)
{
    (void)esKRNL_MboxPost(EventMbox, 1);
}

/*
*********************************************************************************************************
*                                      KEYBOARD INTERFACE FUNCTIONS
*
* Purpose: The keyboard routines are required only by some widgets.
*          If widgets are not used, they may be eliminated.
*
* Note(s): If uC/OS-II is used, characters typed into the log window will be placed in the keyboard buffer.
*          This is a neat feature which allows you to operate your target system without having to use or
*          even to have a keyboard connected to it. (useful for demos !)
*********************************************************************************************************
*/
void GUI_X_Init(void)
{
    KeySem = esKRNL_SemCreate(0);
}


static  void  CheckInit(void)
{
    if (KeyIsInited == FALSE)
    {
        KeyIsInited = TRUE;
        GUI_X_Init();
    }
}


int  GUI_X_GetKey(void)
{
    int r;
    r          = KeyPressed;
    CheckInit();
    KeyPressed = 0;
    return (r);
}


int  GUI_X_WaitKey(void)
{
    int    r;
    INT8U  err;
    CheckInit();

    if (KeyPressed == 0)
    {
        esKRNL_SemPend(KeySem, 0, &err);
    }

    r          = KeyPressed;
    KeyPressed = 0;
    return (r);
}


void  GUI_X_StoreKey(int k)
{
    KeyPressed = k;
    esKRNL_SemPost(KeySem);
}


void GUI_TOUCH_X_ActivateX(void)
{
}

void GUI_TOUCH_X_ActivateY(void)
{
}

int  GUI_TOUCH_X_MeasureX(void)
{
    return 0;
}

int  GUI_TOUCH_X_MeasureY(void)
{
    return 0;
}

/*********************************************************************
*
*      Logging: OS dependent

Note:
  Logging is used in higher debug levels only. The typical target
  build does not use logging and does therefor not require any of
  the logging routines below. For a release build without logging
  the routines below may be eliminated to save some space.
  (If the linker is not function aware and eliminates unreferenced
  functions automatically)

*/

void GUI_X_Log(const char *s)
{
    /*__inf(s);*/
}  // mcl  08.11.26
void GUI_X_Warn(const char *s)
{
    /*__inf(s);*/
}
void GUI_X_ErrorOut(const char *s)
{
    /*__inf(s);*/
}
