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
#include <typedef.h>
#include <log.h>
#include <libc.h>
#include "OAL.h"
#include "ScanStackPri.h"

#define ENTRY_PATH_OFFSET(entry) ((int)((entry)->Path - (__u8 *)(entry)))

/*
****************************************************************************************************
*
*                                       ScanStackAlloc
*
*  Description:
*               -   allocate scan stack for save directory path.
*
*  Parameters:
*               NONE
*
*  Return value:
*   NULL        -   allocate failed.
*   others      -   handle to scan stack.
****************************************************************************************************
*/
HSTACK ScanStackAlloc(void)
{
    ScanStack_t *pStack;
    /* allocate scan stack structure first */
    pStack = (ScanStack_t *)rat_malloc(sizeof(ScanStack_t));

    if (NULL == pStack)
    {
        __wrn("malloc failed\n");
        return NULL;
    }

    /* preallocate STACK_GROWN_LEN space for stack data area,
     * if the scan stack space not enough, will grown scan stack data space.
     */
    pStack->Data = (__u8 *)rat_malloc(STACK_GROWN_LEN);
    /* initialize scan stack structure */
    pStack->Alloc   =   STACK_GROWN_LEN;
    pStack->Size    =   0;
    pStack->Top     =   0;
    return (HSTACK)pStack;
}


/*
****************************************************************************************************
*
*                                       ScanStackPush
*
*  Description:
*               -   push one directory path to scan stack.
*
*  Parameters:
*   hStack      -   handle to scan stack.
*   path        -   pointer to push directory path.
*
*  Return value:
*   EPDK_OK     -   push succeeded.
*   EPDK_FAIL   -   push failed.
****************************************************************************************************
*/
int ScanStackPush(HSTACK hStack, const char *path)
{
    ScanStack_t         *pStack;
    ScanStackEntry_t    *pEntry;
    __u32                    RecLen = 0;

    if (NULL == hStack || NULL == path)
    {
        __wrn("invalid parameter\n");
        return EPDK_FAIL;
    }

    pStack = (ScanStack_t *)hStack;
    /* calc the pushed path needed space, should align by 64bit(8byte) */
    RecLen = RAT_ALIGN(ENTRY_PATH_OFFSET(pEntry) + eLIBs_strlen(path) + 1, sizeof(__u64));

    /* check stack free space enough or not */
    if (RecLen > (pStack->Alloc - pStack->Size))
    {
        /* stack data area free space not enough,
         * gown stack data area space.
         */
        __u8 *pNewData;
        __u32 Len = pStack->Alloc + STACK_GROWN_LEN;
        pNewData = rat_realloc(pStack->Data, Len);

        if (NULL == pNewData)
        {
            __wrn("rat_realloc for scan stack failed\n");
            return EPDK_FAIL;
        }

        pStack->Data = pNewData;
        pStack->Alloc += STACK_GROWN_LEN;
    }

    /* push directory to the end of stack */
    pEntry = (ScanStackEntry_t *)(pStack->Data + pStack->Size);
    pEntry->RecLen  = RecLen;
    pEntry->Prev    = pStack->Top;
    eLIBs_strcpy((char *)(pEntry->Path), path);
    /* adjust stack managemant information */
    pStack->Top       = pStack->Size;
    pStack->Size     += pEntry->RecLen;
    return EPDK_OK;
}


/*
****************************************************************************************************
*
*                                       ScanStackPop
*
*  Description:
*               -   pos the last insert directory path from scan stack.
*
*  Parameters:
*   hStack      -   handle to scan stack.
*
*  Return value:
*   NULL        -   pop failed.
*   others      -   last insert directory path.
****************************************************************************************************
*/
char *ScanStackPop(HSTACK hStack)
{
    ScanStack_t         *pStack;
    ScanStackEntry_t    *pEntry;

    if (NULL == hStack)
    {
        __wrn("invalid parameter\n");
        return NULL;
    }

    pStack  = (ScanStack_t *)hStack;

    /* stack can't be empty */
    if (0 == pStack->Size)
    {
        __wrn("scan stack is empty\n");
        return NULL;
    }

    /* pop the top entry */
    pEntry  = (ScanStackEntry_t *)(pStack->Data + pStack->Top);
    pStack->Top   = pEntry->Prev;
    pStack->Size -= pEntry->RecLen;
    return (char *)(pEntry->Path);
}


/*
****************************************************************************************************
*
*                                       ScanStackEmpty
*
*  Description:
*               -   check scan stack empty or not.
*
*  Parameters:
*   hStack      -   handle to scan stack.
*
*  Return value:
*   EPDK_TRUE   -   check succeeded.
*   EPDK_FALSE  -   check failed.
****************************************************************************************************
*/
__bool ScanStackEmpty(HSTACK hStack)
{
    ScanStack_t *pStack;

    if (NULL == hStack)
    {
        __wrn("invalid parameter\n");
        return EPDK_FALSE;
    }

    pStack  = (ScanStack_t *)hStack;

    if (0 == pStack->Size)
    {
        return EPDK_TRUE;
    }

    return EPDK_FALSE;
}


/*
****************************************************************************************************
*
*                                       ScanStackFree
*
*  Description:
*               -   destory scan stack.
*
*  Parameters:
*   hStack      -   handle to scan stack.
*
*  Return value:
*               NONE
****************************************************************************************************
*/
void ScanStackFree(HSTACK hStack)
{
    ScanStack_t *pStack;

    if (NULL == hStack)
    {
        __wrn("invalid parameter\n");
        return ;
    }

    pStack  = (ScanStack_t *)hStack;

    if (pStack->Data)
    {
        rat_free(pStack->Data);
    }

    rat_free((void *)pStack);
}
