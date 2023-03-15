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
#include "sys_input.h"
#include <sys_device.h>
#include <log.h>

/*
**********************************************************************************************************************
*                                               INPUT_Init
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
int32_t input_init(void)
{
    __inf("input system initialize....");
    if (INPUT_CoreInit() != EPDK_OK)
    {
        __wrn("INPUT_CoreInit failed");
        return EPDK_FAIL;
    }

    if (INPUT_LKeyDevInit() != EPDK_OK)
    {
        __wrn("INPUT_LkeyDevInit failed");
        INPUT_CoreExit();
        return EPDK_FAIL;
    }
#if 0
    if (INPUT_LMouseDevInit() != EPDK_OK)
    {
        __wrn("INPUT_LMouseDevInit failed");
        INPUT_LKeyDevExit();
        INPUT_CoreExit();
        return EPDK_FAIL;
    }
#endif

#ifdef CONFIG_SUPPORT_TOUCHPANEL
    if (INPUT_LTSDevInit() != EPDK_OK)
    {
        __wrn("INPUT_LTPDevInit failed");
        //INPUT_LMouseDevExit();
        INPUT_LKeyDevExit();
        INPUT_CoreExit();
        return EPDK_FAIL;
    }
#endif
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               INPUT_Exit
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
int32_t input_exit(void)
{
    __inf("input system exit....");
    #ifdef CONFIG_SUPPORT_TOUCHPANEL
    INPUT_LTSDevExit();
    #endif
    #ifdef CONFIG_SUPPORT_MOUSE
    INPUT_LMouseDevExit();
    #endif
    INPUT_LKeyDevExit();
    INPUT_CoreExit();
    return EPDK_OK;
}