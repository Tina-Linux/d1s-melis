
/*
* Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : ionAllocEntry.h
* Description :
* History :
*   Author  : xyliu <xyliu@allwinnertech.com>
*   Date    : 2016/04/13
*   Comment :
*
*
*/
#ifndef ION_ENTRY_H
#define ION_ENTRY_H

#include "typedef.h"

struct ScMemOpsS* __GetIonMemOpsS(void);

// 64M memory by default.
#define TOTAL_MEM_SIZE      (0x4000000)

void esMEMS_Pfree(void *mblk, __u32 npage);
void *esMEMS_Palloc(__u32 npage, __u32 mode);
unsigned long esMEMS_VA2PA(unsigned long vaddr);
void esMEMS_CleanFlushDCacheRegion(void *adr, __u32 bytes);

#endif

