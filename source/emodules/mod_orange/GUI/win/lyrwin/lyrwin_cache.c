/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lion
* File    : lyrwin_cache.c
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-4 11:24:32
**********************************************************************************************************************
*/

#include "lyrwin_i.h"
#include <mod_display.h>

/*
**********************************************************************************************************************
*                                               GUI_LyrWinCacheOn
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
__s32  GUI_LyrWinCacheOn(void)
{
	__mp *mp_display = NULL;
	
    /*open display driver handle*/
	mp_display = esKSRV_Get_Display_Hld();

    if (!mp_display)
    {
        __err("open display device fail!\n");
    }

    /*start hardware cache layer*/
    esMODS_MIoctrl(mp_display, MOD_DISP_CMD_START_CMD_CACHE, SEL_SCREEN, 0);
    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                               GUI_LyrWinCacheOff
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
__s32  GUI_LyrWinCacheOff(void)
{
    ES_FILE       *p_disp;
	__mp *mp_display = NULL;

    /*open display driver handle*/
	mp_display = esKSRV_Get_Display_Hld();
    if (!mp_display)
    {
        __err("open display device fail!\n");
    }

    esMODS_MIoctrl(mp_display, MOD_DISP_CMD_EXECUTE_CMD_AND_STOP_CACHE, SEL_SCREEN, 0);
#ifdef SIM_PC_WIN
    RefreshDeContent(0);
#endif
    return EPDK_OK;
}
