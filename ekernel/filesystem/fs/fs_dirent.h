/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : dirent.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: directory entery of vfs, code is extracted from linux.
* Update  : date                auther      ver     notes
*           2011-3-15 15:10:18  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __DIRENT_H__
#define __DIRENT_H__

#define NAME_OFFSET(de) ((int) ((de)->d_name - (char *) (de)))

struct dirent
{
    long        d_ino;
    unsigned short  d_reclen;
    char        d_name[256]; /* We must not include limits.h! */
};

struct linux_dirent64
{
    __u64           d_ino;
    __s64           d_off;
    unsigned short  d_reclen;
    unsigned char   d_type;
    __u64           d_size;
    char            d_name[1];
};

#endif  /* __DIRENT_H__ */
