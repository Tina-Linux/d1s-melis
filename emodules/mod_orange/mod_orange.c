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
#include "mod_orange_i.h"
#include <log.h>

__orange_t   orange;


/*
****************************************************************************************************
*
*             orange_MInit
*
*  Description:
*       ORANGE_MInit
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 ORANGE_MInit(void)
{
    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             ORANGE_MExit
*
*  Description:
*       ORANGE_MExit
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 ORANGE_MExit(void)
{
    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             ORANGE_MOpen
*
*  Description:
*       ORANGE_MOpen
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__mp *ORANGE_MOpen(__u8 mid, __u8 mod)
{
    orange.used = 1;
    orange.mid = mid;
#ifdef SIM_PC_WIN
    g_nModualID = mid;
#endif
    GUI_Init();
    return (__mp *)&orange;
}
/*
****************************************************************************************************
*
*             ORANGE_MClose
*
*  Description:
*       ORANGE_MClose
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 ORANGE_MClose(__mp *mp)
{
    GUI_Exit();
    esMEMS_Pfree(((__orange_t *)mp)->fbaddr[0], 255);
    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             ORANGE_MRead
*
*  Description:
*       ORANGE_MRead
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__u32 ORANGE_MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}
/*
****************************************************************************************************
*
*             ORANGE_MWrite
*
*  Description:
*       ORANGE_MWrite
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__u32 ORANGE_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    __msg("md_mmp->I am MWrite function!");
    return size * n;
}
/*
****************************************************************************************************
*
*             ORANGE_MIoctrl
*
*  Description:
*       ORANGE_MIoctrl
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
long ORANGE_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    switch (cmd)
    {
        case ORANGE_CMD_GET_FBADDR:
            if (aux == 0)                   /* 图层号，0为图层0                                   */
            {
                return (__s32)orange.fbaddr[0];
            }
            else                            /* 目前由于只支持一个硬件图层，所以其他的值返回NULL   */
            {
                return  0;
            }

        case ORANGE_CMD_SET_KEYID:
            //  drv_key = esMODS_MOpen(aux, 0);
            break;

        default:
            break;
    }

    return EPDK_OK;
}
