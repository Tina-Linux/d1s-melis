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
#ifndef  __DSK_LANGRES_H__
#define  __DSK_LANGRES_H__


/*
****************************************************************************************************
*Name        : dsk_langres_init
*Prototype   : __s32 dsk_langres_init( const char *file )
*Arguments   : file    input. the language resouce file.
*Return      : EPDK_OK      succeed
*              EPDK_FAIL    fail
*Description : init language resource module.
*Other       :
****************************************************************************************************
*/
__s32 dsk_langres_init(const char *file);

/*
****************************************************************************************************
*Name        : dsk_langres_exit
*Prototype   : void dsk_langres_exit( void )
*Arguments   : void
*Return      : void
*Description : exit language resource module.
*Other       :
****************************************************************************************************
*/
void  dsk_langres_exit(void);

/*
****************************************************************************************************
*Name        : dsk_langres_set_type
*Prototype   : __s32 dsk_langres_set_type( __u32 language_id )
*Arguments   : language_id    input. language type id.
*Return      : EPDK_OK      succeed
*              EPDK_FAIL    fail
*Description : set language type.
*Other       :
****************************************************************************************************
*/
__s32 dsk_langres_set_type(__u32 language_id);

/*
****************************************************************************************************
*Name        : dsk_langres_get_type
*Prototype   : __s32 dsk_langres_get_type( void )
*Arguments   : void
*Return      : the current language type id.
*Description : get the current language type.
*Other       :
****************************************************************************************************
*/
__s32 dsk_langres_get_type(void);

/*
****************************************************************************************************
*Name        : dsk_langres_get_menu_len
*Prototype   : __s32 dsk_langres_get_menu_len( __u32 text_id )
*Arguments   : text_id   input. the ID of a menu text
*Return      : the length of the menu text string. return value <= 0, means error.
*Description : get the length of the menu text string.
*Other       :
****************************************************************************************************
*/
__s32 dsk_langres_get_menu_len(__u32 text_id);

/*
****************************************************************************************************
*Name        : dsk_langres_get_menu_text
*Prototype   : __s32  dsk_langres_get_menu_text( __u16 text_id, void *buf, __u32 size )
*Arguments   : text_id        input. the ID of a menu text
*              buf            output. the buffer to store the menu text
*              size           input. the size of the buffer
*Return      : EPDK_OK      succeed
*              EPDK_FAIL    fail
*Description : get the content of a menu text according to its ID. the content is a string with '\0'
*              ending.
*Other       :
****************************************************************************************************
*/
__s32 dsk_langres_get_menu_text(__u32 text_id, void *buf, __u32 size);



#endif     //  ifndef __DSK_LANGRES_H__

/* end of dsk_lang.h */
