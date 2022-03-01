/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, SW.China
*                                             All Rights Reserved
*
* Moudle  : Lemon
* File    : LangDec_private.h
*
* By      : Andy
* Version : v1.0
* Date    : 2008-11-9 9:13:15
**********************************************************************************************************************
*/

#ifndef __LANGDEC_PRIVATE_H__
#define __LANGDEC_PRIVATE_H__

#include "mod_orange_i.h"
#define LANGDEC_CHECK_ARG_EN 0

#define __PACKED     __packed
#define fopen1       esRESM_ROpen
#define fclose1      esRESM_RClose
#define fread1       esRESM_RRead
#define fseek1       esRESM_RSeek
#define malloc(x)    esMEMS_Malloc(0, x)
#define free(x)      esMEMS_Mfree(0, x)
#define FILE         __resm_rsb_t
#define strcmp       eLIBs_strcmp
#define memcpy       eLIBs_memcpy

#include "Lang_fmt.h"
typedef struct tag_HLANGLIST
{
    __u16       LangID;         //LangID号 如：Ox400
    __u32       StringTabOff;   //字符串表对应起始地址；
} HLANGLIST;

typedef struct tag_HLANHEAD
{
    FILE        *fp;
    HLANGLIST   *pHLangList;    //指向语言信息表信息；
    __u32       LangTabOff;     //LANG表起始地址偏移量
    __u16       LangSize;       //LANG表中每个条目大小
    __u32       LangNum;        //LANG表条目个数,既语言种类个数
    __u16       StringSize;     //字符串size
    __u32       StringNum;      //字符串个数；
    __u32       align;          //数据边界对齐模式；
    __u16       *pIdTab;
} HHEAD;
#endif /* __LANGDEC_PRIVATE_H__  */
