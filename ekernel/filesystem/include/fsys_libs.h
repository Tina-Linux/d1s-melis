/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : fsys_libs.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-15
* Descript: basic libary for file system.
* Update  : date                auther      ver     notes
*           2011-3-15 14:43:08  Sunny       1.0     Create this file.
*********************************************************************************************************
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
