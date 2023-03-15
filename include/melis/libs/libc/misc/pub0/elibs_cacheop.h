/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eLIBs Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : eLIBs
* File    : elibs_cacheop.h
*
* By      : Steven
* Version : v1.0
* Date    : 2008-9-22 16:27:58
**********************************************************************************************************************
*/

#ifndef _ELIBS_CACHEOP_H_
#define _ELIBS_CACHEOP_H_
#include <typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*cache operations*/
void eLIBs_CleanDCache(void);
void eLIBs_CleanFlushDCache(void);
void eLIBs_CleanFlushCache(void);
void eLIBs_FlushDCache(void);
void eLIBs_FlushICache(void);
void eLIBs_FlushCache(void);

void eLIBs_CleanDCacheRegion(void *adr, __u32 bytes);
void eLIBs_CleanFlushDCacheRegion(void *adr, __u32 bytes);
void eLIBs_CleanFlushCacheRegion(void *adr, __u32 bytes);
void eLIBs_FlushDCacheRegion(void *adr, __u32 bytes);
void eLIBs_FlushICacheRegion(void *adr, __u32 bytes);
void eLIBs_FlushCacheRegion(void *adr, __u32 bytes);

#ifdef __cplusplus
}
#endif

#endif /* end of _ELIBS_CACHEOP_H_ */
