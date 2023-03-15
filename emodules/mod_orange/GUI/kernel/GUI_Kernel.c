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
#include "GUI_Kernel_i.h"
#include "GUI_Kernel_e.h"

static __gui_nestlock_t  ORANGE_KrnlMutex;
/*
**********************************************************************************************************************
*                                               GUI_Kernel_Init
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
void ORANGE_KernelInit(void)
{
    ORANGE_KrnlMutex.ContextSize     = 0;
    GUI_NestLockInit(&ORANGE_KrnlMutex);
    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_Kernel_Exit
*
* author:
*
* date:             2009-10-29
*
* Description:      gui kernel exit
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void ORANGE_KernelExit(void)
{
    ORANGE_KrnlMutex.ContextSize     = 0;
    GUI_NestLockExit(&ORANGE_KrnlMutex);
    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_KrnlLock
*
* author:
*
* date:             2009-10-29
*
* Description:      gui krnllock
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void ORANGE_KrnlLock(void)
{
    GUI_NestLock(&ORANGE_KrnlMutex, NULL);
}

/*
**********************************************************************************************************************
*                                               GUI_KrnlUnlock
*
* author:
*
* date:             2009-10-29
*
* Description:      gui krnlunlock
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void ORANGE_KrnlUnlock(void)
{
    GUI_NestUnlock(&ORANGE_KrnlMutex);
}

/*
**********************************************************************************************************************
*                                               GUI_KrnlAccept
*
* author:
*
* date:             2009-10-29
*
* Description:      gui krnlaccept
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void ORANGE_KrnlAccept(void)
{
    GUI_NestLockAccept(&ORANGE_KrnlMutex, NULL);
}
