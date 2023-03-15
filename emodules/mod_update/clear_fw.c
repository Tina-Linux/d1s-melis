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
#include <libc.h>
#include <kapi.h>
#include <log.h>
#include "image/image.h"
#include "sunxi_update_misc.h"

/*
*********************************************************************************************************
* Function   : FW_GetClear
*              
* Description: 
*              
* Arguments  : 
*              
* Return     : 
*              
* Notes      : 
*              
*********************************************************************************************************
*/
HIMAGE g_h_img;
HIMAGEITEM g_h_item;

__s32 FW_Img_Open(const char *fw_name) {
	HIMAGE h_img;
	
	h_img = Img_Open((const char*)fw_name);
	if (!h_img)
	{
		eLIBs_printf("img is invalid\n");
		return EPDK_FAIL;
	}
	g_h_img = h_img;
	return EPDK_OK;
}

__u64 FW_GetItemSize(__Update_Info_t *info) {
	__u64 size=0;
	HIMAGEITEM h_item;

#ifdef 	UPDATE_DEBUG	
	eLIBs_printf("info->MainType:%s\n", info->MainType);
	eLIBs_printf("info->subType:%s\n", info->subType);
#endif		
	h_item = Img_OpenItem(g_h_img, info->MainType, info->subType);
	if(h_item)
	{
		size = Img_GetItemSize(g_h_img, h_item);
#ifdef 	UPDATE_DEBUG	
		eLIBs_printf("===========================size=%d\n", size);
#endif		
		Img_CloseItem(g_h_img, h_item);
	} else {
		return 0;
	}
	return size;
}
__s32 FW_ImgItem_Open(__Update_Info_t *info) {
	
#ifdef 	UPDATE_DEBUG	
	eLIBs_printf("info->MainType:%s\n", info->MainType);
	eLIBs_printf("info->subType:%s\n", info->subType);
#endif		
	g_h_item = Img_OpenItem(g_h_img, info->MainType, info->subType);
	if(g_h_item)
	{
		return EPDK_OK;
	} else {
		return EPDK_FAIL;
	}
}
__s32 FW_GetItemData(__Update_Info_t *info, __u64 len) {
	if(g_h_item)
	{
#ifdef 	UPDATE_DEBUG	
		eLIBs_printf("info->MainType:%s\n", info->MainType);
		eLIBs_printf("info->subType:%s\n", info->subType);
#endif			
		if (Img_ReadItemData(g_h_img, g_h_item, info->Data, len) != len) {
			return EPDK_FAIL;
		}		
		return EPDK_OK;
	} else {
		return EPDK_FAIL;
	}
}

__s32 FW_ImgItem_Close(void) 
{
	if (g_h_item && g_h_img)
	{
		Img_CloseItem(g_h_img, g_h_item);
	}
	g_h_item = NULL;
	return 0;
}
__s32 FW_Img_Close(void) 
{
	if (g_h_img)
	{
		Img_Close(g_h_img);
	}
	g_h_img = NULL;
	return 0;
}
