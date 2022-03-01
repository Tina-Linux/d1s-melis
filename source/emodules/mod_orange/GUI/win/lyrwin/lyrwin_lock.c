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
* File    : layerlock.c
*
* By      : Steven
* Version : v1.0
* Date    : 2008-11-25 19:07:41
* Note    : Saves/Restores task context with supported OSs. It also uses a resource semaphore.
**********************************************************************************************************************
*/

#include "lyrwin_i.h"

/*********************************************************************
*       Configuration defaults
**********************************************************************
*/
#if GUI_OS

static  __krnl_event_t      *LyrSem;

/*
**********************************************************************************************************************
*                                               GUI_LyrLock
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

void  GUI_LyrLock(void)
{
    //INT8U  err;
    //esKRNL_SemPend(LyrSem, 0, &err);
    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_LyrUnlock
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
void  GUI_LyrUnlock(void)
{
    //esKRNL_SemPost(LyrSem);
    return ;
}
/*
**********************************************************************************************************************
*                                               GUI_LyrLockAccept
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
__u16  GUI_LyrLockAccept(void)
{
    return EPDK_OK;
    //return      esKRNL_SemAccept(LyrSem);
}

/*
**********************************************************************************************************************
*                                               GUI_LyrInitOS
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

void  GUI_LyrInitOS(void)
{
    LyrSem   = esKRNL_SemCreate(1);
    return ;
}

#else

/*********************************************************************
*
*       Dummy Kernel routines
*
* The routines below are dummies in case configuration tells us not
* to use any kernel. In this case the routines below should
* not be required, but it can not hurt to have them. The linker
* will eliminate them anyhow.
*/
void    GUI_LyrUnlock(void);
void    GUI_LyrUnlock(void) {}
void    GUI_LyrLock(void);
void    GUI_LyrLock(void) {}
__u16   GUI_LyrLockAccept(void);
__u16   GUI_LyrLockAccept(void)  {}

#endif

/*************************** End of file ****************************/

