/*
 * ===========================================================================================
 *
 *       Filename:  cleanflushcachebyregion.c
 *
 *    Description:  cache op collect.
 *
 *        Version:  Melis3.0
 *         Create:  2019-04-11 14:03:43
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-04-11 14:03:55
 *
 * ===========================================================================================
 */
#include <typedef.h>
#include <kapi.h>
#include <const.h>

void eLIBs_CleanDCacheRegion(void *adr, __u32 bytes)
{
    esMEMS_CleanDCacheRegion(adr, bytes);
}

void eLIBs_CleanFlushDCacheRegion(void *adr, __u32 bytes)
{
    esMEMS_CleanFlushDCacheRegion(adr, bytes);
}

void eLIBs_CleanFlushCacheRegion(void *adr, __u32 bytes)
{
    esMEMS_CleanFlushCacheRegion(adr, bytes);
}

void eLIBs_FlushICacheRegion(void *adr, __u32 bytes)
{
    esMEMS_FlushICacheRegion(adr, bytes);
}

void eLIBs_FlushDCacheRegion(void *adr, __u32 bytes)
{
    esMEMS_FlushDCacheRegion(adr, bytes);
}

void eLIBs_FlushCacheRegion(void *adr, __u32 bytes)
{
    esMEMS_CleanFlushCacheRegion(adr, 1 << CLINE);
    esMEMS_CleanFlushCacheRegion((void *)((__u32)adr + bytes), 1);

    if (bytes > (1 << CLINE))
    {
        esMEMS_FlushCacheRegion((void *)((__u32)adr + (1 << CLINE)), bytes);
    }
}
