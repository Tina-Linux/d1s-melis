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
* File    : Lang_fmt.h
*
* By      : Andy
* Version : v1.0
* Date    : 2008-11-9 9:13:15
**********************************************************************************************************************
*/

#ifndef __LANG_FMT_H__
#define __LANG_FMT_H__

#define SEC_NAME        ".langdat"  //8字节长度，节名

//语言资源文件数据格式定义
typedef struct tag_LANG_HEAD
{
    __s8        SecName[8];     //节名
    __u16       version;        //版本：0x0100
    __u16       size;           //LANG_HEAD数据结构大小
    __u32       LangTabOff;     //LANG表起始地址偏移量----------------->
    __u16       LangSize;       //LANG表中每个条目大小
    __u32       LangNum;        //LANG表条目个数,既语言种类个数
    __u16       StringSize;     //字符串size
    __u32       StringNum;      //字符串个数；
    __u32       align;          //数据边界对齐模式；
    __u32       flags;          //标志:SYSRES,LIBRES,APPRES
} __attribute__((__packed__)) LANG_HEAD; //34 byte
typedef struct tag_LANG
{
    __u16       LangID;             //0X400 0X420
    __u32       LangOffSet;         //LANG 条目起始地址偏移量
    __u32       LangSize;           //LANG 条目大小
    __u32       StringTabOff;       //字符表起始地址
} __attribute__((__packed__)) LANG; //

typedef struct tag_STRING
{
    __u16       LangID;         //LangID
    __u16       StringID;       //such as:0x0001
    __u32       offset;         //string data位置
    __u32       size;           //data数据长度
} __attribute__((__packed__)) LANG_STRING;

#endif /* __LANG_FMT_H__  */
