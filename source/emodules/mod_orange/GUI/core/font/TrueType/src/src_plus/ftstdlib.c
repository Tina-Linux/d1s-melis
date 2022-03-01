/*
************************************************************************************************************************
*                                                     REGISTER TABLE
*
*                             Copyright(C), 2006-2008, Allwinner Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : ftstdlib.c
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2008.06.26
*
* Description :
*
* Functions list :
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2008.06.26       1.1.0        build the file
*
************************************************************************************************************************
*/
#include "freetype/config/ftstdlib.h"

long int atol(char *s)
{
    long r = 0;
    int neg = 0;
    switch (*s)
    {
        case '-':
            neg = 1;
        // 这里没有break
        case '+':
            s++;
            break;
    }
    while (*s >= '0' && *s <= '9')
    {
        int n = *s++ - '0';
        if (neg)
        {
            n = -n;
        }
        r = r * 10 + n;
    }
    return r;
}

/*---------------------------------------- EPDK_WIN32 ----------------------------------------*/
#if defined( EPDK_WIN32 )



/*----------------------------------------- EPDK_AC320 ----------------------------------------*/
#elif defined( EPDK_AC320 )



void *ft_srealloc(void *ptr, size_t size)
{
#ifdef SIM_PC_WIN
    return esMEMS_Realloc(NULL, ptr, size);
#else
    char *p;
    char *src;
    size_t i;

    if (ptr == NULL && size == 0)
    {
        return NULL;
    }

    if (ptr == NULL)
    {
        p = (char *)ft_smalloc(size);
        return p;
    }

    if (size == 0)
    {
        ft_sfree(ptr);
        return NULL;
    }

    p = (char *)ft_smalloc(size);

    if (p == NULL)
    {
        return NULL;
    }

    for (src = (char *)ptr, i = 0;  i < size;  i++)
    {
        p[i] = src[i];
    }

    ft_sfree(ptr);
    return p;
#endif
}


/*----------------------------------------- platform end -----------------------------------------*/
#else
#error PLATFORM must be selected.
#endif
