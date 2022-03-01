/*
************************************************************************************************************************
*                                                   language resource
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_langres.c
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.29
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
* Gary.Wang      2010.10.29        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_LANGRES_C__
#define  __DSK_LANGRES_C__


#include "functions_i.h"


static HLANG  volatile  hlang   = NULL;
static __u16  volatile  lang_ID = LANG_ENGLISH_TYPE;


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
int32_t dsk_langres_init(const char *file)
{
    if (file == NULL)
    {
    	__err("errerrerr");
        return EPDK_FAIL;
    }

    if (hlang == NULL)
    {
        hlang = Lang_Open((char *)file, 0);

        if (hlang == NULL)
        {
            __err("Error in openning language res file %s.\n", file);
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}



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
void  dsk_langres_exit(void)
{
    if (hlang != NULL)
    {
        Lang_Close(hlang);
        hlang = NULL;
    }
}



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
int32_t dsk_langres_set_type(uint32_t language_id)
{
    lang_ID = language_id;
    return EPDK_OK;
}



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
int32_t dsk_langres_get_type(void)
{
    return lang_ID;
}



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
int32_t dsk_langres_get_menu_len(uint32_t text_id)
{
    if (hlang == NULL)
    {
        return -1;
    }

    return Lang_GetStringSize(hlang, lang_ID, text_id);
}



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
int32_t dsk_langres_get_menu_text(uint32_t text_id, void *buf, uint32_t size)
{
    __s32   ret;

    eLIBs_memset(buf, 0, size);

    if (hlang == NULL)
    {
        return EPDK_FAIL;
    }

    if (buf == NULL || size == 0)
    {
        return EPDK_FAIL;
    }

    __msg("hlang=%d, lang_ID=%d, text_id=%d, buf=%x, size=%d\n"
          , hlang, lang_ID, text_id, buf, size);
    ret = Lang_GetString(hlang, lang_ID, text_id, buf, size);

    if (0 == ret)
    {
        __msg("************Lang_GetString fail *****************\n");
        return EPDK_FAIL;
    }

    __msg("buf=%s\n", buf);
    return EPDK_OK;
}



#endif     //  ifndef __DSK_LANGRES_C__

/* end of dsk_langres.c */
