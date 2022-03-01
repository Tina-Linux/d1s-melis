/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : namei.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: name lookup of vfs, code is extracted from linux.
* Update  : date                auther      ver     notes
*           2011-3-15 15:52:19  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __NAMEI_H__
#define __NAMEI_H__

#include "dcache.h"

struct open_intent
{
    int flags;
    int create_mode;
    struct file *file;
};

enum { MAX_NESTED_LINKS = 8 };

struct nameidata
{
    struct dentry   *dentry;
    struct qstr last;
    unsigned int    flags;
    int     last_type;

    /* Intent data */
    union
    {
        struct open_intent open;
    } intent;
};

/*
 * Type of the last component on LOOKUP_PARENT
 */
enum {LAST_NORM, LAST_ROOT, LAST_DOT, LAST_DOTDOT, LAST_BIND};

/*
 * The bitmask for a lookup event:
 *  - follow links at the end
 *  - require a directory
 *  - ending slashes ok even for nonexistent files
 *  - internal "there are more path compnents" flag
 *  - locked when lookup done with dcache_lock held
 *  - dentry cache is untrusted; force a real lookup
 */
#define LOOKUP_DIRECTORY     2
#define LOOKUP_CONTINUE      4
#define LOOKUP_PARENT       16
#define LOOKUP_NOALT        32
#define LOOKUP_REVAL        64
/*
 * Intent data
 */
#define LOOKUP_OPEN     (0x0100)
#define LOOKUP_CREATE       (0x0200)
#define LOOKUP_ACCESS       (0x0400)
#define LOOKUP_CHDIR        (0x0800)

extern struct file *nameidata_to_filp(struct nameidata *nd, int flags);
extern void release_open_intent(struct nameidata *);

#endif /* __NAMEI_H__ */
