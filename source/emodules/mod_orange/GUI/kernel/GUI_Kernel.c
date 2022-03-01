/*
*******************************************************************************************************************
*                                                               Mod orange
*                                                   the embedded graphic support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º    GUI_Kernel.c
*
*Author£º       William Wang
*
*Version :      1.1.0
*
*Date:          2009-11-2
*
*Description :  gui kernel implement function
*
*Others :       None at present.
*
* History :
*
* <Author>         <time>         <version>     <description>
*
* William Wang     2009-11-2         1.1.0          Create File
*
*******************************************************************************************************************
*/
#include "GUI_Kernel_i.h"
#include "GUI_Kernel_e.h"

static __gui_nestlock_t  ORANGE_KrnlMutex;
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
void ORANGE_KernelInit(void)
{
    ORANGE_KrnlMutex.ContextSize     = 0;
    GUI_NestLockInit(&ORANGE_KrnlMutex);
    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_Kernel_Exit
*
* author:
*
* date:             2009-10-29
*
* Description:      gui kernel exit
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void ORANGE_KernelExit(void)
{
    ORANGE_KrnlMutex.ContextSize     = 0;
    GUI_NestLockExit(&ORANGE_KrnlMutex);
    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_KrnlLock
*
* author:
*
* date:             2009-10-29
*
* Description:      gui krnllock
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void ORANGE_KrnlLock(void)
{
    GUI_NestLock(&ORANGE_KrnlMutex, NULL);
}

/*
**********************************************************************************************************************
*                                               GUI_KrnlUnlock
*
* author:
*
* date:             2009-10-29
*
* Description:      gui krnlunlock
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void ORANGE_KrnlUnlock(void)
{
    GUI_NestUnlock(&ORANGE_KrnlMutex);
}

/*
**********************************************************************************************************************
*                                               GUI_KrnlAccept
*
* author:
*
* date:             2009-10-29
*
* Description:      gui krnlaccept
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void ORANGE_KrnlAccept(void)
{
    GUI_NestLockAccept(&ORANGE_KrnlMutex, NULL);
}
