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
#ifndef __FSYS_LIBS_H__
#define __FSYS_LIBS_H__
#include "fstypedef.h"
#include <port.h>
#include <string.h>
#include <stdlib.h>

#define  POSITIVE       0
#define  NEGATIVE       1

int snprintf(char *buf, size_t size, const char *fmt, ...);
void *memscan(void *addr, int c, size_t size);

#define malloc            malloc
#undef  calloc
#define calloc            calloc
#undef  realloc
#define realloc           realloc
#undef  free
#define free              free
#undef  time
//#define time(t)           (0)
#undef  srandom
#define srandom(s)        fsyslib_srandom(s)
#undef  random
#define random            fsyslib_random
#undef  sleep
#define sleep             fsyslib_sleep

#define palloc(size)      k_malloc(FSYS_ALIGN((size), 1024))
#define pfree(addr, size) do { \
        if(addr) k_free(addr); \
    } while (0)

static inline void melis_itoa(int input, char *str)
{
    char stack[12];
    char sign_flag = 0 ;      // 'sign_flag indicates wheater 'input' is positive or negative, default
    int i ;                           // value is 'POSITIVE'.
    int j ;

    if (input == 0)
    {
        str[0] = '0';
        str[1] = '\0';                   // 'str' must end with '\0'
        return ;
    }

    if (input < 0)                       // If 'input' is negative, 'input' is assigned to its absolute value.
    {
        sign_flag = 1 ;
        input = -input ;
    }

    for (i = 0; input > 0; ++i)
    {
        stack[i] = input % 10 + '0';    // characters in reverse order are put in 'stack' .
        input /= 10;
    }                                   // at the end of 'for' loop, 'i' is the number of characters.


    j = 0;
    if (sign_flag == 1)
    {
        str[j++] = '-';    // If 'input' is negative, minus sign '-' is placed in the head.
    }
    for (--i  ; i >= 0; --i, ++j)
    {
        str[j] = stack[i];
    }
    str[j] = '\0';                      // 'str' must end with '\0'
}

char *fsyslib_strrchr(const char *s, int c);
char *fsyslib_basename(const char *filename);
int  fsyslib_srandom(int s);
int  fsyslib_random(void);
void fsyslib_sleep(unsigned seconds);
#endif    /* __FSYS_LIBS_H__ */