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

#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Public routines: Using internal memory management
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_ALLOC_Realloc
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
GUI_HMEM GUI_ALLOC_Realloc(GUI_HMEM hOld, int NewSize)
{
    GUI_HMEM hNew;
    hNew = GUI_ALLOC_AllocNoInit(NewSize);

    if (hNew && hOld)
    {
        void *pNew, *pOld;
        int Size, OldSize;
        OldSize = GUI_ALLOC_GetSize(hOld);
        Size = (OldSize < NewSize) ? OldSize : NewSize;
        GUI_LOCK();
        pNew = GUI_ALLOC_h2p(hNew);
        pOld = GUI_ALLOC_h2p(hOld);
        memcpy(pNew, pOld, Size);
        GUI_UNLOCK();
        GUI_ALLOC_Free(hOld);
    }

    return hNew;
}

/*************************** End of file ****************************/
