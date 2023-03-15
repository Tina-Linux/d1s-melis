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
#include "lyrwin_i.h"

/*********************************************************************
*       Configuration defaults
**********************************************************************
*/
#if GUI_OS

static  __krnl_event_t      *LyrSem;

/*
**********************************************************************************************************************
*                                               GUI_LyrLock
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

void  GUI_LyrLock(void)
{
    //INT8U  err;
    //esKRNL_SemPend(LyrSem, 0, &err);
    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_LyrUnlock
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
void  GUI_LyrUnlock(void)
{
    //esKRNL_SemPost(LyrSem);
    return ;
}
/*
**********************************************************************************************************************
*                                               GUI_LyrLockAccept
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
__u16  GUI_LyrLockAccept(void)
{
    return EPDK_OK;
    //return      esKRNL_SemAccept(LyrSem);
}

/*
**********************************************************************************************************************
*                                               GUI_LyrInitOS
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

void  GUI_LyrInitOS(void)
{
    LyrSem   = esKRNL_SemCreate(1);
    return ;
}

#else

/*********************************************************************
*
*       Dummy Kernel routines
*
* The routines below are dummies in case configuration tells us not
* to use any kernel. In this case the routines below should
* not be required, but it can not hurt to have them. The linker
* will eliminate them anyhow.
*/
void    GUI_LyrUnlock(void);
void    GUI_LyrUnlock(void) {}
void    GUI_LyrLock(void);
void    GUI_LyrLock(void) {}
__u16   GUI_LyrLockAccept(void);
__u16   GUI_LyrLockAccept(void)  {}

#endif

/*************************** End of file ****************************/
