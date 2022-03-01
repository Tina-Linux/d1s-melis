/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : mod_init_i.h
* By        : Andy.zhang
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2010-9-25 17:10:55  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "mod_init_i.h"

__init_mod_t init_data;

extern void application_init_process(void *arg);

__s32 INIT_MInit(void)
{
    return EPDK_OK;
}

__s32 INIT_MExit(void)
{
    return EPDK_OK;
}

__mp *INIT_MOpen(__u32 mid, __u32 mod)
{
    __msg("----------------------INIT_MOpen --------------------------\n");
    init_data.mid      = mid;
    init_data.init_tid = esKRNL_TCreate(application_init_process, NULL, 0x10000, KRNL_priolevel4);

    if (init_data.init_tid == 0)
    {
        __err(" application_init_process main thread create error\n");
        return NULL;
    }

    return (__mp *)&init_data;
}

__s32 INIT_MClose(__mp *mp)
{
    while (esKRNL_TDelReq(init_data.init_tid) != OS_TASK_NOT_EXIST)
    {
        esKRNL_TimeDly(1);
    }

    return EPDK_OK;
}

__u32 INIT_MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}

__u32 INIT_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}

long INIT_MIoctrl(__mp *mp, uint32_t cmd, int32_t aux, void *pbuffer)
{
    return EPDK_OK;
}



