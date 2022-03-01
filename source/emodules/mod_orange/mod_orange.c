/*
*********************************************************************************************************
*                                                   eMOD
*                                  the Easy Portable/Player Operation System
*                                              mod_mmp sub-system
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File   : mod_orange_i.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/
#include "mod_orange_i.h"
#include <log.h>

__orange_t   orange;


/*
****************************************************************************************************
*
*             orange_MInit
*
*  Description:
*       ORANGE_MInit
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 ORANGE_MInit(void)
{
    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             ORANGE_MExit
*
*  Description:
*       ORANGE_MExit
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 ORANGE_MExit(void)
{
    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             ORANGE_MOpen
*
*  Description:
*       ORANGE_MOpen
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__mp *ORANGE_MOpen(__u8 mid, __u8 mod)
{
    orange.used = 1;
    orange.mid = mid;
#ifdef SIM_PC_WIN
    g_nModualID = mid;
#endif
    GUI_Init();
    return (__mp *)&orange;
}
/*
****************************************************************************************************
*
*             ORANGE_MClose
*
*  Description:
*       ORANGE_MClose
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 ORANGE_MClose(__mp *mp)
{
    GUI_Exit();
    esMEMS_Pfree(((__orange_t *)mp)->fbaddr[0], 255);
    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             ORANGE_MRead
*
*  Description:
*       ORANGE_MRead
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__u32 ORANGE_MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}
/*
****************************************************************************************************
*
*             ORANGE_MWrite
*
*  Description:
*       ORANGE_MWrite
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__u32 ORANGE_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    __msg("md_mmp->I am MWrite function!");
    return size * n;
}
/*
****************************************************************************************************
*
*             ORANGE_MIoctrl
*
*  Description:
*       ORANGE_MIoctrl
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
long ORANGE_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    switch (cmd)
    {
        case ORANGE_CMD_GET_FBADDR:
            if (aux == 0)                   /* 图层号，0为图层0                                   */
            {
                return (__s32)orange.fbaddr[0];
            }
            else                            /* 目前由于只支持一个硬件图层，所以其他的值返回NULL   */
            {
                return  0;
            }

        case ORANGE_CMD_SET_KEYID:
            //  drv_key = esMODS_MOpen(aux, 0);
            break;

        default:
            break;
    }

    return EPDK_OK;
}
