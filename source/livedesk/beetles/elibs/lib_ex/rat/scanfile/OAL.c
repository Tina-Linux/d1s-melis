/*
*********************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Development Kit
*                                       ebook information manager libary
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                              All Rights Reserved
*
* File    : OAL.c
* By      : Sunny
* Version : V1.00
* Date    : 2010-4-27 14:59:38
*********************************************************************************************************
*/
#include <typedef.h>
#include <libc.h>
#include    "OAL.h"
//#include  <rt_fp.h>   /* ARM fp libary, for _dfixu()  */

//extern unsigned int _dfixu(double);

void rat_dtoa(double value, char *string)
{
    unsigned int wn;
    unsigned int df;
    char *str1;
    char str2[16];
    char str3[16];
    str1 = value < 0.0 ? "-" : "";
    value = value < 0.0 ? (-value) : value;
    //    wn = _dfixu(value);
    //    df = _dfixu((value - wn)*1000);
    wn = (unsigned int)value;
    df = (unsigned int)((value - wn) * 1000);
    eLIBs_uint2str_dec(wn, str2);
    eLIBs_uint2str_dec(df, str3);
    eLIBs_strcpy(string, str1);
    eLIBs_strcat(string, str2);
    eLIBs_strcat(string, ".");
    eLIBs_strcat(string, str3);
}

int __isdigit(int cChar)
{
    // Return true if the character is between 0 and 9 inclusive.
    if ((cChar >= '0') && (cChar <= '9'))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

double rat_atof(const char *nptr)
{
    double retval = 0.0;
    double result;
    const char *p;
    int shift = 0, sign = 1, exp_sign = 1, exp = 0;
    p = nptr;

    while (*p)
    {
        if (*p == ' ' || *p == '\t')
        {
            p++;
            continue;
        }

        break;
    }

    /**
     * '+'/'-'判断符号位: 不能两个连续的'+'或'-'否则函数立即返回
     */
    if (*p == '+')
    {
        sign = 1;
        ++p;
    }
    else if (*p == '-')
    {
        sign = -1;
        ++p;
    }
    else
    {
        ;
    }

    // int_part:
    while (*p)
    {
        if (__isdigit(*p))
        {
            retval = (double)((int)retval * 10 + (*p - '0'));
        }
        else if (*p == '.')
        {
            ++p;
            goto non_int_part;
        }
        else if (*p == 'e' || *p == 'E')
        {
            ++p;
            goto exp_part;
        }
        else
        {
            return retval;
        }

        ++p;
    }

non_int_part:

    while (*p)
    {
        ++shift;

        if (p - nptr > 100)
        {
            break;
        }

        if (__isdigit(*p))
        {
            int f = shift, f1 = (*p - '0');
            double f2 = (double)f1;

            while (f--)
            {
                f2 /= 10;
            }

            retval += f2;
        }
        else if (*p == 'e' || *p == 'E')
        {
            ++p;
            goto exp_part;
        }
        else
        {
            return retval;
        }

        ++p;
    }

exp_part:

    /**
     * 判断符号位: 但不允许同时出现两个符号
     * 否则函数立即返回
     */
    if (*p == '-')
    {
        exp_sign = -1;
        ++p;
    }
    else if (*p == '+')
    {
        exp_sign = 1;
        ++p;
    }
    else
    {
        ;
    }

    while (*p)
    {
        if (__isdigit(*p))
        {
            exp = exp * 10 + (*p - '0');
        }
        else
        {
            break;
        }

        ++p;
    }

    if (exp_sign > 0)
    {
        while (exp)
        {
            retval *= 10;
            exp--;
        }
    }
    else
    {
        while (exp)
        {
            retval /= 10;
            exp--;
        }
    }

    result = retval * sign;
    return (double)(result);
}

/**
 * _strcpy - Copy a %NUL terminated string
 * @dest    : Where to copy the string to
 * @src     : Where to copy the string from
 * Note     : to avoid SQL syntax error, should replace ''' to ' ',
 * By sunny at 2012-5-10 18:20:41.
 */
static char *_strcpy(char *pdest, const char *psrc, int *repcnt)
{
    char  *pstr;
    char  *pstr_next;

    /* Rtn NULL if str ptr(s) NULL (see Note #2a).                  */
    if (pdest == (char *)0)
    {
        return ((char *)0);
    }

    if (psrc  == (char *)0)
    {
        return ((char *)0);
    }

    *repcnt = 0;
    pstr      = pdest;
    pstr_next = pstr;
    pstr_next++;

    while ((pstr_next != (char *)0) &&          /* Copy str until NULL ptr(s) (see Note #2b) ...                */
           (psrc      != (char *)0) &&
           (*psrc      != (char)0))             /* ... or NULL char found     (see Note #2c).                   */
    {
        if (*psrc == '\'')
        {
            *pstr++ = '\'';                     /* add '\'' to avoid SQL syntax error */
            (*repcnt)++;
        }

        *pstr = *psrc;
        pstr++;
        pstr_next++;
        psrc++;
    }

    *pstr = (char)0;                             /* Append NULL char (see Note #2b2).                            */
    return (pdest);
}

void rat_sprintf(char *buffer, const char *str, ...)
{
    char string[16];
    char *p, *_p = buffer;
    __s32 hex_flag;
    va_list argp;
    int     repcnt = 0;
    va_start(argp, str);

    while (*str)
    {
        if (*str == '%')
        {
            ++str;
            p = string;
            hex_flag = HEX_X;

            switch (*str)
            {
                case 'd':
                    eLIBs_int2str_dec(va_arg(argp,  __s32), string);
                    eLIBs_strcpy(_p, p);
                    _p += eLIBs_strlen(p);
                    ++str;
                    break;

                case 'x':
                    hex_flag = HEX_x;          // jump to " case 'X' "
                    break;

                case 'p':
                case 'X':
                    eLIBs_int2str_hex(va_arg(argp,  __s32), string, hex_flag);
                    eLIBs_strcpy(_p, p);
                    _p += eLIBs_strlen(p);
                    ++str;
                    break;

                case 'u':
                    eLIBs_uint2str_dec(va_arg(argp,  __s32), string);
                    eLIBs_strcpy(_p, p);
                    _p += eLIBs_strlen(p);
                    ++str;
                    break;

                case 'c':
                    _p[0] = va_arg(argp,  __s32);
                    _p ++;
                    ++str;
                    break;

                case 's':
                    p =  va_arg(argp,  char *);
                    _strcpy(_p, p, &repcnt);          /* should replace '\'' to ' ', by sunny at 2012-5-10 18:29:27 */
                    _p += (eLIBs_strlen(p) + repcnt);
                    ++str;
                    break;

                case 'f':
                    rat_dtoa(va_arg(argp,  double), p);
                    eLIBs_strcpy(_p, p);
                    _p += eLIBs_strlen(p);
                    ++str;
                    break;

                default :
                    _p[0] = '%';
                    _p ++;
                    _p[0] = *str;
                    _p ++;
                    ++str;                        // let 'str' point to next character.
            }
        }
        else
        {
            _p[0] = *str++;
            _p ++;
        }
    }

    _p[0] = '\0';
    va_end(argp);
}
