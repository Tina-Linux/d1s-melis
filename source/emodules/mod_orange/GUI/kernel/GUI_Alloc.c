/*
*******************************************************************************************************************
*                                                               Mod orange
*                                                   the embedded graphic support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º    ORANGE_Kernel.c
*
*Author£º       William Wang
*
*Version :      1.1.0
*
*Date:          2009-11-2
*
*Description :  gui memory alloc implement function
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
#ifndef __ORANGE_ALLOC_C__
#define __ORANGE_ALLOC_C__

#include <stddef.h>           /* needed for definition of NULL */
#include <string.h>           /* for memcpy, memset */

#include "GUI_Alloc.h"



/*********************************************************************
*
*       Types
*
**********************************************************************
*/

typedef union
{
    int aintHeap[ORANGE_ALLOC_SIZE / 4];   /* required for proper alignement */
    U8  abHeap[ORANGE_ALLOC_SIZE];
} ORANGE_HEAP;

typedef struct
{
    ORANGE_ALLOC_DATATYPE  Off;       /* Offset of memory area          */
    ORANGE_ALLOC_DATATYPE  Size;      /* usable size of allocated block */
    HANDLE              Next;         /* next handle in linked list     */
    HANDLE              Prev;
} tBlock;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

ORANGE_HEAP ORANGE_Heap;         /* Public for debugging only */

static tBlock aBlock[ORANGE_MAXBLOCKS];

struct
{
    int                   NumUsedBlocks;
    int                   NumFreeBlocks;
    int                   NumFreeBlocksMin; /* For statistical purposes only */
    ORANGE_ALLOC_DATATYPE    NumUsedBytes;
    ORANGE_ALLOC_DATATYPE    NumFreeBytes;
    ORANGE_ALLOC_DATATYPE    NumFreeBytesMin;
} ORANGE_ALLOC;

static char   IsInitialized = 0;

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

/*
**********************************************************************************************************************
*                                               _Size2LegalSize
*
* author:
*
* date:             2009-10-29
*
* Description:       Legal allocation size
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static ORANGE_ALLOC_DATATYPE _Size2LegalSize(ORANGE_ALLOC_DATATYPE size)
{
    return (size + ((1 << ORANGE_BLOCK_ALIGN) - 1)) & ~((1 << ORANGE_BLOCK_ALIGN) - 1);
}


/*
**********************************************************************************************************************
*                                               _GetSize
*
* author:
*
* date:             2009-10-29
*
* Description:       getsize
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static ORANGE_ALLOC_DATATYPE _GetSize(ORANGE_HMEM  hMem)
{
    return aBlock[hMem].Size;
}

/*********************************************************************
*
*       _Free
*/
static void _Free(ORANGE_HMEM hMem)
{
    ORANGE_ALLOC_DATATYPE Size;
    /* Do some error checking ... */
#if ORANGE_DEBUG_LEVEL>0

    /* Block not allocated ? */
    if (aBlock[hMem].Size == 0)
    {
        ORANGE_DEBUG_ERROROUT("ORANGE_ALLOC_Free(): Invalid hMem");
        return;
    }

#endif
    Size = aBlock[hMem].Size;
#ifdef WIN32
    memset(&ORANGE_Heap.abHeap[aBlock[hMem].Off], 0xcc, Size);
#endif
    ORANGE_ALLOC.NumFreeBytes += Size;
    ORANGE_ALLOC.NumUsedBytes -= Size;
    aBlock[hMem].Size = 0;
    {
        int Next = aBlock[hMem].Next;
        int Prev = aBlock[hMem].Prev;
        aBlock[Prev].Next = Next;

        if (Next)
        {
            aBlock[Next].Prev = Prev;
        }
    }
    ORANGE_ALLOC.NumFreeBlocks++;
    ORANGE_ALLOC.NumUsedBlocks--;
}

/*********************************************************************
*
*       _FindFreeHandle
*
* Return value:
*   Free handle
*/
static ORANGE_HMEM _FindFreeHandle(void)
{
    int i;

    for (i = 1; i < ORANGE_MAXBLOCKS; i++)
    {
        if (aBlock[i].Size == 0)
        {
            return i;
        }
    }

    return ORANGE_HMEM_NULL;
}

/*********************************************************************
*
*       _FindHole
*
* Return value:
*   Offset to the memory hole (if available)
*   -1 if not available
*/
static ORANGE_HMEM _FindHole(ORANGE_ALLOC_DATATYPE Size)
{
    int i, iNext;

    for (i = 0; (iNext = aBlock[i].Next) != 0; i = iNext)
    {
        int NumFreeBytes;
        NumFreeBytes = aBlock[iNext].Off - (aBlock[i].Off + aBlock[i].Size);

        if (NumFreeBytes >= Size)
        {
            return i;
        }

        /* Check last block */
        if (ORANGE_ALLOC_SIZE - (aBlock[i].Off + aBlock[i].Size) >= Size)
        {
            return i;
        }
    }

    return -1;
}


/*
**********************************************************************************************************************
*                                               _CreateHole
*
* author:
*
* date:             2009-10-29
*
* Description:      Offset to the memory hole (if available)
*                   -1 if not available
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
static ORANGE_HMEM _CreateHole(ORANGE_ALLOC_DATATYPE Size)
{
    int i, iNext;
    int r = -1;

    for (i = 0; (iNext = aBlock[i].Next) != 0; i = iNext)
    {
        ORANGE_ALLOC_DATATYPE NumFreeBytes = aBlock[iNext].Off - (aBlock[i].Off + aBlock[i].Size);

        if (NumFreeBytes < Size)
        {
            ORANGE_ALLOC_DATATYPE NumBytesBeforeBlock = aBlock[iNext].Off - (aBlock[i].Off + aBlock[i].Size);

            if (NumBytesBeforeBlock)
            {
                U8 *pData = &ORANGE_Heap.abHeap[aBlock[iNext].Off];
                memmove(pData - NumBytesBeforeBlock, pData, aBlock[iNext].Size);
                aBlock[iNext].Off -= NumBytesBeforeBlock;
            }
        }
    }

    /* Check last block */
    if (ORANGE_ALLOC_SIZE - (aBlock[i].Off + aBlock[i].Size) >= Size)
    {
        r = i;
    }

    return r;
}

/*********************************************************************
*
*       _CheckInit
*/
static void _CheckInit(void)
{
    if (!IsInitialized)
    {
        ORANGE_ALLOC_Init();
    }
}

/*********************************************************************
*
*       _Alloc
*/
static ORANGE_HMEM _Alloc(ORANGE_ALLOC_DATATYPE size)
{
    ORANGE_HMEM hMemNew, hMemIns;
    _CheckInit();
    size = _Size2LegalSize(size);

    /* Check if memory is available at all ...*/
    if (size > ORANGE_ALLOC.NumFreeBytes)
    {
        ORANGE_WRN("ORANGE_ALLOC_Alloc: Insufficient memory configured (Trying to alloc % bytes)", size);
        return 0;
    }

    /* Locate free handle */
    if ((hMemNew = _FindFreeHandle()) == 0)
    {
        return 0;
    }

    /* Locate or Create hole of sufficient size */
    hMemIns = _FindHole(size);
#if ORANGE_ALLOC_AUTDEFRAG

    if (hMemIns == -1)
    {
        hMemIns = _CreateHole(size);
    }

#endif

    /* Occupy hole */
    if (hMemIns == -1)
    {
        ORANGE_WRN("ORANGE_ALLOC_Alloc: Could not allocate %d bytes", size);
        return 0;
    }

    {
        ORANGE_ALLOC_DATATYPE  Off     = aBlock[hMemIns].Off + aBlock[hMemIns].Size;
        int                 Next    = aBlock[hMemIns].Next;
        aBlock[hMemNew].Size  = size;
        aBlock[hMemNew].Off   = Off;

        if ((aBlock[hMemNew].Next  = Next) > 0)
        {
            aBlock[Next].Prev = hMemNew;
        }

        aBlock[hMemNew].Prev  = hMemIns;
        aBlock[hMemIns].Next  = hMemNew;
    }

    /* Keep track of number of blocks and av. memory */
    ORANGE_ALLOC.NumUsedBlocks++;
    ORANGE_ALLOC.NumFreeBlocks--;

    if (ORANGE_ALLOC.NumFreeBlocksMin > ORANGE_ALLOC.NumFreeBlocks)
    {
        ORANGE_ALLOC.NumFreeBlocksMin = ORANGE_ALLOC.NumFreeBlocks;
    }

    ORANGE_ALLOC.NumUsedBytes += size;
    ORANGE_ALLOC.NumFreeBytes -= size;

    if (ORANGE_ALLOC.NumFreeBytesMin > ORANGE_ALLOC.NumFreeBytes)
    {
        ORANGE_ALLOC.NumFreeBytesMin = ORANGE_ALLOC.NumFreeBytes;
    }

    return hMemNew;
}

/*********************************************************************
*
*       Exported routines
*
**********************************************************************
*/
/*********************************************************************
*
*       ORANGE_ALLOC_Init
*/
void ORANGE_ALLOC_Init(void)
{
    ORANGE_ALLOC.NumFreeBlocksMin = ORANGE_ALLOC.NumFreeBlocks = ORANGE_MAXBLOCKS - 1;
    ORANGE_ALLOC.NumFreeBytesMin  = ORANGE_ALLOC.NumFreeBytes  = ORANGE_ALLOC_SIZE;
    ORANGE_ALLOC.NumUsedBlocks = 0;
    ORANGE_ALLOC.NumUsedBytes = 0;
    aBlock[0].Size = (1 << ORANGE_BLOCK_ALIGN); /* occupy minimum for a block */
    aBlock[0].Off  = 0;
    aBlock[0].Next = 0;
    IsInitialized = 1;
}

/*********************************************************************
*
*       ORANGE_ALLOC_AllocNoInit
*/
ORANGE_HMEM ORANGE_ALLOC_AllocNoInit(ORANGE_ALLOC_DATATYPE Size)
{
    ORANGE_HMEM hMem;

    if (Size == 0)
    {
        return (ORANGE_HMEM)0;
    }

    ORANGE_KrnlLock();
    hMem = _Alloc(Size);
    ORANGE_KrnlUnlock();
    return hMem;
}

/*********************************************************************
*
*       ORANGE_ALLOC_AllocInit
*
* Purpose:
*   Alloc memory block of the given size and initialize from the given pointer.
*   If the pointer is a NULL pointer, zeroinit
*/
ORANGE_HMEM ORANGE_ALLOC_AllocInit(const void *pInitData, ORANGE_ALLOC_DATATYPE Size)
{
    ORANGE_HMEM hMem;
    ORANGE_KrnlLock();

    if (Size == 0)
    {
        return (ORANGE_HMEM)0;
    }

    hMem = ORANGE_ALLOC_AllocNoInit(Size);

    if (hMem)
    {
        void *pMem;
        pMem = ORANGE_ALLOC_h2p(hMem);

        if (pInitData)
        {
            orange_memcpy(pMem, pInitData, Size);
        }
        else
        {
            orange_memset((U8 *)pMem, 0, Size);
        }
    }

    ORANGE_KrnlUnlock();
    return hMem;
}

/*********************************************************************
*
*       ORANGE_ALLOC_h2p
*/
void *ORANGE_ALLOC_h2p(ORANGE_HMEM  hMem)
{
    if (!hMem)
    {
        ORANGE_WRN("\n"__FILE__ " ORANGE_ALLOC_h2p: illegal argument (0 handle)");
        return 0;
    }

    if (aBlock[hMem].Size == 0)
    {
        ORANGE_WRN("Dereferencing free block");
    }

    return HMEM2PTR(hMem);
}

/*********************************************************************
*
*       ORANGE_ALLOC_GetNumFreeBytes
*/
ORANGE_ALLOC_DATATYPE ORANGE_ALLOC_GetNumFreeBytes(void)
{
    _CheckInit();
    return ORANGE_ALLOC.NumFreeBytes;
}

/*********************************************************************
*
*       ORANGE_ALLOC_GetMaxSize
*
* Purpose:
*   Returns the biggest available blocksize (without relocation).
*/
ORANGE_ALLOC_DATATYPE ORANGE_ALLOC_GetMaxSize(void)
{
    ORANGE_ALLOC_DATATYPE r = 0;
    ORANGE_ALLOC_DATATYPE NumFreeBytes;
    int               i;
    int               iNext;
    ORANGE_KrnlLock();
    _CheckInit();

    for (i = 0; (iNext = aBlock[i].Next) != 0; i = iNext)
    {
        NumFreeBytes = aBlock[iNext].Off - (aBlock[i].Off + aBlock[i].Size);

        if (NumFreeBytes > r)
        {
            r = NumFreeBytes;
        }
    }

    /* Check last block */
    NumFreeBytes = (ORANGE_ALLOC_SIZE - (aBlock[i].Off + aBlock[i].Size));

    if (NumFreeBytes > r)
    {
        r = NumFreeBytes;
    }

    ORANGE_KrnlUnlock();
    return r;
}

/*********************************************************************
*
*       Public code, common memory management functions
*
**********************************************************************
*/
/*********************************************************************
*
*       ORANGE_ALLOC_GetSize
*/
ORANGE_ALLOC_DATATYPE ORANGE_ALLOC_GetSize(ORANGE_HMEM  hMem)
{
    /* Do the error checking first */
#if GUI_CHECK_ARG_EN
    if (!hMem)
    {
        ORANGE_WRN("\n"__FILE__ " ORANGE_ALLOC_h2p: illegal argument (0 handle)");
        return 0;
    }

#endif
    return _GetSize(hMem);
}

/*********************************************************************
*
*       ORANGE_ALLOC_Free
*/
void ORANGE_ALLOC_Free(ORANGE_HMEM hMem)
{
    if (hMem == ORANGE_HMEM_NULL)
    {
        /* Note: This is not an error, it is permitted */
        return;
    }

    ORANGE_KrnlLock();
    _Free(hMem);
    ORANGE_KrnlUnlock();
}


/*********************************************************************
*
*       ORANGE_ALLOC_FreePtr
*/
void ORANGE_ALLOC_FreePtr(ORANGE_HMEM *ph)
{
    ORANGE_KrnlLock();
    ORANGE_ALLOC_Free(*ph);
    *ph = 0;
    ORANGE_KrnlUnlock();
}


/*********************************************************************
*
*       ORANGE_ALLOC_AllocZero
*/
ORANGE_HMEM ORANGE_ALLOC_AllocZero(ORANGE_ALLOC_DATATYPE Size)
{
    ORANGE_HMEM hMem;
    ORANGE_KrnlLock();
    hMem = ORANGE_ALLOC_AllocNoInit(Size);

    if (hMem)
    {
        orange_memset((U8 *)ORANGE_ALLOC_h2p(hMem), 0, Size);  /* Zeroinit ! */
    }

    ORANGE_KrnlUnlock();
    return hMem;
}

/*********************************************************************
*
*       ORANGE_ALLOC_Realloc
*
* Purpose:
*   Reallocate a memory block. This is typically used to grow memory
*   blocks. The contents of the old memory block are copied into the
*   new block (or as much as fits in case of shrinkage).
*   In case of error the old memory block (and its handle) remain
*   unchanged.
*
* Return value:
*   On success:    Handle of newly allocated memory block
*   On error:      0
*/
ORANGE_HMEM ORANGE_ALLOC_Realloc(ORANGE_HMEM hOld, int NewSize)
{
    ORANGE_HMEM    hNew;
    hNew = ORANGE_ALLOC_AllocNoInit(NewSize);

    if (hNew && hOld)
    {
        void    *pNew;
        void    *pOld;
        int     Size;
        int     OldSize;
        OldSize = ORANGE_ALLOC_GetSize(hOld);
        Size = (OldSize < NewSize) ? OldSize : NewSize;
        ORANGE_KrnlLock();
        pNew = ORANGE_ALLOC_h2p(hNew);
        pOld = ORANGE_ALLOC_h2p(hOld);
        orange_memcpy(pNew, pOld, Size);
        ORANGE_KrnlUnlock();
        ORANGE_ALLOC_Free(hOld);
    }

    return hNew;
}


/*************************** End of file **************************/

#endif
