/*
*******************************************************************************************************************
*                                                               Mod orange
*                                                   the embedded graphic support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º    GUI_Time.c
*
*Author£º       William Wang
*
*Version :      1.1.0
*
*Date:          2009-11-2
*
*Description :  gui time relaxation
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>        <version>     <description>
*
* William Wang     2009-11-2         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __GUI_TIME_C__
#define __GUI_TIME_C__

#include "GUI_Time.h"

#if(GUI_WIN_OS == GUI_OS_EPOS)
/*
**********************************************************************************************************************
*                                               GUI_GetTime
*
* author:
*
* date:             2009-11-2
*
* Description:      gui gettime
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 GUI_GetSysTime(void)
{
    return ((int)esKRNL_TimeGet());
}


/*
**********************************************************************************************************************
*                                               GUI_Delay
*
* author:
*
* date:             2009-11-2
*
* Description:      gui delay
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_TimeDelay(__u32 Period)
{
    __u32  ticks;
    ticks = (Period * OS_TICKS_PER_SEC) / 1000;
    esKRNL_TimeDly((__u16)ticks);
}

#elif(GUI_WIN_OS == GUI_OS_LINUX)

/*
**********************************************************************************************************************
*                                               GUI_GetTime
*
* author:
*
* date:             2009-11-2
*
* Description:      gui gettime
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

int GUI_GetSysTime(void)
{
    return 0;
}


/*
**********************************************************************************************************************
*                                               GUI_Delay
*
* author:
*
* date:             2009-11-2
*
* Description:      gui delay
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_TimeDelay(__u32 Period)
{
}

#elif(GUI_WIN_OS == GUI_OS_WINCE)

#endif

#endif

/*************************** End of file ****************************/
