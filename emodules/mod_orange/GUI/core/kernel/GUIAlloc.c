/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stddef.h>           /* needed for definition of NULL */
#include <string.h>           /* for memcpy, memset */
#include <kconfig.h>
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Internal memory management
*
**********************************************************************
*/

#ifndef GUI_ALLOC_ALLOC

#if GUI_ALLOC_SIZE==0
#error GUI_ALLOC_SIZE needs to be > 0 when using this module
#endif
static __u32  g_gui_alloc_size = GUI_ALLOC_SIZE;//add by lihaoyi
/*********************************************************************
*
*       Defines, config defaults
*
**********************************************************************
*/

/* Permit automatic defragmentation when necessary */
#ifndef GUI_ALLOC_AUTDEFRAG
#define GUI_ALLOC_AUTDEFRAG 1
#endif

#ifndef GUI_BLOCK_ALIGN        /* 2 means 4 bytes, 1 means 2 bytes      */
#define GUI_BLOCK_ALIGN 2    /* 1 can be used on 16-bit CPUs and CPUs */
#endif                         /* which do not require aligned 32-bit   */
/* values (such as x86)                  */

#ifndef GUI_MAXBLOCKS
#define GUI_MAXBLOCKS (2 + GUI_ALLOC_SIZE / 32)
#endif

#ifndef GUI_ALLOC_LOCATION
#define GUI_ALLOC_LOCATION
#endif

#ifndef GUI_MEM_ALLOC          /* Allows us in some systems to place the GUI memory */
#define GUI_MEM_ALLOC        /* in a different memory space ... eg "__far"        */
#endif

#define Min(v0,v1) ((v0>v1) ? v1 : v0)
#define Max(v0,v1) ((v0>v1) ? v0 : v1)
#define ASSIGN_IF_LESS(v0,v1) if (v1<v0) v0=v1
#define HMEM2PTR(hMem) (void*)&GUI_Heap.abHeap[aBlock[hMem].Off]

#if GUI_MAXBLOCKS >= 256
#define HANDLE U16
#else
#define HANDLE U8
#endif

typedef union
{
    int *aintHeap;   /* required for proper alignement */
    U8  *abHeap;
} GUI_HEAP;

typedef struct
{
    GUI_ALLOC_DATATYPE Off;       /* Offset of memory area          */
    GUI_ALLOC_DATATYPE Size;      /* usable size of allocated block */
    HANDLE Next;         /* next handle in linked list     */
    HANDLE Prev;
} tBlock;


GUI_MEM_ALLOC GUI_HEAP GUI_Heap GUI_ALLOC_LOCATION;         /* Public for debugging only */

//static tBlock aBlock[GUI_MAXBLOCKS];
static tBlock *aBlock;

struct
{
    int       NumUsedBlocks, NumFreeBlocks, NumFreeBlocksMin; /* For statistical purposes only */
    GUI_ALLOC_DATATYPE NumUsedBytes,  NumFreeBytes,  NumFreeBytesMin;
} GUI_ALLOC;

static char   IsInitialized = 0;


/*********************************************************************
*
*       _Size2LegalSize
*
* Return value:
*   Legal allocation size
*/
static GUI_ALLOC_DATATYPE _Size2LegalSize(GUI_ALLOC_DATATYPE size)
{
    return (size + ((1 << GUI_BLOCK_ALIGN) - 1)) & ~((1 << GUI_BLOCK_ALIGN) - 1);
}

static GUI_ALLOC_DATATYPE _GetSize(GUI_HMEM  hMem)
{
    return aBlock[hMem].Size;
}

static void _Free(GUI_HMEM hMem)
{
    GUI_ALLOC_DATATYPE Size;
    GUI_DEBUG_LOG1("\nGUI_ALLOC_Free(%d)", hMem);
    /* Do some error checking ... */
#if GUI_DEBUG_LEVEL>0

    /* Block not allocated ? */
    if (aBlock[hMem].Size == 0)
    {
        GUI_DEBUG_ERROROUT("GUI_ALLOC_Free(): Invalid hMem");
        return;
    }

#endif
    Size = aBlock[hMem].Size;
#ifdef WIN32
    GUI_MEMSET(&GUI_Heap.abHeap[aBlock[hMem].Off], 0xcc, Size);
#endif
    GUI_ALLOC.NumFreeBytes += Size;
    GUI_ALLOC.NumUsedBytes -= Size;
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
    GUI_ALLOC.NumFreeBlocks++;
    GUI_ALLOC.NumUsedBlocks--;
}

static GUI_HMEM _FindFreeHandle(void)
{
    int i;

    for (i = 1; i < GUI_MAXBLOCKS; i++)
    {
        if (aBlock[i].Size == 0)
        {
            return i;
        }
    }

    GUI_DEBUG_ERROROUT1("Insufficient memory handles configured (GUI_MAXBLOCKS == %d (See GUIConf.h))", GUI_MAXBLOCKS);
    return GUI_HMEM_NULL;
}

/*********************************************************************
*
*       _FindHole
*
* Return value:
*   Offset to the memory hole (if available)
*   -1 if not available
*/
static GUI_HMEM _FindHole(GUI_ALLOC_DATATYPE Size)
{
    int i, iNext;

    for (i = 0; (iNext = aBlock[i].Next) != 0; i = iNext)
    {
        int NumFreeBytes = aBlock[iNext].Off - (aBlock[i].Off + aBlock[i].Size);

        if (NumFreeBytes >= Size)
        {
            return i;
        }
    }

    /* Check last block */
    if (g_gui_alloc_size - (aBlock[i].Off + aBlock[i].Size) >= Size)
    {
        return i;
    }

    return -1;
}

/*********************************************************************
*
*       _CreateHole
*
* Return value:
*   Offset to the memory hole (if available)
*   -1 if not available
*/
static GUI_HMEM _CreateHole(GUI_ALLOC_DATATYPE Size)
{
    int i, iNext;
    int r = -1;

    for (i = 0; (iNext = aBlock[i].Next) != 0; i = iNext)
    {
        GUI_ALLOC_DATATYPE NumFreeBytes = aBlock[iNext].Off - (aBlock[i].Off + aBlock[i].Size);

        if (NumFreeBytes < Size)
        {
            GUI_ALLOC_DATATYPE NumBytesBeforeBlock = aBlock[iNext].Off - (aBlock[i].Off + aBlock[i].Size);

            if (NumBytesBeforeBlock)
            {
                U8 *pData = &GUI_Heap.abHeap[aBlock[iNext].Off];
                memmove(pData - NumBytesBeforeBlock, pData, aBlock[iNext].Size);
                aBlock[iNext].Off -= NumBytesBeforeBlock;
            }
        }
    }

    /* Check last block */
    if (g_gui_alloc_size - (aBlock[i].Off + aBlock[i].Size) >= Size)
    {
        r = i;
    }

    return r;
}

static __s32 _CheckInit(void)
{
    __s32 ret = 0;
    if (!IsInitialized)
    {
        ret = GUI_ALLOC_Init();
    }
    return ret;
}

static GUI_HMEM _Alloc(GUI_ALLOC_DATATYPE size)
{
    GUI_HMEM hMemNew, hMemIns;

    if(_CheckInit() < 0)
    {
        return ORANGE_FAIL;
    }
    size = _Size2LegalSize(size);

    /* Check if memory is available at all ...*/
    if (size > GUI_ALLOC.NumFreeBytes)
    {
        GUI_DEBUG_WARN1("GUI_ALLOC_Alloc: Insufficient memory configured (Trying to alloc % bytes)", size);
        return 0;
    }

    /* Locate free handle */
    if ((hMemNew = _FindFreeHandle()) == 0)
    {
        return 0;
    }

    /* Locate or Create hole of sufficient size */
    hMemIns = _FindHole(size);
#if GUI_ALLOC_AUTDEFRAG

    if (hMemIns == -1)
    {
        hMemIns = _CreateHole(size);
    }

#endif

    /* Occupy hole */
    if (hMemIns == -1)
    {
        GUI_DEBUG_ERROROUT1("GUI_ALLOC_Alloc: Could not allocate %d bytes", size);
        return 0;
    }

    {
        GUI_ALLOC_DATATYPE Off = aBlock[hMemIns].Off + aBlock[hMemIns].Size;
        int Next = aBlock[hMemIns].Next;
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
    GUI_ALLOC.NumUsedBlocks++;
    GUI_ALLOC.NumFreeBlocks--;

    if (GUI_ALLOC.NumFreeBlocksMin > GUI_ALLOC.NumFreeBlocks)
    {
        GUI_ALLOC.NumFreeBlocksMin = GUI_ALLOC.NumFreeBlocks;
    }

    GUI_ALLOC.NumUsedBytes += size;
    GUI_ALLOC.NumFreeBytes -= size;

    if (GUI_ALLOC.NumFreeBytesMin > GUI_ALLOC.NumFreeBytes)
    {
        GUI_ALLOC.NumFreeBytesMin = GUI_ALLOC.NumFreeBytes;
    }

    return hMemNew;
}

__s32 GUI_ALLOC_Init(void)
{
    GUI_DEBUG_LOG("\nGUI_ALLOC_Init...");
    GUI_Heap.abHeap = (U8 *)esMEMS_Palloc(((g_gui_alloc_size) + 1023) >> 10,  0);
    if(GUI_Heap.abHeap == NULL )
    {
        __wrn("memdev abHeap palloc fail");
        return ORANGE_FAIL;
    }
    aBlock          = (tBlock *)esMEMS_Palloc((sizeof(tBlock) * GUI_MAXBLOCKS + 1023) >> 10, 0);
    if(aBlock == NULL)
    {
        __wrn("memdev aBlock palloc fail");
        esMEMS_Pfree(GUI_Heap.abHeap, 0);
        return ORANGE_FAIL;
    }
    orange_memset((void *)GUI_Heap.abHeap, 0, ((g_gui_alloc_size + 1023) >> 10) << 10);
    orange_memset((void *)aBlock, 0, ((sizeof(tBlock) * GUI_MAXBLOCKS + 1023) >> 10) << 10);
    GUI_ALLOC.NumFreeBlocksMin = GUI_ALLOC.NumFreeBlocks = GUI_MAXBLOCKS - 1;
    GUI_ALLOC.NumFreeBytesMin  = GUI_ALLOC.NumFreeBytes  = g_gui_alloc_size;
    GUI_ALLOC.NumUsedBlocks = 0;
    GUI_ALLOC.NumUsedBytes = 0;
    aBlock[0].Size = (1 << GUI_BLOCK_ALIGN); /* occupy minimum for a block */
    aBlock[0].Off  = 0;
    aBlock[0].Next = 0;
    IsInitialized = 1;
    return ORANGE_OK;
}

#if defined(CONFIG_SOC_SUN3IW2) || defined(CONFIG_SOC_SUN8IW19) || defined (CONFIG_SOC_SUN20IW1)

void GUI_Memdev_FrameBuffer_Release(__u8 type)
{
    if (IsInitialized)
    {
        if(GUI_Heap.abHeap == NULL || aBlock == NULL)
        {
            return;
        }
        esMEMS_Pfree(GUI_Heap.abHeap, ((g_gui_alloc_size) + 1023) >> 10);
        esMEMS_Pfree(aBlock, (sizeof(tBlock) * GUI_MAXBLOCKS + 1023) >> 10);
        aBlock = NULL;
        IsInitialized = 0;

        if (type == 0)
        {
            //g_gui_alloc_size = (1 << 20) / 16; //1MB   GUI_ALLOC_SIZE/8;
            g_gui_alloc_size = GUI_ALLOC_SIZE / 2; //8MB   GUI_ALLOC_SIZE/8;
        }
        else if (type == 1)
        {
            g_gui_alloc_size = GUI_ALLOC_SIZE;
        }
    }

    return;
}
#elif defined CONFIG_SOC_SUN3IW1

void GUI_Memdev_FrameBuffer_Release(void)
{
    esMEMS_Pfree(GUI_Heap.abHeap, ((GUI_ALLOC_SIZE) + 1023) >> 10);
    esMEMS_Pfree(aBlock, (sizeof(tBlock) * GUI_MAXBLOCKS + 1023) >> 10);
    IsInitialized = 0;
    return;
}

#endif

GUI_HMEM GUI_ALLOC_AllocNoInit(GUI_ALLOC_DATATYPE Size)
{
    GUI_HMEM hMem;

    if (Size == 0)
    {
        return (GUI_HMEM)0;
    }

    GUI_LOCK();
    GUI_DEBUG_LOG2("\nGUI_ALLOC_AllocNoInit... requesting %d, %d avail\n", Size, GUI_ALLOC.NumFreeBytes);
    hMem = _Alloc(Size);
    GUI_DEBUG_LOG1("\nGUI_ALLOC_AllocNoInit : Handle", hMem);
    GUI_UNLOCK();
    return hMem;
}

void *GUI_ALLOC_h2p(GUI_HMEM  hMem)
{
    GUI_ASSERT_LOCK();
#if GUI_DEBUG_LEVEL > 0

    if (!hMem)
    {
        GUI_DEBUG_ERROROUT("\n"__FILE__ " GUI_ALLOC_h2p: illegal argument (0 handle)");
        return 0;
    }

    if (aBlock[hMem].Size == 0)
    {
        GUI_DEBUG_ERROROUT("Dereferencing free block");
    }

#endif
    return HMEM2PTR(hMem);
}

GUI_ALLOC_DATATYPE GUI_ALLOC_GetNumFreeBytes(void)
{
    if(_CheckInit() < 0)
    {
        return ORANGE_FAIL;
    }
    return GUI_ALLOC.NumFreeBytes;
}

/*********************************************************************
*
*       GUI_ALLOC_GetMaxSize
*
* Purpose:
*   Returns the biggest available blocksize (without relocation).
*/
GUI_ALLOC_DATATYPE GUI_ALLOC_GetMaxSize(void)
{
    GUI_ALLOC_DATATYPE r = 0;
    GUI_ALLOC_DATATYPE NumFreeBytes;
    int i, iNext;
    GUI_LOCK();
    
    if(_CheckInit()<0)
    {
        GUI_UNLOCK();
        return ORANGE_FAIL;
    }
    for (i = 0; (iNext = aBlock[i].Next) != 0; i = iNext)
    {
        NumFreeBytes = aBlock[iNext].Off - (aBlock[i].Off + aBlock[i].Size);

        if (NumFreeBytes > r)
        {
            r = NumFreeBytes;
        }
    }

    /* Check last block */
    NumFreeBytes = (g_gui_alloc_size - (aBlock[i].Off + aBlock[i].Size));

    if (NumFreeBytes > r)
    {
        r = NumFreeBytes;
    }

    GUI_UNLOCK();
    return r;
}

#else

/*********************************************************************
*
*       External memory management functions
*
* The functions below will generate code only if the GUI memory
* management is not used (GUI_ALLOC_ALLOC defined).
*
* Note:
* The memory block allocated is bigger than the requested one, as we
* store some add. information (size of the memory block) there.
*
**********************************************************************
*/

typedef struct
{
    union
    {
        GUI_ALLOC_DATATYPE Size;
        int Dummy;               /* Needed to guarantee alignment on 32 / 64 bit CPUs */
    } Info;      /* Unnamed would be best, but is not supported by all compilers */
} INFO;

static GUI_ALLOC_DATATYPE _GetSize(GUI_HMEM  hMem)
{
    INFO *pInfo;
    pInfo = (INFO *)GUI_ALLOC_H2P(hMem);
    return pInfo->Info.Size;
}

static void _Free(GUI_HMEM  hMem)
{
    GUI_ALLOC_FREE(hMem);
}

GUI_HMEM GUI_ALLOC_AllocNoInit(GUI_ALLOC_DATATYPE Size)
{
    GUI_HMEM hMem;

    if (Size == 0)
    {
        return (GUI_HMEM)0;
    }

    hMem = GUI_ALLOC_ALLOC(Size + sizeof(INFO));

    /* Init info structure */
    if (hMem)
    {
        INFO *pInfo;
        pInfo = (INFO *)GUI_ALLOC_H2P(hMem);
        pInfo->Info.Size = Size;
    }

    return hMem;
}

void GUI_Memdev_FrameBuffer_Release(void)
{
    return;
}

void *GUI_ALLOC_h2p(GUI_HMEM  hMem)
{
    U8 *p = (U8 *)GUI_ALLOC_H2P(hMem);   /* Pointer to memory block from memory manager */
    p += sizeof(INFO);                   /* Convert to pointer to usable area */
    return p;
}

GUI_ALLOC_DATATYPE GUI_ALLOC_GetMaxSize(void)
{
    return GUI_ALLOC_GETMAXSIZE();
}

__s32 GUI_ALLOC_Init(void)
{
#ifdef GUI_ALLOC_INIT
    GUI_ALLOC_INIT();
#endif
}

#endif

/*********************************************************************
*
*       Public code, common memory management functions
*
**********************************************************************
*/
GUI_ALLOC_DATATYPE GUI_ALLOC_GetSize(GUI_HMEM  hMem)
{
    /* Do the error checking first */
#if GUI_DEBUG_LEVEL>0
    if (!hMem)
    {
        GUI_DEBUG_ERROROUT("\n"__FILE__ " GUI_ALLOC_h2p: illegal argument (0 handle)");
        return 0;
    }

#endif
    return _GetSize(hMem);
}

void GUI_ALLOC_Free(GUI_HMEM hMem)
{
    if (hMem == GUI_HMEM_NULL)   /* Note: This is not an error, it is permitted */
    {
        return;
    }

    GUI_LOCK();
    GUI_DEBUG_LOG1("\nGUI_ALLOC_Free(%d)", hMem);
    _Free(hMem);
    GUI_UNLOCK();
}

void GUI_ALLOC_FreePtr(GUI_HMEM *ph)
{
    GUI_LOCK();
    GUI_ALLOC_Free(*ph);
    *ph = 0;
    GUI_UNLOCK();
}
