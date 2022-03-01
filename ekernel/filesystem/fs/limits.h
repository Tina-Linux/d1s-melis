/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : limits.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: code is extracted from linux.
* Update  : date                auther      ver     notes
*           2011-3-15 15:41:02  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __LIMITS_H__
#define __LIMITS_H__

//#define NR_OPEN           1024

#define NGROUPS_MAX    65536    /* supplemental group IDs are available */
#define ARG_MAX       131072    /* # bytes of args + environ for exec() */
#define CHILD_MAX        999    /* no limit :-) */
#define OPEN_MAX         256    /* # open files a process may have */
#define LINK_MAX         127    /* # links a file may have */
#define MAX_CANON        255    /* size of the canonical input queue */
#define MAX_INPUT        255    /* size of the type-ahead buffer */
#define NAME_MAX         255    /* # chars in a file name */
#define PATH_MAX        1024    /* # chars in a path name including nul, 
                                 * modify from 256 to 1024 by sunny at 2010.6.12 */
#define PIPE_BUF        4096    /* # bytes in atomic write to a pipe */
#define XATTR_NAME_MAX   255    /* # chars in an extended attribute name */
#define XATTR_SIZE_MAX 65536    /* size of an extended attribute value (64k) */
#define XATTR_LIST_MAX 65536    /* size of extended attribute namelist (64k) */

#define RTSIG_MAX     32

#endif  /* __LIMITS_H__ */
