/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Charset Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : charset.c
* By      : kevin.z
* Version : v2.0
* Date    : 2010-11-24 9:24
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-24 9:24     kevin.z     2.0     build the file.
*********************************************************************************************************
*/
#include <typedef.h>
#include <mod_defs.h>
#include <mod_charset.h>
#include <log.h>

static uint32_t    CharsetMid = 0;
int32_t UTF8_uni2char(const uint16_t uni, uint8_t *out, uint32_t boundlen);
int32_t UTF8_char2uni(const uint8_t *rawstring, uint32_t boundlen, uint16_t *uni);
uint32_t UTF8_get_charset2upper_table(void *buf, uint32_t size);
uint32_t UTF8_get_charset2lower_table(void *buf, uint32_t size);

/*
*********************************************************************************************************
*                                   CHARSET INITIALISE
*
* Description: charset initialise.
*
* Arguments  : mode     mode of charset initialise;
*              arg      argument for charset initialise;
*
* Returns    : result, EPDK_OK/EPDK_FAIL
*
* Note       : charset is an external module, just create a link to access the module here.
*********************************************************************************************************
*/
int32_t eschs_init(uint32_t mode, void *p_arg)
{
    //install the charset module
    CharsetMid = esMODS_MInstall("d:\\mod\\charset.mod", 0);
    if (!CharsetMid)
    {
        __wrn("try to install charset module failed!");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   CHARSET EXIT
*
* Description: charset exit.
*
* Arguments  : none
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void esCHS_Exit(void)
{
    if (CharsetMid)
    {
        esMODS_MUninstall(CharsetMid);
        CharsetMid = 0;
    }

    return;
}


/*
*********************************************************************************************************
*                               TRANSLATE UNICODE TO THE GIVEN CHARSET
*
* Description: translate unicode to the given charset.
*
* Arguments  : type
*              uni
*              str
*              len
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int32_t esCHS_Uni2Char(int32_t type, uint16_t uni, uint8_t *str, uint32_t len)
{
    if (CharsetMid)
    {
        return CHS_Uni2Char(type, uni, str, len);
    }
    else
    {
        //use default charset (utf8)
        return UTF8_uni2char(uni, str, len);
    }
}


/*
*********************************************************************************************************
*                               TRANSLIATE THE GIVEN CHARSET TO UNICODE
*
* Description: translate the given charset to unicode.
*
* Arguments  : type
*              str
*              len
*              uni
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int32_t esCHS_Char2Uni(int32_t type, const uint8_t *str, uint32_t len, uint16_t *uni)
{
    if (CharsetMid)
    {
        return CHS_Char2Uni(type, str, len, uni);
    }
    else
    {
        return UTF8_char2uni(str, len, uni);
    }
}


/*
*********************************************************************************************************
*                                   GET CHARSET UPPER TABLE
*
* Description: get charset upper table.
*
* Arguments  : type
*              buf
*              size
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
uint32_t esCHS_GetChUpperTbl(int32_t type, void *buf, uint32_t size)
{
    if (CharsetMid)
    {
        return CHS_GetChUpperTbl(type, buf, size);
    }

    return UTF8_get_charset2upper_table(buf, size);
}


/*
*********************************************************************************************************
*                                   GET CHARSET LOWER TABLE
*
* Description: get charset lower table.
*
* Arguments  : type
*              buf
*              size
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
uint32_t esCHS_GetChLowerTbl(int32_t charset_type, void *buf, uint32_t size)
{
    if (CharsetMid)
    {
        return CHS_GetChLowerTbl(charset_type, buf, size);
    }

    return UTF8_get_charset2lower_table(buf, size);
}
