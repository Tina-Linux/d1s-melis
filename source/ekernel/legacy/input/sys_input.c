/*
*********************************************************************************************************
*                                                    eMOD
*                                         the melis Operation System
*                                               input sub-system
*                                        input system initialize and exit
*
*                                    (c) Copyright 2010-2012, sunny China
*                                              All Rights Reserved
*
* File   : sys_input.c
* Version: V1.0
* By     : Sunny
* Date   : 2010-7-14
* Note   : this module is for input system initialize and exit.
*********************************************************************************************************
*/
#include "sys_input.h"
#include <sys_device.h>
#include <log.h>

/*
**********************************************************************************************************************
*                                               INPUT_Init
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
int32_t input_init(void)
{
    __inf("input system initialize....");
    if (INPUT_CoreInit() != EPDK_OK)
    {
        __wrn("INPUT_CoreInit failed");
        return EPDK_FAIL;
    }

    if (INPUT_LKeyDevInit() != EPDK_OK)
    {
        __wrn("INPUT_LkeyDevInit failed");
        INPUT_CoreExit();
        return EPDK_FAIL;
    }
#if 0
    if (INPUT_LMouseDevInit() != EPDK_OK)
    {
        __wrn("INPUT_LMouseDevInit failed");
        INPUT_LKeyDevExit();
        INPUT_CoreExit();
        return EPDK_FAIL;
    }
#endif

#if CONFIG_SUPPORT_TOUCHPANEL
    if (INPUT_LTSDevInit() != EPDK_OK)
    {
        __wrn("INPUT_LTPDevInit failed");
        //INPUT_LMouseDevExit();
        INPUT_LKeyDevExit();
        INPUT_CoreExit();
        return EPDK_FAIL;
    }
#endif
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               INPUT_Exit
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
int32_t input_exit(void)
{
    __inf("input system exit....");
    #ifdef CONFIG_SUPPORT_TOUCHPANEL
    INPUT_LTSDevExit();
    #endif
    #ifdef CONFIG_SUPPORT_MOUSE
    INPUT_LMouseDevExit();
    #endif
    INPUT_LKeyDevExit();
    INPUT_CoreExit();
    return EPDK_OK;
}
