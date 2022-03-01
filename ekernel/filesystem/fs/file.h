/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : file.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: file operations of vfs, code is extracted from linux.
* Update  : date                auther      ver     notes
*           2011-3-15 15:18:23  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __FILE_H__
#define __FILE_H__

struct file;
extern void fput(struct file *);

static inline void fput_light(struct file *file, int fput_needed)
{
    //  if (fput_needed)
    //      fput(file);
}

extern void fget(struct file *);
extern void put_filp(struct file *);

#endif /* __FILE_H__ */
