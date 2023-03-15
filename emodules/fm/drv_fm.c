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
#include "drv_fm_i.h"
#include "mod_fm.h"
#include <log.h>
#include <rtthread.h>

 extern sunxi_driver_fm_t fm;
 extern   int sunxi_driver_fm_init(void);
 extern   int sunxi_driver_fm_uninit(void);

 __fm_drv_t  fm_drv;
 __krnl_event_t  *fm_lock = NULL;
/*
****************************************************************************************************
*
*             DRV_FM_MInit
*
*  Description:
*       DRV_FM_MInit
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/

int32_t DRV_FM_MInit(void)
{
	fm_lock = esKRNL_SemCreate(1);
    fm_drv.used = 0;
    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             DRV_FM_MExit
*
*  Description:
*       DRV_FM_MExit
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
int32_t DRV_FM_MExit(void)
{
    __u8 err;
    esKRNL_SemDel(fm_lock, OS_DEL_ALWAYS, &err);

    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             DRV_FM_MOpen
*
*  Description:
*       DRV_FM_MOpen
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__mp *DRV_FM_MOpen(__u32 mid, __u32 mod)
{
    fm_drv.mid = mid;
    return (__mp *)&fm_drv;

}
/*
****************************************************************************************************
*
*             DRV_FM_MClose
*
*  Description:
*       DRV_FM_MClose
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
int32_t DRV_FM_MClose(__mp *mp)
{
    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             DRV_FM_MRead
*
*  Description:
*       DRV_FM_MRead
*
*  Parameters:
*
*  Return value:
*       size*n
*
****************************************************************************************************
*/
uint32_t DRV_FM_MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}
/*
****************************************************************************************************
*
*             DRV_FM_MWrite
*
*  Description:
*       DRV_FM_MWrite
*
*  Parameters:
*
*  Return value:
*       size*n
*
****************************************************************************************************
*/
uint32_t DRV_FM_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}
/*
****************************************************************************************************
*
*             DRV_FM_MIoctrl
*
*  Description:
*       DRV_FM_MIoctrl
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/


 long DRV_FM_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    switch (cmd)
    {
        case DRV_CMD_PLUGIN:
        {
            if (fm_drv.used)
            {
                __err("fm already used\n");
                return EPDK_FAIL;
            }
            sunxi_driver_fm_init();

            fm_drv.used = 1;
            fm_drv.fm_dev.status = FM_DRV_STATUS_RECEIVE;
            return EPDK_OK;
        }

        case DRV_CMD_PLUGOUT:
        {
            if (fm_drv.used == 1)
            {
               sunxi_driver_fm_uninit();
            }

            fm_drv.used = 0;
            return EPDK_OK;
        }

        case DRV_CMD_GET_STATUS:
        {
            if (fm_drv.used)
            {
                return DRV_STA_BUSY;
            }
            else
            {
                return DRV_STA_FREE;
            }
        }

        default:
            return EPDK_FAIL;
    }
}

