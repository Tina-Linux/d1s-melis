/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                             All Rights Reserved
*
* File      : mnpl.h
* By        : Andy.zhang
* Func      : now play list
* Version   : v1.0
* ============================================================================================================
* 2009-11-3 9:39:42  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __MNPL_H__
#define __MNPL_H__

struct tag_MNPL;
typedef struct tag_MNPL MNPL;


MNPL   *mnpl_create(__s32 media_type);

__s32   mnpl_addItem(MNPL *npl, const char *file);

__s32   mnpl_deleteItem(MNPL *npl, const char *file);

__s32   mnpl_clear(MNPL *npl);

__s32   mnpl_playingSeek(MNPL *npl,  __s32 origin, __s32 offset);

__s32   mnpl_getNext(MNPL *npl);

__s32   mnpl_getCur(MNPL *npl);

__s32   mnpl_fileToIndex(MNPL *npl, const char *file);

__s32   mnpl_indexToFile(MNPL *npl, __u32 index, char *file);

__s32   mnpl_invalidItem(MNPL *npl, __u32 index);

__s32   mnpl_itemIsValid(MNPL *npl, __u32 index);

__s32   mnpl_getTotalCount(MNPL *npl);

__s32   mnpl_getValidCount(MNPL *npl);

__s32   mnpl_delete(MNPL *npl);


#endif /* __MNPL_H__ */


