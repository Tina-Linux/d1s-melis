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
#ifndef  __DSK_KEYTONE_H__
#define  __DSK_KEYTONE_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef enum tag_SET_KEYTON
{
    SET_KEYTONE_OFF = 0,
    SET_KEYTONE_ON,
}
__set_keytone_t;

typedef enum
{
    KEYTONE_KEY_TYPE_NONE = 0,
    KEYTONE_KEY_TYPE_UP = 1,
    KEYTONE_KEY_TYPE_DOWN = 2,
    KEYTONE_KEY_TYPE_REPEAT = 4
} __set_keytone_key_e;


/*
****************************************************************************************************
*Name        : dsk_keytone_init
*Prototype   : __s32 dsk_keytone_init( const char *keytone_file )
*Arguments   : keytone_file  input. the keytone file.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : init the keytone module.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_keytone_init(const char *keytone_file);

/*
****************************************************************************************************
*Name        : dsk_keytone_exit
*Prototype   : __s32 dsk_keytone_exit(void)
*Arguments   : void
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : exit the keytone module.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_keytone_exit(void);

/*
****************************************************************************************************
*Name        : dsk_keytone_on
*Prototype   : __s32 dsk_keytone_on(void)
*Arguments   : void
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : ring the tone.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_keytone_on(void);

/*
****************************************************************************************************
*Name        : dsk_keytone_set_state
*Prototype   : dsk_keytone_set_state(__set_keytone_t state)
*Arguments   : state       input. gate status, on or off.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : set the keytone gate status. on or off.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_keytone_set_state(__set_keytone_t state);

/*
****************************************************************************************************
*Name        : dsk_keytone_get_state
*Prototype   : __s32 dsk_keytone_get_state(__set_keytone_t *p_state)
*Arguments   : p_state     output. the keytone gate status. on or off.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : get the keytone gate status. on or off.
*Other       :
****************************************************************************************************
*/
__s32 dsk_keytone_get_state(__set_keytone_t *p_state);

void dsk_keytone_set_key_type(__u32 key_type);

__u32 dsk_keytone_get_key_type(void);


#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_KEYTONE_H__

/* end of keytone.h */
