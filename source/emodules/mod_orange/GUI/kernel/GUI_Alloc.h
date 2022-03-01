/*
*******************************************************************************************************************
*                                                               Mod orange
*                                                   the embedded graphic support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    GUI_Alloc.h
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2009-10-29
*
*Description :  gui internal memory definition
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>         <version>     <description>
*
* William Wang     2009-10-29         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __GUI_ALLOC_H__
#define __GUI_ALLOC_H__

#include "GUI_Kernel_i.h"

/*********************************************************************
*
*       Defines, config defaults
*
**********************************************************************
*/

#if ORANGE_ALLOC_SIZE==0
#error ORANGE_ALLOC_SIZE needs to be > 0 when using this module
#endif

/* Permit automatic defragmentation when necessary */
/*碎片整理定义*/
#define ORANGE_ALLOC_AUTDEFRAG 1
#define ORANGE_BLOCK_ALIGN     2       /* 2 means 4 bytes, 1 means 2 bytes      */
/* 1 can be used on 16-bit CPUs and CPUs */
/* which do not require aligned 32-bit   */
/* values (such as x86)                  */

#define ORANGE_MAXBLOCKS       (2 + ORANGE_ALLOC_SIZE / 32)
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define HMEM2PTR(hMem) (void*)&ORANGE_Heap.abHeap[aBlock[hMem].Off]

#define HANDLE   U32

#define MEM_LOCK            GUI_KrnlLock
#define MEM_UNLOCK          GUI_UNLOCK

#endif
