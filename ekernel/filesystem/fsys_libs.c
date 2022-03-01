/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : fsys_libs.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-15
* Descript: basic libary for file system.
* Update  : date                auther      ver     notes
*           2011-3-15 14:43:08  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include <stdarg.h>
#include <string.h>
#include "fsys_libs.h"

char *fsyslib_strrchr(const char *s, int c)
{
    char *found, *p;

    c = (unsigned char) c;

    /* Since strchr is fast, we use it rather than the obvious loop.  */

    if (c == '\0')
    {
        return (char *)strchr(s, '\0');
    }

    found = NULL;
    while ((p = (char *)strchr(s, c)) != NULL)
    {
        found = p;
        s = p + 1;
    }

    return (char *) found;
}

char *fsyslib_basename(const char *filename)
{
    char *p = fsyslib_strrchr(filename, '\\');
    return p ? p + 1 : (char *) filename;
}

/**
 * memscan - Find a character in an area of memory.
 * @addr: The memory area
 * @c: The byte to search for
 * @size: The size of the area.
 *
 * returns the address of the first occurrence of @c, or 1 byte past
 * the area if @c is not found
 */
void *memscan(void *addr, int c, size_t size)
{
    unsigned char *p = addr;

    while (size)
    {
        if (*p == c)
        {
            return (void *)p;
        }
        p++;
        size--;
    }
    return (void *)p;
}
