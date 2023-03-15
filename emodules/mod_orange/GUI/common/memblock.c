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
#include "common_i.h"

/*
**********************************************************************************************************************
*                                               InitMemBlock
*
* author:
*
* date:             2009-7-21
*
* Description:      initmemblock
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void InitMemBlockLink(pmemblock heap, __u32 bd_size, __u32 heap_size)
{
    pthread_mutex_init(&heap->lock, (void *)1);

    if (heap->lock == NULL)
    {
        __wrn("create message queue mutex failed");
        return ;
    }

    heap->heap      = NULL;
    heap->bd_size   = bd_size + sizeof(__u32);
    heap->heap_size = heap_size;
    return ;
}

/*
**********************************************************************************************************************
*                                               MemBlockAlloc
*
* author:
*
* date:             2009-7-21
*
* Description:      memblockalloc
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void *MemBlockAlloc(pmemblock heap)
{
    __u32       i;
    char       *block_data = NULL;
    pthread_mutex_lock(heap->lock);

    if (heap->heap == NULL)
    {
        heap->heap = orange_bmalloc(heap->heap_size * heap->bd_size);

        if (!heap->heap)
        {
            goto ret;
        }

        heap->free = 0;
    }

    block_data = (char *) heap->heap + heap->bd_size * heap->free;

    for (i = heap->free; i < heap->heap_size; i++)
    {
        if (*((__u32 *)block_data) == MBS_FREE)
        {
            heap->free = i + 1;
            *((__u32 *)block_data) = MBS_USED;
            goto ret;
        }

        block_data += heap->bd_size;
    }

    block_data = orange_malloc(heap->bd_size);

    if (!block_data)
    {
        goto ret;
    }

    *((__u32 *)block_data) = MBS_SPECIAL;
ret:
    pthread_mutex_unlock(heap->lock);

    if (block_data)
    {
        return block_data + sizeof(__u32);
    }

    return NULL;
}

/*
**********************************************************************************************************************
*                                               MemBlockFree
*
* author:
*
* date:             2009-7-21
*
* Description:      memblockfree
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void MemBlockFree(pmemblock heap, void *data)
{
    __u32   i;
    char   *block_data;
    pthread_mutex_lock(heap->lock);
    block_data = (char *) data - sizeof(__u32);

    if (*((__u32 *)block_data) == MBS_SPECIAL)
    {
        orange_mfree(block_data);
    }
    else if (*((__u32 *)block_data) == MBS_USED)
    {
        *((__u32 *)block_data) = MBS_FREE;
        i = (block_data - (char *)heap->heap) / heap->bd_size;

        if (heap->free > i)
        {
            heap->free = i;
        }
    }

    pthread_mutex_unlock(heap->lock);
}

/*
**********************************************************************************************************************
*                                               DestroyMemBlockLink
*
* author:
*
* date:             2009-7-21
*
* Description:      destroymemblocklink
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void DestroyMemBlockLink(pmemblock heap)
{
    /*delete memlink sem*/
    pthread_mutex_destroy(heap->lock);
    orange_bfree(heap->heap, heap->heap_size * heap->bd_size);
}
