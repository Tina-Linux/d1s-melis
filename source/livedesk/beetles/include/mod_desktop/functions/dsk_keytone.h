/*
************************************************************************************************************************
*                                                       keytone
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : keytone.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.13
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2010.10.13        1.0         build the file
*
************************************************************************************************************************
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
