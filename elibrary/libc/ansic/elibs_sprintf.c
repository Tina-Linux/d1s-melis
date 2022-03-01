/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               ANSIC Libary
*
*                                    (c) Copyright 2010-2011, Sunny China
*                                             All Rights Reserved
*
* File    : sprintf.c
* By      : Sunny
* Version : v1.0
* Date    : 2010-11-9
* Descript:
* Update  : date                auther      ver     notes
*           2020-11-9 14:17:56  Sunny       1.0     sprintf function implement.
*********************************************************************************************************
*/
#include <typedef.h>
#include <mod_slib.h>

/*
*********************************************************************************************************
*                                           ELIBS_VSNPRINTF
*
* Description: Write formatted output using a pointer to a list of arguments.
*
* Arguments  :
*              buffer   -Storage location for output
*              size     -Maximum number of characters to store
*              format   -Format-control string
*              args     -Pointer to list of arguments
*
* Returns    : the number of bytes stored in buffer, not counting the terminating null character.
*********************************************************************************************************
*/
int eLIBs_vsnprintf(char *buffer, size_t size, const char *format, va_list arg)
{
    return SLIB_vsnprintf(buffer, size, format, arg);
}

/*
*********************************************************************************************************
*                                           ELIBS_VSCNPRINTF
*
* Description: Write formatted output using a pointer to a list of arguments.
*
* Arguments  :
*              buffer   -Storage location for output
*              size     -Maximum number of characters to store
*              format   -Format-control string
*              args     -Pointer to list of arguments
*
* Returns    : the number of bytes stored in buffer, not counting the terminating null character.
*********************************************************************************************************
*/
int eLIBs_vscnprintf(char *buffer, size_t size, const char *format, va_list args)
{
    int i;
    i = eLIBs_vsnprintf(buffer, size, format, args);
    return (i >= size) ? (size - 1) : i;
}

/*
*********************************************************************************************************
*                                           ELIBS_SNPRINTF
*
* Description: Write formatted data to a string.
*
* Arguments  :
*              buffer   -Storage location for output
*              size     -Maximum number of characters to store
*              format   -Format-control string
*              ...      -Optional arguments
*
* Returns    : the number of bytes stored in buffer, not counting the terminating null character.
*********************************************************************************************************
*/
int eLIBs_snprintf(char *buffer, size_t size, const char *format, ...)
{
    va_list args;
    int i;
    va_start(args, format);
    i = eLIBs_vsnprintf(buffer, size, format, args);
    va_end(args);
    return i;
}

/*
*********************************************************************************************************
*                                           ELIBS_SCNPRINTF
*
* Description: Write formatted data to a string.
*
* Arguments  :
*              buffer   -Storage location for output
*              size     -Maximum number of characters to store
*              format   -Format-control string
*              ...      -Optional arguments
*
* Returns    : the number of bytes stored in buffer, not counting the terminating null character.
*********************************************************************************************************
*/
int eLIBs_scnprintf(char *buffer, size_t size, const char *format, ...)
{
    va_list args;
    int i;
    va_start(args, format);
    i = eLIBs_vsnprintf(buffer, size, format, args);
    va_end(args);
    return (i >= size) ? (size - 1) : i;
}

/*
*********************************************************************************************************
*                                           ELIBS_SPRINTF
*
* Description: Write formatted output using a pointer to a list of arguments.
*
* Arguments  :
*              buffer   -Storage location for output
*              format   -Format-control string
*              args     -Pointer to list of arguments
*
* Returns    : the number of bytes stored in buffer, not counting the terminating null character.
*
*********************************************************************************************************
*/
int eLIBs_vsprintf(char *buffer, const char *format, va_list args)
{
    return eLIBs_vsnprintf(buffer, SIZE_MAX, format, args);
}

/*
*********************************************************************************************************
*                                           ELIBS_SPRINTF
*
* Description: Write formatted data to a string.
*
* Arguments  :
*              buffer   -Storage location for output
*              format   -Format-control string
*              ...      -Optional arguments
*
* Returns    : the number of bytes stored in buffer, not counting the terminating null character.
*
* Note       : The sprintf function formats and stores a series of characters and values in buffer.
*              Each argument (if any) is converted and output according to the corresponding format
*              specification in format. The format consists of ordinary characters and has the same
*              form and function as the format argument for ANSI-C printf.
*              but the int64(or long long) format control string is : "%qd".
*
* Example:
*           {
*               char    buffer[512];
*               char    s[] = "computer";
*               char    c = 'l';
*               int     i = 35;
*               float   fp = 1.7320534f;
*               __int64 i64 = 0xFFFFFFFFFFFF;
*               int     len;
*
*               len  = eLIBs_sprintf(buffer,        "\tString:    %s\n", s);
*               len += eLIBs_sprintf(buffer + len,  "\tCharacter: %c\n", c);
*               len += eLIBs_sprintf(buffer + len,  "\tInteger:   %d\n", i);
*               len += eLIBs_sprintf(buffer + len,  "\tint64:     %qd\n",i64);
*               len += eLIBs_sprintf(buffer + len,  "\tReal:      %f\n", fp);
*
*               eLIBs_printf("Output:\n%s\ncharacter count = %d\n", buffer, len);
*           }
*           eLIBs_printf output result
*           Output:
*           String:    computer
*           Character: l
*           Integer:   35
*           int64:     281474976710655
*           Real:      1.732053
*
*           character count = 99
*********************************************************************************************************
*/
int eLIBs_sprintf(char *buffer, const char *format, ...)
{
    va_list args;
    int rv;
    va_start(args, format);
    rv = eLIBs_vsnprintf(buffer, SIZE_MAX, format, args);
    va_end(args);
    return rv;
}
