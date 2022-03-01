/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eLIBs Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : ASCII C
* File    : elibs_stdlib.c
*
* By      : Steven
* Version : v1.0
* Date    : 2008-8-22 9:03:42
* Descritp:C Library function implementation. To avoid usage of C runtime library function, the ePDKS
*          uses its own implementation of the required functions. Do not use them yourself in the
*          application, because they might be limited to the functionality required by the ePDK.
*          They are NOT meant to be a replacement for an ANSI C runtime library !
**********************************************************************************************************************
*/
#include <typedef.h>
#include <kapi.h>
#include <mod_slib.h>
#include <log.h>
#include <stdio.h>

/*
******************************************************************************************************************
*
*Function Name : int_to_string_dec
*
*Description : This function is to convert an 'int' data 'input' to a string in decimalism, and the string
*              converted is in 'str'.
*
*Input : int input : 'int' data to be converted.
*        char * str : the start address of the string storing the converted result.
*
*Output : void
*
*call for :
*
*Others : None at present.
*
*******************************************************************************************************************
*/
void eLIBs_int2str_dec(int input, char *str)
{
    SLIB_int2str_dec(input, str);
}

/*
******************************************************************************************************************
*
*Function Name : Uint_to_string_dec
*
*Description : This function is to convert an 'unsigned int' data 'input' to a string in decimalism, and the
*              string converted is in 'str'.
*
*Input : int input : 'unsigned int' data to be converted.
*        char * str : the start address of the string storing the converted result.
*
*Output : void
*
*call for :
*
*Others : None at present.
*
*******************************************************************************************************************
*/
void eLIBs_uint2str_dec(unsigned int input, char *str)
{
    SLIB_uint2str_dec(input, str);
}

/*
******************************************************************************************************************
*
*Function Name : int_to_string_hex
*
*Description : This function is to convert an 'int' data 'input' to a string in hex, and the string
*              converted is in 'str'.
*
*Input : int input : 'int' data to be converted.
*        char * str : the start address of the string storing the converted result.
*        int hex_flag : hex_flag is just a option to distinguish whether hex format is '0f' or '0F'.
*
*Output : void
*
*call for :
*
*Others : None at present.
*
*******************************************************************************************************************
*/
void eLIBs_int2str_hex(int input, char *str, int hex_flag)
{
    SLIB_int2str_hex(input, str, hex_flag);
}

/*******************************************************************************
*
*             eLIBs_atoi
*
  Description:
  FS internal function. Convert string to int. The function stops with
  the first character it cannot convert. It expects decimal numbers only.
  It can handle +/- at the beginning and leading 0. It cannot handle
  HEX or any other numbers.

  Parameters:
  s           - Pointer to a zero terminated string.

  Return value:
  0           - In case of any problem or if the converted value is zero.
  !=0         - Integer value of the converted string.
*/
int eLIBs_atoi(const char *s)
{
    return SLIB_atoi(s);
}

/*
**********************************************************************************************************************
*                                               eLIBC_toupper
*
* Description: FS internal function. Convert a lowecase letter to a corresponding
*              uppercase letter.
*
* Arguments  : c           - Letter to convert.
*
* Returns    : Corresponding uppercase character.
*
* Notes      :
*
**********************************************************************************************************************
*/
int eLIBs_toupper(int c)
{
    return SLIB_toupper(c);
}

/*
**********************************************************************************************************************
*                                               eLIBs_isspace
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int eLIBs_isspace(int ch)
{
    return SLIB_isspace(ch);
}

/*
**********************************************************************************************************************
*                                               eLIBs_strtol
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
long eLIBs_strtol(const char *str, const char **err, int base)
{
    return SLIB_strtol(str, err, base);
}

/*
**********************************************************************************************************************
*                                               eLIBs_ramdom
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int eLIBs_ramdom(int max)
{
    return esKSRV_Random(max);
}


/*
**********************************************************************************************************************
*                                               eLIBs_malloc
*
* Description: request memory from system heap.
*
* Arguments  : size     size of memory need be requested.
*
* Returns    : the pointer of the memory requested. NULL means request failed.
*
**********************************************************************************************************************
*/
void *eLIBs_malloc(size_t size)
{
    return esMEMS_Malloc(0, size);
}

/*
**********************************************************************************************************************
*                                               eLIBs_free
*
* Description: free memory.
*
* Arguments  : ptr  memory address need be free.
*
* Returns    : none.
*
**********************************************************************************************************************
*/
void eLIBs_free(void *ptr)
{
    /* if ptr is NULL, the pointer is ignored and immediately returns. */
    if (NULL == ptr)
    {
        return ;
    }

    esMEMS_Mfree(0, ptr);
}

/*
**********************************************************************************************************************
*                                               eLIBs_realloc
*
* Description: reallocate memory from system heap.
*
* Arguments  : size     size of memory need be requested.
*
* Returns    : the pointer of the memory requested. NULL means request failed.
*
**********************************************************************************************************************
*/
void *eLIBs_realloc(void *ptr, size_t size)
{
    return esMEMS_Realloc(0, ptr, size);
}

/*******************************************************************************
*
*             eLIBs_atof
*
  Description:
  FS internal function. Convert string to float. The function stops with
  the first character it cannot convert. It expects decimal numbers only.
  It can handle +/- at the beginning and leading 0. It cannot handle
  HEX or any other numbers.

  Parameters:
  s           - Pointer to a zero terminated string.

  Return value:
  0           - In case of any problem or if the converted value is zero.
  !=0         - Float value of the converted string.
*/
double eLIBs_atof(const char *s)
{
    return SLIB_atoi(s);
}

__s8 eLIBs_sscanf(const char *buf, const char *fmt, ...)
{
    va_list args;
    __s8 rc;
    va_start(args, fmt);
    rc = SLIB_sscanf(buf, fmt, args);
    va_end(args);
    return rc;
}

void _exit(int status)
{
    __err("error, status = %d.", status);
    while (1);
}

void _write(void)
{
    __err("error, status");
    while (1);
}

void _kill(void)
{
    __err("error, status");
    while (1);
}

void _getpid(void)
{
    __err("error, status");
    while (1);
}

void _sbrk(void)
{
    __err("error, status");
    while (1);
}
