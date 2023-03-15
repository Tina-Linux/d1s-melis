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
#include "mod_desktop_i.h"

__desktop_t desktp_data;

/*
****************************************************************************************************
*
*             DESKTOP_MInit
*
*  Description:
*       DESKTOP_MInit
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 DESKTOP_MInit(void)
{
    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             DESKTOP_MExit
*
*  Description:
*       DESKTOP_MExit
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 DESKTOP_MExit(void)
{
    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             DESKTOP_MOpen
*
*  Description:
*       DESKTOP_MOpen
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__mp *DESKTOP_MOpen(__u32 mid, __u32 mod)
{
    __log("DESKTOP_MOpen.");
    int ret = -1, value = 0;
    desktp_data.mid  = mid;
	
	ret = esCFG_GetKeyValue("card_product", "card_product_used", (int32_t *)&value, 1);
	if (ret == -1 || value == 0)
	{
		desktp_data.init_id = esMODS_MInstall(BEETLES_APP_ROOT"apps\\init.axf", 0);
	} else if (value) {
		desktp_data.init_id = esMODS_MInstall(BEETLES_APP_ROOT"apps\\init_sd_product.axf", 0);
	}
    desktp_data.init_mp = esMODS_MOpen(desktp_data.init_id, 0);
    return (__mp *)&desktp_data;
}

/*
****************************************************************************************************
*
*             DESKTOP_MClose
*
*  Description:
*       DESKTOP_MClose
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 DESKTOP_MClose(__mp *mp)
{
    esMODS_MClose(desktp_data.init_mp);
    esMODS_MUninstall(desktp_data.init_id);
    desktp_data.init_mp = NULL;
    desktp_data.init_id = 0;
    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             DESKTOP_MRead
*
*  Description:
*       DESKTOP_MRead
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__u32 DESKTOP_MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}
/*
****************************************************************************************************
*
*             DESKTOP_MWrite
*
*  Description:
*       DESKTOP_MWrite
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__u32 DESKTOP_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}
/*
****************************************************************************************************
*
*             DESKTOP_MIoctrl
*
*  Description:
*       DESKTOP_MIoctrl
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
long DESKTOP_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    return EPDK_OK;
}
