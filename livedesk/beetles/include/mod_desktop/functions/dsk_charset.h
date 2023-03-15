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
#ifndef  __DSK_CHARSET_H__
#define  __DSK_CHARSET_H__



/*
****************************************************************************************************
*Name        : dsk_get_langres_charset
*Prototype   : __epdk_charset_enm_e  dsk_get_langres_charset( void )
*Arguments   : void
*Return      : the charset of the text of the language resource.
*Description : get the charset of the text of the language resource.
*Other       :
****************************************************************************************************
*/
extern __epdk_charset_enm_e  dsk_get_langres_charset(void);

/*
****************************************************************************************************
*Name        : dsk_set_fs_charset
*Prototype   : __s32 dsk_set_fs_charset( __epdk_charset_enm_e charset )
*Arguments   : charset    input. charset.
*Return      : == EPDK_OK      succeed
*              != EPDK_OK      fail
*Description : set the charset of the file system.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_set_fs_charset(__epdk_charset_enm_e charset);

/*
****************************************************************************************************
*Name        : dsk_get_fs_charset
*Prototype   : __epdk_charset_enm_e  dsk_get_fs_charset( void )
*Arguments   : void
*Return      : the charset of the file system
*Description : get the charset of the file system.
*Other       :
****************************************************************************************************
*/
extern __epdk_charset_enm_e  dsk_get_fs_charset(void);

/*
****************************************************************************************************
*Name        : dsk_set_isn_charset
*Prototype   : __s32 dsk_set_isn_charset( __epdk_charset_enm_e charset )
*Arguments   : charset    input. charset.
*Return      : == EPDK_OK      succeed
*              != EPDK_OK      fail
*Description : set the charset of the isn( internal status number).
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_set_isn_charset(__epdk_charset_enm_e charset);

/*
****************************************************************************************************
*Name        : dsk_get_isn_charset
*Prototype   : __epdk_charset_enm_e  dsk_get_isn_charset( void )
*Arguments   : void
*Return      : the charset of the isn( internal status number)
*Description : get the charset of the isn( internal status number).
*Other       :
****************************************************************************************************
*/
extern __epdk_charset_enm_e  dsk_get_isn_charset(void);



#endif     //  ifndef __DSK_CHARSET_H__

/* end of dsk_charset.h */
