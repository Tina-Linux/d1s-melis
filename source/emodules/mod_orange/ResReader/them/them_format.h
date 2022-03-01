//------------------------------------------------------------------------------------------------------------
//2008-05-12 19:08:55
//scott
//res.h
//------------------------------------------------------------------------------------------------------------

#ifndef __THEM_FORMAT_H__
#define __THEM_FORMAT_H__     1


#include "themdec_cfg.h"
#include "os_related.h"

//------------------------------------------------------------------------------------------------------------
//资源数据节名
//------------------------------------------------------------------------------------------------------------
#define RES_SEC_NAME_SIZE   8           //
#define RES_SEC_NAME        ".lionres"  //8字节长度
#define RES_VERSION         0x0100      //资源数据节格式版本
#define RES_ALIGN           32


//------------------------------------------------------------------------------------------------------------
//资源文件类型
//------------------------------------------------------------------------------------------------------------
#define SYSRES              0
#define LIBRES              1
#define APPRES              2



//------------------------------------------------------------------------------------------------------------
//资源数据头定义
//------------------------------------------------------------------------------------------------------------
#ifdef WIN32_DATA_PACK
#pragma pack(push, 1)
#endif
typedef struct tag_RESHEAD
{
    int8        SecName[8];         //节名
    uint16      version;            //版本0x0100
    uint16      size;               //头数据结构大小
    uint32      StyleOffset;        //风格数据结构偏移量
    uint32      StyleNum;           //风格个数
    uint32      ResNum;             //资源个数
    uint32      align;              //数据边界对齐模式
    uint32      flags;              //标志，SYSRES LIBRES  APPRES
} __attribute__((__packed__)) RESHEAD;
#ifdef WIN32_DATA_PACK
#pragma pack(pop)
#endif

//本结构的大小
#define RESHEAD_SIZE    sizeof(RESHEAD) //32 byte




//------------------------------------------------------------------------------------------------------------
//主题描述表定义
//------------------------------------------------------------------------------------------------------------
#define STYLE_NAME_SIZE 18

#ifdef WIN32_DATA_PACK
#pragma pack(push, 1)
#endif
typedef struct tag_STYLE
{
    uint16      ID;                 //风格ID， 唯一
    int8        Name[STYLE_NAME_SIZE];//风格名称，ASCII码
    uint32      ResTableOffset;     //资源表起始地址偏移量
    uint32      ResOffset;          //资源数据起始地址偏移量
    uint32      ResSize;            //资源数据长度
} __attribute__((__packed__)) STYLE;
#ifdef WIN32_DATA_PACK
#pragma pack(pop)
#endif

//本结构的大小
#define STYLE_SIZE  sizeof(STYLE)   //32 byte




//------------------------------------------------------------------------------------------------------------
//资源描述表定义
//------------------------------------------------------------------------------------------------------------
#define RES_NAME_SIZE    52

#ifdef WIN32_DATA_PACK
#pragma pack(push, 1)
#endif
typedef struct tag_RESITEM
{
    uint16      type;               //资源类型
    uint16      ID;                 //资源ID,不同风格的相同资源ID相同
    int8        Name[RES_NAME_SIZE];//资源名称，名称可以重复
    uint32      offset;             //资源起始地址在res节的偏移
    uint32      size;               //资源的size
} __attribute__((__packed__)) RESITEM;
#ifdef WIN32_DATA_PACK
#pragma pack(pop)
#endif

//本结构的大小
#define RESITEM_SIZE    sizeof(RESITEM) //32



#endif //__THEM_FORMAT_H__

