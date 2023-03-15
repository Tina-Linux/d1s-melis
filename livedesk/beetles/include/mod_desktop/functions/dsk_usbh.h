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
#ifndef  __DSK_USBH_H__
#define  __DSK_USBH_H__

#ifdef __cplusplus
extern "C" {
#endif


/*
****************************************************************************************************
*Name        : dsk_send_insmod_usbh_cmd
*Prototype   : __s32 dsk_send_insmod_usbh_cmd(void)
*Arguments   : void
*Return      : EPDK_OK     发送成功
*              EPDK_FAIL   发送失败
*Description : 向usbh_monitor发送加载host驱动命令
*Other       : 命令发送完毕后，delay 2s 确保加载成功
****************************************************************************************************
*/
extern __s32 dsk_send_insmod_usbh_cmd(void);

/*
****************************************************************************************************
*Name        : dsk_send_rmmod_usbh_cmd
*Prototype   : __s32 dsk_send_rmmod_usbh_cmd(void)
*Arguments   : void
*Return      : EPDK_OK     发送成功
*              EPDK_FAIL   发送失败
*Description : 向usbh_monitor发送卸载host驱动命令
*Other       : 命令发送完毕后，delay 1s 确保卸载成功
****************************************************************************************************
*/
extern __s32 dsk_send_rmmod_usbh_cmd(void);

/*
****************************************************************************************************
*Name        : dsk_usbh_is_working
*Prototype   : __bool dsk_usbh_is_working(void)
*Arguments   : void
*Return      : EPDK_TRUE     usb host is working
*              EPDK_FAIL     usb host is NOT working
*Description : check whether the usb host is working or not.
*Other       :
****************************************************************************************************
*/
__bool dsk_usbh_is_working(void);



#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_USBH_H__

/* end of dsk_usbh.h */
