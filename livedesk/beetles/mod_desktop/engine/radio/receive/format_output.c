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
//#include "dsk_radio_receive_i.h"
#include "dsk_radio_i.h"
#include <stdarg.h>
#include <elibs_string.h>


/**
 * eLIBs_strnlen - Find the length of a length-limited string
 * @s: The string to be sized
 * @count: The maximum number of bytes to search
 */
/*__u32 eLIBs_strnlen(const char * s, __u32 count)
{
    const char *sc;

    for (sc = s; count-- && *sc != '\0'; ++sc)
        // nothing ;
    return sc - s;
}*/
/*
**********************************************************************************************************************
*                                               my_isdigit
*
* Description: 判断该字符是否为数字
*
* Arguments  : s：待判断的字符
*
* Returns    : 正确返回1，错误返回0
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 my_isdigit(char s)
{
    if (s > '0' && s < '9')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/*
**********************************************************************************************************************
*                                               myitoa
*
* Description: 把数字转化为字符串
*
* Arguments  : num：待转化的数值；buf_len：buf长度；buf转化后的输出buf
*
* Returns    : 正确返回0，错误返回-1
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 myitoa(__s32 num, __s32 buf_len, char *buf)
{
    while (buf_len > 0)
    {
        buf[buf_len - 1] = (__u8)(num % 10 + '0');
        buf_len--;
        num = num / 10;
    }

    return 0;
}
/*
**********************************************************************************************************************
*                                               skip_atoi
*
* Description: 把字符串s里的数字字符转化为数字，并使字符串指针跳到数字字符后面
*
* Arguments  : s：字符串指针
*
* Returns    : 正确返回转化后的数值，错误返回-1
*
* Notes      :
*
**********************************************************************************************************************
*/
static __s32 skip_atoi(const char **s)
{
    __s32 i = 0;

    while (my_isdigit(**s))
    {
        i = i * 10 + *((*s)++) - '0';
    }

    return i;
}

/*
**********************************************************************************************************************
*                                               number
*
* Description: 格式化数值num为字符串
*
* Arguments  : str：字符串指针；num：待转化的数值；precision：待转化的数值个数
*
* Returns    : 正确返回转化后的字符串指针，错误返回-1
*
* Notes      :
*
**********************************************************************************************************************
*/
static char *number(char *str, __u32 num, __s32 precision)
{
    __u32 num_len = 1;
    __u32 temp;
    char s[10];
    __s32 i;
    temp = num;

    while (temp / 10 > 0)
    {
        num_len++;
        temp = temp / 10;
    }

    if (precision > num_len)
    {
        myitoa(num, num_len, s);

        for (i = num_len; i < precision; i++)
        {
            *str++ = '0';
        }

        for (i = 0; i < num_len; i++)
        {
            *str++ = s[i];
        }
    }
    else
    {
        myitoa(num, precision, s);

        for (i = 0; i < precision; i++)
        {
            *str++ = s[i];
        }
    }

    return str;
}

/*
**********************************************************************************************************************
*                                               my_output_format
*
* Description: 格式化输出到buf，该函数类似与sprintf
*
* Arguments  : buf：输出buf，fmt：格式化
*
* Returns    : 正确返回实际转化的个数，错误返回-1
*
* Notes      :
*
**********************************************************************************************************************
*/

__s32 my_output_format(char *buf, const char *fmt, ...)
{
    __s32 len;
    __u32 num;
    __s32 i;
    char *str;
    const char *s;
    va_list args;
    __s32 precision;        /* min. # of digits for integers; max
                   number of chars for from string */
    va_start(args, fmt);

    for (str = buf ; *fmt ; ++fmt)
    {
        if (*fmt != '%')
        {
            *str++ = *fmt;
            continue;
        }

        ++fmt;
        /* get the precision */
        precision = -1;

        if (*fmt == '.')
        {
            ++fmt;

            if (my_isdigit(*fmt))
            {
                precision = skip_atoi(&fmt);
            }

            if (precision < 0)
            {
                precision = 0;
            }
        }

        switch (*fmt)
        {
            case 's':
                s = va_arg(args, char *);

                if (!s)
                {
                    s = "<NULL>";
                }

                len = eLIBs_strnlen(s, precision);

                for (i = 0; i < len; ++i)
                {
                    *str++ = *s++;
                }

                while (len < precision--)
                {
                    *str++ = ' ';
                }

                continue;

            case 'd':
                num = va_arg(args, unsigned int);
                str = number(str, num, precision);
                continue;

            default:
                continue;
        }
    }

    *str = '\0';
    va_end(args);
    return str - buf;
}
